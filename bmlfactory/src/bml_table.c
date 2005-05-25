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
******  File:      bml_table.c
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

#include <str_to_upper.h>
#include <beam_element.h>
#include <bml_table.h>
#include <pair.h>

   /*
     Takes a string and returns its hash table index
   */
static gint
bml_hash_func( gconstpointer str ) {
  return g_str_hash( str );
}

   /*
     Takes 2 strings and returns TRUE if they are equal
   */
static gint
bml_comp_func( gconstpointer left,
               gconstpointer right ) {
  return g_str_equal( left, right );
}

   /*
     Takes a char pointer "key", bml "value", and "user_data" (allocator), 
     and frees the memory for "key" and "value"
   */
static gboolean
bml_free_func( gpointer key,
               gpointer value,
               gpointer user_data ) {
  free( key );
  beam_line_delete( ((beam_line*) value), (fb_allocator*)user_data );
  return TRUE;
}

   /*
     Takes a char pointer "key", bml "value", and arr_ptr address "user_data", 
     and makes the array element at "user_data" point to "value"
   */
static void
bml_table_el_to_array_el( gpointer key,
                          gpointer value,
                          gpointer user_data ) {
     /* user_data is arr_ptr address */
  beam_line** ptr = *((beam_line***)user_data);

  assert( ptr != NULL );
  
  *ptr = (beam_line*)value;
  *((beam_line***)user_data) = ++ptr;
}

   /*
     Takes a char pointer "data" and "user_data" (not used), and prints "data"
   */
static void
bml_print_element( gpointer data,
                   gpointer user_data ) {
  fprintf( (FILE*)user_data, "%s; ", (char*)data );
}

   /*
     Takes a char pointer "key", bml "value", and "user_data" (not used),
     and prints information about "value"
   */
static void
bml_display_func( gpointer key,
                  gpointer value,
                  gpointer user_data ) {
  
  beam_line* bml = (beam_line*)value;
  assert( bml != NULL );
  
  fprintf( (FILE*)user_data, "Beam line: %s; defined on line %d in file %s\n", bml->name_, bml->local_linenum_, bml->filename_ );
  fprintf( (FILE*)user_data, "Elements: ");
  g_list_foreach( bml->bel_list_, (GFunc)bml_print_element, user_data );
  fprintf( (FILE*)user_data, "\n" );
}

   /*
     Initializes the hash table for bmls and returns an OK or ERROR value
   */
GHashTable*
bml_table_init( void ) {
  
  GHashTable* bml_table = g_hash_table_new( (GHashFunc)bml_hash_func, (GCompareFunc)bml_comp_func );
  return bml_table;
}

   /*
     Deletes the hash table for bmls and returns an OK value
   */
int
bml_table_delete( GHashTable* bml_table, fb_allocator* bml_alloc) {
  
  g_hash_table_foreach_remove( bml_table, bml_free_func, (void*)bml_alloc );  
  g_hash_table_destroy( bml_table );
  
  return BML_TABLE_OK;
}

   /*
     Displays the contents of the hash table for bmls
   */
void
bml_table_display( FILE* out, GHashTable* bml_table ) {
  fprintf( out, "\nTHE CONTENTS OF THE BEAM LINE TABLE:\n" );
  g_hash_table_foreach( bml_table, (GHFunc)bml_display_func, (void*)out );
}

   /*
     Take a bml name, converts it to upper case, and returns the pointer to
     the bml if it's found or NULL if the bml is not found
   */
gpointer
bml_table_lookup( char*       bml_name,
                  GHashTable* bml_table ) {

  assert( bml_name != NULL );
  assert( bml_table != NULL );
  
  str_to_upper( bml_name );
  return g_hash_table_lookup( bml_table, bml_name );
}

   /*
     Takes a bml name and a bml, converts the name to upper case, inserts the bml 
     into the bml table using the name as a key, and returns an OK value
   */
int
bml_table_add( char*       bml_name,
               beam_line*  value,
               GHashTable* bml_table ) {
  
  assert( bml_name != NULL );
  assert( bml_table != NULL );
  
  str_to_upper( bml_name );
  g_hash_table_insert( bml_table, bml_name, value );

  return BML_TABLE_OK;
}

   /*
     Takes a pointer to a bml array, allocates the array, copies the contents
     of the bml table to that array, and returns the array's size
   */
