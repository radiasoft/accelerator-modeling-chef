/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                 
******                                                
******  File:      bmlfactory.cc
******  Version:   1.6
******                                                                
******  Copyright (c) 1999  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Authors:    Dmitri Mokhov and Oleg Krivosheev                  
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
******  Usage, modification, and redistribution are subject to terms    
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
**************************************************************************
* Revision History: 
*
*  Version 1.5: August 2004 
*
*  ostiguy@fnal.gov: 
*
*  -Numerous and **severe** memory leaks eliminated. 
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

#include <iostream>
using namespace std;

#undef allocate
#undef deallocate

#include <functional>

   // from MXYZPLTK
#include <beamline.h>

#if !defined(beam_element_h)
#include "beam_element.h"
#endif /* beam_element_h */

#if !defined(beamel_table_h)
#include "beamel_table.h"
#endif /* beamel_table_h */

#if !defined(beam_line_h)
#include "beam_line.h"
#endif /* beam_line_h */

#if !defined(bml_table_h)
#include "bml_table.h"
#endif /* bml_table_h */

#if !defined(bmlfactory_h)
#include "bmlfactory.h"
#endif /* bmlfactory_h */

#if !defined(expression_h)
#include "expression.h"
#endif /* expression_h */

#if !defined(madparser_h)
#include "madparser.h"
#endif /* madparser_h */

#include "var_table.h"

#include "bel_inst_fns.h"

#include <GenericException.h>

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>


extern struct madparser_* mp;

using namespace std;

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

  bel_arr_size_ = bel_table_to_array( &bel_arr_, bel_table_ );        /* bel_arr is allocated here */
  qsort( bel_arr_, bel_arr_size_, sizeof(beam_element*), bel_compare );
  
  bml_arr_size_ = bml_table_to_array( &bml_arr_, madparser_bml_table( mp_ ) ); /* bml_arr is allocated here */
  if ( bml_arr_size_ > 1 ) {
    qsort( bml_arr_, bml_arr_size_, sizeof(beam_line*), bml_compare );    
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

  // the code below is for debugging only

#if 0 
  for ( std::list<beamline*>::iterator it=_beamline_objects_list.begin(); 
        it != _beamline_objects_list.end() );
        it++) 
  {

  };

  for ( std::list<bmlnElmnt*>::iterator it=_bmlnElmnt_objects_list.begin(); 
        it != _bmlnElmnt_objects_list.end() );
        it++) 
  {

  };

#endif

}

// Deprecated function 

beamline* 
bmlfactory::create_beamline( const char* bmlname) {
  
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



// Deprecated function 

beamline*
bmlfactory::create_beamline_private( const char* bmlname) {

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
    cerr << "Beam line \"" << bmlname << "\" not found." << endl;
    return NULL;
  } else {
    bml_ptr = (beamline*)(beam_line_instantiate( *(bml_arr_+i) ));
    return bml_ptr;
  }
}

