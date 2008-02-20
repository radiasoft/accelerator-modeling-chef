/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      DecapolePropagators.h
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

#ifndef DECAPOLEPROPAGATORS_H
#define DECAPOLEPROPAGATORS_H

#include <beamline/decapole.h>

template<typename Particle_t>
class TBunch;

class Particle;
class JetParticle;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

class thinDecapole::Propagator: public BasePropagator<thinDecapole> {

public:

  Propagator* Clone() const { return new Propagator(*this); }

  void  operator()(  thinDecapole& elm,            Particle& p);
  void  operator()(  thinDecapole& elm,         JetParticle& p);
  void  operator()(  thinDecapole& elm,       ParticleBunch& p);
  void  operator()(  thinDecapole& elm,    JetParticleBunch& p);

};


#endif // DECAPOLEPROPAGATORS_H

