/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      ICircuit.cc
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


#include <basic_toolkit/iosetup.h>
#include <beamline/ICircuit.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


ICircuit::ICircuit() : circuit () {
  current = 0.0;
}

ICircuit::ICircuit(const char* n) : circuit (n) {
  current = 0.0;
}

ICircuit::ICircuit(bmlnElmnt* q) : circuit (q) {
  current = q->Current();
}

ICircuit::ICircuit(const char* n, bmlnElmnt* q) : circuit (n,q) {
  current = q->Current();
}
ICircuit::~ICircuit() {
}
void ICircuit::switchOn() {
  onOffSwitch = 1;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  while((  p = (bmlnElmnt*) getNext()  )) {
    p -> setCurrent( current );
  }
}

void ICircuit::switchOff() {
  onOffSwitch = 0;
  double dummy = 0.0;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  while((  p = (bmlnElmnt*) getNext()  )) {
    p -> setStrength( dummy );
  }
}

void ICircuit::set(void* x) {
  double* curr = (double*)x;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  
  current = *curr;
  while((  p = (bmlnElmnt*) getNext()  )) {
    p -> setCurrent( *curr );
  }
}

void ICircuit::get(void* x) {
  double* curr = (double*)x;
  *curr = current;
}

