/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      var_table.c
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
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


   /* -*- C -*- */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void bmlfactory_exit();

#include <str_to_upper.h>
#include <beam_element.h>
#include <beamel_table.h>
#include <const_table.h>
#include <expression.h>
#include <pair.h>
#include <quatro.h>
#include <var_table.h>


   /*
     Takes a string and returns its hash table index
   */
static gint
var_hash_func( gconstpointer str ) {
  return g_str_hash( str );
}

   /*
     Takes 2 strings and returns TRUE if they are equal
   */
static gint
var_comp_func( gconstpointer left,
               gconstpointer right ) {
  return g_str_equal( left, right );
}

   /*
     Takes a char pointer "key", variable "value", and "user_data" (not used),
     and frees the memory for "key" and "value"
   */
static gboolean
var_free_func( gpointer key,
               gpointer value,
               gpointer user_data ) {
  free( key );
  var_delete( (variable*)value, (fb_allocator*)user_data );
  return TRUE;
}

   /*
     Takes a char pointer "key", variable "value", and arr_ptr address "user_data",
     and makes the array element at "user_data" point to "value"
   */
static void
var_table_el_to_array_el( gpointer key,
                          gpointer value,
                          gpointer user_data ) {
     /* user_data is arr_ptr address */
  variable **ptr = *((variable***)user_data);
  *ptr = (variable*)value;
  *((variable***)user_data) = ++ptr;
}



   /*
     Takes a char pointer "key", variable "value", and "user_data" (not used), 
     and prints information about "value"
   */
static void
var_display_func( gpointer key,
                  gpointer value,
                  gpointer user_data ) {

  quatro* p   = (quatro*)user_data;
  FILE*  out  = (FILE*)(p->first_);
  GNode* expr = ((variable*)value)->expr_;
  
  fprintf( out, "Variable: %s; defined on line %d in file %s\n", ((variable*)value)->name_, ((variable*)value)->local_linenum_, ((variable*)value)->filename_ );
  fprintf( out, "Expression: ");
  expr_display( out, expr, (GHashTable*)(p->third_), (GHashTable*)(p->fourth_) );
  fprintf( out, "\nValue: ");
  if ( ((expr_struct*)(expr->data))->kind_ == STRING_EXPR ) {
    fprintf( out, "\"%s\"", ((expr_struct*)(expr->data))->svalue_);
  } else if ( ((expr_struct*)(expr->data))->kind_ == STR_IDENT_EXPR ) {
    fprintf( out, "\"%s\"", ( (constant*)const_table_lookup( ((expr_struct*)(expr->data))->svalue_, (GHashTable*)(p->second_) ))->svalue_ );
  } else if ( ((expr_struct*)(expr->data))->kind_ == VAR_IDENT_EXPR ) {
    char* str = expr_is_string( expr, (GHashTable*)(p->second_), (GHashTable*)(p->third_) );
    if ( str != NULL ) {
      fprintf( out, "\"%s\"", str);
    } else {
      fprintf( out, "%f", expr_evaluate( expr, (GHashTable*)(p->third_), (GHashTable*)(p->fourth_) ) );
    }
  } else {
    fprintf( out, "%f", expr_evaluate( expr, (GHashTable*)(p->third_), (GHashTable*)(p->fourth_) ) );
  }

  fprintf( out, "\n" );
}

   /*
     Initializes the hash table for variables and returns an OK or ERROR value
   */
GHashTable*
var_table_init( void ) {

  GHashTable* var_table = g_hash_table_new( (GHashFunc)var_hash_func, (GCompareFunc)var_comp_func );
  return var_table;
}

   /*
     Deletes the hash table for variables and returns an OK value
   */
int
var_table_delete( GHashTable*   var_table,
                  fb_allocator* expr_alloc ) {
  
  assert( var_table != NULL );
  assert( expr_alloc != NULL );
  
  g_hash_table_foreach_remove( var_table, var_free_func, expr_alloc );
  g_hash_table_destroy( var_table );  
  
  return VAR_OK;
}

   /*
     Displays the contents of the hash table for variables
   */
void
var_table_display( FILE*       out,
                   GHashTable* const_table,
                   GHashTable* var_table,
                   GHashTable* bel_table ) {
  quatro p;

  p.first_  = (void*)out;
  p.second_ = (void*)const_table;
  p.third_  = (void*)var_table;
  p.fourth_ = (void*)bel_table;
  
  fprintf( out, "\nTHE CONTENTS OF THE VARIABLE TABLE:\n");
  g_hash_table_foreach( var_table, (GHFunc)var_display_func, &p );
}

   /*
     Takes a variable name, converts it to upper case, and returns the pointer to
     the variable if it's found or NULL if the variable is not found
   */
