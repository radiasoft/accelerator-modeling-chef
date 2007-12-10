/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LinacCavityPartsPropagators.h
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
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
******  Author: Jean-Francois Ostiguy  ostiguy@fnal.gov
******
******
**************************************************************************
*************************************************************************/
#ifndef  LINACCAVITYPARTSPROPAGATORS_H
#define  LINACCAVITYPARTSPROPAGATORS_H

#include <beamline/LinacCavityParts.h>

class LCavityUpstream::Propagator : public BasePropagator<LCavityUpstream> {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void       setup( LCavityUpstream& elm);
  void  operator()( LCavityUpstream& elm,         Particle& p);
  void  operator()( LCavityUpstream& elm,      JetParticle& p);

};

class LCavityDnstream::Propagator : public BasePropagator<LCavityDnstream> {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void       setup( LCavityDnstream& elm);
  void  operator()( LCavityDnstream& elm,         Particle& p);
  void  operator()( LCavityDnstream& elm,      JetParticle& p);


};

#endif // LINACCAVITYPARTSPROPAGATORS_H



