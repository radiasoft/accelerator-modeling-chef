/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      OctupolePropagators.h
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

#ifndef OCTUPOLEPROPAGATORS_H
#define OCTUPOLEPROPAGATORS_H

#include <beamline/octupole.h>

template<typename Particle_t>
class TBunch;

class Particle;
class JetParticle;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

class octupole::Propagator: public BasePropagator<octupole> {

public:

  Propagator(int n=4):  n_(n) {}
  Propagator* Clone() const { return new Propagator(*this); }

  void  setup( octupole& elm); 

  void  operator()( octupole& elm,             Particle& p);
  void  operator()( octupole& elm,          JetParticle& p);
 
 private:

  int n_; // number of thin kicks 
 
};

//------------------------------------------------------------------  

class thinOctupole::Propagator: public BasePropagator<thinOctupole> {

public:

  Propagator* Clone() const { return new Propagator(*this); }

  void  operator()( thinOctupole& elm,             Particle& p);
  void  operator()( thinOctupole& elm,          JetParticle& p);

};

#endif // OCTUPOLEPROPAGATORS_H

