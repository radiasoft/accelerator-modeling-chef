   /* -*- C -*- */

#ifndef cfg_output_h
#define cfg_output_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <glib.h>  

void  const_def_output( FILE* out, GHashTable* const_table, GHashTable* var_table, GHashTable* bel_table, GPtrArray* comment_arr );
void  var_def_output( FILE* out, GHashTable* const_table, GHashTable* var_table, GHashTable* bel_table, GPtrArray* comment_arr );
void  bel_def_output( FILE* out, GHashTable* var_table, GHashTable* bel_table, GPtrArray* comment_arr );
void  bml_def_output( FILE* out, GHashTable* bml_table, GPtrArray* comment_arr );

#ifdef __cplusplus
};
#endif

#endif /* cfg_output_h */
