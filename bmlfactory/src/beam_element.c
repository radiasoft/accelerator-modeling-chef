#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      beam_element.c
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


/* -*- C -*- */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(beam_element_h)
#include "beam_element.h"
#endif /* beam_element_h */

#if !defined(expression_h)
#include "expression.h"
#endif /* expression_h */

   /*
     Takes a bel name, manipulates it, and returns the result. Used for outputting
     beam elements with desired names.
   */
static const char*
beam_element_make_name( const char* name ) {
  return name;
}

   /*
     Takes a bel and initializes its length using expr_node_init from expression.[hc]
   */
static void
beam_element_init_length( beam_element* bel,
                          fb_allocator* expr_alloc ) {
  
  bel->length_ = expr_node_init( 0.0, "0.", expr_alloc );
  assert( bel->length_ != NULL );
}

   /*
     Takes a bel and initializes its parameters using expr_node_init from expression.[hc]
   */
static void
beam_element_init_params( beam_element* bel,
                          fb_allocator* expr_alloc ) {
  
  int i;
  for ( i = 0; i < BEL_NOF_PARAMS; ++i ) {
    bel->params_[i] = expr_node_init( 0.0, "0.", expr_alloc );
    assert( bel->params_[i] != NULL );
  }
}

   /*
     Takes a pointer to bel, allocates the bel, initializes its length and parameters,
     and returns 0
   */
beam_element*
beam_element_init( fb_allocator* expr_alloc,
                   fb_allocator* bel_alloc ) {
  beam_element* bel;

  assert( bel_alloc != NULL );
  assert( expr_alloc != NULL );
  
  PRIVATE_ALLOCATE( bel, bel_alloc );
  if ( bel != NULL ) {
    memset( bel, 0, sizeof( beam_element ) );

    beam_element_init_length( bel, expr_alloc );
    beam_element_init_params( bel, expr_alloc );
  }

  return bel;
}

   /*
     Takes a beam element and a kind, and sets the bel's kind to the given kind
    */
void
beam_element_set_kind( beam_element* bel,
                       int           kind ) {
  assert( bel != NULL );

  bel->kind_ = kind;
}

   /*
     Takes a bel, a name, and a type, and sets the bel's name to "given_name:given_type"
   */
void
beam_element_set_name( beam_element* bel,
                       const char*   name,
                       const char*   type ) {
  assert( bel != NULL );
  assert( strlen( name ) + 1 + strlen( type ) < BEL_NAME_LENGTH );

  strcpy( bel->name_, name );
  strcat( bel->name_, ":" );
  strcat( bel->name_, type );
}

   /*
     Takes a bel and a length, and sets the bel's length to the given length
   */
void
beam_element_set_length( beam_element* bel,
                         GNode*        length,
                         fb_allocator* expr_alloc  ) {
  assert( bel != NULL );

  expr_node_delete( bel->length_, expr_alloc );
  bel->length_ = length;
}

   /*
     Takes a bel, a parameter index, and a value, and sets the bel's parameter at that
     index to the given value
   */
void
beam_element_set_param( beam_element* bel,
                        int           param_idx,
                        GNode*        value ) {
  assert( bel != NULL );
  assert( param_idx >= 0 );
  assert( param_idx < BEL_NOF_PARAMS );

  bel->params_[param_idx] = value;
}

   /*
     set user-defined data for given beam element
   */
void
beam_element_set_udata( beam_element* bel,
                        void*         udata ) {
  assert( bel != NULL );
  
  bel->udata_ = udata;
}

   /*
     Takes a bel and a line number, and sets the bel's linenum to the given value
   */
void
beam_element_set_linenum( beam_element* bel,
                          int           linenum,
                          const char*   filename,
                          int           local_linenum ) {
  assert( bel != NULL );

  bel->linenum_       = linenum;
  bel->filename_      = filename;
  bel->local_linenum_ = local_linenum;
}

   /*
     Takes a bel, deallocates the memory given to it, and returns 0
   */
int
beam_element_delete( beam_element* bel, fb_allocator* bel_alloc ) {
  PRIVATE_DEALLOCATE( bel, bel_alloc );
  return 0;
}

   /*
     Takes a bel, allocates a new bel, copies the contents of the original bel 
     to the new bel, and returns a pointer to the new bel
   */