gpointer
var_table_lookup( char*       var_name,
                  GHashTable* var_table ) {
  str_to_upper( var_name );
  return g_hash_table_lookup( var_table, var_name );
}

   /*
     Takes a variable name and a variable, converts the name to upper case, inserts        
     the variable into the variable table using the name as a key, and returns an OK value
   */
int
var_table_add( char*       var_name,
               variable*   value,
               GHashTable* var_table ) {
  str_to_upper( var_name );
  g_hash_table_insert( var_table, var_name, value );

  return VAR_OK;
}

   /*
     Takes a variable, a name, an expression, and a line number, initializes the variable 
     with these (the variable will contain the expression), and returns an OK value
   */
int
var_init( variable*    ptr,
          const char*  name,
          GNode*       expr,
          int          linenum,
          const char*  filename,
          int          local_linenum ) {
  
  assert( ptr != NULL );
  assert( expr != NULL );
  assert( name != NULL );
  assert( strlen(name) < VAR_NAME_LENGTH );

  strcpy( ptr->name_, name );
  ptr->expr_    = expr;

  ptr->linenum_       = linenum;
  ptr->filename_      = filename;
  ptr->local_linenum_ = local_linenum;

  return VAR_OK;
}

variable*
var_alloc_init( const char*   name,
                GNode*        expr,
                int           linenum,
                const char*   filename,
                int           local_linenum,
                fb_allocator* var_alloc ) {
  variable* var = NULL;
  
  assert( expr != NULL );
  assert( name != NULL );
  assert( var_alloc != NULL );
  assert( strlen(name) < VAR_NAME_LENGTH );

  PRIVATE_ALLOCATE( var, var_alloc );
  if ( var != NULL ) {
    int errcode = var_init( var, name, expr, linenum, filename, local_linenum );
    if ( errcode != VAR_OK ) {
      PRIVATE_DEALLOCATE( var, var_alloc );
      var = NULL;
    }
  }
  return var;
}

   /*
     Takes a variable, frees the memory used by it, and returns an OK value
   */
int
var_delete( variable*     ptr,
            fb_allocator* expr_alloc ) {
  
  assert( ptr != NULL );
  assert( expr_alloc != NULL );

  if ( ptr->expr_ != NULL ) {
    expr_node_delete( ptr->expr_, expr_alloc );
    ptr->expr_ = NULL;
  }

  return VAR_OK;
}

   /*
     Takes a pointer to a variable array, allocates the array, copies the contents 
     of the variable table to that array, and returns the array's size
   */
int
var_table_to_array( variable***  var_arr,
                    GHashTable*  var_table ) {
  
  variable **arr_ptr;
  size_t size = g_hash_table_size( var_table );
  arr_ptr = *var_arr = (variable**)malloc( size*sizeof(variable*) );
  g_hash_table_foreach( var_table, (GHFunc)var_table_el_to_array_el, &arr_ptr );

  return size;
}

   /*
     takes the variable array and check for circular referencies
    */

