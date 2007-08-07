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
**************************************************************************
*************************************************************************/

#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/gkick.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

namespace {

template <typename Particle_t>
void propagate( gkick& elm,  Particle_t& p)
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  state[0] += elm.xOffset();
  state[1] += elm.yOffset();

  Component_t npz = p.get_npz(); 
    
  //------------------------------------------------------------------
  // dxp_ and dyp_ represent a change in *geometric* trajectory angle; 
  // This angle change is *not* momentum dependent ! 
  //------------------------------------------------------------------

  Component_t xp =  p.get_npx()/npz + elm.xOffset();  
  Component_t yp =  p.get_npy()/npz + elm.yOffset();

  Component_t pz = sqrt(1.0 - xp*xp -yp*yp )*p.Momentum(); 
  
  state[3] = xp * ( pz / p.ReferenceMomentum()) ;  
  state[4] = yp * ( pz / p.ReferenceMomentum()) ; 
}

} // anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void gkick::localPropagate( Particle& p )
{

  ::propagate(*this, p);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void gkick::localPropagate( JetParticle& p )
{

  ::propagate(*this, p);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
