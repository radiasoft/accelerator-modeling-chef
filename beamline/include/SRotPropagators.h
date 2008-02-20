/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SrotPropagators.h
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
#ifndef  SROTPROPAGATORS_H
#define  SROTPROPAGATORS_H

#include <beamline/srot.h>

template<typename Particle_t>
class TBunch;

class Particle;
class JetParticle;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

class srot::Propagator : public BasePropagator<srot> {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void       setup( srot& elm);
  void  operator()( srot& elm,         Particle& p);
  void  operator()( srot& elm,      JetParticle& p);
  void  operator()( srot& elm,    ParticleBunch& b);
  void  operator()( srot& elm, JetParticleBunch& b);

};

#endif //  SROTPROPAGATORS_H



