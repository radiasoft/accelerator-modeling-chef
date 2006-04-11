/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      fb.c
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
******  URA/FNAL reserves all rights.
******                                          
******                                                                
******  Authors:    Dmitri Mokhov and Oleg Krivosheev                  
******                                                                
******
******  Contacts:            Leo Michelotti michelotti@fnal.gov
******             Jean-Francois Ostiguy    ostiguy@fnal.gov
******                                                              
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <fb_allocator.h>

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

  PRIVATE_ALLOCATE( p0, alloc );
  PRIVATE_ALLOCATE( p1, alloc );
  PRIVATE_ALLOCATE( p2, alloc );

  printf("and printing their addresses\n");

  printf("%li\n", (long)p0 );
  printf("%li\n", (long)p1 );
  printf("%li\n\n", (long)p2 );

  printf("Deallocating middle element \n");

  PRIVATE_DEALLOCATE( p1, alloc );

  printf("and printing again \n");

  printf("%li\n", (long)p0 );
  printf("%li\n", (long)p1 );
  printf("%li\n\n", (long)p2 );

  printf("Reallocating middle element \n");
  PRIVATE_ALLOCATE( p1, alloc );
  
  printf("and printing again \n");
  
  printf("%li\n", (long)p0 );
  printf("%li\n", (long)p1 );
  printf("%li\n\n", (long)p2 );

  printf("Deallocating all stuff... \n");
  
  PRIVATE_DEALLOCATE(p1, alloc );
  PRIVATE_DEALLOCATE(p0, alloc );
  PRIVATE_DEALLOCATE(p2, alloc );

  fb_alloc_delete( alloc );

  return EXIT_SUCCESS;
}
