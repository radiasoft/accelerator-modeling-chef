   /* -*- C -*- */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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


/* Begin DGN section -----
#if !defined(madparser_h)
#include "madparser.h"
#endif 
typedef struct {
    void* yybuff_;
    char* filename_;
    FILE* yyfile_;
    int   linenum_;
} yybuff;

struct madparser_ {
    char*           filename_in_;
    char*           filename_out_;
    
    fb_allocator*   expr_alloc_;
    fb_allocator*   ident_alloc_;

    fb_allocator*   const_alloc_;
    GHashTable*     const_table_;
    
    fb_allocator*   var_alloc_;
    GHashTable*     var_table_;

    fb_allocator*   bel_alloc_;
    fb_allocator*   matrix_alloc_;
    GHashTable*     bel_table_;

    GHashTable*     bml_table_;
    fb_allocator*   bml_alloc_;

    GPtrArray*      comment_arr_;
    fb_allocator*   comment_alloc_;

    GSList*         yybuff_list_;
    GSList*         filenames_list_;
    fb_allocator*   yybuff_alloc_;

    const char*     current_filename_;
    yybuff*         current_yybuff_;

    int             linenum_;

    int             comment_at_eof_;
    int             comment_mode_;

    beam_element*   current_bel_;
    char            current_bel_type_[BEL_NAME_LENGTH];
};
extern madparser* mp;
extern void mpwatch();
-----    End DGN section */

   /*
     grow the existing allocator if there is no room for allocation in existing blocks.
     Allocate new block and add it content to the free list 
    */
static void
grow( fb_allocator* alloc ) {

  char* last;
  char* p;
  char* start = (char*)malloc( alloc->esize_*alloc->nelems_ + sizeof(char*) );

  /* DGN static int entnum = 0;  */ 
  /* DGN fprintf( stderr, "DGN:  Entering grow: %i\n", ++entnum );  */ 

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
  
  char *ptr;
  /* DGN  int counter;*/
  /* DGN  fprintf( stderr, "DGN:  Entering fb_alloc_delete\n" );*/
  /* DGN  fprintf( stderr, "DGN:  char* ptr;\n" );*/
  /* DGN  fprintf( stderr, "DGN:  assert(alloc!=NULL);\n" );*/
  assert(alloc!=NULL);
  
  /* DGN  fprintf( stderr, "DGN:  ptr = alloc->block_head_;\n" );*/
  ptr = alloc->block_head_;
  /* DGN  mpwatch();*/
  /* DGN  fprintf( stderr, "DGN:  while ( ptr != 0 ) {\n" );*/
  /* DGN  counter = 0;*/
  while ( ptr != 0 ) {
    /* DGN  counter++;*/
    /* DGN  fprintf( stderr, "DGN:    memcpy( &(alloc->block_head_), ptr+(alloc->esize_)*(alloc->nelems_), sizeof(char*) );\n" );*/
    /* DGN  fprintf( stderr, "DGN:    memcpy( %i, %i+%i*%i, %i );\n", &(alloc->block_head_), ptr, (alloc->esize_), (alloc->nelems_), sizeof(char*));*/
    memcpy( &(alloc->block_head_), ptr+(alloc->esize_)*(alloc->nelems_), sizeof(char*) );
    /* DGN  fprintf( stderr, "DGN:    free( ptr );\n" );*/
    free( ptr );
    /* DGN  fprintf( stderr, "DGN:    ptr = alloc->block_head_;\n" );*/
    ptr = alloc->block_head_;
    /* DGN  fprintf( stderr, "DGN:    ptr = %i;\n", ptr );*/
    /* DGN  mpwatch();*/
  }
  /* DGN  fprintf( stderr, "DGN:  looped %i times\n", counter );*/
  /* DGN  fprintf( stderr, "DGN:  free( alloc );\n" );*/
  free( alloc );

  /* DGN  fprintf( stderr, "DGN:  return ALLOC_DELETE_OK;\n" );*/
  /* DGN  fprintf( stderr, "DGN:  Exiting fb_alloc_delete\n" );*/
  return ALLOC_DELETE_OK;
}


   /*
     allocate and return memory block from allocator alloc
   */
void*
allocate( fb_allocator* alloc ) {
  
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
deallocate( void* p, fb_allocator* alloc ) {

  link* ptr = (link*)p;
  
  assert( p != NULL );
  assert( alloc != NULL );

  ptr->next_ = alloc->pool_head_;
  alloc->pool_head_ = ptr;
}