int
bml_table_to_array( beam_line*** bml_arr,
                    GHashTable*  bml_table ) {
  
  beam_line **arr_ptr;  
  size_t size;

  assert( bml_arr != NULL );
  assert( bml_table != NULL );
  
  size = g_hash_table_size( bml_table );
  
  arr_ptr = *bml_arr = (beam_line**)malloc( size*sizeof(beam_line*) );
  g_hash_table_foreach( bml_table, (GHFunc)bml_table_el_to_array_el, &arr_ptr );

  return size;
}

   /*
     Takes 2 pointers to bmls and returns -1, 0, or 1, depending on whether      
     the first bml's linenum is less than, equal to, or greater than the second.
   */
int
bml_compare( const void* ptr1,
             const void* ptr2 ) {
  
  const beam_line* bml1 = NULL;
  const beam_line* bml2 = NULL;
  
  assert( ptr1 != NULL );
  assert( ptr2 != NULL );

  bml1 = *((beam_line**)ptr1);
  bml2 = *((beam_line**)ptr2);

  if ( bml1->linenum_ > bml2->linenum_ ) {
    return 1;
  } else if ( bml1->linenum_ < bml2->linenum_ ) {
    return -1;
  } else {
    if ( (long)bml1 > (long)bml2 ) {
      return 1;
    } else if ( (long)bml1 < (long)bml2 ) {
      return -1;
    } else {
      return 0;
    }
  }
}

   /*
     Takes a name and a pointer to a list ("user_data"). If the name refers to    
     a beam line, then that beam line is added to the list. Always returns FALSE.
   */
static gboolean
bml_get_refs_helper( char*    name,
                     gpointer user_data ) {
  
  pair* p = (pair*)user_data;
  
  GList** ref_list = (GList**)(p->first_);
  beam_line* bml = (beam_line*)bml_table_lookup( ++name, (GHashTable*)(p->second_) );
  
  if ( bml != NULL ) {
    if ( ref_list != NULL ) {
      GList *list_ptr = g_list_first( *ref_list );
      while ( list_ptr != NULL ) {
        if ( (long)(list_ptr->data) == (long)bml )
          return FALSE;
        
        list_ptr = g_list_next( list_ptr );
      }
    }
    *ref_list = g_list_append( *ref_list, bml );
  }
  
  return FALSE;
}

   /*
     Takes a bel list and returns a list of all the beam lines it contains
   */
static GList*
bml_get_refs( GList*      bel_list,
              GHashTable* bml_table ) {
  
  pair p;
  GList* ref_list = NULL;
  
  p.first_  = (void*)&ref_list;
  p.second_ = (void*)bml_table;
  
  g_list_foreach( bel_list, (GFunc)bml_get_refs_helper, &p );
  
  return ref_list;
}

   /*
     Takes a pointer to a bml array and the array's size, and sorts 
     the array so that there are no forward bml references
   */
void
bml_remove_forward( beam_line*** bml_arr,
                    int          size,
                    GHashTable*  bml_table ) {
  int i;

  GList *bml_list = NULL, *bml_list_ptr = NULL, *ref_list = NULL;
  beam_line **new_arr;
  for ( i = 0; i < size; ++i ) {
    bml_list = g_list_append( bml_list, *(*bml_arr+i) );
  }
  
  bml_list_ptr = g_list_first( bml_list );
  for ( ;; ) {
    gboolean moved = FALSE;
    ref_list = bml_get_refs( ((beam_line*)(bml_list_ptr->data))->bel_list_, bml_table );
    if ( ref_list != NULL ) {
      GList *list_ptr = g_list_first( ref_list );
      while ( list_ptr != NULL ) {
        GList *tmp = g_list_find( bml_list_ptr, list_ptr->data );
        if ( tmp != NULL ) {
          moved = TRUE;
          g_list_remove_link( bml_list_ptr, tmp );
          tmp->next = bml_list_ptr;
          tmp->prev = bml_list_ptr->prev;
          if ( bml_list_ptr->prev != NULL )
            (bml_list_ptr->prev)->next = tmp;
          
          bml_list_ptr->prev = tmp;
        }
        
        list_ptr = g_list_next( list_ptr );
      }
      g_list_free( ref_list );
    }
    if ( moved == TRUE )
      bml_list_ptr = g_list_first( bml_list );
    else {
      bml_list_ptr = g_list_next( bml_list_ptr );
      if ( bml_list_ptr == NULL)
        break;
    }
  }

  new_arr = (beam_line**)malloc( size*sizeof(beam_line*) );
  bml_list_ptr = g_list_first( bml_list );
  for ( i = 0; i < size; ++i ) {
    assert( bml_list_ptr != NULL );
    *(new_arr+i) = (beam_line*)(bml_list_ptr->data);
    bml_list_ptr = g_list_next( bml_list_ptr );
  }
  free( *bml_arr );
  g_list_free( bml_list );

  *bml_arr = new_arr;   

}
