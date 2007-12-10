/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      CombinedFunctionPropagators.h
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
******
**************************************************************************
*************************************************************************/

#ifndef COMBINEDFUNCTIONPROPAGATORS_H
#define COMBINEDFUNCTIONPROPAGATORS_H

#include <beamline/combinedFunction.h>

class combinedFunction::Propagator: public BasePropagator<combinedFunction> {

 public:
 
  Propagator* Clone() const { return new Propagator(*this); }

  void  setup( combinedFunction& elm ); 

  void  operator()( combinedFunction& elm,        Particle& p);
  void  operator()( combinedFunction& elm,    JetParticle& p);

};


#endif // COMBINEDFUNCTIONPROPAGATORS_H
