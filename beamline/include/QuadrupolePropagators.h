/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      QuadrupolePropagators.h
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
******  Authors:   Leo Michelotti         michelotti@fnal.gov
******             Jean-Francois Ostiguy  ostiguy@fnal.gov
******
******  May 2008 ostiguy@fnal.gov
******  - propagator moved (back) to base class
******  - generic type bmlnElmnt used as function argument 
******
**************************************************************************
*************************************************************************/
#ifndef  QUADRUPOLEPROPAGATORS_H
#define  QUADRUPOLEPROPAGATORS_H

#include <beamline/BasePropagator.h>
#include <beamline/quadrupole.h>
#include <beamline/ParticleFwd.h>


class quadrupole::Propagator: public BasePropagator {

 public:

  Propagator ( int n): n_(n) {}
  Propagator ( Propagator const& o ): BasePropagator(), n_(o.n_) {}

  Propagator* Clone() const { return new Propagator(*this); }

  void  setup(bmlnElmnt& elm); 

  void  setAttribute(  bmlnElmnt& elm, std::string const& name, boost::any const& value ); 
 
  void  operator()( bmlnElmnt const& elm,             Particle& p);
  void  operator()( bmlnElmnt const& elm,          JetParticle& p);

 private:

  int n_; // number of thin kicks 
 
};

//------------------------------------------------------------------  

class thinQuad::Propagator: public BasePropagator {

public:

  Propagator* Clone() const { return new Propagator(*this); }

  void  operator()( bmlnElmnt const& elm,              Particle& p);
  void  operator()( bmlnElmnt const& elm,           JetParticle& p);

};

//----------------------------------------------------------------

class quadrupole::MADPropagator: public BasePropagator {

 public:

  MADPropagator* Clone() const { return new MADPropagator(*this); }

  void  setup(bmlnElmnt& elm); 

  void  setAttribute(  bmlnElmnt& elm, std::string const& name, boost::any const& value ); 
 
  void  operator()( bmlnElmnt const& elm,             Particle& p);
  void  operator()( bmlnElmnt const& elm,          JetParticle& p);

};

#endif //  QUADRUPOLEPROPAGATORS_H



