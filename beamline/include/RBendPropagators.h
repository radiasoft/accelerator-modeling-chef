/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      RbendPropagators.h
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

#ifndef RBENDPROPAGATORS_H
#define RBENDPROPAGATORS_H

#include <beamline/rbend.h>

class Particle;
class JetParticle;

class rbend::Propagator: public BasePropagator<rbend> {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void  setup(rbend& elm); 

  void  setLength   ( double const& length   );
  void  setStrength ( double const& strength );
 
  void  operator()( rbend& elm,             Particle& p);
  void  operator()( rbend& elm,          JetParticle& p);
};



#endif // RBENDPROPAGATORS_H

