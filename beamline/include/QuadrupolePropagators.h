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
******
**************************************************************************
*************************************************************************/
#ifndef  QUADRUPOLEPROPAGATORS_H
#define  QUADRUPOLEPROPAGATORS_H

#include <beamline/quadrupole.h>

class quadrupole::Propagator: public BasePropagator<quadrupole> {

 public:

  Propagator ( int n): n_(n) {}
  Propagator ( Propagator const& o ): BasePropagator<quadrupole>(), n_(o.n_) {}

  Propagator* Clone() const { return new Propagator(*this); }

  void  setup(quadrupole& elm); 

  void  operator()( quadrupole& elm,        Particle& p);
  void  operator()( quadrupole& elm,     JetParticle& p);

 private:

  int n_; // number of thin kicks 
 
};

//------------------------------------------------------------------  

class thinQuad::Propagator: public BasePropagator<thinQuad> {

public:

  Propagator* Clone() const { return new Propagator(*this); }

  void  operator()( thinQuad& elm,         Particle& p);
  void  operator()( thinQuad& elm,     JetParticle& p);


};

#endif //  QUADRUPOLEPROPAGATORS_H



