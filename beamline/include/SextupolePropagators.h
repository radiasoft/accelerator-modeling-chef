/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SextupolePropagators.h
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
#ifndef SEXTUPOLEPROPAGATORS_H
#define SEXTUPOLEPROPAGATORS_H

#include <beamline/sextupole.h>

template<typename Particle_t>
class TBunch;

class Particle;
class JetParticle;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

class sextupole::Propagator: public BasePropagator<sextupole> {

public:

  Propagator ( int n=4) : n_(n) {}
 
  Propagator* Clone() const { return new Propagator(*this); }

  void  setup( sextupole& elm ); 

  void  operator()(  sextupole& elm,            Particle& p);
  void  operator()(  sextupole& elm,         JetParticle& p);
  void  operator()(  sextupole& elm,       ParticleBunch& b);
  void  operator()(  sextupole& elm,    JetParticleBunch& b);

 private:

  int n_; // number of thin kicks 
 
};


//------------------------------------------------------------------  

class thinSextupole::Propagator: public BasePropagator<thinSextupole> {

public:

  Propagator* Clone() const { return new Propagator(*this); }

  void  operator()( thinSextupole& elm,            Particle& p);
  void  operator()( thinSextupole& elm,         JetParticle& p);
  void  operator()( thinSextupole& elm,       ParticleBunch& b);
  void  operator()( thinSextupole& elm,    JetParticleBunch& b);

};


#endif // SEXTUPOLEPROPAGATORS_H

