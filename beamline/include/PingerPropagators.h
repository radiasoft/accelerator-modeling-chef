/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      PingerPropagators.h
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
******
**************************************************************************
*************************************************************************/
#ifndef PINGERPROPAGATORS_H
#define PINGERPROPAGATORS_H

template<typename Particle_t>
class TBunch;

class Particle;
class JetParticle;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

#include <beamline/pinger.h>

class Pinger::Propagator: public BasePropagator<Pinger> {

public:

  Propagator* Clone() const { return new Propagator(*this); }

  void  setup( Pinger& elm ); 

  void  setLength   ( double const& length   );
  void  setStrength ( double const& strength );
 
  void  operator()(  Pinger& elm,            Particle& p);
  void  operator()(  Pinger& elm,         JetParticle& p);
  void  operator()(  Pinger& elm,       ParticleBunch& p);
  void  operator()(  Pinger& elm,    JetParticleBunch& p);
};


#endif // PINGERPROPAGATORS_H

