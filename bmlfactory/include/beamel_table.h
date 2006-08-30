/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      beamel_table.h
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
**************************************************************************
*************************************************************************/
#ifndef BMLFACTORY_BEAMEL_TABLE_H
#define BMLFACTORY_BEAMEL_TABLE_H

#include <glib.h>
#include <bmlfactory/beam_element.h>
#include <bmlfactory/matrix.h>
#include <bmlfactory/madparser_types.h>

#define BEL_OK             0
#define BEL_TABLE_INIT_ERR 1
#define BEL_TABLE_ADD_ERR  2

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { 

fb_allocator *   bel_alloc_;  
fb_allocator *   expr_alloc_; 

}  bel_expr_alloc_struct;



GHashTable* bel_table_init( void );
int         bel_table_delete( GHashTable* bel_table, fb_allocator* bel_alloc,  fb_allocator* expr_alloc );
void        bel_table_display( FILE* out, GHashTable* var_table, GHashTable* bel_table );

gpointer    bel_table_lookup( char* bel_name, GHashTable* bel_table );
int         bel_table_add( char* bel_name, beam_element* value, GHashTable* bel_table );
int         bel_table_to_array( beam_element*** bel_arr, GHashTable* bel_table );

int         bel_compare( const void* ptr1, const void* ptr2 );

#ifdef __cplusplus
};
#endif

#endif /* BMLFACTORY_BEAMEL_TABLE_H */
