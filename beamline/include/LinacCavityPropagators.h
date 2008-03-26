/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LinacCavityPropagators.h
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
**************************************************************************
*************************************************************************/
#ifndef  LINACCAVITYPROPAGATORS_H
#define  LINACCAVITYPROPAGATORS_H

#include <beamline/LinacCavity.h>

class Particle;
class JetParticle;

class LinacCavity::Propagator : public BasePropagator<LinacCavity> {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void       setup( LinacCavity& elm);
  void  operator()( LinacCavity& elm,         Particle& p);
  void  operator()( LinacCavity& elm,      JetParticle& p);

  void   setWakeOn( LinacCavity& elm, bool set);
  bool      wakeOn( LinacCavity const& elm )     const;
};

#endif //  LINACCAVITYPROPAGATORS_H



