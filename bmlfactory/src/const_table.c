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
******  File:      const_table.c
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(str_to_upper_h)
#include "str_to_upper.h"
#endif /* str_to_upper_h */

#if !defined(expression_h)
#include "expression.h"
#endif /* expression_h */

#if !defined(troika_h)
#include "troika.h"
#endif /* troika_h */

#if !defined(const_table_h)
#include "const_table.h"
#endif /* const_table_h */

enum constant_kind current_constant = CONSTANT_UNKNOWN;

   /*
     Takes a string and returns its hash table index
   */
static gint
const_hash_func( gconstpointer str ) {
  return g_str_hash( str );
}

   /*
     Takes 2 strings and returns TRUE if they are equal
   */
static gint
const_comp_func( gconstpointer left,
                 gconstpointer right ) {
  return g_str_equal( left, right );
}

   /*
     Takes a char pointer "key", constant "value", and "user_data" (not used),
     and frees the memory for "key" and "value"
   */
static gboolean
const_free_func( gpointer key,
                 gpointer value,
                 gpointer user_data ) {
  free( key );
  const_delete( (constant*)value, (fb_allocator*)user_data );
  return TRUE;
}

   /*
     Takes a char pointer "key", constant "value", and arr_ptr address "user_data", 
     and makes the array element at "user_data" point to "value"
   */
static void
const_table_el_to_array_el( gpointer key,
                            gpointer value,
                            gpointer user_data ) {
     /* user_data is arr_ptr address! */
  constant** ptr = *((constant***)user_data);
  *ptr = (constant*)value;
  *((constant***)user_data) = ++ptr;
}

   /*
     Adds predefined values to the constant table and returns an OK value
   */
static int
const_add_predef( fb_allocator* expr_alloc,
                  fb_allocator* const_alloc,
                  GHashTable*   const_table ) {
  
     /* Adding PI */
  char*        name = (char*)malloc( 3 );
  constant*    const_ptr;
  expr_struct* data;

  PRIVATE_ALLOCATE( const_ptr, const_alloc );
  
  strcpy( name, "PI" );
  const_init_a( const_ptr, name, 0, NULL, 0, expr_alloc );
  data = (expr_struct*)(const_ptr->expr_->data);
  data->dvalue_ = M_PI;
  data->svalue_ = (char*)malloc( 24 );
  strcpy( data->svalue_, "3.14159265358979323846" );
  const_table_add( name, const_ptr, const_table );

  return CONST_OK;
}

   /*
     Initializes the hash table for constants and returns an OK or ERROR value
   */
GHashTable*
const_table_init( fb_allocator* expr_alloc,
                  fb_allocator* const_alloc ) {

  GHashTable* const_table = g_hash_table_new( (GHashFunc)const_hash_func, (GCompareFunc)const_comp_func );

  if ( const_table != NULL ) {
    const_add_predef( expr_alloc, const_alloc, const_table );
  }

  return const_table;
}

   /*
     Deletes the hash table for constants and returns an OK value
    */
int
const_table_delete( GHashTable*   const_table,
                    fb_allocator* expr_alloc ) {

  assert( const_table != NULL );
  assert( expr_alloc != NULL );
  
  g_hash_table_foreach_remove( const_table, const_free_func, expr_alloc );
  g_hash_table_destroy( const_table );
  
  return CONST_OK;
}

   /*
     Takes a char pointer "key", constant "value", and "user_data" (not used),
     and prints information about "value"
   */
static void
const_display_func( gpointer key,
                    gpointer value,
                    gpointer user_data ) {

  troika* p = ((troika*)user_data);
  FILE* out = (FILE*)(p->first_);
  
  GNode* expr = ((constant*)value)->expr_;
  
  fprintf( out,
           "Constant: %s; defined on line %d in file %s\n",
           ((constant*)value)->name_, ((constant*)value)->local_linenum_, ((constant*)value)->filename_ );
  
  fprintf( out, "Expression: ");
  expr_display( out, expr, (GHashTable*)(p->second_), (GHashTable*)(p->third_) );
  fprintf( out, "\nValue: ");
  if ( ((constant*)value)->svalue_ != NULL ) {
    fprintf( out, "\"%s\"\n", ((constant*)value)->svalue_ );
  } else {
    fprintf( out, "%f\n", ((expr_struct*)(expr->data))->dvalue_ );
  }
}

   /*
     Displays the contents of the hash table for constants
   */
void
const_table_display( FILE*       out,
                     GHashTable* const_table,
                     GHashTable* var_table,
                     GHashTable* bel_table ) {
  troika p;

  p.first_  = (void*)out;
  p.second_ = (void*)var_table;
  p.third_  = (void*)bel_table;
  
  fprintf( out, "\nTHE CONTENTS OF THE CONSTANT TABLE:\n");
  g_hash_table_foreach( const_table, (GHFunc)const_display_func, &p );
}

   /*
     Takes a constant name, converts it to upper case, and returns the pointer to
     the constant if it's found or NULL if the constant is not found
   */
