/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BBLensPropagators.h
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
#ifndef  BBLENSPROPAGATORS_H
#define  BBLENSPROPAGATORS_H

#include <beamline/BBLens.h>

class BBLens::Propagator : public BasePropagator<BBLens> {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void       setup( BBLens& elm);
  void  operator()( BBLens& elm,    Particle& p);
  void  operator()( BBLens& elm, JetParticle& p);
};

#endif //  BBLENSPROPAGATORS_H



