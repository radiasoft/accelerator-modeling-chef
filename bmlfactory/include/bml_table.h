   /* -*- C -*- */

#ifndef bml_table_h
#define bml_table_h

#include <stdio.h>
#include <glib.h>

#include "beam_line.h"

#define BML_TABLE_OK       0
#define BML_TABLE_INIT_ERR 1
#define BML_TABLE_ADD_ERR  2

#ifdef __cplusplus
extern "C" {
#endif

GHashTable*  bml_table_init( void );
int          bml_table_delete( GHashTable* bml_table, fb_allocator* bml_alloc );
void         bml_table_display( FILE* out, GHashTable* bml_table );

int          bml_table_lookup( char* bml_name, GHashTable* bml_table );
int          bml_table_add( char* bml_name, beam_line* value, GHashTable* bml_table );
int          bml_table_to_array( beam_line*** bml_arr, GHashTable* bml_table );

int          bml_compare( const void* ptr1, const void* ptr2 );
void         bml_remove_forward( beam_line*** bml_arr, int size, GHashTable* bml_table );

#ifdef __cplusplus
};
#endif

#endif /* bml_table_h */
