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

gpointer     bml_table_lookup( char* bml_name, GHashTable* bml_table );
int          bml_table_add( char* bml_name, beam_line* value, GHashTable* bml_table );
int          bml_table_to_array( beam_line*** bml_arr, GHashTable* bml_table );

int          bml_compare( const void* ptr1, const void* ptr2 );
void         bml_remove_forward( beam_line*** bml_arr, int size, GHashTable* bml_table );

#ifdef __cplusplus
};
#endif

#endif /* bml_table_h */
