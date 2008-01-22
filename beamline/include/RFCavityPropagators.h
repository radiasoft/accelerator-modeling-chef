/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******
******  File:      RFCavityPropagators.h
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

#ifndef RFCAVITYPROPAGATORS_H
#define RFCAVITYPROPAGATORS_H

#include <beamline/rfcavity.h>

class rfcavity::Propagator: public BasePropagator<rfcavity> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( rfcavity& elm ); 

  void  operator()(  rfcavity& elm,         Particle& p);
  void  operator()(  rfcavity& elm,      JetParticle& p);
  void  operator()(  rfcavity& elm,    ParticleBunch& b);

};


class thinrfcavity::Propagator: public BasePropagator<thinrfcavity> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( thinrfcavity& elm ); 

  void  operator()(  thinrfcavity& elm,        Particle& p);
  void  operator()(  thinrfcavity& elm,     JetParticle& p);
  void  operator()(  thinrfcavity& elm,   ParticleBunch& b);

};

#endif    // RFCAVITYPROPAGATORS_H