static char*
add_str( char* ptr, char* s1, char* s2 ) {
  strcpy( ptr, s1 );
  return strcat( ptr, s2 );
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
      lbel = new drift( bel->name_, length );
      break;
    }
    case BEL_MARKER: {
      lbel = new marker( bel->name_ );
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
      
      simple = (k1->kind_ == NUMBER_EXPR && k1->dvalue_ == 0.0 && k2->kind_ == NUMBER_EXPR && k2->dvalue_ == 0.0 && k3->kind_ == NUMBER_EXPR && k3->dvalue_ == 0.0 && tilt->kind_ == NUMBER_EXPR && tilt->dvalue_ == 0.0);

      if( true == simple ) {
        lbel = new sbend( bel->name_, length, BRHO_*angle/length, angle );
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
        lbel = new CF_sbend( bel->name_, length, BRHO_*angle/length, angle );

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
      
      simple = (k1->kind_ == NUMBER_EXPR && k1->dvalue_ == 0.0 && k2->kind_ == NUMBER_EXPR && k2->dvalue_ == 0.0 && k3->kind_ == NUMBER_EXPR && k3->dvalue_ == 0.0 && tilt->kind_ == NUMBER_EXPR && tilt->dvalue_ == 0.0);

      if( true == simple ) {
        lbel = new rbend( bel->name_, length, BRHO_*(2.0*sin(0.5*angle))/length, (angle/2.0) );
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
        lbel = new CF_rbend( bel->name_, length, BRHO_*(2.0*sin(0.5*angle))/length, (angle/2.0) );

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
        lbel = new quadrupole( bel->name_, length, BRHO_*k1 );
      } else {
        lbel = new thinQuad( bel->name_, length );
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
        lbel = new sextupole( bel->name_, length, BRHO_*k2/2.0 );
      } else {
        lbel = new thinSextupole( bel->name_, length );
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
        lbel = new octupole( bel->name_, length, BRHO_*k3/6.0 );
      } else {
        lbel = new thinOctupole( bel->name_, length );
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
      double momentum = PH_CNV_brho_to_p*BRHO_;
      double energy   = sqrt( momentum*momentum + PH_NORM_mp*PH_NORM_mp );
      bmlnElmnt* q    = 0;
      beamline* temp  = new beamline( bel->name_ );
      temp->setEnergy( energy );

      double k0l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K0L], var_table_, bel_table_ );
      if( k0l != 0.0 ) {
        cerr << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K0L is being ignored.       "
             << "\n*** WARNING *** K0L = " << k0l << " for " <<  bel->name_
             << "\n*** WARNING ***                                      "
             << endl;
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
        cerr << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K4L is being ignored.       "
             << "\n*** WARNING *** K4L = " << k4l << " for " <<  bel->name_
             << "\n*** WARNING ***                                      "
             << endl;
      }

      double k5l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K5L], var_table_, bel_table_ );
      if( k5l != 0.0 ) {
        cerr << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K5L is being ignored.       "
             << "\n*** WARNING *** K5L = " << k5l << " for " <<  bel->name_
             << "\n*** WARNING ***                                      "
             << endl;
      }

      double k6l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K6L], var_table_, bel_table_ );
      if( k6l != 0.0 ) {
        cerr << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K6L is being ignored.       "
             << "\n*** WARNING *** K6L = " << k6l << " for " <<  bel->name_
             << "\n*** WARNING ***                                      "
             << endl;
      }

      double k7l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K7L], var_table_, bel_table_ );
      if( k7l != 0.0 ) {
        cerr << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K7L is being ignored.       "
             << "\n*** WARNING *** K7L = " << k7l << " for " <<  bel->name_
             << "\n*** WARNING ***                                      "
             << endl;
      }

      double k8l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K8L], var_table_, bel_table_ );
      if( k8l != 0.0 ) {
        cerr << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K8L is being ignored.       "
             << "\n*** WARNING *** K8L = " << k8l << " for " <<  bel->name_
             << "\n*** WARNING ***                                      "
             << endl;
      }

      double k9l    = expr_evaluate( bel->params_[BEL_MULTIPOLE_K9L], var_table_, bel_table_ );
      if( k9l != 0.0 ) {
        cerr << "\n*** WARNING *** " << __FILE__ << ", " << __LINE__ << ": "
             << "\n*** WARNING *** Value of K9L is being ignored.       "
             << "\n*** WARNING *** K9L = " << k9l << " for " <<  bel->name_
             << "\n*** WARNING ***                                      "
             << endl;
      }

      int n = temp->howMany();

      if( 0 == n ) {
        lbel = new marker( bel->name_ );
        delete temp;
      }
      else if( 1 == n ) {
        lbel = temp->firstElement();
        lbel->Rename( bel->name_ );
        delete temp;  // This should not delete the element.
      }
      else {
        lbel = temp;
      }
      break;
    }
    case BEL_SOLENOID:
      lbel = make_solenoid( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_HKICKER: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double kck    = expr_evaluate( bel->params_[BEL_HKICKER_KICK], var_table_, bel_table_ );
      char   name[BEL_NAME_LENGTH];
      
      if ( length != 0.0 || kck != 0.0 ) {
        lbel = new hkick( bel->name_, length, kck );
      } else {
        lbel = new hkick( bel->name_, kck );
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
        lbel = new vkick( bel->name_, length, kck );
      } else {
        lbel = new vkick( bel->name_, kck );
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
        lbel = new kick( bel->name_, length, hkck, vkck );
      } else {
        lbel = new kick( bel->name_, hkck, vkck );
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
      char   name[BEL_NAME_LENGTH];
      
      lbel = new beamline( bel->name_ );
      ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "left" ), length/2.0 ) );
      ((beamline*)lbel)->append( (bmlnElmnt*)new thinrfcavity( add_str( name, bel->name_, "center" ), harmon, volt*1.0e6, lag*2*M_PI, 0.0, shunt ) );
      ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "right" ), length/2.0 ) );
      
         // Ignored parameters: BETRF, PG, TFILL
      
      break;
    }
    case BEL_ELSEPARATOR: {
      // lbel = make_elseparator( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );

      // According to the MAD manual, field is in MV/m
      // Separator strengths are in MV/m.
      // approximate bend angle is theta(ur) = E_MAD(MV/m)*2.57175(m)/E(Tev)

      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double field  = expr_evaluate( bel->params_[BEL_ELSEPARATOR_E], var_table_, bel_table_ );

      double momentum = PH_CNV_brho_to_p*BRHO_;
      double energy   = sqrt( momentum*momentum + PH_NORM_mp*PH_NORM_mp );
      double angle    = 0.001 * (field*length/momentum) * (energy/momentum);

      lbel = new vkick( bel->name_, length, angle );

      if ( ((expr_struct*)(bel->params_[BEL_ELSEPARATOR_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_ELSEPARATOR_TILT]->data))->kind_ != NUMBER_EXPR ) {
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
        lbel = new hmonitor( bel->name_, length );
      }
      else {
        lbel = new hmonitor( bel->name_ );
      }

      // REMOVE: lbel = new beamline( bel->name_ );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "left" ), length/2.0 ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new hmonitor( add_str( name, bel->name_, "center" ) ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "right" ), length/2.0 ) );
      
      break;
    }
    case BEL_VMONITOR: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      // REMOVE: char   name[BEL_NAME_LENGTH];
      
      if( length > 0.0 ) {
        lbel = new vmonitor( bel->name_, length );
      }
      else {
        lbel = new vmonitor( bel->name_ );
      }

      // REMOVE: lbel = new beamline( bel->name_ );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "left" ), length/2.0 ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new vmonitor( add_str( name, bel->name_, "center" ) ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "right" ), length/2.0 ) );
      
      break;
    }
    case BEL_MONITOR: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      // REMOVE: char   name[BEL_NAME_LENGTH];
      
      if( length > 0.0 ) {
        lbel = new monitor( bel->name_, length );
      }
      else {
        lbel = new monitor( bel->name_ );
      }

      // REMOVE: lbel = new beamline( bel->name_ );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "left" ), length/2.0 ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new monitor( add_str( name, bel->name_, "center" ) ) );
      // REMOVE: ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "right" ), length/2.0 ) );
      
      break;
    }
    case BEL_INSTRUMENT:
      lbel = make_instrument( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_ECOLLIMATOR:
      lbel = make_ecollimator( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_RCOLLIMATOR:
      lbel = make_rcollimator( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_YROT:
      lbel = make_yrot( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_SROT: {
      double angle = expr_evaluate( bel->params_[BEL_SROT_ANGLE], var_table_, bel_table_ );
      lbel = new srot( bel->name_, angle );
      break;
    }
    case BEL_BEAMBEAM:
      lbel = make_beambeam( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_MATRIX:
      lbel = make_matrix( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_LUMP:
      lbel = make_lump( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    default:
      cerr << "Unknown beam element." << endl;
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
  
  double energy;
  energy = PH_CNV_brho_to_p*BRHO_;
  energy = energy*energy + PH_NORM_mp*PH_NORM_mp;
  energy = sqrt( energy );
  lbml->setEnergy( energy );

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
      cerr << "Beam line element \"" << ptr+1 << "\" not found." << endl;
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
