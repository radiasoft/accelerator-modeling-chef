/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      var_table.h
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
**************************************************************************/
#ifndef BMLFACTORY_VAR_TABLE_H
#define BMLFACTORY_VAR_TABLE_H

#include <stddef.h>
#include <glib.h>
#include <bmlfactory/fb_allocator.h>


#define VAR_OK             0
#define VAR_TABLE_INIT_ERR 1
#define VAR_INIT_ERR       2

#define VAR_NAME_LENGTH 128


#if defined (__cplusplus)
extern "C" {
#endif

typedef  struct {
    char        name_[VAR_NAME_LENGTH];
    GNode*      expr_;
    int         linenum_;
    const char* filename_;
    int         local_linenum_;
} variable;

struct var_link_;

typedef struct var_link_ {
    variable*         var_;
    struct var_link_* next_;
} var_link;



int          var_init( variable* ptr, const char* name, GNode* expr, int linenum, const char* filename, int local_linenum );
variable*    var_alloc_init( const char* name, GNode* expr, int linenum, const char* filename, int local_linenum, fb_allocator* alloc );

int          var_delete( variable* ptr, fb_allocator* expr_alloc );

GHashTable*  var_table_init( void );
int          var_table_delete( GHashTable* var_table, fb_allocator* expr_alloc );
void         var_table_display( FILE* out, GHashTable* const_table, GHashTable* var_table, GHashTable* bel_table );

gpointer     var_table_lookup( char* var_name, GHashTable* var_table );
int          var_table_add( char* var_name, variable* value, GHashTable* var_table );
int          var_table_to_array( variable*** var_arr, GHashTable* var_table );

var_link*    var_table_check_circ( variable* var_arr[], int var_arr_size, int* var_idx, GHashTable* var_table, GHashTable* bel_table );
int          var_compare( const void* ptr1, const void* ptr2 );
void         var_remove_forward( variable*** var_arr, int size, GHashTable* var_table );

#if defined (__cplusplus)
}
#endif


#endif /* BMLFACTORY_VAR_TABLE_H */
