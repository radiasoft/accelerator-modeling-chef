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

#ifndef fb_allocator_h
#define fb_allocator_h

#include <stddef.h>
#include <stdlib.h>

#define ALLOC_DELETE_OK   0

typedef struct fb_allocator_ fb_allocator;

size_t        fb_alloc_esize( const fb_allocator* alloc );

fb_allocator* fb_alloc_init( size_t esize, size_t alignment, unsigned int nelems );
int           fb_alloc_delete( fb_allocator* alloc );

void*         allocatei__( fb_allocator* alloc );
void          deallocate__( void* p, fb_allocator* alloc );

#define allocate_block__( p,alloc ) ((p) = allocatei__(alloc) )

#define allocate_malloc__( p,alloc ) ((p) = malloc( fb_alloc_esize(alloc) ) )
#define deallocate_free__( p,alloc ) ( free(p) )

#endif /* fb_allocator_h */
