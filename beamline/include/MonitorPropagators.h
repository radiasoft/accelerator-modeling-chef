/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******
******  File:      MonitorPropagators.h
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

#ifndef MONITORPROPAGATORS_H
#define MONITORPROPAGATORS_H
#include <beamline/monitor.h>

template<typename Particle_t>
class TBunch;

class Particle;
class JetParticle;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

class monitor::Propagator: public BasePropagator<monitor> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( monitor& elm ); 

  void  setLength   ( double const& length   );
  void  setStrength ( double const& strength );
 
  void  operator()(  monitor& elm,            Particle& p);
  void  operator()(  monitor& elm,         JetParticle& p);
  void  operator()(  monitor& elm,       ParticleBunch& p);
  void  operator()(  monitor& elm,    JetParticleBunch& p);

};


class hmonitor::Propagator: public BasePropagator<hmonitor> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( hmonitor& elm ); 

  void  setLength   ( double const& length   );
  void  setStrength ( double const& strength );
 
  void  operator()(  hmonitor& elm,            Particle& p);
  void  operator()(  hmonitor& elm,         JetParticle& p);
  void  operator()(  hmonitor& elm,       ParticleBunch& p);
  void  operator()(  hmonitor& elm,    JetParticleBunch& p);

};

class vmonitor::Propagator: public BasePropagator<vmonitor> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( vmonitor& elm ); 

  void  setLength   ( double const& length   );
  void  setStrength ( double const& strength );
 
  void  operator()(  vmonitor& elm,            Particle& p);
  void  operator()(  vmonitor& elm,         JetParticle& p);
  void  operator()(  vmonitor& elm,       ParticleBunch& p);
  void  operator()(  vmonitor& elm,    JetParticleBunch& p);

};

#endif    // MONITORPROPAGATORS_H
