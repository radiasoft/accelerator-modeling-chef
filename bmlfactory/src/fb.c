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

  allocate( p0, alloc );
  allocate( p1, alloc );
  allocate( p2, alloc );

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
  allocate( p1, alloc );
  
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
