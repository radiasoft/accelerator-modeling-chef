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

#endif /* beamel_table_h */
