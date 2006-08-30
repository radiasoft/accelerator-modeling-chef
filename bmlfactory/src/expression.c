/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      expression.c
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
******  Contacts:  Leo Michelotti or Jean-Francois Ostiguy            
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
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void bmlfactory_exit(const char* filename, int lineno, const char* errmessage);
extern void bmlfactory_parse_error(int inmemory, const char* input_filename, int input_file_lineno, const char* parser_message);

#include <bmlfactory/beam_element.h>
#include <bmlfactory/beamel_table.h>
#include <bmlfactory/const_table.h>
#include <bmlfactory/var_table.h>
#include <bmlfactory/expression.h>
#include <bmlfactory/madparser.h>

/* Including mp here is a temporary fix. mp is required for error reporting */
/* the entire error reporting mechanism should be overhauled. -jfo          */ 

static char error_msg[132];

void
expression_init( void ) {
}

   /*
     De-initializes expression system and returns 0
    */
void
expression_delete( void ) {
}

   /*
     Takes an expression and "user_data" (not used), frees the memory given
     to the expression, and returns FALSE
   */
static gboolean
expr_expr_delete( GNode*   node,
                  gpointer user_data ) {
  
  expr_struct* data = NULL;

  assert( node != NULL );
  
  data = (expr_struct*)(node->data);
  
  if ( data->svalue_ != NULL ) {
    free( data->svalue_ );
    data->svalue_ = NULL;
  }
  PRIVATE_DEALLOCATE( data, (fb_allocator*)user_data );
  data = NULL;

  return FALSE;
}

   /*
     Takes a pointer to an expression, a number, and a string, creates a new
     number expression with the given values, and returns an OK value
   */
GNode*
expr_node_init( double        dvalue,
                const char*   svalue,
                fb_allocator* expr_alloc ) {
  
  GNode*       expr = NULL;
  expr_struct* data;

  assert( svalue != NULL );
  assert( expr_alloc != NULL );

  PRIVATE_ALLOCATE( data, expr_alloc );
  if ( data != NULL ) {
    data->kind_ = NUMBER_EXPR;
    data->dvalue_ = dvalue;
    data->svalue_ = (char*)malloc( strlen( svalue ) + 1 );
    if ( data->svalue_ != NULL ) {
      strcpy( data->svalue_, svalue );
      expr = g_node_new( data );
    } else {
      PRIVATE_DEALLOCATE( expr, expr_alloc );
    }
  }

  return expr;
}

   /*
     Takes an expression, frees memory used by it, and returns an OK value
   */
int
expr_node_delete( GNode*        expr,
                  fb_allocator* expr_alloc ) {
  assert( expr != NULL );
  assert( expr_alloc != NULL );

  g_node_traverse( expr, G_PRE_ORDER, G_TRAVERSE_ALL, -1, (GNodeTraverseFunc)expr_expr_delete, expr_alloc );
  g_node_destroy( expr );

  return EXPR_OK;
}

   /*
     Takes an expression and outputs it in C++ style
   */
