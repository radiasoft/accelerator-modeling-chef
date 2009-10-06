/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BeamlinePropagators.h
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
******  Author: Jean-Francois Ostiguy  
******          ostiguy@fnal.gov
******
******
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef  BEAMLINEPROPAGATORS_H
#define  BEAMLINEPROPAGATORS_H

#include <beamline/BasePropagator.h>
#include <beamline/beamline.h>
#include <beamline/ParticleFwd.h>

class beamline::Propagator: public BasePropagator {

 public:

  Propagator ();
  Propagator ( beamline   const& elm );
  Propagator ( Propagator const& o   );

  Propagator* clone() const;

  double       getReferenceTime()  const;
  void         setReferenceTime( double );

  void  operator()( BmlnElmnt const& elm,                  Particle& p);
  void  operator()( BmlnElmnt const& elm,               JetParticle& p);
  void  operator()( BmlnElmnt const& elm,             ParticleBunch& p);
  void  operator()( BmlnElmnt const& elm,          JetParticleBunch& p);

 
};

#endif //  BEAMLINEPROPAGATORS_H



