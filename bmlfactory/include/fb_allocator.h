   /* -*- C -*- */

#ifndef fb_allocator_h
#define fb_allocator_h

#include <stddef.h>

#define ALLOC_DELETE_OK   0

typedef struct fb_allocator_ fb_allocator;

fb_allocator* fb_alloc_init( size_t esize, size_t alignment, unsigned int nelems );
int           fb_alloc_delete( fb_allocator* alloc );

void*         allocate( fb_allocator* alloc );
void          deallocate( void* p, fb_allocator* alloc );

#endif /* fb_allocator_h */
