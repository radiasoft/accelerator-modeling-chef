/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      const_table.h
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
**************************************************************************/
#ifndef CONST_TABLE_H
#define CONST_TABLE_H

#include <stddef.h>
#include <glib.h>
#include <madparser_types.h>
#include <fb_allocator.h>


enum constant_kind {
  CONSTANT_UNKNOWN = -1,
  CONSTANT_DOUBLE  = 0,
  CONSTANT_STRING  = 1
};

extern enum constant_kind current_constant;

#define CONST_OK             0
#define CONST_INIT_ERR       1

#define CONST_NAME_LENGTH    128

typedef  struct {
    char        name_[CONST_NAME_LENGTH];
    char*       svalue_;
    GNode*      expr_;
    int         linenum_;
    const char* filename_;
    int         local_linenum_;
} constant;

int          const_init_a( constant* ptr, const char* name, int linenum, const char* filename, int local_linenum, fb_allocator* expr_alloc );
int          const_init_b( constant* ptr, const char* name, size_t ssize, int linenum, const char* filename, int local_linenum, fb_allocator* expr_alloc );
int          const_init_c( constant* ptr, const char* name, const char* sptr, int linenum, const char* filename, int local_linenum, fb_allocator* expr_alloc );
int          const_init_d( constant* ptr, const char* name, GNode* expr, int linenum, const char* filename, int local_linenum );
int          const_init_e( constant* dst, const char* name, const constant* src, const char* src_name, int linenum, const char* filename, int local_linenum, fb_allocator* expr_alloc );

int          const_delete( constant* ptr, fb_allocator* expr_alloc );

GHashTable*  const_table_init( fb_allocator* expr_alloc, fb_allocator* const_alloc );
int          const_table_delete( GHashTable* const_table, fb_allocator* expr_alloc );
void         const_table_display( FILE* out, GHashTable* const_table, GHashTable* var_table, GHashTable* bel_table );

gpointer     const_table_lookup( char* const_name, GHashTable* const_table );
int          const_table_add( char* const_name, constant* value, GHashTable* const_table );
int          const_table_to_array( constant*** const_arr, GHashTable* const_table );

int          const_compare( const void* ptr1, const void* ptr2 );

#endif /* CONST_TABLE_H */
