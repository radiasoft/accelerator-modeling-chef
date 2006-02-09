/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                 
******                                                
******  File:      bmlfactory.cc
******  Version:   2.0
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved                             
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
******  Authors:    Dmitri Mokhov, Oleg Krivosheev                  
******              Jean-Francois Ostiguy 
******
******  Contact:   Leo Michelotti or Jean-Francois Ostiguy            
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******                    (630) 840 2231                              
******             Email: michelotti@fnal.gov                         
******                    ostiguy@fnal.gov                            
******
**************************************************************************
**************************************************************************
* Revision History: 
*
*  Version 1.5: August 2004 
*
*  ostiguy@fnal.gov: 
*
*  -Numerous **severe** memory leaks eliminated. 
*  -MAD CALL and RETURN statements now properly interpreted
*  -improved error recovery
*  -Expressions associated with MAD type parameters are now properly deleted.  
*  -Added member function allow external access list of beamline identifiers.
*  -Added member function to allow external evaluation of a variable expression. 
*  -Added member function to allow parsing from in-memory buffer
*****************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

using namespace std;

// allocate and deallocate are strings used 
// int the stdlib headers. Unfortunately, they
// are also used by the bmlfactory private block 
// allocators 

#undef   allocate
#undef   deallocate

#include <iostream>
#include <iosetup.h> 
#include <beamline.h>
#include <GenericException.h>

#include <functional>
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>

#include <beam_element.h>
#include <beamel_table.h>
#include <beam_line.h>
#include <bml_table.h>
#include <bmlfactory.h>
#include <expression.h>
#include <madparser.h>
#include <var_table.h>
#include <bel_inst_fns.h>

extern struct madparser_* mp;

using namespace std;
using FNAL::pcout;
using FNAL::pcerr;

bmlfactory::bmlfactory( const char* fname, double BRHO, const char* stringbuffer) {

  BRHO_ = BRHO;
  try 
  {
     bmlfactory_init(fname, stringbuffer);

  }
  catch (GenericException& e)
  {
    /* this is a constructor so the destructor will not be called when an exception is thrown. 
      We need to clean up here in order to avoid leaks and ensure that flex is reset to a sane state.
    */

     madparser_delete(mp);

     delete_bel_list();
     delete_bml_list();

     if (fname_) delete [] fname_;
  
     if ( bel_arr_) free(bel_arr_);  // memory allocated by call to bel_table_to_array in constructor
     if ( bml_arr_) free(bml_arr_);  // memory allocated by call to bml_table_to_array in constructor

     throw e;
  }

}


bmlfactory::bmlfactory( const char* fname, const char* stringbuffer) {

  BRHO_ = 0;
  try 
  {

     bmlfactory_init(fname, stringbuffer);

  }
  catch (GenericException& e)
  {
    /* this is a constructor so the destructor will not be called when an exception is thrown. 
      We need to clean up here in order to avoid leaks and ensure that flex is reset to a sane state.
    */
  
      madparser_delete(mp);
   
      delete_bel_list();
      delete_bml_list();

      if (fname_) delete [] fname_;
  
      if (bel_arr_) free(bel_arr_);  // memory allocated by call to bel_table_to_array in constructor
      if (bml_arr_) free(bml_arr_);  // memory allocated by call to bml_table_to_array in constructor

    throw e;
  }

}


void 
bmlfactory::bmlfactory_init(const char* fname, const char* stringbuffer) {

  mp_      = NULL;
  fname_   = NULL;
  bel_arr_ = NULL;
  bml_arr_ = NULL;
  
  assert( fname != NULL );
  
  int length = strlen( fname );
  assert( length > 0 );
  
  fname_ = new char[length+1];
  assert( fname_ != NULL );
  strcpy( fname_, fname );
  
  mp_ = madparser_init( fname_, NULL);

  assert( mp_ != NULL );
  
  mp = mp_; // set the global variable mp to point to the same parser structure as mp_

  var_table_ = madparser_var_table( mp_ );
  bel_table_ = madparser_bel_table( mp_ );

  create_bel_list();
  create_bml_list();

  try 
  { 
    madparser_parse( mp_ , stringbuffer);
  }
  catch(GenericException& e) 
  {
    throw e;
  }

  bel_arr_size_ = bel_table_to_array( &bel_arr_, bel_table_ );              /* bel_arr is allocated here */
  qsort( bel_arr_, bel_arr_size_, sizeof(beam_element*), bel_compare );     // sorting w/r to declaration line number
  
  bml_arr_size_ = bml_table_to_array( &bml_arr_, madparser_bml_table( mp_ ) ); /* bml_arr is allocated here */
  if ( bml_arr_size_ > 1 ) {
    qsort( bml_arr_, bml_arr_size_, sizeof(beam_line*), bml_compare );          // sorting w/r to declartion line number 
    bml_remove_forward( &bml_arr_, bml_arr_size_, madparser_bml_table( mp_ ) ); // When removing forward references, line order isn't preserved
  }
  
}

