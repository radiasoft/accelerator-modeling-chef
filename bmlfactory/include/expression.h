/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      expression.h
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


#ifndef expression_h
#define expression_h

#include <stdio.h>
#include <glib.h>

#if !defined(fb_allocator_h)
#include "fb_allocator.h"
#endif /* fb_allocator_h */

#define EXPR_OK            0

enum expression_kind {
  UNKNOWN_EXPR = -1,
  NUMBER_EXPR  = 0,
  STRING_EXPR,
  NUM_IDENT_EXPR,
  STR_IDENT_EXPR,
  VAR_IDENT_EXPR,
  BEAMEL_LEN_EXPR,
  BRACKETS_EXPR,
  UN_PLUS_EXPR,
  UN_MINUS_EXPR,
  ADD_OP_EXPR,
  SUB_OP_EXPR,
  MUL_OP_EXPR,
  DIV_OP_EXPR,
  POW_OP_EXPR,
  SQRT_EXPR,
  LOG_EXPR,
  EXP_EXPR,
  SIN_EXPR,
  COS_EXPR,
  TAN_EXPR,
  ASIN_EXPR,
  ABS_EXPR,
  MAX_EXPR,
  MIN_EXPR
};

typedef  struct {
    enum expression_kind kind_;
    char*                svalue_;
    double               dvalue_;
} expr_struct;

void   expression_init(void);
void   expression_delete(void);

#ifdef __cplusplus
extern "C" {
#endif

GNode* expr_node_init( double dvalue, const char* svalue, fb_allocator* expr_alloc );
int    expr_node_delete( GNode* expr, fb_allocator* expr_alloc );

void   expr_display( FILE* out, GNode* expr, GHashTable* var_table, GHashTable* bel_table );
double expr_evaluate( GNode* expr, GHashTable* var_table, GHashTable* bel_table );
char*  expr_is_string( GNode* expr, GHashTable* const_table, GHashTable* var_table );

#ifdef __cplusplus
};
#endif

#endif /* expression_h */
