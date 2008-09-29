/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SrotPropagators.h
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
******  May 2008 ostiguy@fnal.gov
******  - propagator moved (back) to base class
******  - generic type bmlnElmnt used as function argument 
******
**************************************************************************
*************************************************************************/
#ifndef  SROTPROPAGATORS_H
#define  SROTPROPAGATORS_H

#include <beamline/BasePropagator.h>
#include <beamline/srot.h>
#include <beamline/ParticleFwd.h>


class srot::Propagator : public BasePropagator {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void       setup( bmlnElmnt& elm);

  void  operator()( bmlnElmnt const& elm,         Particle& p);
  void  operator()( bmlnElmnt const& elm,      JetParticle& p);

};

#endif //  SROTPROPAGATORS_H



