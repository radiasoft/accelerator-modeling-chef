#include <stdio.h>
#include <stdlib.h>

#include "fb_allocator.h"

const size_t       esize = 71;
const size_t       align  = 8;
const unsigned int nelem  = 2;

int
main( void ) {

  void* p0;
  void* p1;
  void* p2;

  fb_allocator* alloc = NULL;

  printf( "Constructing allocator with element size %i, alignment %i and buffer for %i\n\n",
          esize, align, nelem );

  alloc = fb_alloc_init( esize, align, nelem );

  printf("Allocating three element...\n");

  p0 = allocate( alloc );
  p1 = allocate( alloc );
  p2 = allocate( alloc );

  printf("and printing their addresses\n");

  printf("%li\n", (long)p0 );
  printf("%li\n", (long)p1 );
  printf("%li\n\n", (long)p2 );

  printf("Deallocating middle element \n");

  deallocate( p1, alloc );

  printf("and printing again \n");

  printf("%li\n", (long)p0 );
  printf("%li\n", (long)p1 );
  printf("%li\n\n", (long)p2 );

  printf("Reallocating middle element \n");
  p1 = allocate( alloc );
  
  printf("and printing again \n");
  
  printf("%li\n", (long)p0 );
  printf("%li\n", (long)p1 );
  printf("%li\n\n", (long)p2 );

  printf("Deallocating all stuff... \n");
  
  deallocate(p1, alloc );
  deallocate(p0, alloc );
  deallocate(p2, alloc );

  fb_alloc_delete( alloc );

  return EXIT_SUCCESS;
}
