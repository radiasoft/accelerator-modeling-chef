/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      fb_allocator.c
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
#include <string.h>

#if !defined(fb_allocator_h)
#include "fb_allocator.h"
#endif /* fb_allocator_h */

typedef struct link_ {
    struct link_* next_;
} link;

struct fb_allocator_ {
    link*        pool_head_;
    char*        block_head_;
    size_t       esize_;
    size_t       alignment_;
    unsigned int nelems_;
};

size_t
fb_alloc_esize( const fb_allocator* alloc ) {
  assert( alloc != NULL );
  return alloc->esize_;
}

   /*
     grow the existing allocator if there is no room for allocation in existing blocks.
     Allocate new block and add it content to the free list 
    */
static void
grow( fb_allocator* alloc ) {

  char* last;
  char* p;
  char* start = (char*)malloc( alloc->esize_*alloc->nelems_ + sizeof(char*) );

  memcpy( start+alloc->esize_*alloc->nelems_, &alloc->block_head_, sizeof(char*) );
  alloc->block_head_ = start;
  
  last  = start+(alloc->nelems_-1)*alloc->esize_;
  
  for( p = start; p < last; p += alloc->esize_ ) { /* alignment! */
    ( (link*)(p) )->next_ = (link*)( p+alloc->esize_ );
  }
  ((link*)( last ))->next_ = 0;
  alloc->pool_head_ = (link*)( start );  
}

   /*
     Allocate fixed block allocator with element size,
     number of elements in one block, alignment requirements,
     pointer to allocator
    */

fb_allocator*
fb_alloc_init( size_t         esize,
               size_t         alignment,
               unsigned int   nelems ) {
  
  fb_allocator* alloc = NULL;

  assert( esize > 0 );
  assert( alignment > 0 );
  assert( nelems > 1 );

  alloc = (fb_allocator*)malloc( sizeof( fb_allocator ) );

  if ( alloc != NULL ) {

    alloc->alignment_  = alignment;
    alloc->nelems_     = nelems;
    alloc->pool_head_  = NULL;
    alloc->block_head_ = NULL;
  
    for( ; esize < esize+alignment; ++esize ) {
      if ( (esize % alignment ) == 0 ) {
        break;
      }
    }
    alloc->esize_      = esize;
  }
  
  return alloc;
}

   /*
     takes pointer to allocator and deletes it
   */
int
fb_alloc_delete( fb_allocator* alloc ) {
  
  char* ptr;
  assert(alloc!=NULL);
  
  ptr = alloc->block_head_;
  while ( ptr != 0 ) {
    memcpy( &(alloc->block_head_), ptr+(alloc->esize_)*(alloc->nelems_), sizeof(char*) );
    free( ptr );
    ptr = alloc->block_head_;
  }
  free( alloc );

  return ALLOC_DELETE_OK;
}


   /*
     allocate and return memory block from allocator alloc
   */
void*
allocatei__( fb_allocator* alloc ) {
  
  link* p = NULL;
  
  assert( alloc != NULL );
  
  if ( alloc->pool_head_ == NULL ) {
    grow( alloc );
  }
  p                 = alloc->pool_head_;
  alloc->pool_head_ = p->next_;
  return p;
}

   /*
     deallocate memory block and return it to free list
    */
void
deallocate__( void* p, fb_allocator* alloc ) {

  link* ptr = (link*)p;
  
  assert( p != NULL );
  assert( alloc != NULL );

  ptr->next_ = alloc->pool_head_;
  alloc->pool_head_ = ptr;
}
