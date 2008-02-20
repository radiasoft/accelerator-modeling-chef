/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LambertsonPropagators.h
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
#ifndef  LAMBERTSONPROPAGATORS_H
#define  LAMBERTSONPROPAGATORS_H

#include <beamline/lambertson.h>

template<typename Particle_t>
class TBunch;

class Particle;
class JetParticle;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

class thinLamb::Propagator : public BasePropagator<thinLamb> {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void  operator()( thinLamb& elm,         Particle&  p);
  void  operator()( thinLamb& elm,      JetParticle&  p);
  void  operator()( thinLamb& elm,    ParticleBunch&  p);
  void  operator()( thinLamb& elm, JetParticleBunch&  p);

};

#endif //  LAMBERTSONPROPAGATORS_H



