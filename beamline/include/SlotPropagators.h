/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******
******  File:      SlotPropagators.h
******                                                                
******  Copyright (c) Fermi Research Alliance LLC 
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov  
****** 
******                                                                
**************************************************************************
*************************************************************************/

#ifndef SLOTPROPAGATORS_H
#define SLOTPROPAGATORS_H

#include <beamline/Slot.h>

class Particle;
class JetParticle;

class Slot::Propagator: public BasePropagator<Slot> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( Slot& elm ); 

  void  operator()(  Slot& elm,             Particle& p);
  void  operator()(  Slot& elm,          JetParticle& p);

};


#endif    // SLOTPROPAGATORS_H
