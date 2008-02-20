/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******
******  File:      SeptumPropagators.h
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

#ifndef SEPTUMPROPAGATORS_H
#define SEPTUMPROPAGATORS_H

#include <beamline/septum.h>

template<typename Particle_t>
class TBunch;

class Particle;
class JetParticle;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

class thinSeptum::Propagator: public BasePropagator<thinSeptum> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( thinSeptum& elm ); 

  void  operator()(  thinSeptum& elm,             Particle& p);
  void  operator()(  thinSeptum& elm,          JetParticle& p);
  void  operator()(  thinSeptum& elm,        ParticleBunch& p);
  void  operator()(  thinSeptum& elm,     JetParticleBunch& p);

};

#endif    // SEPTUMPROPAGATORS_H
