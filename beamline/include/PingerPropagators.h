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
******  May 2008 ostiguy@fnal.gov
******  - propagator moved (back) to base class
******  - generic type BmlnElmnt used as function argument 
******
******
******
**************************************************************************
*************************************************************************/
#ifndef PINGERPROPAGATORS_H
#define PINGERPROPAGATORS_H

#include <beamline/BasePropagator.h>
#include <beamline/pinger.h>
#include <beamline/ParticleFwd.h>

template<typename Particle_t>
class TBunch;


typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;


class Pinger::Propagator: public BasePropagator {

public:

  Propagator();
  Propagator(Pinger const& elm);
  Propagator(Propagator const& elm);

  Propagator* clone() const { return new Propagator(*this); }

  void  ctor( BmlnElmnt const& elm ); 

  void  operator()(  BmlnElmnt const& elm,            Particle& p);
  void  operator()(  BmlnElmnt const& elm,         JetParticle& p);
  void  operator()(  BmlnElmnt const& elm,       ParticleBunch& p);
  void  operator()(  BmlnElmnt const& elm,    JetParticleBunch& p);
};


#endif // PINGERPROPAGATORS_H

