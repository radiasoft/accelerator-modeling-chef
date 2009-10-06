/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LinacCavityPartsPropagators.h
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
**************************************************************************
*************************************************************************/
#ifndef  LINACCAVITYPARTSPROPAGATORS_H
#define  LINACCAVITYPARTSPROPAGATORS_H

#include <beamline/BasePropagator.h>
#include <beamline/LinacCavityParts.h>
#include <beamline/ParticleFwd.h>


class LCavityUpstream::Propagator : public BasePropagator {

 public:
 
  Propagator();
  Propagator(LCavityUpstream const& elm);
  Propagator(Propagator const&);

  Propagator* clone() const { return new Propagator(*this); }

  void       ctor( BmlnElmnt const& elm);

  void  setAttribute ( BmlnElmnt& elm, std::string const& name, boost::any const& value );

  void  operator()( BmlnElmnt const& elm,              Particle& p);
  void  operator()( BmlnElmnt const& elm,           JetParticle& p);

};

class LCavityDnstream::Propagator : public BasePropagator {

 public:
 
  Propagator();
  Propagator(LCavityDnstream const& elm);
  Propagator(Propagator const&);

  Propagator* clone() const { return new Propagator(*this); }

  void       ctor( BmlnElmnt const& elm);

  void  setAttribute ( BmlnElmnt& elm, std::string const& name, boost::any const& value );
 
  void  operator()( BmlnElmnt const& elm,              Particle& p);
  void  operator()( BmlnElmnt const& elm,           JetParticle& p);

};

#endif // LINACCAVITYPARTSPROPAGATORS_H



