/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      fb_allocator.h
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
******                                                                
**************************************************************************
*************************************************************************/
#ifndef FB_ALLOCATOR_H
#define FB_ALLOCATOR_H

#include <stddef.h>
#include <stdlib.h>

#define ALLOC_DELETE_OK   0

typedef struct fb_allocator_ fb_allocator;

size_t        fb_alloc_esize( const fb_allocator* alloc );

fb_allocator* fb_alloc_init( size_t esize, size_t alignment, unsigned int nelems );
int           fb_alloc_delete( fb_allocator* alloc );

void*         allocatei__( fb_allocator* alloc );
void          deallocate__( void* p, fb_allocator* alloc );

static void* p;

#define allocate_block__( p,alloc )  ( p = allocatei__(alloc) )
#define allocate_malloc__( p,alloc ) ( p = malloc( fb_alloc_esize(alloc) ) )
#define deallocate_free__( p,alloc ) ( free(p) )

#endif /*  FB_ALLOCATOR_H */