static void
var_check_circ( GNode*          expr,
                const variable* vartocomp,
                var_link**      res,
                GHashTable*     var_table,
                GHashTable*     bel_table ) {

  static const variable* var_to_compare;

  GNode*       node = NULL;
  expr_struct* data = NULL;

  assert( expr != NULL );

  if ( vartocomp != NULL ) {
    var_to_compare = vartocomp;
    assert( *res == NULL );
  }

  if ( *res == NULL ) {

    data = (expr_struct*)(expr->data);
    
    switch ( data->kind_ ) {
      case NUMBER_EXPR :
        break;
      case STRING_EXPR :
        /* fprintf(stderr, "error ! string \"%s\" used in an algebraic expression\n", data->svalue_ );*/
        send_to_stderr_stream(stderr, "error ! string \"%s\" used in an algebraic expression\n", data->svalue_ );
        break;
      case NUM_IDENT_EXPR :
        break;
      case STR_IDENT_EXPR :
        /* fprintf(stderr, "error ! string id %s used in an algebraic expression\n", data->svalue_); */
        send_to_stderr_stream(stderr, "error ! string id %s used in an algebraic expression\n", data->svalue_);
        break;
      case VAR_IDENT_EXPR :
        {
          variable* var = (variable*)var_table_lookup( data->svalue_, var_table );
          if ( var == NULL ) {
            /* fprintf(stderr, "error ! variable %s never defined\n", data->svalue_ ); */
            send_to_stderr_stream(stderr, "error ! variable %s never defined\n", data->svalue_ );
            bmlfactory_exit();
          } else {
            if ( var == var_to_compare ) {
              *res = (var_link*)16; /* !!! */
            }
          }
        }
        break;
      case BEAMEL_LEN_EXPR :
        {
          beam_element* bel = (beam_element*)bel_table_lookup( data->svalue_, bel_table );
          if ( bel == NULL ) {
            /* fprintf(stderr, "error ! beam element %s never defined\n", data->svalue_ ); */
            send_to_stderr_stream(stderr, "error ! beam element %s never defined\n", data->svalue_ ); 
            bmlfactory_exit();
          } else {
            var_check_circ( bel->length_, NULL, res, var_table, bel_table );
          }
        }
        break;
      case BRACKETS_EXPR:
        var_check_circ( g_node_first_child( expr ), NULL, res, var_table, bel_table );
        break;
      case UN_PLUS_EXPR :
        var_check_circ( g_node_first_child( expr ), NULL, res, var_table, bel_table );
        break;
      case UN_MINUS_EXPR :
        var_check_circ( g_node_first_child( expr ), NULL, res, var_table, bel_table );
        break;
      case ADD_OP_EXPR :
        node = g_node_first_child( expr );
        ( var_check_circ( node, NULL, res, var_table, bel_table ), var_check_circ( node->next, NULL, res, var_table, bel_table ) );
        break;
      case SUB_OP_EXPR :
        node = g_node_first_child( expr );
        ( var_check_circ( node, NULL, res, var_table, bel_table ), var_check_circ( node->next, NULL, res, var_table, bel_table ) );
        break;
      case MUL_OP_EXPR :
        node = g_node_first_child( expr );
        ( var_check_circ( node, NULL, res, var_table, bel_table ), var_check_circ( node->next, NULL, res, var_table, bel_table ) );
        break;
      case DIV_OP_EXPR :
        node = g_node_first_child( expr );
        ( var_check_circ( node, NULL, res, var_table, bel_table ), var_check_circ( node->next, NULL, res, var_table, bel_table ) );
        break;
      case POW_OP_EXPR :
        node = g_node_first_child( expr );
        ( var_check_circ( node, NULL, res, var_table, bel_table ), var_check_circ( node->next, NULL, res, var_table, bel_table ) );
        break;
      case SQRT_EXPR :
        var_check_circ( g_node_first_child( expr ), NULL, res, var_table, bel_table );
        break;
      case LOG_EXPR :
        var_check_circ( g_node_first_child( expr ), NULL, res, var_table, bel_table );
        break;
      case EXP_EXPR :
        var_check_circ( g_node_first_child( expr ), NULL, res, var_table, bel_table );
        break;
      case SIN_EXPR :
        var_check_circ( g_node_first_child( expr ), NULL, res, var_table, bel_table );
        break;
      case COS_EXPR :
        var_check_circ( g_node_first_child( expr ), NULL, res, var_table, bel_table );
        break;
      case TAN_EXPR :
        var_check_circ( g_node_first_child( expr ), NULL, res, var_table, bel_table );
        break;
      case ASIN_EXPR :
        var_check_circ( g_node_first_child( expr ), NULL, res, var_table, bel_table );
        break;
      case ABS_EXPR :
        var_check_circ( g_node_first_child( expr ), NULL, res, var_table, bel_table );
        break;
      case MAX_EXPR :
        node = g_node_first_child( expr );
        ( var_check_circ( node, NULL, res, var_table, bel_table ),var_check_circ( node->next, NULL, res, var_table, bel_table ) );
        break;
      case MIN_EXPR :
        node = g_node_first_child( expr );
        ( var_check_circ( node, NULL, res, var_table, bel_table ), var_check_circ( node->next, NULL, res, var_table, bel_table ) );
        break;
      default :
        /* fprintf(stderr, "error ! unknown expression type\n"); */
        bmlfactory_exit();
    }
  }
  return;
}

var_link*
var_table_check_circ( variable*   var_arr[],
                      int         var_arr_size,
                      int*        var_idx,
                      GHashTable* var_table,
                      GHashTable* bel_table ) {

  int i;
  var_link* lres;
  
  assert( var_arr != NULL );
  assert( var_arr_size > 0 );
  assert( var_idx != NULL );

  for( i = 0; i < var_arr_size; ++i ) {
    lres = NULL;
    var_check_circ( var_arr[i]->expr_, var_arr[i], &lres, var_table, bel_table );
    if ( lres != NULL ) {
      *var_idx = i;
      break;
    }
  }

  return lres;
}

   /* Takes 2 pointers to variables and returns -1, 0, or 1, depending on whether
      the first variable's linenum is less than, equal to, or greater than the second.
   */
