/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      bml_table.h
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
#ifndef BML_TABLE_H
#define BML_TABLE_H

#include <stdio.h>
#include <glib.h>
#include <bmlfactory/beam_line.h>

#define BML_TABLE_OK       0
#define BML_TABLE_INIT_ERR 1
#define BML_TABLE_ADD_ERR  2

#ifdef __cplusplus
extern "C" {
#endif

GHashTable*  bml_table_init( void );
int          bml_table_delete( GHashTable* bml_table, fb_allocator* bml_alloc);
void         bml_table_display( FILE* out, GHashTable* bml_table );

gpointer     bml_table_lookup( char* bml_name, GHashTable* bml_table );
int          bml_table_add( char* bml_name, beam_line* value, GHashTable* bml_table );
int          bml_table_to_array( beam_line*** bml_arr, GHashTable* bml_table );

int          bml_compare( const void* ptr1, const void* ptr2 );
void         bml_remove_forward( beam_line*** bml_arr, int size, GHashTable* bml_table );

#ifdef __cplusplus
};
#endif

#endif /* BML_TABLE_H */