gpointer
const_table_lookup( char*       const_name,
                    GHashTable* const_table ) {
  str_to_upper( const_name );
  return g_hash_table_lookup( const_table, const_name );
}

   /*
     Takes a constant name and a constant, converts the name to upper case, inserts
     the constant into the constant table using the name as a key, and returns an OK value
   */
int
const_table_add( char*       const_name,
                 constant*   value,
                 GHashTable* const_table ) {
  
  str_to_upper( const_name );
  g_hash_table_insert( const_table, const_name, value );

  return CONST_OK;
}

   /*
     Takes a constant, a name, and a line number, initializes the constant
     with these (the numeric value is 0), and returns an OK value
   */
int
const_init_a( constant*     ptr,
              const char*   name,
              int           linenum,
              const char*   filename,
              int           local_linenum,
              fb_allocator* expr_alloc ) {
  
  expr_struct* data = NULL;

  assert( ptr != NULL );
  assert( name != NULL );
  assert( strlen(name) < CONST_NAME_LENGTH );
  assert( expr_alloc != NULL );

  strcpy( ptr->name_, name );
  ptr->svalue_ = NULL;

  PRIVATE_ALLOCATE( data, expr_alloc );
  data->kind_ = NUMBER_EXPR;
  data->dvalue_ = 0.0;
  data->svalue_ = (char*)malloc( 3 );
  strcpy( data->svalue_, "0." );

  ptr->expr_ = g_node_new( data );
  
  ptr->linenum_       = linenum;
  ptr->filename_      = filename;
  ptr->local_linenum_ = local_linenum;

  return CONST_OK;
}

   /*
     Takes a constant, a name, a size, and a line number, initializes the constant         
     with these (the constant is a string of given size), and returns an OK or ERROR value
   */
int
const_init_b( constant*     ptr,
              const char*   name,
              size_t        ssize,
              int           linenum,
              const char*   filename,
              int           local_linenum,
              fb_allocator* expr_alloc ) {
  
  expr_struct* data;

  assert( ptr != NULL );
  assert( ssize > 0 );
  assert( name != NULL );
  assert( strlen(name) < CONST_NAME_LENGTH );
  assert( expr_alloc != NULL );

  strcpy( ptr->name_, name );

  PRIVATE_ALLOCATE( data, expr_alloc );
  data->kind_ = STRING_EXPR;
  data->svalue_  = (char*)malloc( ssize );
  if ( data->svalue_ != NULL ) {
    memset( data->svalue_, 0, ssize );
  } else {
    return CONST_INIT_ERR;
  }

  ptr->expr_    = g_node_new( data );
  
  ptr->linenum_       = linenum;
  ptr->filename_      = filename;
  ptr->local_linenum_ = local_linenum;

  ptr->svalue_  = (char*)malloc( ssize );
  if ( ptr->svalue_ != NULL ) {
    memset( ptr->svalue_, 0, ssize );
    return CONST_OK;
  } else {
    return CONST_INIT_ERR;
  }
}

   /*
     Takes a constant, a name, a string, and a line number, initializes the constant
     with these (the constant will contain the string), and returns an OK or ERROR value
   */
int
const_init_c( constant*     ptr,
              const char*   name,
              const char*   sptr,
              int           linenum,
              const char*   filename,
              int           local_linenum,
              fb_allocator* expr_alloc ) {
  
  expr_struct *data;

  assert( ptr != NULL );
  assert( sptr != NULL );
  assert( name != NULL );
  assert( strlen(name) < CONST_NAME_LENGTH );
  assert( expr_alloc != NULL );

  strcpy( ptr->name_, name );

  PRIVATE_ALLOCATE( data, expr_alloc );
  data->kind_ = STRING_EXPR;
  data->svalue_  = (char*)malloc( strlen(sptr) + 1 );
  if ( data->svalue_ != NULL ) {
    strcpy( data->svalue_, sptr );
  } else {
    return CONST_INIT_ERR;
  }

  ptr->expr_    = g_node_new( data );
  
  ptr->linenum_       = linenum;
  ptr->filename_      = filename;
  ptr->local_linenum_ = local_linenum;
  
  ptr->svalue_  = (char*)malloc( strlen(sptr) + 1 );
  if ( ptr->svalue_ != NULL ) {
    strcpy( ptr->svalue_, sptr );
    return CONST_OK;
  } else {
    return CONST_INIT_ERR;
  }
}

   /*
     Takes a constant, a name, an expression, and a line number, initializes the constant 
     with these (the constant will contain the expression), and returns an OK value
   */
