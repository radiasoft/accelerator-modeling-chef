/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SolenoidPropagators.h
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

#ifndef SOLENOIDPROPAGATORS_H
#define SOLENOIDPROPAGATORS_H

#include <beamline/Solenoid.h>

class Particle;
class JetParticle;

class Solenoid::Propagator: public BasePropagator<Solenoid> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( Solenoid& elm ); 

  void  operator()(  Solenoid& elm,             Particle& p);
  void  operator()(  Solenoid& elm,          JetParticle& p);

};

#endif // SOLENOIDPROPAGATORS_H