void
expr_display( madparser* mp,
              FILE*       out,
              GNode*      expr,
              GHashTable* var_table,
              GHashTable* bel_table ) {
  
  GNode* node = NULL;

  expr_struct* data = NULL;

  assert( expr != NULL );

  data = (expr_struct*)(expr->data);
  
  switch (data->kind_) {
    case NUMBER_EXPR :
      fprintf( out, "%s", data->svalue_ );
      break;
    case STRING_EXPR :
      fprintf( out, "\"%s\"", data->svalue_ );
      break;
    case NUM_IDENT_EXPR :
      fprintf( out, "%s", data->svalue_ );
      break;
    case STR_IDENT_EXPR :
      fprintf( out, "%s", data->svalue_ );
      break;
    case VAR_IDENT_EXPR :
      fprintf( out, "%s", data->svalue_ );
      break;
    case BEAMEL_LEN_EXPR : {
      beam_element* bel = (beam_element*)bel_table_lookup( data->svalue_, bel_table );
      if ( bel == NULL ) {
        fprintf( out, "error ! beam element %s never defined\n", data->svalue_ );
      } else {
        fprintf( out, "%e /* %s.Length() */", expr_evaluate(mp,  bel->length_, var_table, bel_table ), data->svalue_ );
      }
      break;
    }
    case BRACKETS_EXPR:
      fprintf( out, "(");
      expr_display(mp,  out, g_node_first_child( expr ), var_table, bel_table );      
      fprintf( out, ")");
      break;
    case UN_PLUS_EXPR :
      expr_display(mp,  out, g_node_first_child( expr ), var_table, bel_table );
      break;
    case UN_MINUS_EXPR :
      fprintf( out, "(-(");
      expr_display(mp,  out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, "))" );
      break;
    case ADD_OP_EXPR :
      node = g_node_first_child( expr );
      expr_display(mp,  out, node, var_table, bel_table );
      fprintf( out, "+");
      expr_display(mp,  out, node->next, var_table, bel_table );
      break;
    case SUB_OP_EXPR :
      node = g_node_first_child( expr );
      expr_display(mp,  out, node, var_table, bel_table );
      fprintf( out, "-");
      expr_display(mp,  out, node->next, var_table, bel_table );
      break;
    case MUL_OP_EXPR :
      node = g_node_first_child( expr );
      fprintf( out, "(");
      expr_display(mp,  out, node, var_table, bel_table );
      fprintf( out, ")*(");
      expr_display(mp,  out, node->next, var_table, bel_table );
      fprintf( out, ")");
      break;
    case DIV_OP_EXPR :
      node = g_node_first_child( expr );
      fprintf( out, "(");
      expr_display(mp,  out, node, var_table, bel_table );
      fprintf( out, ")/(");
      expr_display(mp,  out, node->next, var_table, bel_table );
      fprintf( out, ")");
      break;
    case POW_OP_EXPR :
      node = g_node_first_child( expr );
      fprintf( out, "pow(");
      expr_display(mp,  out, node, var_table, bel_table );
      fprintf( out, ",");
      expr_display(mp,  out, node->next, var_table, bel_table );
      fprintf( out, ")");
      break;
    case SQRT_EXPR :
      fprintf( out, "sqrt(");
      expr_display(mp,  out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case LOG_EXPR :
      fprintf( out, "log(");
      expr_display(mp,  out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case EXP_EXPR :
      fprintf( out, "exp(");
      expr_display(mp,  out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case SIN_EXPR :
      fprintf( out, "sin(");
      expr_display(mp,  out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case COS_EXPR :
      fprintf( out, "cos(");
      expr_display(mp,  out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case TAN_EXPR :
      fprintf( out, "tan(");
      expr_display(mp,  out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case ASIN_EXPR :
      fprintf( out, "asin(");
      expr_display(mp,  out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case ABS_EXPR :
      fprintf( out, "fabs(");
      expr_display(mp,  out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")"); 
      break;
    case MAX_EXPR :
      node = g_node_first_child( expr );
      fprintf( out, "((");
      expr_display(mp,  out, node, var_table, bel_table );
      fprintf( out, ") > (");
      expr_display(mp,  out, node->next, var_table, bel_table );
      fprintf( out, ")) ? (");
      expr_display(mp,  out, node, var_table, bel_table );
      fprintf( out, ") : (");
      expr_display(mp,  out, node->next, var_table, bel_table );
      fprintf( out, ")");
      break;
    case MIN_EXPR :
      node = g_node_first_child( expr );
      fprintf( out, "((");
      expr_display(mp,  out, node, var_table, bel_table );
      fprintf( out, ") < (");
      expr_display(mp,  out, node->next, var_table, bel_table );
      fprintf( out, ")) ? (");
      expr_display(mp,  out, node, var_table, bel_table );
      fprintf( out, ") : (");
      expr_display(mp,  out, node->next, var_table, bel_table );
      fprintf( out, ")");
      break;
    default :
      fprintf(stderr, "error ! unknown expression type\n");
  }
}

   /*
     Takes an expression, evaluates it, and returns the result
   */
double
expr_evaluate(madparser* mp,  GNode*      expr,
               GHashTable* var_table,
               GHashTable* bel_table ) {
  double val1, val2;
  GNode* node;
  
  expr_struct* data = NULL;

  assert( expr != NULL );

  data = (expr_struct*)(expr->data);
  
  switch (data->kind_) {
    case NUMBER_EXPR :
      return data->dvalue_;
      break;
    case STRING_EXPR :
      sprintf(error_msg, "String \"%s\" used in an algebraic expression\n", data->svalue_);
      bmlfactory_parse_error( madparser_is_reading_from_memory(mp), madparser_current_filename(mp), madparser_linenum(mp), error_msg);
      return 0;
      break;
    case NUM_IDENT_EXPR :
      return data->dvalue_;
      break;
    case STR_IDENT_EXPR :
      sprintf(error_msg, "String id %s used in an algebraic expression\n", data->svalue_);
      bmlfactory_parse_error( madparser_is_reading_from_memory(mp), madparser_current_filename(mp), madparser_linenum(mp), error_msg);
      return 0;
      break;
    case VAR_IDENT_EXPR :
      {
        variable* var = (variable*)var_table_lookup( data->svalue_, var_table );
        if ( var == NULL ) {
          sprintf(error_msg, "Variable %s was never defined\n", data->svalue_);
          bmlfactory_parse_error( madparser_is_reading_from_memory(mp), madparser_current_filename(mp), madparser_linenum(mp), error_msg);
          /* bmlfactory_exit(); */
          return 0;
        } else {
          return expr_evaluate(mp,  var->expr_, var_table, bel_table );
        }
      }
      break;
    case BEAMEL_LEN_EXPR :
      {
        beam_element* bel = (beam_element*)bel_table_lookup( data->svalue_, bel_table );
        if ( bel == NULL ) {
           sprintf(error_msg, "Beamline element %s was never defined\n", data->svalue_);
           bmlfactory_parse_error( madparser_is_reading_from_memory(mp), madparser_current_filename(mp), madparser_linenum(mp), error_msg);
           /* bmlfactory_exit(); */
           return 0;
        } else {
          return expr_evaluate(mp,  bel->length_, var_table, bel_table );
        }
      }
      break;
    case BRACKETS_EXPR :
      return expr_evaluate(mp,  g_node_first_child( expr ), var_table, bel_table );
      break;
    case UN_PLUS_EXPR :
      return expr_evaluate(mp,  g_node_first_child( expr ), var_table, bel_table );
      break;
    case UN_MINUS_EXPR :
      return -expr_evaluate(mp,  g_node_first_child( expr ), var_table, bel_table );
      break;
    case ADD_OP_EXPR :
      node = g_node_first_child( expr );
      return ( expr_evaluate(mp,  node, var_table, bel_table ) +
               expr_evaluate(mp,  node->next, var_table, bel_table ) );
      break;
    case SUB_OP_EXPR :
      node = g_node_first_child( expr );
      return ( expr_evaluate(mp,  node, var_table, bel_table ) -
               expr_evaluate(mp,  node->next, var_table, bel_table ) );
      break;
    case MUL_OP_EXPR :
      node = g_node_first_child( expr );
      return ( expr_evaluate(mp,  node, var_table, bel_table ) *
               expr_evaluate(mp,  node->next, var_table, bel_table ) );
      break;
    case DIV_OP_EXPR :
      node = g_node_first_child( expr );
      return ( expr_evaluate(mp,  node, var_table, bel_table ) /
               expr_evaluate(mp,  node->next, var_table, bel_table ) );
      break;
    case POW_OP_EXPR :
      node = g_node_first_child( expr );
      return pow( expr_evaluate(mp,  node, var_table, bel_table ),
                  expr_evaluate(mp,  node->next, var_table, bel_table ) );
      break;
    case SQRT_EXPR :
      return sqrt( expr_evaluate(mp,  g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case LOG_EXPR :
      return log( expr_evaluate(mp,  g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case EXP_EXPR :
      return exp( expr_evaluate(mp,  g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case SIN_EXPR :
      return sin( expr_evaluate(mp,  g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case COS_EXPR :
      return cos( expr_evaluate(mp,  g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case TAN_EXPR :
      return tan( expr_evaluate(mp,  g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case ASIN_EXPR :
      return atan( expr_evaluate(mp,  g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case ABS_EXPR :
      return fabs( expr_evaluate(mp,  g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case MAX_EXPR :
      node = g_node_first_child( expr );
      val1 = expr_evaluate(mp,  node, var_table, bel_table );
      val2 = expr_evaluate(mp,  node->next, var_table, bel_table );
      return ( val1 > val2 ) ? val1 : val2;
      break;
    case MIN_EXPR :
      node = g_node_first_child( expr );
      val1 = expr_evaluate(mp,  node, var_table, bel_table );
      val2 = expr_evaluate(mp,  node->next, var_table, bel_table );
      return ( val1 < val2 ) ? val1 : val2;
      break;
    default :
      sprintf(error_msg, "Unknown expression type\n");
      bmlfactory_parse_error( madparser_is_reading_from_memory(mp), madparser_current_filename(mp), madparser_linenum(mp), error_msg);
      return 0;
  }
}

   /*
     Takes an expression, and returns its string value if it is
     a string expression or NULL if it's not
   */
char*
expr_is_string( madparser*  mp, 
                GNode*      expr,
                GHashTable* const_table,
                GHashTable* var_table ) {
  expr_struct* data;
  variable* assigned_var = (variable*)var_table_lookup( ((expr_struct*)(expr->data))->svalue_, var_table );
  if ( assigned_var == NULL ) {
    sprintf(error_msg, "Variable %s was never defined\n", ((expr_struct*)(expr->data))->svalue_);
    bmlfactory_parse_error( madparser_is_reading_from_memory(mp), madparser_current_filename(mp), madparser_linenum(mp), error_msg);
    return 0;    
    /* bmlfactory_exit(); */
  }
  data = (expr_struct*)(((GNode*)(assigned_var->expr_))->data);
  if ( data->kind_ == STRING_EXPR ) {
    return data->svalue_;
  } else if ( data->kind_ == STR_IDENT_EXPR ) {
    return ((constant*)const_table_lookup( data->svalue_, const_table ))->svalue_;
  } else if ( data->kind_ == VAR_IDENT_EXPR ) {
    return expr_is_string( mp, assigned_var->expr_, const_table, var_table );
  } else {
    return NULL;
  }
}
