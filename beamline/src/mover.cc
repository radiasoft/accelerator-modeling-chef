/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      mover.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <mover.h>

using namespace std;

mover::mover() : circuit (), align() {
}

mover::mover(const char* n) : circuit (n), align() {
}

mover::mover(bmlnElmnt* q) : circuit (q), align(q->Alignment()) {
}

mover::mover(const char* n, bmlnElmnt* q) : circuit (n,q), align(q->Alignment()) {
}

mover::~mover() {}

void mover::append( bmlnElmnt* q ) {
   dlist::append( q );
   if ( align.xOffset == 0 && align.yOffset == 0 && align.tilt == 0 ) {
     align = q->Alignment();
   }
   numElm++;
}

void mover::switchOn() {
  onOffSwitch = 1;
  set(&align);
}

void mover::switchOff() {
  onOffSwitch = 0;
  alignmentData dummy;
  set(&dummy);
}

void mover::set(void* x) {
  alignmentData* curr = (alignmentData*)x;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  align = *curr;
  while((  p = (bmlnElmnt*) getNext()  )) {
    p -> setAlignment( *curr );
  }
}

void mover::get(void* x) {
   alignmentData* curr = (alignmentData*)x;
  *curr = align;
}

