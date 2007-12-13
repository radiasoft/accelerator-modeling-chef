/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      kickPhysics.cc
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov      
******                                              
****** REVISION HISTORY
******
******  Dec 2007   ostiguy@fnal.gov - new implementation 
******  - angular kick is now dependent on total momentum
******  - templated propagation functions                  
******
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/kick.h>

using namespace std;

using FNAL::pcerr;
using FNAL::pcout;

namespace {

  int const i_x   = Particle::xIndex();     
  int const i_y   = Particle::yIndex();  
  int const i_cdt = Particle::cdtIndex();
  int const i_npx = Particle::npxIndex();
  int const i_npy = Particle::npyIndex();
  int const i_ndp = Particle::ndpIndex();


template<typename Particle_t>
void driftpropagate( double const& length, bmlnElmnt& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.State();

     Component_t npz = p.get_npz();

     Component_t xpr = state[i_npx] / npz;
     Component_t ypr = state[i_npy] / npz;

     state[i_x] += length* xpr;
     state[i_y] += length* ypr;

     state[i_cdt] += length*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 
}

template<typename Particle_t>
void applyKick( kick& elm, Particle_t& p)
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  if (elm.Strength() == 0.0 ) return;

  double const hk =  elm.HorStrength() /  p.ReferenceBRho();
  double const vk =  elm.VerStrength() / p.ReferenceBRho();    
  
  State_t& state = p.State();

  state[i_npx] += hk;
  state[i_npy] += vk;
}

template<typename Particle_t>
void applyKick( hkick& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  if (elm.Strength() == 0.0 ) return;

  double const hk =  elm.Strength()/p.ReferenceBRho();
  
  State_t& state = p.State();

  state[i_npx] += hk;
}

template<typename Particle_t>
void applyKick( vkick& elm, Particle_t& p)
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  if (elm.Strength() == 0.0 ) return;

  double const vk =  elm.Strength()/p.ReferenceBRho();
  
  State_t& state = p.State();

  state[i_npy] += vk;
}

template<typename Element_t, typename Particle_t>
void propagate( Element_t& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t& state = p.State();

  if (elm.Length() > 0.0 ) {

    Component_t const cdt_in = state[i_cdt];

    ::driftpropagate( elm.Length()/2, elm, p );   // Drift through first  half of the length
    applyKick(elm,p); 
    ::driftpropagate( elm.Length()/2, elm, p );   // Drift through second half of the length.

    state[i_cdt] += (state[i_cdt] - cdt_in) - elm.getReferenceTime();  
 }
  else {
    applyKick(elm,p); 
  }
 
 }


} // anonymous namespace


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vkick::localPropagate( Particle& p )
{
  ::propagate(*this,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// =================================================
// hkick propagators
// =================================================

void vkick::localPropagate( JetParticle& p )
{
  ::propagate(*this,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// =================================================
// hkick propagators
// =================================================

void hkick::localPropagate( Particle& p )
{

  ::propagate(*this,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hkick::localPropagate( JetParticle& p )
{
  ::propagate(*this,p);
}

// =================================================
// kick propagators
// =================================================

void kick::localPropagate( Particle& p ) 
{
  ::propagate(*this,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void kick::localPropagate( JetParticle& p ) 
{
  ::propagate(*this,p);
}
