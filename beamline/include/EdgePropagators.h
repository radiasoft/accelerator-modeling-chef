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
******
**************************************************************************
*************************************************************************/
#ifndef  EDGEPROPAGATORS_H
#define  EDGEPROPAGATORS_H

#include <beamline/Edge.h>

class Particle;
class JetParticle;

class Edge::Propagator: public BasePropagator<Edge> {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void  operator()(  Edge& elm,            Particle& p);
  void  operator()(  Edge& elm,         JetParticle& p);

};

#endif //  EDGEPROPAGATORS_H