beam_element*
beam_element_copy( beam_element* bel, fb_allocator* bel_alloc ) {

  beam_element* new_bel = NULL;
  assert( bel != NULL );

  PRIVATE_ALLOCATE( new_bel, bel_alloc );
  memcpy( new_bel, bel, sizeof(beam_element) );

  return new_bel;
}

   /*
     Takes a bel and a char pointer "label", and copies the contents of
     the bel's name, up to the ':' separator, to where "label" points
   */
void
beam_element_get_label( beam_element* bel, char* label ) {
  char *colon = (char*)memccpy( label, bel->name_, ':', BEL_NAME_LENGTH );
  *(--colon) = '\0';
}

   /*
     Takes char pointers "label" and "name" and an expression "length", and 
     outputs constructors of drifts with lengths equal to half of "length"  
     and of a beamline that consists of these 2 drifts and of an element    
     called "<label>_center" located in between them
   */
static void
beam_element_plus_drift( FILE*       out,
                         char*       label,
                         const char* name,
                         GNode*      length,
                         GHashTable* var_table,
                         GHashTable* bel_table ) {
  
  fprintf( out, "drift %s_left ( (char*)\"%s:left\", (", label, name );
  expr_display( out, length, var_table, bel_table );
  fprintf( out, ")/2.0 );\ndrift %s_right ( (char*)\"%s:right\", (", label, name );
  expr_display( out, length, var_table, bel_table );
  fprintf( out, ")/2.0 );\nbeamline %s ( \"%s\" );\n         %s.append( %s_left );\n", label, name, label, label);
  fprintf( out, "         %s.append( %s_center );\n         %s.append( %s_right );\n", label, label, label, label);
}

   /*
     Takes a bel and outputs corresponding C++ code for the bel's definition
   */
