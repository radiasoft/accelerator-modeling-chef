/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      DipedgePropagators.cc
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
******  Author:    Leo Michelotti
******             Phone: (630) 840 4956
******             Email: michelotti@fnal.gov
******
******
****** REVISION HISTORY
****** ----------------
******
****** Apr 2013           Leo Michelotti
******                    michelotti@fnal.gov
****** - initial implementation, complete with errors.
******   THIS IS ONLY A PROTOTYPE VERSION, little
******   more than a placeholder file. 
****** - class Dipedge is meant to mimic the DIPEDGE element
******   used by MAD-X.
****** - Associated files:
******     beamline/include/Dipedge.h
******     beamline/src/Dipedge.cc
******     beamline/src/DipedgePropagators.cc
******
**************************************************************************
*************************************************************************/

#ifndef DIPEDGEPROPAGATORS_H
#define DIPEDGEPROPAGATORS_H

#include <beamline/Dipedge.h>

class Particle;
class JetParticle;

class Dipedge::Propagator : public BasePropagator<Dipedge>
{
 public:
  // No ctors  ???

  inline Propagator* Clone() const;

  void  setup( Dipedge& elm );

  void  operator()( Dipedge& elm,    Particle& p);
  void  operator()( Dipedge& elm, JetParticle& p);

  // Bunch propagators not redefined; will use the default.
};


Dipedge::Propagator* Dipedge::Propagator::Clone() const
{ 
  return new Dipedge::Propagator( *this ); 
}

#endif // DIPEDGEPROPAGATORS_H
