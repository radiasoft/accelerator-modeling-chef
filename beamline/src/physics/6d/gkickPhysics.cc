/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******
******  File:      gkickPhysics.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov      
******                                                                
**************************************************************************
*************************************************************************/

#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/gkick.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

void gkick::localPropagate( Particle& p )
{

  Vector& state = p.getState();

  state[0] += dx_;
  state[1] += dy_;

  //------------------------------------------------------------------
  // dxp_ and dyp_ represent a change in *geometric* trajectory angle; 
  // This angle change is *not* momentum dependent ! 
  //------------------------------------------------------------------

  double npz = p.get_npz(); 
    
  double xp =  p.get_npx()/npz + dxp_;  
  double yp =  p.get_npy()/npz + dyp_;

  double pz = sqrt(1 - xp*xp -yp*yp )*p.Momentum(); 

  state[3] = xp * ( pz / p.ReferenceMomentum() );  
  state[4] = yp * ( pz / p.ReferenceMomentum() ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void gkick::localPropagate( JetParticle& p )
{

  Mapping& state = p.getState();

  state[0] += dx_;
  state[1] += dy_;

  Jet npz = p.get_npz(); 
    
  //------------------------------------------------------------------
  // dxp_ and dyp_ represent a change in *geometric* trajectory angle; 
  // This angle change is *not* momentum dependent ! 
  //------------------------------------------------------------------

  Jet xp =  p.get_npx()/npz + dxp_;  
  Jet yp =  p.get_npy()/npz + dyp_;

  Jet pz = sqrt(1.0 - xp*xp -yp*yp )*p.Momentum(); 
  
  state[3] = xp * ( pz / p.ReferenceMomentum()) ;  
  state[4] = yp * ( pz / p.ReferenceMomentum()) ; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