void
beam_element_display( FILE*         out,
                      beam_element* bel,
                      GHashTable*   var_table,
                      GHashTable*   bel_table ) {
  switch ( bel->kind_ ) {
    case BEL_MARKER: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "marker %s ( (char*)\"%s\" );\n", left, beam_element_make_name( bel->name_ ));
      break;
    }
    case BEL_DRIFT: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "drift %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, " );\n");
      break;
    }
    case BEL_SBEND: {
      char left[BEL_NAME_LENGTH];
      gboolean simple = FALSE;
      expr_struct *k1   = (expr_struct*)bel->params_[BEL_SBEND_K1]->data;
      expr_struct *k2   = (expr_struct*)bel->params_[BEL_SBEND_K2]->data;
      expr_struct *k3   = (expr_struct*)bel->params_[BEL_SBEND_K3]->data;
      expr_struct *tilt = (expr_struct*)bel->params_[BEL_SBEND_TILT]->data;
      beam_element_get_label( bel, left );
      simple = (k1->kind_ == NUMBER_EXPR && k1->dvalue_ == 0.0 && k2->kind_ == NUMBER_EXPR && k2->dvalue_ == 0.0 && k3->kind_ == NUMBER_EXPR && k3->dvalue_ == 0.0 && tilt->kind_ == NUMBER_EXPR && tilt->dvalue_ == 0.0);
      if ( simple == TRUE ) {
        fprintf( out, "sbend");
      } else {
        fprintf( out, "CF_sbend");
      }

      fprintf( out, " %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, ", BRHO*(");
      expr_display( out, bel->params_[BEL_SBEND_ANGLE], var_table, bel_table );
      fprintf( out, ")/(");
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, "), ");
      expr_display( out, bel->params_[BEL_SBEND_ANGLE], var_table, bel_table );
      fprintf( out, " );\n");

      if ( simple == FALSE ) {
        fprintf( out, "         %s.setQuadrupole( ", left);
        expr_display( out, bel->params_[BEL_SBEND_K1], var_table, bel_table );
        fprintf( out, " );\n         %s.setSextupole ( ", left);
        expr_display( out, bel->params_[BEL_SBEND_K2], var_table, bel_table );
        fprintf( out, " );\n         %s.setOctupole  ( ", left);
        expr_display( out, bel->params_[BEL_SBEND_K3], var_table, bel_table );
        fprintf( out, " );\n");
      }

      fprintf( out, "// Ignored for %s: E1=", left);
      expr_display( out, bel->params_[BEL_SBEND_E1], var_table, bel_table );
      fprintf( out, "; E2=");
      expr_display( out, bel->params_[BEL_SBEND_E2], var_table, bel_table );
      fprintf( out, "; H1=");
      expr_display( out, bel->params_[BEL_SBEND_H1], var_table, bel_table );
      fprintf( out, "; H2=");
      expr_display( out, bel->params_[BEL_SBEND_H2], var_table, bel_table );
      fprintf( out, "; HGAP=");
      expr_display( out, bel->params_[BEL_SBEND_HGAP], var_table, bel_table );
      fprintf( out, "; FINT=");
      expr_display( out, bel->params_[BEL_SBEND_FINT], var_table, bel_table );
      fprintf( out, "\n");
      
      if ( tilt->dvalue_ != 0.0 || tilt->kind_ != NUMBER_EXPR ) {
        fprintf( out, "aligner->xOffset = 0.0;\n");
        fprintf( out, "aligner->yOffset = 0.0;\n");
        fprintf( out, "aligner->tilt    = ");
        expr_display( out, bel->params_[BEL_SBEND_TILT], var_table, bel_table );
        fprintf( out, ";\n%s.setAlignment( *aligner );\n", left);
      }
      break;
    }
    case BEL_RBEND: {
      char left[BEL_NAME_LENGTH];
      gboolean simple = FALSE;
      expr_struct* k1   = (expr_struct*)bel->params_[BEL_RBEND_K1]->data;
      expr_struct* k2   = (expr_struct*)bel->params_[BEL_RBEND_K2]->data;
      expr_struct* k3   = (expr_struct*)bel->params_[BEL_RBEND_K3]->data;
      expr_struct* tilt = (expr_struct*)bel->params_[BEL_RBEND_TILT]->data;
      beam_element_get_label( bel, left );
      simple = ( k1->kind_ == NUMBER_EXPR && k1->dvalue_ == 0.0 && k2->kind_ == NUMBER_EXPR && k2->dvalue_ == 0.0 && k3->kind_ == NUMBER_EXPR && k3->dvalue_ == 0.0 && tilt->kind_ == NUMBER_EXPR && tilt->dvalue_ == 0.0 );
      if ( simple == TRUE ) {
        fprintf( out, "rbend");
      } else {
        fprintf( out, "CF_rbend");
      }

      fprintf( out, " %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, ", BRHO*(2.0*sin(0.5*(");
      expr_display( out, bel->params_[BEL_RBEND_ANGLE], var_table, bel_table );
      fprintf( out, ")))/(");
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, "), 0.5*(");
      expr_display( out, bel->params_[BEL_RBEND_ANGLE], var_table, bel_table );
      fprintf( out, ") );\n");

      if ( simple == FALSE ) {
        fprintf( out, "         %s.setQuadrupole( BRHO*(", left);
        expr_display( out, bel->length_, var_table, bel_table );
        fprintf( out, ")*(");
        expr_display( out, bel->params_[BEL_RBEND_K1], var_table, bel_table );
        fprintf( out, ") );\n" );
        fprintf( out, "         %s.setSextupole ( (1.0/2.0)*BRHO*(", left);
        expr_display( out, bel->length_, var_table, bel_table );
        fprintf( out, ")*(");
        expr_display( out, bel->params_[BEL_RBEND_K2], var_table, bel_table );
        fprintf( out, ") );\n");
        fprintf( out, "         %s.setOctupole  ( (1.0/6.0)*BRHO*(", left);
        expr_display( out, bel->length_, var_table, bel_table );
        fprintf( out, ")*(");        
        expr_display( out, bel->params_[BEL_RBEND_K3], var_table, bel_table );
        fprintf( out, ") );\n");
      }
      
      fprintf( out, "// Ignored for %s: E1=", left);
      expr_display( out, bel->params_[BEL_RBEND_E1], var_table, bel_table );
      fprintf( out, "; E2=");
      expr_display( out, bel->params_[BEL_RBEND_E2], var_table, bel_table );
      fprintf( out, "; H1=");
      expr_display( out, bel->params_[BEL_RBEND_H1], var_table, bel_table );
      fprintf( out, "; H2=");
      expr_display( out, bel->params_[BEL_RBEND_H2], var_table, bel_table );
      fprintf( out, "; HGAP=");
      expr_display( out, bel->params_[BEL_RBEND_HGAP], var_table, bel_table );
      fprintf( out, "; FINT=");
      expr_display( out, bel->params_[BEL_RBEND_FINT], var_table, bel_table );
      fprintf( out, "\n");

      if ( tilt->dvalue_ != 0.0 || tilt->kind_ != NUMBER_EXPR ) {
        fprintf( out, "aligner->xOffset = 0.0;\n");
        fprintf( out, "aligner->yOffset = 0.0;\n");
        fprintf( out, "aligner->tilt    = ");
        expr_display( out, bel->params_[BEL_RBEND_TILT], var_table, bel_table );
        fprintf( out, ";\n%s.setAlignment( *aligner );\n", left);
      }
      break;
    }
    case BEL_QUADRUPOLE: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "quadrupole %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, ", BRHO*(");
      expr_display( out, bel->params_[BEL_QUADRUPOLE_K1], var_table, bel_table );
      fprintf( out, ") );\n");

      if ( ((expr_struct*)(bel->params_[BEL_QUADRUPOLE_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_QUADRUPOLE_TILT]->data))->kind_ != NUMBER_EXPR ) {
        fprintf( out, "aligner->xOffset = 0.0;\n");
        fprintf( out, "aligner->yOffset = 0.0;\n");
        fprintf( out, "aligner->tilt    = ");
        expr_display( out, bel->params_[BEL_QUADRUPOLE_TILT], var_table, bel_table );
        fprintf( out, ";\n%s.setAlignment( *aligner );\n", left);
      }
      break;
    }
    case BEL_SEXTUPOLE: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "sextupole %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, ", BRHO*(");
      expr_display( out, bel->params_[BEL_SEXTUPOLE_K2], var_table, bel_table );
      fprintf( out, ")/2.0 );\n");

      if ( ((expr_struct*)(bel->params_[BEL_SEXTUPOLE_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_SEXTUPOLE_TILT]->data))->kind_ != NUMBER_EXPR ) {
        fprintf( out, "aligner->xOffset = 0.0;\n");
        fprintf( out, "aligner->yOffset = 0.0;\n");
        fprintf( out, "aligner->tilt    = ");
        expr_display( out, bel->params_[BEL_SEXTUPOLE_TILT], var_table, bel_table );
        fprintf( out, ";\n%s.setAlignment( *aligner );\n", left);
      }
      break;
    }
    case BEL_OCTUPOLE: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "octupole %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, ", BRHO*(");
      expr_display( out, bel->params_[BEL_OCTUPOLE_K3], var_table, bel_table );
      fprintf( out, ")/6.0 );\n");

      if ( ((expr_struct*)(bel->params_[BEL_OCTUPOLE_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_OCTUPOLE_TILT]->data))->kind_ != NUMBER_EXPR ) {
        fprintf( out, "aligner->xOffset = 0.0;\n");
        fprintf( out, "aligner->yOffset = 0.0;\n");
        fprintf( out, "aligner->tilt    = ");
        expr_display( out, bel->params_[BEL_OCTUPOLE_TILT], var_table, bel_table );
        fprintf( out, ";\n%s.setAlignment( *aligner );\n", left);
      }
      break;
    }
    case BEL_MULTIPOLE:
      fprintf( out, "// MULTIPOLE \"%s\": LRAD=", beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, "; K0L=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_K0L], var_table, bel_table );
      fprintf( out, "; T0=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_T0], var_table, bel_table );
      fprintf( out, "; K1L=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_K1L], var_table, bel_table );
      fprintf( out, "; T1=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_T1], var_table, bel_table );
      fprintf( out, ";\n// K2L=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_K2L], var_table, bel_table );
      fprintf( out, "; T2=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_T2], var_table, bel_table );
      fprintf( out, "; K3L=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_K3L], var_table, bel_table );
      fprintf( out, "; T3=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_T3], var_table, bel_table );
      fprintf( out, "; K4L=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_K4L], var_table, bel_table );
      fprintf( out, "; T4=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_T4], var_table, bel_table );
      fprintf( out, "; K5L=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_K5L], var_table, bel_table );
      fprintf( out, "; T5=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_T5], var_table, bel_table );
      fprintf( out, ";\n// K6L=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_K6L], var_table, bel_table );
      fprintf( out, "; T6=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_T6], var_table, bel_table );
      fprintf( out, "; K7L=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_K7L], var_table, bel_table );
      fprintf( out, "; T7=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_T7], var_table, bel_table );
      fprintf( out, "; K8L=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_K8L], var_table, bel_table );
      fprintf( out, "; T8=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_T8], var_table, bel_table );
      fprintf( out, "; K9L=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_K9L], var_table, bel_table );
      fprintf( out, "; T9=");
      expr_display( out, bel->params_[BEL_MULTIPOLE_T9], var_table, bel_table );
      fprintf( out, "\n");
      break;
    case BEL_SOLENOID: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "solenoid %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, ", BRHO*(");
      expr_display( out, bel->params_[BEL_SOLENOID_KS], var_table, bel_table );
      fprintf( out, ") );\n");
      break;
    }
    case BEL_HKICKER: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "hkick %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, ", ");
      expr_display( out, bel->params_[BEL_HKICKER_KICK], var_table, bel_table );
      fprintf( out, " );\n");

      if ( ((expr_struct*)(bel->params_[BEL_HKICKER_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_HKICKER_TILT]->data))->kind_ != NUMBER_EXPR ) {
        fprintf( out, "aligner->xOffset = 0.0;\n" );
        fprintf( out, "aligner->yOffset = 0.0;\n" );
        fprintf( out, "aligner->tilt    = " );
        expr_display( out, bel->params_[BEL_HKICKER_TILT], var_table, bel_table );
        fprintf( out, ";\n%s.setAlignment( *aligner );\n", left);
      }
      break;
    }
    case BEL_VKICKER: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "vkick %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, ", ");
      expr_display( out, bel->params_[BEL_VKICKER_KICK], var_table, bel_table );
      fprintf( out, " );\n");

      if ( ((expr_struct*)(bel->params_[BEL_VKICKER_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_VKICKER_TILT]->data))->kind_ != NUMBER_EXPR ) {
        fprintf( out, "aligner->xOffset = 0.0;\n");
        fprintf( out, "aligner->yOffset = 0.0;\n");
        fprintf( out, "aligner->tilt    = ");
        expr_display( out, bel->params_[BEL_VKICKER_TILT], var_table, bel_table );
        fprintf( out, ";\n%s.setAlignment( *aligner );\n", left);
      }
      break;
    }
    case BEL_KICKER: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "kick %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, ", ");
      expr_display( out, bel->params_[BEL_KICKER_HKICK], var_table, bel_table );
      fprintf( out, ", ");
      expr_display( out, bel->params_[BEL_KICKER_VKICK], var_table, bel_table );
      fprintf( out, " );\n");

      if ( ((expr_struct*)(bel->params_[BEL_KICKER_TILT]->data))->dvalue_ != 0.0 || ((expr_struct*)(bel->params_[BEL_KICKER_TILT]->data))->kind_ != NUMBER_EXPR ) {
        fprintf( out, "aligner->xOffset = 0.0;\n");
        fprintf( out, "aligner->yOffset = 0.0;\n");
        fprintf( out, "aligner->tilt    = ");
        expr_display( out, bel->params_[BEL_KICKER_TILT], var_table, bel_table );
        fprintf( out, ";\n%s.setAlignment( *aligner );\n", left);
      }
      break;
    }
    case BEL_RFCAVITY: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "thinrfcavity %s_center ( (char*)\"%s:center\", (int)(", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->params_[BEL_RFCAVITY_HARMON], var_table, bel_table );
      fprintf( out, "), (");
      expr_display( out, bel->params_[BEL_RFCAVITY_VOLT], var_table, bel_table );
      fprintf( out, ")*1.0e6*1.602177e-19, (");
      expr_display( out, bel->params_[BEL_RFCAVITY_LAG], var_table, bel_table );
      fprintf( out, ")*2*PI, 0.0, ");
      expr_display( out, bel->params_[BEL_RFCAVITY_SHUNT], var_table, bel_table );
      fprintf( out, " );\n");
      beam_element_plus_drift( out, left, beam_element_make_name( bel->name_ ), bel->length_, var_table, bel_table );
      fprintf( out, "// Ignored for %s: BETRF=", left);
      expr_display( out, bel->params_[BEL_RFCAVITY_BETRF], var_table, bel_table );
      fprintf( out, "; PG=");
      expr_display( out, bel->params_[BEL_RFCAVITY_PG], var_table, bel_table );
      fprintf( out, "; TFILL=");
      expr_display( out, bel->params_[BEL_RFCAVITY_TFILL], var_table, bel_table );
      fprintf( out, "\n");
      break;
    }
    case BEL_ELSEPARATOR: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "drift %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, " );\n");
      fprintf( out, "// The above drift is really ELSEPARATOR \"%s\": E=", beam_element_make_name( bel->name_ ));
      expr_display( out, bel->params_[BEL_ELSEPARATOR_E], var_table, bel_table );
      fprintf( out, "; TILT=");
      expr_display( out, bel->params_[BEL_ELSEPARATOR_TILT], var_table, bel_table );
      fprintf( out, "\n");
      break;
    }
    case BEL_HMONITOR: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "hmonitor %s_center ( (char*)\"%s:center\" );\n", left, beam_element_make_name( bel->name_ ));
      beam_element_plus_drift( out, left, beam_element_make_name( bel->name_ ), bel->length_, var_table, bel_table );
      break;
    }
    case BEL_VMONITOR: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "vmonitor %s_center ( (char*)\"%s:center\" );\n", left, beam_element_make_name( bel->name_ ));
      beam_element_plus_drift( out, left, beam_element_make_name( bel->name_ ), bel->length_, var_table, bel_table );
      break;
    }
    case BEL_MONITOR: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "monitor %s_center ( (char*)\"%s:center\" );\n", left, beam_element_make_name( bel->name_ ));
      beam_element_plus_drift( out, left, beam_element_make_name( bel->name_ ), bel->length_, var_table, bel_table );
      break;
    }
    case BEL_INSTRUMENT: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "drift %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, " );\n");
      fprintf( out, "// The above drift is really INSTRUMENT \"%s\"\n", beam_element_make_name( bel->name_ ));
      break;
    }
    case BEL_ECOLLIMATOR: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "drift %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, " );\n");
      fprintf( out, "// The above drift is really ECOLLIMATOR \"%s\": XSIZE=", beam_element_make_name( bel->name_ ));
      expr_display( out, bel->params_[BEL_ECOLLIMATOR_XSIZE], var_table, bel_table );
      fprintf( out, "; YSIZE=");
      expr_display( out, bel->params_[BEL_ECOLLIMATOR_YSIZE], var_table, bel_table );
      fprintf( out, "\n");
      break;
    }
    case BEL_RCOLLIMATOR: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "drift %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->length_, var_table, bel_table );
      fprintf( out, " );\n");
      fprintf( out, "// The above drift is really RCOLLIMATOR \"%s\": XSIZE=", beam_element_make_name( bel->name_ ));
      expr_display( out, bel->params_[BEL_RCOLLIMATOR_XSIZE], var_table, bel_table );
      fprintf( out, "; YSIZE=");
      expr_display( out, bel->params_[BEL_RCOLLIMATOR_YSIZE], var_table, bel_table );
      fprintf( out, "\n");
      break;
    }
    case BEL_YROT:
      fprintf( out, "// YROT \"%s\": ANGLE=", beam_element_make_name( bel->name_ ));
      expr_display( out, bel->params_[BEL_YROT_ANGLE], var_table, bel_table );
      fprintf( out, "\n");
      break;
    case BEL_SROT: {
      char left[BEL_NAME_LENGTH];
      beam_element_get_label( bel, left );
      fprintf( out, "srot %s ( (char*)\"%s\", ", left, beam_element_make_name( bel->name_ ));
      expr_display( out, bel->params_[BEL_SROT_ANGLE], var_table, bel_table );
      fprintf( out, " );\n");
      break;
    }
    case BEL_BEAMBEAM:
      fprintf( out, "// BEAMBEAM \"%s\": SIGX=", beam_element_make_name( bel->name_ ));
      expr_display( out, bel->params_[BEL_BEAMBEAM_SIGX], var_table, bel_table );
      fprintf( out, "; SIGY=");
      expr_display( out, bel->params_[BEL_BEAMBEAM_SIGY], var_table, bel_table );
      fprintf( out, "; XMA=");
      expr_display( out, bel->params_[BEL_BEAMBEAM_XMA], var_table, bel_table );
      fprintf( out, "; YMA=");
      expr_display( out, bel->params_[BEL_BEAMBEAM_YMA], var_table, bel_table );
      fprintf( out, "; CHARGE=");
      expr_display( out, bel->params_[BEL_BEAMBEAM_CHARGE], var_table, bel_table );
      fprintf( out, "\n");
      break;
    case BEL_MATRIX:
      fprintf( out, "// MATRIX \"%s\"\n", beam_element_make_name( bel->name_ ));
      break;
    case BEL_LUMP:
      fprintf( out, "// LUMP \"%s\": ORDER=", beam_element_make_name( bel->name_ ));
      expr_display( out, bel->params_[BEL_LUMP_ORDER], var_table, bel_table );
      fprintf( out, "\n");
      break;
    default:
      fprintf(stderr, "// ! Unknown beam element !");
  }
}
