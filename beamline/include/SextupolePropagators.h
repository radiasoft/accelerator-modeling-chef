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
******  Author: Jean-Francois Ostiguy  
******          ostiguy@fnal.gov
******
******  May 2008 ostiguy@fnal.gov
******  - propagator moved (back) to base class
******  - generic type bmlnElmnt used as function argument 
******
******
**************************************************************************
*************************************************************************/
#ifndef SEXTUPOLEPROPAGATORS_H
#define SEXTUPOLEPROPAGATORS_H

#include <beamline/BasePropagator.h>
#include <beamline/sextupole.h>
#include <beamline/ParticleFwd.h>


class sextupole::Propagator: public BasePropagator {

public:

  Propagator ( int n=4);
  Propagator ( sextupole const& elm, int n=4);
  Propagator ( Propagator const& p);
 
  Propagator* clone() const { return new Propagator(*this); }

  void  ctor( BmlnElmnt const& elm ); 

  void  setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const&  value);
  void  operator()(  BmlnElmnt const& elm,            Particle& p);
  void  operator()(  BmlnElmnt const& elm,         JetParticle& p);

 private:

  int n_; // number of thin kicks 
 
};


//------------------------------------------------------------------  

class thinSextupole::Propagator: public BasePropagator {

public:

  Propagator ();
  Propagator ( thinSextupole const& elm);
  Propagator ( Propagator const& p);
 
  Propagator* clone() const { return new Propagator(*this); }

  void  operator()( BmlnElmnt const& elm,            Particle& p);
  void  operator()( BmlnElmnt const& elm,         JetParticle& p);

};


#endif // SEXTUPOLEPROPAGATORS_H