int
const_init_d( constant*   ptr,
              const char* name,
              GNode*      expr,
              int         linenum,
              const char* filename,
              int         local_linenum ) {
  
  assert( ptr != NULL );
  assert( expr != NULL );
  assert( name != NULL );
  assert( strlen(name) < CONST_NAME_LENGTH );

  strcpy( ptr->name_, name );
  ptr->svalue_  = NULL;
  ptr->expr_    = expr;

  ptr->linenum_       = linenum;
  ptr->filename_      = filename;
  ptr->local_linenum_ = local_linenum;

  return CONST_OK;
}

   /*
     Takes a constant, a name, another constant, its name, and a line number, initializes the constant
     with these (the first constant will be equal to the second), and returns an OK or ERROR value
   */
int
const_init_e( constant*       dst,
              const char*     name,
              const constant* src,
              const char*     src_name,
              int             linenum,
              const char*     filename,
              int             local_linenum,
              fb_allocator*   expr_alloc ) {
  
  expr_struct* data;

  assert( dst != NULL );
  assert( src != NULL );
  assert( name != NULL );
  assert( strlen(name) < CONST_NAME_LENGTH );
  assert( expr_alloc != NULL );

  strcpy( dst->name_, name );
  
  dst->linenum_       = linenum;
  dst->filename_      = filename;
  dst->local_linenum_ = local_linenum;

  PRIVATE_ALLOCATE( data, expr_alloc );
  data->svalue_ = (char*)src_name;

  if ( src->svalue_ == NULL ) {
    data->kind_ = NUM_IDENT_EXPR;
    data->dvalue_ = ((expr_struct*)(((GNode*)(src->expr_))->data))->dvalue_;

    dst->svalue_ = NULL;
  } else {
    data->kind_ = STR_IDENT_EXPR;

    dst->svalue_ = (char*)malloc( strlen(src->svalue_) + 1);
    if ( dst->svalue_ != NULL ) {
      strcpy( dst->svalue_, src->svalue_ );
    } else {
      return CONST_INIT_ERR;
    }
  }

  dst->expr_ = g_node_new( data );

  return CONST_OK;
}

   /*
     Takes a constant, frees the memory used by it, and returns an OK value
   */
int
const_delete( constant*     ptr,
              fb_allocator* expr_alloc ) {
  
  assert( ptr != NULL );
  assert( expr_alloc != NULL );

  if ( ptr->svalue_ != NULL ) {
    free( ptr->svalue_ );
    ptr->svalue_ = NULL;
  }

  if ( ptr->expr_ != NULL ) {
    expr_node_delete( ptr->expr_, expr_alloc );
    ptr->expr_ = NULL;
  }

  return CONST_OK;
}

   /*
     Takes a pointer to a constant array, allocates the array, copies the contents 
     of the constant table to that array, and returns the array's size
   */
int
const_table_to_array( constant*** const_arr,
                      GHashTable* const_table ) {
  constant **arr_ptr;
  size_t size = g_hash_table_size( const_table );
  arr_ptr = *const_arr = (constant**)malloc( size*sizeof(constant*) );
  g_hash_table_foreach( const_table, (GHFunc)const_table_el_to_array_el, &arr_ptr );

  return size;
}

   /*
     Takes a node of an expression and a pointer to a char pointer "user_data". If 
     the node contains the identifier to which "user_data" points, then that char  
     pointer is set to NULL and TRUE is returned. Otherwise, FALSE is returned.
   */
static gboolean
const_find_reference( GNode*   node,
                      gpointer user_data ) {
  
  expr_struct* data = (expr_struct*)(node->data);
  char** ptr = (char**)user_data;
  if ( data->kind_ == NUM_IDENT_EXPR || data->kind_ == STR_IDENT_EXPR ) {
    if ( strcmp( data->svalue_, *ptr ) == 0 ) {
      *ptr = NULL;
      return TRUE;
    }
  }
  return FALSE;
}

   /*
     Takes an expression and a name, and returns TRUE if the expression contains the name
   */
static gboolean
const_references( GNode* expr,
                  char*  name ) {
  char* ptr = name;
  g_node_traverse( expr, G_PRE_ORDER, G_TRAVERSE_ALL, -1,
                   (GNodeTraverseFunc)const_find_reference,
                   &ptr );
  if ( ptr == NULL )
    return TRUE;
  else
    return FALSE;
}

   /*
     Takes 2 pointers to constants and returns -1, 0, or 1, depending on whether
     the first constant's linenum is less than, equal to, or greater than the second.
   */
int
const_compare( const void* ptr1,
               const void* ptr2 ) {
  
  const constant* const1 = *((constant**)ptr1);
  const constant* const2 = *((constant**)ptr2);
  
  if ( const1->linenum_ > const2->linenum_ ) {
    return 1;
  } else if ( const1->linenum_ < const2->linenum_ ) {
    return -1;
  } else { /*
             This correctly arranges 2 (no more!!!) constant declarations made on the same line
           */
    if ( const_references( const1->expr_, (char*)(const2->name_) ) == TRUE ) {
      return 1;
    } else {
      return -1;
    }
  }
}
