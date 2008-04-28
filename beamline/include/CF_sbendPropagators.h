/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      CF_sbendPropagators.h
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
******
**************************************************************************
*************************************************************************/
#ifndef CFSBENDPROPAGATORS_H
#define CFSBENDPROPAGATORS_H

#include <beamline/CF_sbend.h>

class Particle;
class JetParticle;

class CF_sbend::Propagator: public BasePropagator<CF_sbend> {

public:

  Propagator ( int n=1) : n_(n) {}
  
  Propagator* Clone() const { return new Propagator(*this); }

  void  setup( CF_sbend& elm ); 

  void  setLength   ( double const& length   ); 
  void  setStrength ( double const& strength ); 

  void  operator()(  CF_sbend& elm,            Particle& p);
  void  operator()(  CF_sbend& elm,         JetParticle& p);

 private:

  int n_; // number of slices  
 
};


#endif // CFSBENDPROPAGATORS_H


