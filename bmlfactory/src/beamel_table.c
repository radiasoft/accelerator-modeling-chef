#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      beamel_table.c
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#if !defined(str_to_upper_h)
#include "str_to_upper.h"
#endif /* str_to_upper_h */

#if !defined(var_table_h)
#include "var_table.h"
#endif /* var_table_h */

#if !defined(troika_h)
#include "troika.h"
#endif /* troika_h */

#if !defined(beamel_table_h)
#include "beamel_table.h"
#endif /* beamel_table_h */

   /*
     Takes a string and returns its hash table index
   */
static gint
bel_hash_func( gconstpointer str ) {
  return g_str_hash( str );
}

   /*
     Takes 2 strings and returns TRUE if they are equal
   */
static gint
bel_comp_func( gconstpointer left,
               gconstpointer right ) {
  return g_str_equal( left, right );
}

   /*
     Takes a char pointer "key", bel "value", and "user_data" (not used),
     and frees the memory for "key" and "value"
   */
static gboolean
bel_free_func( gpointer key,
               gpointer value,
               gpointer user_data ) {
  free( key );
  beam_element_delete( (beam_element*)value,  (fb_allocator*)user_data );
  return TRUE;
}

   /*
     Takes a char pointer "key", bel "value", and arr_ptr address "user_data", 
     and makes the array element at "user_data" point to "value"
   */
static void
bel_table_el_to_array_el( gpointer key,
                          gpointer value,
                          gpointer user_data ) {
     /* user_data is arr_ptr address */
  beam_element** ptr = *((beam_element***)user_data);
  
  assert( ptr != NULL );
  
  *ptr = (beam_element*)value;
  *((beam_element***)user_data) = ++ptr;
}

   /*
     Takes a char pointer "key", bel "value", and "user_data" (not used),
     and prints information about "value"
   */
static void
bel_display_func( gpointer key,
                  gpointer value,
                  gpointer user_data ) {
  
  beam_element* bel = (beam_element*)value;
  troika*       p   = (troika*)user_data;
  assert( bel != NULL );
  
  beam_element_display( (FILE*)(p->first_), bel, (GHashTable*)(p->second_), (GHashTable*)(p->third_) );
  fprintf( (FILE*)(p->first_), "(Defined on line %d in file %s)\n\n", bel->local_linenum_, bel->filename_ );
}

   /*
     Initializes the hash table for bels and returns an OK or ERROR value
   */
GHashTable*
bel_table_init( void ) {
  
  GHashTable* bel_table = g_hash_table_new( (GHashFunc)bel_hash_func, (GCompareFunc)bel_comp_func );
  return bel_table;
}

   /*
     Deletes the hash table for bels and returns an OK value
   */
int
bel_table_delete( GHashTable*   bel_table,
                  fb_allocator* bel_alloc ) {

  assert( bel_table != NULL );
  
  g_hash_table_foreach_remove( bel_table, bel_free_func, bel_alloc );
  g_hash_table_destroy( bel_table );
  
  return BEL_OK;
}

   /*
     Displays the contents of the hash table for bels
   */
void
bel_table_display( FILE*       out,
                   GHashTable* var_table,
                   GHashTable* bel_table ) {
  troika p;
  
  p.first_  = (void*)out;
  p.second_ = (void*)var_table;
  p.third_  = (void*)bel_table;
  
  printf("\nTHE CONTENTS OF THE BEAM ELEMENT TABLE:\n");
  g_hash_table_foreach( bel_table, (GHFunc)bel_display_func, &p );
}

   /*
     Take a bel name, converts it to upper case, and returns the pointer to
     the bel if it's found or NULL if the bel is not found
   */
gpointer
bel_table_lookup( char*       bel_name,
                  GHashTable* bel_table ) {
  str_to_upper( bel_name );
  return g_hash_table_lookup( bel_table, bel_name );
}

   /*
     Takes a bel name and a bel, converts the name to upper case, inserts the bel 
     into the bel table using the name as a key, and returns an OK value
   */
int
bel_table_add( char*         bel_name,
               beam_element* value,
               GHashTable*   bel_table ) {
  str_to_upper( bel_name );
  g_hash_table_insert( bel_table, bel_name, value );

  return BEL_OK;
}

   /*
     Takes a pointer to a bel array, allocates the array, copies the contents 
     of the bel table to that array, and returns the array's size
   */
int
bel_table_to_array( beam_element*** bel_arr,
                    GHashTable*     bel_table ) {
  beam_element** arr_ptr;
  size_t size = g_hash_table_size( bel_table );
  arr_ptr = *bel_arr = (beam_element**)malloc( size*sizeof(beam_element*) );
  g_hash_table_foreach( bel_table, (GHFunc)bel_table_el_to_array_el, &arr_ptr );

  return size;
}

   /*
     Takes 2 pointers to bels and returns -1, 0, or 1, depending on whether      
     the first bel's linenum is less than, equal to, or greater than the second.
   */
int
bel_compare( const void* ptr1,
                      const void* ptr2 ) {
  const beam_element* bel1 = *((beam_element**)ptr1);
  const beam_element* bel2 = *((beam_element**)ptr2);
  if ( bel1->linenum_ > bel2->linenum_ ) {
    return 1;
  } else if ( bel1->linenum_ < bel2->linenum_ ) {
    return -1;
  } else {
    if ( (long)bel1 > (long)bel2 ) {
      return 1;
    } else if ( (long)bel1 < (long)bel2 ) {
      return -1;
    } else {
      return 0;
    }
  }
}
