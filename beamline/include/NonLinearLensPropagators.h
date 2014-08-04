/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      NonLinearLensPropagtors.h
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
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
******
******  Author:    Chong Shik Park
******             Email: cspark@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******
******  Aug 2014            cspark@fnal.gov
******  - implement non-linear lens first time
******
**************************************************************************
*************************************************************************/

#ifndef  NONLINEARLENSPROPAGATORS_H
#define  NONLINEARLENSPROPAGATORS_H

#include <beamline/nonLinearLens.h>

class Particle;
class JetParticle;

class nonLinearLens::Propagator: public BasePropagator<nonLinearLens> {

 public:

  Propagator* Clone() const { return new Propagator(*this); }

  void setup( nonLinearLens& elm );

  void  operator()( nonLinearLens& elm,             Particle& p);
  void  operator()( nonLinearLens& elm,          JetParticle& p);

};

#endif //  NONLINEARLENSPROPAGATORS_H
