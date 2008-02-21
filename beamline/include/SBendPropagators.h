/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SBendPropagators.h
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

#ifndef SBENDPROPAGATORS_H
#define SBENDPROPAGATORS_H

#include <beamline/sbend.h>

class Particle;
class JetParticle;

class sbend::Propagator: public BasePropagator<sbend> {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void  setup( sbend& elm); 

  void  operator()(  sbend& elm,             Particle& p);
  void  operator()(  sbend& elm,          JetParticle& p);

};



#endif // SBENDPROPAGATORS_H

