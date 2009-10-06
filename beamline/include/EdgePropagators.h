/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      EdgePropagators.h
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
******  Authors:   Leo Michelotti         michelotti@fnal.gov
******             Jean-Francois Ostiguy  ostiguy@fnal.gov
******
******  May 2008 ostiguy@fnal.gov
******  - propagator moved (back) to base class
******  - generic type BmlnElmnt used as function argument 
******
**************************************************************************
*************************************************************************/
#ifndef  EDGEPROPAGATORS_H
#define  EDGEPROPAGATORS_H

#include <beamline/BasePropagator.h>
#include <beamline/Edge.h>
#include <beamline/ParticleFwd.h>


class Edge::Propagator: public BasePropagator {

 public:
 
  Propagator();
  Propagator(Edge const& elm); 
  Propagator(Propagator const& p);

  Propagator* clone() const { return new Propagator(*this); }

  void  operator()(  BmlnElmnt const& elm,            Particle& p);
  void  operator()(  BmlnElmnt const& elm,         JetParticle& p);

};

#endif //  EDGEPROPAGATORS_H