bmlfactory::~bmlfactory() {

  madparser_delete( mp_ );

  mp_ = NULL;
  mp  = NULL;

  delete_bel_list();
  delete_bml_list();

  delete [] fname_;
  
  free(bel_arr_);  // memory allocated by call to bel_table_to_array in constructor
  free(bml_arr_);  // memory allocated by call to bml_table_to_array in constructor

  // delete the bmlnElmnt beamline objects created by the parser. 
  // Note: the create_beamline function returns a Cloned() beamline and bmlnElmnts, 
  // so that ownership of these objects is that of the bmlfactory object instance.

}


beamline* 
bmlfactory::create_beamline( const char* bmlname) {   // *** DEPRECATED ***
  
 beamline* bml = create_beamline_private(bmlname);

 if ( bml == 0)
 { 
   throw GenericException(__FILE__, __LINE__, 
                                "bmlfactory::create_beamline( const char* bmlname)",
                                "Beamline not found");
 }  
 else
 {
    return static_cast<beamline*>( bml->Clone() );
 }
}


beamline* 
bmlfactory::create_beamline( const char* bmlname, double brho   ) 
{

 beamline* bml = create_beamline_private(bmlname, brho);
 
 if ( bml == 0)
 { 
   throw GenericException(__FILE__, __LINE__, 
                                "bmlfactory::create_beamline( const char* bmlname, brho)",
                                "Beamline not found");
 }  
 else
 {
    return static_cast<beamline*>( bml->Clone() );
  } 
}



beamline*
bmlfactory::create_beamline_private( const char* bmlname) { // *** DEPRECATED ****

  return create_beamline_private( bmlname, BRHO_); 

}

beamline*
bmlfactory::create_beamline_private( const char* bmlname, double brho ) {
  
  BRHO_ = brho;

  beamline* bml_ptr = 0;  
  int i;
  for ( i = 0; i < bml_arr_size_; ++i ) {
    if ( strcasecmp( (*(bml_arr_+i))->name_, bmlname ) == 0 ) {
      break;
    }
  }
  if ( i == bml_arr_size_ ) {
    (*pcerr) << "Beam line \"" << bmlname << "\" not found." << endl;
    return NULL;
  } else {
    bml_ptr = (beamline*)(beam_line_instantiate( *(bml_arr_+i) ));
    return bml_ptr;
  }
}

static char*
add_str( char* ptr, const char* s1, const char* s2 ) {
  strcpy( ptr, s1 );
  return strcat( ptr, s2 );
}


std::string
bmlfactory::strip_final_colon( const char* stuff )
{
  if( 0 == stuff ) { return std::string("*** ERROR ***"); }
  std::string workString( stuff );
  std::string ret;
  int n = workString.size();
  if( 1 < n ) { 
    n -= 1;
    if( ':' == workString[n] ) {
      ret.assign( workString, 0, n );
    }
    else {
      ret = workString;
    }
  }
  else {
    ret = workString;
  }
  return ret;
}


