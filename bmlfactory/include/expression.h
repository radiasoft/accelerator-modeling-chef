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
******  URA/FNAL reserves all rights.
******                                          
******                                                                
******  Authors:    Dmitri Mokhov and Oleg Krivosheev                  
******                                                                
******
******  Contacts:            Leo Michelotti michelotti@fnal.gov
******             Jean-Francois Ostiguy    ostiguy@fnal.gov
******                                                              
******
******  Revision History:   April 2006, ostiguy@fnal.gov
******                      Made both lexer and parser reentrant. 
******                      Eliminated global variable mp.            
******                                                                
**************************************************************************
*************************************************************************/
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdio.h>
#include <glib.h>
#include <fb_allocator.h>
#include <madparser_types.h>

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

void   expr_display  ( madparser* mp, FILE* out,     GNode* expr, GHashTable* var_table, GHashTable* bel_table );
double expr_evaluate ( madparser* mp, GNode* expr, GHashTable* var_table, GHashTable* bel_table );
char*  expr_is_string( madparser* mp, GNode* expr,   GHashTable* const_table, GHashTable* var_table );

#ifdef __cplusplus
};
#endif

#endif /* EXPRESSION_H  */
