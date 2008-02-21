/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******
******  File:      GkickPropagators.h
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

#ifndef GKICKPROPAGATORS_H
#define GKICKPROPAGATORS_H

#include <beamline/gkick.h>

class Particle;
class JetParticle;


class gkick::Propagator: public BasePropagator<gkick> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }
 
  void  setup( gkick& elm ); 

  void  operator()(  gkick& elm,             Particle& p);
  void  operator()(  gkick& elm,          JetParticle& p);
};

#endif    // GKICK_H
