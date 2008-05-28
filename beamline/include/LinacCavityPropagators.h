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
******  May 2008 ostiguy@fnal.gov
******  - propagator moved (back) to base class
******  - generic type bmlnElmnt used as function argument 
******
******
**************************************************************************
*************************************************************************/
#ifndef  LINACCAVITYPROPAGATORS_H
#define  LINACCAVITYPROPAGATORS_H

#include <beamline/LinacCavity.h>

class Particle;
class JetParticle;

class LinacCavity::Propagator : public BasePropagator {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void       setup( bmlnElmnt& elm);

  void  setAttribute (  bmlnElmnt& elm, std::string const& name, boost::any const& value ); 

  void  operator()( bmlnElmnt const& elm,         Particle& p );
  void  operator()( bmlnElmnt const& elm,      JetParticle& p );
  void  operator()( bmlnElmnt const& elm,    ParticleBunch& b ); 
  void  operator()( bmlnElmnt const& elm, JetParticleBunch& b );   

  void setWakeOn(   LinacCavity&       elm, bool set ); 
  bool    wakeOn(   LinacCavity const& elm           ) const; 
};

#endif //  LINACCAVITYPROPAGATORS_H



