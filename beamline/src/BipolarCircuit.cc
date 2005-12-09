/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      BipolarCircuit.cc
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


#include <iosetup.h>
#include <BipolarCircuit.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

BipolarCircuit::BipolarCircuit() : circuit () {
  field = 0.0;
}

BipolarCircuit::BipolarCircuit(const char* n) : circuit (n) {
  field = 0.0;
}

BipolarCircuit::BipolarCircuit(bmlnElmnt* q) : circuit (q) {
  field = 0.0;
}

BipolarCircuit::BipolarCircuit(const char* n, bmlnElmnt* q) : circuit (n,q) {
  field = 0.0;
}
BipolarCircuit::~BipolarCircuit() {
}
void BipolarCircuit::switchOn() {
  onOffSwitch = 1;
  set(&field);
}

void BipolarCircuit::switchOff() {
  onOffSwitch = 0;
  double dummy = 0.0;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;

  while ( (p = (bmlnElmnt*) getNext()) ) {
    p -> setStrength( dummy );
  }
}

void BipolarCircuit::set(void* x) {
  double* curr = (double*)x;
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  PolarityBarn* pol;
  
  field = *curr;
  while ( (p = (bmlnElmnt*) getNext()) ) {
    pol = (PolarityBarn*)p->dataHook.find("Polarity");
    if(pol == 0) {
      (*pcerr) << "BipolarCircuit: no polarity information for " <<
	p->Name() << " Assuming +1 " << endl;
      p -> setStrength( *curr );

    } else 
      p -> setStrength( (*curr)* pol->polarity );
  }
}

void BipolarCircuit::get(void* x) {
  double* curr = (double*)x;
  *curr = field;
}

void BipolarCircuit::append( bmlnElmnt* q ) {
   dlist::append( q );
   field = q->Strength();
   numElm++;
}
