/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      DriftPropagators.h
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
******
**************************************************************************
*************************************************************************/
#ifndef  DRIFTPROPAGATORS_H
#define  DRIFTPROPAGATORS_H

#include <beamline/drift.h>

class drift::Propagator : public BasePropagator<drift> {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void  setLength   ( double const& length   );
  void  setStrength ( double const& strength );
 
  void  operator()( drift& elm,         Particle& p);
  void  operator()( drift& elm,      JetParticle& p);
};

class MADDriftPropagator : public BasePropagator<drift> {

  MADDriftPropagator* Clone() const { return new MADDriftPropagator(*this); }

  void  setLength   ( double const& length   );
  void  setStrength ( double const& strength );
 
  void  operator()( drift& elm,    Particle& p);
  void  operator()( drift& elm, JetParticle& p);

};

#endif //  DRIFTPROPAGATORS_H



