#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#if !defined(beam_element_h)
#include "beam_element.h"
#endif /* beam_element_h */

#if !defined(beamel_table_h)
#include "beamel_table.h"
#endif /* beamel_table_h */

#if !defined(const_table_h)
#include "const_table.h"
#endif /* const_table_h */

#if !defined(var_table_h)
#include "var_table.h"
#endif /* var_table_h */

#if !defined(expression_h)
#include "expression.h"
#endif /* expression_h */

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
  deallocate( data, (fb_allocator*)user_data );
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

  data = (expr_struct*)allocate( expr_alloc );
  if ( data != NULL ) {
    data->kind_ = NUMBER_EXPR;
    data->dvalue_ = dvalue;
    data->svalue_ = (char*)malloc( strlen( svalue ) + 1 );
    if ( data->svalue_ != NULL ) {
      strcpy( data->svalue_, svalue );
      expr = g_node_new( data );
    } else {
      deallocate( expr, expr_alloc );
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
expr_display( FILE*       out,
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
        fprintf( out, "error (a)! beam element %s never defined\n", data->svalue_ );
        exit( EXIT_FAILURE );
      } else {
        fprintf( out, "%e /* %s.Length() */", expr_evaluate( bel->length_, var_table, bel_table ), data->svalue_ );
      }
      break;
    }
    case BRACKETS_EXPR:
      fprintf( out, "(");
      expr_display( out, g_node_first_child( expr ), var_table, bel_table );      
      fprintf( out, ")");
      break;
    case UN_PLUS_EXPR :
      expr_display( out, g_node_first_child( expr ), var_table, bel_table );
      break;
    case UN_MINUS_EXPR :
      fprintf( out, "(-(");
      expr_display( out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, "))" );
      break;
    case ADD_OP_EXPR :
      node = g_node_first_child( expr );
      expr_display( out, node, var_table, bel_table );
      fprintf( out, "+");
      expr_display( out, node->next, var_table, bel_table );
      break;
    case SUB_OP_EXPR :
      node = g_node_first_child( expr );
      expr_display( out, node, var_table, bel_table );
      fprintf( out, "-");
      expr_display( out, node->next, var_table, bel_table );
      break;
    case MUL_OP_EXPR :
      node = g_node_first_child( expr );
      fprintf( out, "(");
      expr_display( out, node, var_table, bel_table );
      fprintf( out, ")*(");
      expr_display( out, node->next, var_table, bel_table );
      fprintf( out, ")");
      break;
    case DIV_OP_EXPR :
      node = g_node_first_child( expr );
      fprintf( out, "(");
      expr_display( out, node, var_table, bel_table );
      fprintf( out, ")/(");
      expr_display( out, node->next, var_table, bel_table );
      fprintf( out, ")");
      break;
    case POW_OP_EXPR :
      node = g_node_first_child( expr );
      fprintf( out, "pow(");
      expr_display( out, node, var_table, bel_table );
      fprintf( out, ",");
      expr_display( out, node->next, var_table, bel_table );
      fprintf( out, ")");
      break;
    case SQRT_EXPR :
      fprintf( out, "sqrt(");
      expr_display( out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case LOG_EXPR :
      fprintf( out, "log(");
      expr_display( out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case EXP_EXPR :
      fprintf( out, "exp(");
      expr_display( out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case SIN_EXPR :
      fprintf( out, "sin(");
      expr_display( out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case COS_EXPR :
      fprintf( out, "cos(");
      expr_display( out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case TAN_EXPR :
      fprintf( out, "tan(");
      expr_display( out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case ASIN_EXPR :
      fprintf( out, "asin(");
      expr_display( out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")");
      break;
    case ABS_EXPR :
      fprintf( out, "fabs(");
      expr_display( out, g_node_first_child( expr ), var_table, bel_table );
      fprintf( out, ")"); 
      break;
    case MAX_EXPR :
      node = g_node_first_child( expr );
      fprintf( out, "((");
      expr_display( out, node, var_table, bel_table );
      fprintf( out, ") > (");
      expr_display( out, node->next, var_table, bel_table );
      fprintf( out, ")) ? (");
      expr_display( out, node, var_table, bel_table );
      fprintf( out, ") : (");
      expr_display( out, node->next, var_table, bel_table );
      fprintf( out, ")");
      break;
    case MIN_EXPR :
      node = g_node_first_child( expr );
      fprintf( out, "((");
      expr_display( out, node, var_table, bel_table );
      fprintf( out, ") < (");
      expr_display( out, node->next, var_table, bel_table );
      fprintf( out, ")) ? (");
      expr_display( out, node, var_table, bel_table );
      fprintf( out, ") : (");
      expr_display( out, node->next, var_table, bel_table );
      fprintf( out, ")");
      break;
    default :
      fprintf(stderr, "error ! uknown expression type\n");
  }
}

   /*
     Takes an expression, evaluates it, and returns the result
   */
double
expr_evaluate( GNode*      expr,
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
      fprintf(stderr, "error ! string \"%s\" used in an algebraic expression\n", data->svalue_);
      return 0;
      break;
    case NUM_IDENT_EXPR :
      return data->dvalue_;
      break;
    case STR_IDENT_EXPR :
      fprintf(stderr, "error ! string id %s used in an algebraic expression\n", data->svalue_);
      return 0;
      break;
    case VAR_IDENT_EXPR :
      {
        variable* var = (variable*)var_table_lookup( data->svalue_, var_table );
        if ( var == NULL ) {
          fprintf(stderr, "error (b)! variable %s never defined\n", data->svalue_);
          exit( EXIT_FAILURE );
        } else {
          return expr_evaluate( var->expr_, var_table, bel_table );
        }
      }
      break;
    case BEAMEL_LEN_EXPR :
      {
        beam_element* bel = (beam_element*)bel_table_lookup( data->svalue_, bel_table );
        if ( bel == NULL ) {
          fprintf(stderr, "error (c)! beam element %s never defined\n", data->svalue_);
          exit( EXIT_FAILURE );
        } else {
          return expr_evaluate( bel->length_, var_table, bel_table );
        }
      }
      break;
    case BRACKETS_EXPR :
      return expr_evaluate( g_node_first_child( expr ), var_table, bel_table );
      break;
    case UN_PLUS_EXPR :
      return expr_evaluate( g_node_first_child( expr ), var_table, bel_table );
      break;
    case UN_MINUS_EXPR :
      return -expr_evaluate( g_node_first_child( expr ), var_table, bel_table );
      break;
    case ADD_OP_EXPR :
      node = g_node_first_child( expr );
      return ( expr_evaluate( node, var_table, bel_table ) +
               expr_evaluate( node->next, var_table, bel_table ) );
      break;
    case SUB_OP_EXPR :
      node = g_node_first_child( expr );
      return ( expr_evaluate( node, var_table, bel_table ) -
               expr_evaluate( node->next, var_table, bel_table ) );
      break;
    case MUL_OP_EXPR :
      node = g_node_first_child( expr );
      return ( expr_evaluate( node, var_table, bel_table ) *
               expr_evaluate( node->next, var_table, bel_table ) );
      break;
    case DIV_OP_EXPR :
      node = g_node_first_child( expr );
      return ( expr_evaluate( node, var_table, bel_table ) /
               expr_evaluate( node->next, var_table, bel_table ) );
      break;
    case POW_OP_EXPR :
      node = g_node_first_child( expr );
      return pow( expr_evaluate( node, var_table, bel_table ),
                  expr_evaluate( node->next, var_table, bel_table ) );
      break;
    case SQRT_EXPR :
      return sqrt( expr_evaluate( g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case LOG_EXPR :
      return log( expr_evaluate( g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case EXP_EXPR :
      return exp( expr_evaluate( g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case SIN_EXPR :
      return sin( expr_evaluate( g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case COS_EXPR :
      return cos( expr_evaluate( g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case TAN_EXPR :
      return tan( expr_evaluate( g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case ASIN_EXPR :
      return atan( expr_evaluate( g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case ABS_EXPR :
      return fabs( expr_evaluate( g_node_first_child( expr ), var_table, bel_table ) );
      break;
    case MAX_EXPR :
      node = g_node_first_child( expr );
      val1 = expr_evaluate( node, var_table, bel_table );
      val2 = expr_evaluate( node->next, var_table, bel_table );
      return ( val1 > val2 ) ? val1 : val2;
      break;
    case MIN_EXPR :
      node = g_node_first_child( expr );
      val1 = expr_evaluate( node, var_table, bel_table );
      val2 = expr_evaluate( node->next, var_table, bel_table );
      return ( val1 < val2 ) ? val1 : val2;
      break;
    default :
      fprintf(stderr, "error ! unknown expression type\n");
      return 0;
  }
}

   /*
     Takes an expression, and returns its string value if it is
     a string expression or NULL if it's not
   */
char*
expr_is_string( GNode*      expr,
                GHashTable* const_table,
                GHashTable* var_table ) {
  expr_struct* data;
  variable* assigned_var = (variable*)var_table_lookup( ((expr_struct*)(expr->data))->svalue_, var_table );
  if ( assigned_var == NULL ) {
    fprintf(stderr, "error (d)! variable %s never defined\n", ((expr_struct*)(expr->data))->svalue_);
    exit( EXIT_FAILURE );
  }
  data = (expr_struct*)(((GNode*)(assigned_var->expr_))->data);
  if ( data->kind_ == STRING_EXPR ) {
    return data->svalue_;
  } else if ( data->kind_ == STR_IDENT_EXPR ) {
    return ((constant*)const_table_lookup( data->svalue_, const_table ))->svalue_;
  } else if ( data->kind_ == VAR_IDENT_EXPR ) {
    return expr_is_string( assigned_var->expr_, const_table, var_table );
  } else {
    return NULL;
  }
}
