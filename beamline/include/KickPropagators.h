/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******
******  File:      KickPropagators.h
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

#ifndef KICKPROPAGATORS_H
#define KICKPROPAGATORS_H


template<typename Particle_t>
class TBunch;

class Particle;
class JetParticle;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

class kick::Propagator: public BasePropagator<kick> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( kick& elm ); 

  void  operator()(  kick& elm,             Particle& p);
  void  operator()(  kick& elm,          JetParticle& p);
  void  operator()(  kick& elm,        ParticleBunch& p);
  void  operator()(  kick& elm,     JetParticleBunch& p);

};

class hkick::Propagator: public BasePropagator<hkick> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( hkick& elm ); 

  void  operator()(  hkick& elm,             Particle& p);
  void  operator()(  hkick& elm,          JetParticle& p);
  void  operator()(  hkick& elm,        ParticleBunch& p);
  void  operator()(  hkick& elm,     JetParticleBunch& p);

};

class vkick::Propagator: public BasePropagator<vkick> {

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( vkick& elm ); 

  void  operator()(  vkick& elm,             Particle& p);
  void  operator()(  vkick& elm,          JetParticle& p);
  void  operator()(  vkick& elm,        ParticleBunch& p);
  void  operator()(  vkick& elm,     JetParticleBunch& p);

};

#endif    // KICKPROPAGATORS_H