bmlnElmnt*
bmlfactory::beam_element_instantiate( beam_element* bel ) {
  
  bmlnElmnt* lbel = find_beam_element( bel->name_ );
  if ( lbel != NULL ) {
    return lbel;
  }
  
  alignmentData* aligner = new alignmentData;
  
  switch ( bel->kind_ ) {
    case BEL_DRIFT: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      lbel = new drift( strip_final_colon( bel->name_ ).c_str(), length );
      break;
    }
    case BEL_MARKER: {
      lbel = new marker( strip_final_colon( bel->name_ ).c_str() );
      break;
    }
    case BEL_SBEND: {
      bool simple = false;
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double angle  = expr_evaluate( bel->params_[BEL_SBEND_ANGLE], var_table_, bel_table_ );
      expr_struct *k1   = (expr_struct*)bel->params_[BEL_SBEND_K1]->data;
      expr_struct *k2   = (expr_struct*)bel->params_[BEL_SBEND_K2]->data;
      expr_struct *k3   = (expr_struct*)bel->params_[BEL_SBEND_K3]->data;
      expr_struct *tilt = (expr_struct*)bel->params_[BEL_SBEND_TILT]->data;
      
      double e1 = expr_evaluate( bel->params_[BEL_SBEND_E1], var_table_, bel_table_ );
      double e2 = expr_evaluate( bel->params_[BEL_SBEND_E2], var_table_, bel_table_ );


      simple = (k1->kind_ == NUMBER_EXPR && k1->dvalue_ == 0.0 && 
                k2->kind_ == NUMBER_EXPR && k2->dvalue_ == 0.0 && 
                k3->kind_ == NUMBER_EXPR && k3->dvalue_ == 0.0 && 
                tilt->kind_ == NUMBER_EXPR && tilt->dvalue_ == 0.0);

      if( true == simple ) {
        lbel = new sbend( strip_final_colon( bel->name_ ).c_str(), length, BRHO_*angle/length, angle, e1, e2 );
        if ( tilt->dvalue_ != 0.0 || tilt->kind_ != NUMBER_EXPR ) {
          aligner->xOffset = 0.0;
          aligner->yOffset = 0.0;
          aligner->tilt    = expr_evaluate( bel->params_[BEL_SBEND_TILT], var_table_, bel_table_ );
          lbel->setAlignment( *aligner );
          // Ignored parameters: K1, K2, K3, E1, E2, TILT, H1, H2, HGAP, FINT
	}
        break;
      }
      else {
        lbel = new CF_sbend( strip_final_colon( bel->name_ ).c_str(), length, BRHO_*angle/length, angle, e1, e2 );

        double multipoleStrength;

        multipoleStrength = expr_evaluate( bel->params_[BEL_SBEND_K1], var_table_, bel_table_ );
        multipoleStrength = multipoleStrength*BRHO_*length;
        if( multipoleStrength != 0.0 ) {
          ((CF_sbend*) lbel)->setQuadrupole( multipoleStrength );
	}

        multipoleStrength = expr_evaluate( bel->params_[BEL_SBEND_K2], var_table_, bel_table_ );
        multipoleStrength = multipoleStrength*BRHO_*length/2.0;
        if( multipoleStrength != 0.0 ) {
          ((CF_sbend*) lbel)->setSextupole( multipoleStrength );
	}

        multipoleStrength = expr_evaluate( bel->params_[BEL_SBEND_K3], var_table_, bel_table_ );
        multipoleStrength = multipoleStrength*BRHO_*length/6.0;
        if( multipoleStrength != 0.0 ) {
          ((CF_sbend*) lbel)->setOctupole( multipoleStrength );
	}
        break;
      }
    }
    case BEL_RBEND: {
      bool simple = false;
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double angle  = expr_evaluate( bel->params_[BEL_RBEND_ANGLE], var_table_, bel_table_ );
      expr_struct *k1   = (expr_struct*)bel->params_[BEL_RBEND_K1]->data;
      expr_struct *k2   = (expr_struct*)bel->params_[BEL_RBEND_K2]->data;
      expr_struct *k3   = (expr_struct*)bel->params_[BEL_RBEND_K3]->data;
      expr_struct *tilt = (expr_struct*)bel->params_[BEL_RBEND_TILT]->data;

      double e1 = expr_evaluate( bel->params_[BEL_RBEND_E1], var_table_, bel_table_ );
      double e2 = expr_evaluate( bel->params_[BEL_RBEND_E2], var_table_, bel_table_ ); 
    
      simple = (k1->kind_ == NUMBER_EXPR && 
                k1->dvalue_ == 0.0 && 
                k2->kind_ == NUMBER_EXPR && 
                k2->dvalue_ == 0.0 && 
                k3->kind_ == NUMBER_EXPR && 
                k3->dvalue_ == 0.0 && 
                tilt->kind_ == NUMBER_EXPR && 
                tilt->dvalue_ == 0.0);

      if( true == simple ) {
        if( (0.0 == e1) && (0.0 == e2) ) {
          lbel = new rbend( strip_final_colon( bel->name_ ).c_str(), length, BRHO_*(2.0*sin(0.5*angle))/length, (angle/2.0) );
	}
        else {
          lbel = new rbend( strip_final_colon( bel->name_ ).c_str(), length, BRHO_*(2.0*sin(0.5*angle))/length, (angle/2.0), e1, e2 );
        }
        if ( tilt->dvalue_ != 0.0 || tilt->kind_ != NUMBER_EXPR ) {
          aligner->xOffset = 0.0;
          aligner->yOffset = 0.0;
          aligner->tilt    = expr_evaluate( bel->params_[BEL_RBEND_TILT], var_table_, bel_table_ );
          lbel->setAlignment( *aligner );
        }
        // Ignored parameters: K1, K2, K3, E1, E2, H1, H2, HGAP, FINT
        break;
      }
      else {
        if( (0.0 == e1) && (0.0 == e2) ) {
          lbel = new CF_rbend( strip_final_colon( bel->name_ ).c_str(), length, BRHO_*(2.0*sin(0.5*angle))/length, (angle/2.0) );
	}
        else {
          lbel = new CF_rbend( strip_final_colon( bel->name_ ).c_str(), length, BRHO_*(2.0*sin(0.5*angle))/length, (angle/2.0), e1, e2 );
	}

        double multipoleStrength;

        multipoleStrength = expr_evaluate( bel->params_[BEL_RBEND_K1], var_table_, bel_table_ );
        multipoleStrength = multipoleStrength*BRHO_*length;
        if( multipoleStrength != 0.0 ) {
          ((CF_rbend*) lbel)->setQuadrupole( multipoleStrength );
	}

        multipoleStrength = expr_evaluate( bel->params_[BEL_RBEND_K2], var_table_, bel_table_ );
        multipoleStrength = multipoleStrength*BRHO_*length/2.0;
        if( multipoleStrength != 0.0 ) {
          ((CF_rbend*) lbel)->setSextupole( multipoleStrength );
	}

        multipoleStrength = expr_evaluate( bel->params_[BEL_RBEND_K3], var_table_, bel_table_ );
        multipoleStrength = multipoleStrength*BRHO_*length/6.0;
        if( multipoleStrength != 0.0 ) {
          ((CF_rbend*) lbel)->setOctupole( multipoleStrength );
	}
        break;
      }
    }
    case BEL_QUADRUPOLE: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double k1     = expr_evaluate( bel->params_[BEL_QUADRUPOLE_K1], var_table_, bel_table_ );
      
      if ( length != 0.0 || k1 != 0.0 ) {
        lbel = new quadrupole( strip_final_colon( bel->name_ ).c_str(), length, BRHO_*k1 );
      } else {
        lbel = new thinQuad( strip_final_colon( bel->name_ ).c_str(), length );
      }
      
      if ( ((expr_struct*)(bel->params_[BEL_QUADRUPOLE_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_QUADRUPOLE_TILT]->data))->kind_ != NUMBER_EXPR ) {
        aligner->xOffset = 0.0;
        aligner->yOffset = 0.0;
        aligner->tilt    = expr_evaluate( bel->params_[BEL_QUADRUPOLE_TILT], var_table_, bel_table_ );
        lbel->setAlignment( *aligner );
      }
      break;
    }
    case BEL_SEXTUPOLE: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double k2     = expr_evaluate( bel->params_[BEL_SEXTUPOLE_K2], var_table_, bel_table_ );
      
      if ( length != 0.0 || k2 != 0.0 ) {
        lbel = new sextupole( strip_final_colon( bel->name_ ).c_str(), length, BRHO_*k2/2.0 );
      } else {
        lbel = new thinSextupole( strip_final_colon( bel->name_ ).c_str(), length );
      }
      
      if ( ((expr_struct*)(bel->params_[BEL_SEXTUPOLE_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_SEXTUPOLE_TILT]->data))->kind_ != NUMBER_EXPR ) {
        aligner->xOffset = 0.0;
        aligner->yOffset = 0.0;
        aligner->tilt    = expr_evaluate( bel->params_[BEL_SEXTUPOLE_TILT], var_table_, bel_table_ );
        lbel->setAlignment( *aligner );
      }
      break;
    }
    case BEL_OCTUPOLE: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double k3     = expr_evaluate( bel->params_[BEL_OCTUPOLE_K3], var_table_, bel_table_ );
      
      if ( length != 0.0 || k3 != 0.0 ) {
        lbel = new octupole( strip_final_colon( bel->name_ ).c_str(), length, BRHO_*k3/6.0 );
      } else {
        lbel = new thinOctupole( strip_final_colon( bel->name_ ).c_str(), length );
      }
      
      if ( ((expr_struct*)(bel->params_[BEL_OCTUPOLE_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_OCTUPOLE_TILT]->data))->kind_ != NUMBER_EXPR ) {
        aligner->xOffset = 0.0;
        aligner->yOffset = 0.0;
        aligner->tilt    = expr_evaluate( bel->params_[BEL_OCTUPOLE_TILT], var_table_, bel_table_ );
        lbel->setAlignment( *aligner );
      }
      break;
    }
    case BEL_MULTIPOLE: {
      double roll     = 0.0;
      bmlnElmnt* q    = 0;
      beamline* temp  = new beamline( strip_final_colon( bel->name_ ).c_str() );
      temp->setEnergy( this->getEnergy() ); 

      double k0l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K0L], var_table_, bel_table_ );
      if( k0l != 0.0 ) {
        q = new thin2pole(BRHO_*k0l);
        roll = expr_evaluate( bel->params_[BEL_MULTIPOLE_T0], var_table_, bel_table_ );
        if( 0.0 != roll ) {
          aligner->xOffset = 0.0;
          aligner->yOffset = 0.0;
          aligner->tilt    = roll;
          q->setAlignment( *aligner );
	}
        temp->append( q );
      }

      double k1l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K1L], var_table_, bel_table_ );
      if( k1l != 0.0 ) {
        q = new thinQuad( BRHO_*k1l );
        roll = expr_evaluate( bel->params_[BEL_MULTIPOLE_T1], var_table_, bel_table_ );
        if( 0.0 != roll ) {
          aligner->xOffset = 0.0;
          aligner->yOffset = 0.0;
          aligner->tilt    = roll;
          q->setAlignment( *aligner );
	}
        temp->append( q );
      }
      
      double k2l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K2L], var_table_, bel_table_ );
      if( k2l != 0.0 ) {
        q = new thinSextupole( BRHO_*k2l/2.0 );
        roll = expr_evaluate( bel->params_[BEL_MULTIPOLE_T2], var_table_, bel_table_ );
        if( 0.0 != roll ) {
          aligner->xOffset = 0.0;
          aligner->yOffset = 0.0;
          aligner->tilt    = roll;
          q->setAlignment( *aligner );
	}
        temp->append( q );
      }
      
      double k3l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K3L], var_table_, bel_table_ );
      if( k3l != 0.0 ) {
        q = new thinOctupole( BRHO_*k3l/6.0 );
        roll = expr_evaluate( bel->params_[BEL_MULTIPOLE_T3], var_table_, bel_table_ );
        if( 0.0 != roll ) {
          aligner->xOffset = 0.0;
          aligner->yOffset = 0.0;
          aligner->tilt    = roll;
          q->setAlignment( *aligner );
	}
        temp->append( q );
      }
      
      double k4l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K4L], var_table_, bel_table_ );
      if( k4l != 0.0 ) {
        (*pcerr) << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K4L is being ignored.       "
             << "\n*** WARNING *** K4L = " << k4l << " for " <<  strip_final_colon( bel->name_ ).c_str()
             << "\n*** WARNING ***                                      "
             << endl;
      }

      double k5l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K5L], var_table_, bel_table_ );
      if( k5l != 0.0 ) {
        (*pcerr) << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K5L is being ignored.       "
             << "\n*** WARNING *** K5L = " << k5l << " for " <<  strip_final_colon( bel->name_ ).c_str()
             << "\n*** WARNING ***                                      "
             << endl;
      }

      double k6l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K6L], var_table_, bel_table_ );
      if( k6l != 0.0 ) {
        (*pcerr) << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K6L is being ignored.       "
             << "\n*** WARNING *** K6L = " << k6l << " for " <<  strip_final_colon( bel->name_ ).c_str()
             << "\n*** WARNING ***                                      "
             << endl;
      }

      double k7l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K7L], var_table_, bel_table_ );
      if( k7l != 0.0 ) {
        (*pcerr) << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K7L is being ignored.       "
             << "\n*** WARNING *** K7L = " << k7l << " for " <<  strip_final_colon( bel->name_ ).c_str()
             << "\n*** WARNING ***                                      "
             << endl;
      }

      double k8l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K8L], var_table_, bel_table_ );
      if( k8l != 0.0 ) {
        (*pcerr) << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K8L is being ignored.       "
             << "\n*** WARNING *** K8L = " << k8l << " for " <<  strip_final_colon( bel->name_ ).c_str()
             << "\n*** WARNING ***                                      "
             << endl;
      }

      double k9l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K9L], var_table_, bel_table_ );
      if( k9l != 0.0 ) {
        (*pcerr) << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K9L is being ignored.       "
             << "\n*** WARNING *** K9L = " << k9l << " for " <<  strip_final_colon( bel->name_ ).c_str()
             << "\n*** WARNING ***                                      "
             << endl;
      }

      int n = temp->howMany();

      if( 0 == n ) {
        lbel = new marker( strip_final_colon( bel->name_ ).c_str() );
        delete temp;
      }
      else if( 1 == n ) {
        lbel = temp->firstElement();
        lbel->Rename( strip_final_colon( bel->name_ ).c_str() );
        delete temp;  // This should not delete the element.
      }
      else {
        lbel = temp;
      }
      break;
    }
    case BEL_SOLENOID:
      lbel = make_solenoid( strip_final_colon( bel->name_ ).c_str(), expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_HKICKER: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double kck    = expr_evaluate( bel->params_[BEL_HKICKER_KICK], var_table_, bel_table_ );
      char   name[BEL_NAME_LENGTH];
      
      if ( length != 0.0 || kck != 0.0 ) {
        lbel = new hkick( strip_final_colon( bel->name_ ).c_str(), length, kck );
      } else {
        lbel = new hkick( strip_final_colon( bel->name_ ).c_str(), kck );
      }
      
      if ( ((expr_struct*)(bel->params_[BEL_HKICKER_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_HKICKER_TILT]->data))->kind_ != NUMBER_EXPR ) {
        aligner->xOffset = 0.0;
        aligner->yOffset = 0.0;
        aligner->tilt    = expr_evaluate( bel->params_[BEL_HKICKER_TILT], var_table_, bel_table_ );
        lbel->setAlignment( *aligner );
      }
      break;
    }
    case BEL_VKICKER: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double kck    = expr_evaluate( bel->params_[BEL_VKICKER_KICK], var_table_, bel_table_ );
      char   name[BEL_NAME_LENGTH];
      
      if ( length != 0.0 || kck != 0.0 ) {
        lbel = new vkick( strip_final_colon( bel->name_ ).c_str(), length, kck );
      } else {
        lbel = new vkick( strip_final_colon( bel->name_ ).c_str(), kck );
      }
      
      if ( ((expr_struct*)(bel->params_[BEL_VKICKER_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_VKICKER_TILT]->data))->kind_ != NUMBER_EXPR ) {
        aligner->xOffset = 0.0;
        aligner->yOffset = 0.0;
        aligner->tilt    = expr_evaluate( bel->params_[BEL_VKICKER_TILT], var_table_, bel_table_ );
        lbel->setAlignment( *aligner );
      }
      break;
    }
    case BEL_KICKER: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double hkck   = expr_evaluate( bel->params_[BEL_KICKER_HKICK], var_table_, bel_table_ );
      double vkck   = expr_evaluate( bel->params_[BEL_KICKER_VKICK], var_table_, bel_table_ );
      char   name[BEL_NAME_LENGTH];
      
      if ( length != 0.0 || hkck != 0.0 || vkck != 0.0 ) {
        lbel = new kick( strip_final_colon( bel->name_ ).c_str(), length, hkck, vkck );
      } else {
        lbel = new kick( strip_final_colon( bel->name_ ).c_str(), hkck, vkck );
      }
      
      if ( ((expr_struct*)(bel->params_[BEL_KICKER_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_KICKER_TILT]->data))->kind_ != NUMBER_EXPR ) {
        aligner->xOffset = 0.0;
        aligner->yOffset = 0.0;
        aligner->tilt    = expr_evaluate( bel->params_[BEL_KICKER_TILT], var_table_, bel_table_ );
        lbel->setAlignment( *aligner );
      }
      break;
    }
    case BEL_RFCAVITY: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double harmon = expr_evaluate( bel->params_[BEL_RFCAVITY_HARMON], var_table_, bel_table_ );
      double volt   = expr_evaluate( bel->params_[BEL_RFCAVITY_VOLT], var_table_, bel_table_ );
      double lag    = expr_evaluate( bel->params_[BEL_RFCAVITY_LAG], var_table_, bel_table_ );
      double shunt  = expr_evaluate( bel->params_[BEL_RFCAVITY_SHUNT], var_table_, bel_table_ );
      // REMOVE: char   name[BEL_NAME_LENGTH];
      
      // REMOVE: lbel = new beamline( strip_final_colon( bel->name_ ).c_str() );
      // REMOVE: // NOTE: I cannot set the frequency of the rf cavity from harmonic number
      // REMOVE: // until the revolution frequency is known; i.e. until the beamline
      // REMOVE: // model is instantiated.  Thus, the rf cavity must be completed
      // REMOVE: // by a registration visitor before being used.
      // REMOVE: thinrfcavity* rfcPtr = new thinrfcavity( add_str( name, strip_final_colon( bel->name_ ).c_str(), "center" ), 0.0, volt*1.0e6, lag*2*M_PI, 0.0, shunt );
      // REMOVE: rfcPtr->setHarmonicNumber( harmon );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, strip_final_colon( bel->name_ ).c_str(), "left" ), length/2.0 ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*) rfcPtr );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, strip_final_colon( bel->name_ ).c_str(), "right" ), length/2.0 ) );
      
      lbel = new rfcavity( strip_final_colon( bel->name_ ).c_str(), 
                           length, 0, volt*1.0e6, lag*(2.0*M_PI), 0, shunt );
      // ......................................................................
      // NOTE: I cannot set the frequency of the rf cavity from harmonic number
      // until the revolution frequency is known; i.e. until the beamline
      // model is instantiated.  Thus, the rf cavity must be completed
      // by a registration visitor before being used.
      // ......................................................................

      // Ignored parameters: BETRF, PG, TFILL
      
      break;
    }
    case BEL_ELSEPARATOR: {
      // lbel = make_elseparator( strip_final_colon( bel->name_ ).c_str(), expr_evaluate( bel->length_, var_table_, bel_table_ ) );

      // According to the MAD manual, field is in MV/m
      // Separator strengths are in MV/m.
      // approximate bend angle is theta(ur) = E_MAD(MV/m)*2.57175(m)/E(Tev)

      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double field  = expr_evaluate( bel->params_[BEL_ELSEPARATOR_E], var_table_, bel_table_ );

      double momentum = PH_CNV_brho_to_p*BRHO_;
      double energy   = this->getEnergy();
      double angle    = 0.001 * (field*length/momentum) * (energy/momentum);

      lbel = new vkick( strip_final_colon( bel->name_ ).c_str(), length, angle );

      // Original code:. 

      //      if ( ((expr_struct*)(bel->params_[BEL_ELSEPARATOR_TILT]->data))->dvalue_ != 0.0 
      //        || ((expr_struct*)(bel->params_[BEL_ELSEPARATOR_TILT]->data))->kind_ != NUMBER_EXPR ) {
      //
      //  aligner->xOffset = 0.0;
      //  aligner->yOffset = 0.0;
      //  aligner->tilt    = expr_evaluate( bel->params_[BEL_ELSEPARATOR_TILT], var_table_, bel_table_ );
      //  lbel->setAlignment( *aligner );
      // }

      // It seems that under some circumstances, dvalue_ is unitialized when the above test
      // is performed. If dvalue_ contains garbage,the separator tilt will be set to 
      // a nonsensical value.  

      // As a workaround, the test is now performed in two separate steps. 
      // The exact source of the problem remains to be identified. 
      // dvalue_ should *always* default to
      // a well-defined value for any unused parameter. -jfo

      if ( ((expr_struct*)(bel->params_[BEL_ELSEPARATOR_TILT]->data))->kind_ != NUMBER_EXPR ) {    
           aligner->xOffset = 0.0;
           aligner->yOffset = 0.0;
           aligner->tilt    = expr_evaluate( bel->params_[BEL_ELSEPARATOR_TILT], var_table_, bel_table_ );
           lbel->setAlignment( *aligner );

      } else if ( ((expr_struct*)(bel->params_[BEL_ELSEPARATOR_TILT]->data))->dvalue_ != 0.0 ) {
           aligner->xOffset = 0.0;
           aligner->yOffset = 0.0;
           aligner->tilt    = expr_evaluate( bel->params_[BEL_ELSEPARATOR_TILT], var_table_, bel_table_ );
           lbel->setAlignment( *aligner );
      }

      break;
    }
    case BEL_HMONITOR: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      // REMOVE: char   name[BEL_NAME_LENGTH];
      
      if( length > 0.0 ) {
        lbel = new hmonitor( strip_final_colon( bel->name_ ).c_str(), length );
      }
      else {
        lbel = new hmonitor( strip_final_colon( bel->name_ ).c_str() );
      }

      // REMOVE: lbel = new beamline( strip_final_colon( bel->name_ ).c_str() );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, strip_final_colon( bel->name_ ).c_str(), "left" ), length/2.0 ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new hmonitor( add_str( name, strip_final_colon( bel->name_ ).c_str(), "center" ) ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, strip_final_colon( bel->name_ ).c_str(), "right" ), length/2.0 ) );
      
      break;
    }
    case BEL_VMONITOR: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      // REMOVE: char   name[BEL_NAME_LENGTH];
      
      if( length > 0.0 ) {
        lbel = new vmonitor( strip_final_colon( bel->name_ ).c_str(), length );
      }
      else {
        lbel = new vmonitor( strip_final_colon( bel->name_ ).c_str() );
      }

      // REMOVE: lbel = new beamline( strip_final_colon( bel->name_ ).c_str() );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, strip_final_colon( bel->name_ ).c_str(), "left" ), length/2.0 ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new vmonitor( add_str( name, strip_final_colon( bel->name_ ).c_str(), "center" ) ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, strip_final_colon( bel->name_ ).c_str(), "right" ), length/2.0 ) );
      
      break;
    }
    case BEL_MONITOR: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      // REMOVE: char   name[BEL_NAME_LENGTH];
      
      if( length > 0.0 ) {
        lbel = new monitor( strip_final_colon( bel->name_ ).c_str(), length );
      }
      else {
        lbel = new monitor( strip_final_colon( bel->name_ ).c_str() );
      }

      // REMOVE: lbel = new beamline( strip_final_colon( bel->name_ ).c_str() );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, strip_final_colon( bel->name_ ).c_str(), "left" ), length/2.0 ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new monitor( add_str( name, strip_final_colon( bel->name_ ).c_str(), "center" ) ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, strip_final_colon( bel->name_ ).c_str(), "right" ), length/2.0 ) );
      
      break;
    }
    case BEL_INSTRUMENT:
      lbel = make_instrument( strip_final_colon( bel->name_ ).c_str(), expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_ECOLLIMATOR:
      lbel = make_ecollimator( strip_final_colon( bel->name_ ).c_str(), expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_RCOLLIMATOR:
      lbel = make_rcollimator( strip_final_colon( bel->name_ ).c_str(), expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_YROT:
      lbel = make_yrot( strip_final_colon( bel->name_ ).c_str(), expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_SROT: {
      double angle = expr_evaluate( bel->params_[BEL_SROT_ANGLE], var_table_, bel_table_ );
      lbel = new srot( strip_final_colon( bel->name_ ).c_str(), angle );
      break;
    }
    case BEL_BEAMBEAM:
      lbel = make_beambeam( strip_final_colon( bel->name_ ).c_str(), expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_MATRIX:
      lbel = make_matrix( strip_final_colon( bel->name_ ).c_str(), expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_LUMP:
      lbel = make_lump( strip_final_colon( bel->name_ ).c_str(), expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    default:
      (*pcerr) << "Unknown beam element." << endl;
  }
  
  bel_pair p;
  p.bel_ptr_  = bel;
  p.lbel_ptr_ = lbel;
  bel_list_->push_back( p );
  
  delete aligner;

  _bmlnElmnt_objects_list.push_back(lbel);

  return lbel;
}

bmlnElmnt*
bmlfactory::find_beam_element( char* ptr ) {
  
  bel_pair_list__t::iterator res = find_if( bel_list_->begin(),
                                            bel_list_->end(),
                                            bind2nd( bel_predicate(), ptr ) );
  if ( res != bel_list_->end() ) {
    return res->lbel_ptr_;
  } else {
    return NULL;
  }
}

void
bmlfactory::create_bel_list() {
  bel_list_ = new bel_pair_list__t();
}

void
bmlfactory::delete_bel_list() {
  for( bel_pair_list__t::iterator i = bel_list_->begin();
       i != bel_list_->end(); ++i ) {
    delete i->lbel_ptr_;
  }
  delete bel_list_;
}

beamline*
bmlfactory::beam_line_instantiate( beam_line* bml ) {
  
  beamline* lbml = find_beam_line( bml->name_ );
  if ( lbml == NULL ) {
    lbml = new beamline( bml->name_ );
    
    for ( GList* listptr = bml->bel_list_;
          listptr != NULL; listptr = g_list_next( listptr ) ) {
      append_bml_element( (char*)(listptr->data), lbml );
    }
    
    bml_pair p;
    p.bml_ptr_  = bml;
    p.lbml_ptr_ = lbml;
    
    bml_list_->push_back( p );

  }
  
  lbml->setEnergy( this->getEnergy() ); 

  _beamline_objects_list.push_back(lbml);

  return lbml;
}

beamline*
bmlfactory::find_beam_line( char* ptr ) {
  
  bml_pair_list__t::iterator res = find_if( bml_list_->begin(),
                                            bml_list_->end(),
                                            bind2nd( bml_predicate(), ptr ) );
  if ( res != bml_list_->end() ) {
    return res->lbml_ptr_;
  } else {
    return NULL;
  }
}

void
bmlfactory::append_bml_element( char* ptr, beamline* lbml ) {

     // search in the beam element table
  int i;
  char label[BEL_NAME_LENGTH];
  for ( i = 0; i < bel_arr_size_; ++i ) {
    beam_element_get_label( *(bel_arr_+i), label );
    if ( strcasecmp( label, ptr+1 ) == 0 ) {
      break;
    }
  }
  if ( i < bel_arr_size_ ) {
    
    lbml->append( beam_element_instantiate( *(bel_arr_+i) ) );

  } else {
       // create the beam line, if it is the table
    beamline* subbml = create_beamline_private( ptr+1, BRHO_ );
    if ( subbml != NULL ) {
      if ( *ptr == '-' ) {
        lbml->append( -(*subbml) );
      } else {
        lbml->append( subbml );
      }
    } else {
         //
         // if nothing to append then break
         //
      (*pcerr) << "Beam line element \"" << ptr+1 << "\" not found." << endl;
    }
  }
}

void
bmlfactory::create_bml_list() {
  bml_list_ = new bml_pair_list__t();
}

void
bmlfactory::delete_bml_list() {
  for( bml_pair_list__t::iterator i = bml_list_->begin();
       i != bml_list_->end(); ++i ) {
    delete i->lbml_ptr_;
  }
  delete bml_list_;
}



std::list<std::string>& 
bmlfactory::getBeamlineList() {

  beam_line** bml_arr_ptr = bml_arr_;
  
  for (int i = 0; i< bml_arr_size_; i++) {
    
    _beamline_identifiers_list.push_back( std::string( (*(bml_arr_ptr++))->name_ ) );
   
  };

  return _beamline_identifiers_list;

}


double
bmlfactory::getVariableValue(const char* var_name) const
{
   
  double value;  

  char* tmp_name = new char[ strlen(var_name)+ 1 ];
  strcpy(tmp_name, var_name);
  variable* var = (variable*) var_table_lookup( tmp_name, var_table_ );
  return expr_evaluate( var->expr_, var_table_, bel_table_ );

  delete[] tmp_name;

}

bool  
bmlfactory::variableIsDefined(const char* var_name) const
{
 
  char* tmp_name = new char[ strlen(var_name)+ 1 ]; 
  strcpy(tmp_name, var_name);
  if  ( variable* var = (variable*) var_table_lookup( tmp_name, var_table_ ) )
    return true; 
  else
    return false;
  delete[] tmp_name;
  
}

const char* 
bmlfactory::getUseStatementBeamlineName() {

  return madparser_get_use_statement_beamline_name( mp ); 

}


double bmlfactory::getEnergy() const
{
  double ret = 0.0;
  double momentum = (this->BRHO_)*PH_CNV_brho_to_p;
  if( 0 == strcmp("PROTON",this->getParticleType()) ) {
    ret = sqrt( momentum*momentum + PH_NORM_mp*PH_NORM_mp );
  }
  else if( 0 == strcmp("POSITRON",this->getParticleType()) ) {
    ret = sqrt( momentum*momentum + PH_NORM_me*PH_NORM_me );
  }
  else {
    (*pcerr) << "\n*** ERROR *** "
                "\n*** ERROR *** bmlfactory is not programmed to handle"
                "\n*** ERROR *** particle type "
             << this->getParticleType()
             << "\n*** ERROR *** "
             << endl;
    ostringstream uic;
    uic  <<   "bmlfactory is not programmed to handle"
              "\nparticle type "
         << this->getParticleType();
    throw( GenericException( __FILE__, __LINE__, 
           "double bmlfactory::getEnergy() const", 
           uic.str().c_str() ) );
  }
  return ret;
}


const char* bmlfactory::getParticleType() const 
{
  return madparser_get_particle_type( mp );
}


double 
bmlfactory::getBrho( ) const {

  return madparser_get_brho( mp ); 

}

