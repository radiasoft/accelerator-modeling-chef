/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      comment_arr.c
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
******                                                            
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


   /* -*- C -*- */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bmlfactory/fb_allocator.h>
#include <bmlfactory/comment_arr.h>


   /*
     Initializes the array for comments and returns an OK or ERROR value
   */
GPtrArray*
comment_arr_init( void ) {

  GPtrArray* comment_arr = g_ptr_array_new();
  return comment_arr;
}

   /*
     Deletes the array for comments and returns an OK value
   */
int
comment_arr_delete( GPtrArray* comment_arr ) {
  int i;
  
  assert( comment_arr != NULL );
  
  for ( i = 0; i < (int)comment_arr->len; ++i ) {
    comment *ptr = (comment*)g_ptr_array_index( comment_arr, i );
    if ( ptr->svalue_ != NULL ) {
      free( ptr->svalue_ );
      ptr->svalue_ = NULL;
    }
  }
  g_ptr_array_free( comment_arr, FALSE );
  return COMMENT_OK;
}

   /*
     Displays the contents of the array for comments
    */
void
comment_arr_display( FILE*      out,
                     GPtrArray* comment_arr ) {
  
  int i;

  assert( out != NULL );
  assert( comment_arr != NULL );
  
  fprintf( out, "\nTHE CONTENTS OF THE COMMENT TABLE:\n");
  
  for ( i = 0; i < (int)comment_arr->len; ++i ) {
    comment* ptr = (comment*)g_ptr_array_index( comment_arr, i );
    
    fprintf( out, "Comment: %s;\n", ptr->svalue_ );
    fprintf( out, "Defined on line %d in file %s\n\n", ptr->local_linenum_, ptr->filename_ );
  }
}

   /*
     Takes 2 pointers to comments and returns a negative number, 0,  
     or a positive number, depending on whether the first comments's 
     line number is less than, equal to, or greater than the second.
   */
static int
comment_compare( const void* ptr1,
                 const void* ptr2 ) {
  comment* comment1 = NULL;
  comment* comment2 = NULL;

  assert( ptr1 != NULL );
  assert( ptr2 != NULL );
  
  comment1 = *((comment**)ptr1);
  comment2 = *((comment**)ptr2);
  
  return (comment1->linenum_) - (comment2->linenum_);
}

   /*
     Takes a line number and returns a comment associated with that line,
     if there is one, or returns NULL if there isn't
   */
gpointer
comment_arr_lookup( int         linenum,
                    GPtrArray*  comment_arr ) {
  
  comment tmp, *ptr = &tmp, **result = NULL;

  assert( comment_arr != NULL );
  
  ptr->linenum_       = linenum;
  
  if ( comment_arr->len > 0) {
    result = (comment**)bsearch( &ptr, comment_arr->pdata, comment_arr->len, sizeof( comment* ), comment_compare );
  }

  if ( result == NULL ) {
    return NULL;
  } else {
    return (gpointer)(*result);
  }
}

   /*
     Takes a char pointer (pointing to a comment string) and a line number, 
     creates a comment with that string and line number, inserts the comment 
     into the comment array, and returns an OK or ERROR value
   */
static int
comment_arr_add_new( const char*   sptr,
                     int           linenum,
                     const char*   filename,
                     int           local_linenum,
                     GPtrArray*    comment_arr,
                     fb_allocator* comment_alloc ) {

  comment* ptr;
  
  PRIVATE_ALLOCATE( ptr, comment_alloc );
  assert( comment_arr != NULL );
  
  ptr->svalue_ = (char*)malloc( strlen(sptr) + 1 );
  if ( ptr->svalue_ != NULL ) {
    strcpy( ptr->svalue_, sptr );
  } else {
    return COMMENT_INIT_ERR;
  }

  ptr->linenum_        = linenum;
  ptr->filename_       = filename;
  ptr->local_linenum_  = local_linenum;

  g_ptr_array_add( comment_arr, ptr );

  return COMMENT_OK;
}

   /*
     Takes a char pointer (pointing to a comment string) and a line number. Inserts
     the comment into the comment array, appending if necessary to the one that is
     already related to the given line number. Returns an OK or ERROR value.
   */
int
comment_arr_add( const char*   sptr,
                 int           linenum,
                 const char*   filename,
                 int           local_linenum,                 
                 GPtrArray*    comment_arr,
                 fb_allocator* comment_alloc ) {
  
  assert( sptr != NULL );
  assert( comment_arr != NULL );

  if ( comment_arr->len == 0 ) {
    return comment_arr_add_new( sptr, linenum, filename, local_linenum, comment_arr, comment_alloc );
  } else {
    comment *ptr = (comment*)g_ptr_array_index( comment_arr, comment_arr->len - 1 );
    if ( ptr->linenum_ == linenum ) {
      char* new_sval = (char*)malloc( strlen(ptr->svalue_) + 4 + strlen(sptr) + 1 );
      if ( new_sval != NULL ) {
        strcpy( new_sval, ptr->svalue_ );
        strcat( new_sval, "\n// " );
        strcat( new_sval, sptr );
        free( ptr->svalue_ );
        ptr->svalue_ = new_sval;
      } else {
        return COMMENT_INIT_ERR;
      }
    } else {
      return comment_arr_add_new( sptr, linenum, filename, local_linenum, comment_arr, comment_alloc );
    }
  }

  return COMMENT_OK;
}
