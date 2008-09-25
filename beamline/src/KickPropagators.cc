/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      KickPropagators.cc
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
******  Authors:    Jean-Francois Ostiguy ostiguy@fnal.gov
******              Leo Michelotti        michelotti@fnal.gov
******              
****** REVISION HISTORY:
******
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation
******
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
#include <beamline/KickPropagators.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::i_x;
  Particle::PhaseSpaceIndex const& i_y   = Particle::i_y;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::i_cdt;
  Particle::PhaseSpaceIndex const& i_npx = Particle::i_npx;
  Particle::PhaseSpaceIndex const& i_npy = Particle::i_npy;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::i_ndp;


template<typename Particle_t>
void driftpropagate( double const& length, bmlnElmnt const& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.State();

     Component_t npz = p.get_npz();

     Component_t xpr = state[i_npx] / npz;
     Component_t ypr = state[i_npy] / npz;
    

     state[i_x] += length* xpr;
     state[i_y] += length* ypr;

     state[i_cdt] += length*sqrt( 1.0 + xpr*xpr + ypr*ypr )/ p.Beta(); 
}

template<typename Particle_t>
void applyKick( kick const& elm, Particle_t& p)
{
    typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
    typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

    double const hk =  elm.getHorStrength() / p.ReferenceBRho();
    double const vk =  elm.getVerStrength() / p.ReferenceBRho();    

    State_t& state = p.State();

    state[i_npx] += hk;
    state[i_npy] += vk;
}

template<typename Particle_t>
void applyKick( hkick const& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  double const hk =  elm.Strength() /  p.ReferenceBRho();

  State_t& state = p.State();

  state[i_npx] += hk;
}

template<typename Particle_t>
void applyKick( vkick const& elm, Particle_t& p)
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  double const vk =  elm.Strength() /  p.ReferenceBRho();

  State_t& state = p.State();

  state[i_npy] += vk;
}

template<typename Element_t, typename Particle_t>
void propagate( Element_t const& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t& state = p.State();

  if (elm.Length() > 0.0 ) {

    Component_t const cdt_in = state[i_cdt];

    ::driftpropagate( elm.Length()/2, elm, p );   // Drift through first  half of the length
    applyKick(elm,p); 
    ::driftpropagate( elm.Length()/2, elm, p );   // Drift through second half of the length.

    state[i_cdt] -= elm.getReferenceTime();  
 }
  else {
    applyKick(elm,p); 
  }
 
 }

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void driftpropagate( double const& length, bmlnElmnt const& elm, Particle& p );
template void driftpropagate( double const& length, bmlnElmnt const& elm, JetParticle& p );

template void applyKick( kick  const& elm,    Particle& p);
template void applyKick( kick  const& elm, JetParticle& p);
template void applyKick( hkick const& elm,    Particle& p);
template void applyKick( hkick const& elm, JetParticle& p);
template void applyKick( vkick const& elm,    Particle& p);
template void applyKick( vkick const& elm, JetParticle& p);

template void propagate( kick  const& elm,    Particle& p );
template void propagate( kick  const& elm, JetParticle& p );
template void propagate( hkick const& elm,    Particle& p );
template void propagate( hkick const& elm, JetParticle& p );
template void propagate( vkick const& elm,    Particle& p );
template void propagate( vkick const& elm, JetParticle& p );

#endif
//-----------------------------------------------------------------------------------


} // anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void kick::Propagator::setup( bmlnElmnt& elm ) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void kick::Propagator::operator()( bmlnElmnt const& elm, Particle& p ) 
{
  ::propagate(static_cast<kick const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void kick::Propagator::operator()( bmlnElmnt const& elm, JetParticle& p ) 
{
  ::propagate(static_cast<kick const&>(elm),p);
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hkick::Propagator::setup(  bmlnElmnt& elm ) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hkick::Propagator::operator()( bmlnElmnt const& elm, Particle& p ) 
{
  ::propagate(static_cast<hkick const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hkick::Propagator::operator()( bmlnElmnt const&  elm, JetParticle& p ) 
{
  ::propagate(static_cast<hkick const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vkick::Propagator::setup( bmlnElmnt& elm ) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vkick::Propagator::operator()( bmlnElmnt const& elm, Particle& p ) 
{
  ::propagate(static_cast<vkick const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vkick::Propagator::operator()( bmlnElmnt const& elm, JetParticle& p ) 
{
  ::propagate(static_cast<vkick const&>(elm),p );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
