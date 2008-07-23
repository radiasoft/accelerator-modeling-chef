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

#include <beamline/sextupole.h>

class Particle;
class JetParticle;

class sextupole::Propagator: public BasePropagator {

public:

  Propagator ( int n=4) : n_(n) {}
 
  Propagator* Clone() const { return new Propagator(*this); }

  void  setup( bmlnElmnt& elm ); 

  void  setAttribute( bmlnElmnt& elm, std::string const& name, boost::any const&  value);
  void  operator()(  bmlnElmnt const& elm,            Particle& p);
  void  operator()(  bmlnElmnt const& elm,         JetParticle& p);

 private:

  int n_; // number of thin kicks 
 
};


//------------------------------------------------------------------  

class thinSextupole::Propagator: public BasePropagator {

public:

  Propagator* Clone() const { return new Propagator(*this); }

  void  operator()( bmlnElmnt const& elm,            Particle& p);
  void  operator()( bmlnElmnt const& elm,         JetParticle& p);

};


#endif // SEXTUPOLEPROPAGATORS_H
