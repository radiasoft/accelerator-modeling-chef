   /* -*- C -*- */

#ifndef beamel_table_h
#define beamel_table_h

#include <glib.h>

#if !defined(beam_element_h)
#include "beam_element.h"
#endif /* beam_element_h */

#if !defined(matrix_h)
#include "matrix.h"
#endif /* matrix_h */

#define BEL_OK             0
#define BEL_TABLE_INIT_ERR 1
#define BEL_TABLE_ADD_ERR  2

   /*
extern beam_element* curr_bel;
extern char curr_bel_type[];
   */

#ifdef __cplusplus
extern "C" {
#endif

GHashTable* bel_table_init( void );
int         bel_table_delete( GHashTable* bel_table, fb_allocator* bel_alloc );
void        bel_table_display( FILE* out, GHashTable* var_table, GHashTable* bel_table );

int         bel_table_lookup( char* bel_name, GHashTable* bel_table );
int         bel_table_add( char* bel_name, beam_element* value, GHashTable* bel_table );
int         bel_table_to_array( beam_element*** bel_arr, GHashTable* bel_table );

int         bel_compare( const void* ptr1, const void* ptr2 );

#ifdef __cplusplus
};
#endif

#endif /* beamel_table_h */
