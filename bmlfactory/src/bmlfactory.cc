/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      bmlfactory.cc
******                                                                
******  Copyright (c) 1999  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Dmitri Mokhov and Oleg Krivosheev                  
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
*************************************************************************/


#include <algorithm>
#include <assert.h>
#include <cmath>
#include <functional>
#include <iostream>
#include <list>

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

#include "bel_inst_fns.c"

extern struct madparser_* mp;

bmlfactory::bmlfactory( const char* fname,
                        double BRHO ) {
  
  BRHO_ = BRHO;
  mp_ = NULL;
  fname_ = NULL;
  
  assert( fname != NULL );
  
  int length = strlen( fname );
  assert( length > 0 );
  
  fname_ = new char[length+1];
  assert( fname_ != NULL );
  strcpy( fname_, fname );
  
  mp_ = madparser_init( fname_, NULL );
  assert( mp_ != NULL );
  
  mp = mp_; // set the global variable mp to point to the same parser structure as mp_
  madparser_parse( mp_ );
  
  var_table_ = madparser_var_table( mp_ );
  bel_table_ = madparser_bel_table( mp_ );
  
  bel_arr_size_ = bel_table_to_array( &bel_arr_, bel_table_ );
  qsort( bel_arr_, bel_arr_size_, sizeof(beam_element*), bel_compare );
  
  bml_arr_size_ = bml_table_to_array( &bml_arr_, madparser_bml_table( mp_ ) );
  if ( bml_arr_size_ > 1 ) {
    qsort( bml_arr_, bml_arr_size_, sizeof(beam_line*), bml_compare );    
    bml_remove_forward( &bml_arr_, bml_arr_size_, madparser_bml_table( mp_ ) ); // When removing forward references, line order isn't preserved
  }
  
  create_bel_list();
  create_bml_list();
}

bmlfactory::~bmlfactory() {
  madparser_delete( mp_ );
  mp_ = NULL;
  mp  = NULL;
  delete_bel_list();
  delete_bml_list();
  delete fname_;
}

beamline*
bmlfactory::create_beamline( const char* bmlname ) {
  
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
    return (beamline*)(beam_line_instantiate( *(bml_arr_+i) )->Clone());
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
        lbel = new rbend( bel->name_, length, BRHO_*(2.0*sin(0.5*angle))/length );
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
    case BEL_MULTIPOLE:
      lbel = make_multipole( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_SOLENOID:
      lbel = make_solenoid( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
    case BEL_HKICKER: {
      double length = expr_evaluate( bel->length_, var_table_, bel_table_ );
      double kck    = expr_evaluate( bel->params_[BEL_HKICKER_KICK], var_table_, bel_table_ );
      char   name[BEL_NAME_LENGTH];
      
      lbel = new beamline( bel->name_ );
      ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "left" ), length/2.0 ) );
      ((beamline*)lbel)->append( (bmlnElmnt*)new hkick( add_str( name, bel->name_, "center" ), kck ) );
      ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "right" ), length/2.0 ) );
      
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
      
      lbel = new beamline( bel->name_ );
      ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "left" ), length/2.0 ) );
      ((beamline*)lbel)->append( (bmlnElmnt*)new vkick( add_str( name, bel->name_, "center" ), kck ) );
      ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "right" ), length/2.0 ) );
      
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
      
      lbel = new beamline( bel->name_ );
      ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "left" ), length/2.0 ) );
      ((beamline*)lbel)->append( (bmlnElmnt*)new hkick( add_str( name, bel->name_, "center_hkick" ), hkck ) );
      ((beamline*)lbel)->append( (bmlnElmnt*)new vkick( add_str( name, bel->name_, "center_vkick" ), vkck ) );
      ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "right" ), length/2.0 ) );
      
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
      ((beamline*)lbel)->append( (bmlnElmnt*)new thinrfcavity( add_str( name, bel->name_, "center" ), harmon, volt*1.0e6*1.602177e-19, lag*2*M_PI, 0.0, shunt ) );
      ((beamline*)lbel)->append( (bmlnElmnt*)new drift( add_str( name, bel->name_, "right" ), length/2.0 ) );
      
         // Ignored parameters: BETRF, PG, TFILL
      
      break;
    }
    case BEL_ELSEPARATOR:
      lbel = make_elseparator( bel->name_, expr_evaluate( bel->length_, var_table_, bel_table_ ) );
      break;
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
    beamline* subbml = create_beamline( ptr+1 );
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
