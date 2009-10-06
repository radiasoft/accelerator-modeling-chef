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
******  - generic type BmlnElmnt used as function argument 
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

  Propagator ( int n=4);
  Propagator ( quadrupole const& elm, int n=4);
  Propagator ( Propagator const& o );

  Propagator* clone() const;

  void  ctor(BmlnElmnt const& elm); 

  void  setAttribute(  BmlnElmnt& elm, std::string const& name, boost::any const& value ); 
 
  void  operator()( BmlnElmnt const& elm,             Particle& p);
  void  operator()( BmlnElmnt const& elm,          JetParticle& p);

 private:

  int n_; // number of thin kicks 
 
};

//------------------------------------------------------------------  

class thinQuad::Propagator: public BasePropagator {

public:

  Propagator ();
  Propagator ( thinQuad const& elm);
  Propagator ( Propagator const& o );

  Propagator* clone() const { return new Propagator(*this); }

  void  operator()( BmlnElmnt const& elm,              Particle& p);
  void  operator()( BmlnElmnt const& elm,           JetParticle& p);

};

//----------------------------------------------------------------

class quadrupole::MADPropagator: public BasePropagator {

 public:

  MADPropagator ();
  MADPropagator ( quadrupole const& elm);
  MADPropagator ( MADPropagator const& o );

  MADPropagator* clone() const { return new MADPropagator(*this); }

  void  ctor(BmlnElmnt const& elm); 

  void  setAttribute(  BmlnElmnt& elm, std::string const& name, boost::any const& value ); 
 
  void  operator()( BmlnElmnt const& elm,             Particle& p);
  void  operator()( BmlnElmnt const& elm,          JetParticle& p);

};

#endif //  QUADRUPOLEPROPAGATORS_H