int
var_compare( const void* ptr1,
             const void* ptr2 ) {
  
  const variable* var1 = *((variable**)ptr1);
  const variable* var2 = *((variable**)ptr2);
  
  if ( var1->linenum_ > var2->linenum_ ) {
    return 1;
  } else if ( var1->linenum_ < var2->linenum_ ) {
    return -1;
  } else {
    if ( (long)var1 > (long)var2 ) {
      return 1;
    } else if ( (long)var1 < (long)var2 ) {
      return -1;
    } else {
      return 0;
    }
  }
}

   /*
     Takes a node of an expression and a pointer to a list ("user_data").
     If the node contains a reference to a variable, then that variable is
     added to the list. Always returns FALSE.
   */
static gboolean
var_get_refs_helper( GNode*   node,
                     gpointer user_data ) {
  
  pair* p = (pair*)user_data;
  
  expr_struct* data = (expr_struct*)(node->data);
  GList** ref_list  = (GList**)(p->first_);
  if ( data->kind_ == VAR_IDENT_EXPR ) {
    variable* var = (variable*)var_table_lookup( data->svalue_, (GHashTable*)(p->second_) );
    if ( ref_list != NULL ) {
      GList *list_ptr = g_list_first( *ref_list );
      while ( list_ptr != NULL ) {
        if ( (long)(list_ptr->data) == (long)var ) {
          return FALSE;
        }
        
        list_ptr = g_list_next( list_ptr );
      }
    }
    *ref_list = g_list_append( *ref_list, var );
  }

  return FALSE;
}

   /*
     Takes an expression and returns a list of all the variables it references
   */
static GList*
var_get_refs( GNode*      expr,
              GHashTable* var_table ) {
  pair p;
  
  GList* ref_list = NULL;
  
  p.first_  = (void*)(&ref_list);
  p.second_ = (void*)var_table;
  
  g_node_traverse( expr, G_PRE_ORDER, G_TRAVERSE_ALL, -1, (GNodeTraverseFunc)var_get_refs_helper, &p );
  
  return ref_list;
}

   /*
     Takes a pointer to a variable array and the array's size, and sorts 
     the array so that there are no forward variable references
   */
void
var_remove_forward( variable*** var_arr,
                    int         size,
                    GHashTable* var_table ) {
  int i;
  GList *var_list = NULL, *var_list_ptr = NULL, *ref_list = NULL;
  variable **new_arr;
  for ( i = 0; i < size; ++i ) {
    var_list = g_list_append( var_list, *(*var_arr+i) );
  }
  
  var_list_ptr = g_list_first( var_list );
  for ( ;; ) {
    gboolean moved = FALSE;
    
    ref_list = var_get_refs( ((variable*)(var_list_ptr->data))->expr_, var_table );
    
    if ( ref_list != NULL ) {
      GList *list_ptr = g_list_first( ref_list );
      while ( list_ptr != NULL ) {
        GList *tmp = g_list_find( var_list_ptr, list_ptr->data );
        if ( tmp != NULL ) {
          moved = TRUE;
          g_list_remove_link( var_list_ptr, tmp );
          tmp->next = var_list_ptr;
          tmp->prev = var_list_ptr->prev;
          if ( var_list_ptr->prev != NULL )
            (var_list_ptr->prev)->next = tmp;
          
          var_list_ptr->prev = tmp;
        }
        
        list_ptr = g_list_next( list_ptr );
      }
      g_list_free( ref_list );
    }
    if ( moved == TRUE )
      var_list_ptr = g_list_first( var_list );
    else {
      var_list_ptr = g_list_next( var_list_ptr );
      if ( var_list_ptr == NULL)
        break;
    }
  }
  
  new_arr = (variable**)malloc( size*sizeof(variable*) );
  var_list_ptr = g_list_first( var_list );
  for ( i = 0; i < size; ++i ) {
    assert( var_list_ptr != NULL );
    *(new_arr+i) = (variable*)(var_list_ptr->data);
    var_list_ptr = g_list_next( var_list_ptr );
  }
  free( *var_arr );
  g_list_free( var_list );
  *var_arr = new_arr;
}
