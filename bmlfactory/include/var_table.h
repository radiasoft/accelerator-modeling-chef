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

#ifndef var_table_h
#define var_table_h

#include <stddef.h>
#include <glib.h>

#if !defined(fb_allocator_h)
#include "fb_allocator.h"
#endif /* fb_allocator.h */

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


#endif /* var_table_h */
