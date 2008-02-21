/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******
******  File:      SectorPropagators.h
******                                                                
******  Copyright (c) Fermi Research Alliance LLC 
******                All Rights Reserved
******
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
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov  
****** 
******                                                                
**************************************************************************
*************************************************************************/

#ifndef SECTORPROPAGATORS_H
#define SECTORPROPAGATORS_H

#include <beamline/sector.h>

class Particle;
class JetParticle;

class sector::Propagator: public BasePropagator<sector> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( sector& elm ); 

  void  operator()(  sector& elm,             Particle& p);
  void  operator()(  sector& elm,          JetParticle& p);

};


#endif    // SECTORPROPAGATORS_H
