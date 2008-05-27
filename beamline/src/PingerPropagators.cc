/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      PingerPropagators.cc
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
****** REVISION HISTORY:
******
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation.
******
**************************************************************************
*************************************************************************/

#include <beamline/pinger.h>
#include <beamline/PingerPropagators.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template<typename Particle_t>
void propagate( Pinger const& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t& state = p.State();

  if ( elm.isArmed() && elm.countdown() ) {
      state[i_npx] += cos( elm.getKickDirection()) * elm.Strength();
      state[i_npy] += sin( elm.getKickDirection()) * elm.Strength();
  }

} 

template<typename Particle_t>
void propagate_bunch( Pinger const& elm, TBunch<Particle_t>& b )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  if ( elm.isArmed() && elm.countdown() ) {

      for ( typename TBunch<Particle_t>::iterator it = b.begin(); it != b.end(); ++it ) {

        State_t& state = it->State();
        state[i_npx] += cos(elm.getKickDirection())* elm.Strength();
        state[i_npy] += sin(elm.getKickDirection())* elm.Strength();

      }
  }
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(     Pinger const& elm,    Particle& p );
template void propagate(     Pinger const& elm, JetParticle& p );
template void propagate_bunch( Pinger const& elm, TBunch<Particle>& b );
template void propagate_bunch( Pinger const& elm, TBunch<JetParticle>& b );

#endif

} // namespace

 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::Propagator::setup( bmlnElmnt& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::Propagator::operator()( bmlnElmnt const& elm, Particle& p ) 
{
  ::propagate(static_cast<Pinger const&>(elm),p );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::Propagator::operator()( bmlnElmnt const& elm, JetParticle&     p ) 
{
  ::propagate(static_cast<Pinger const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::Propagator::operator()( bmlnElmnt const& elm, ParticleBunch& b ) 
{
  ::propagate_bunch(static_cast<Pinger const&>(elm),b);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::Propagator::operator()( bmlnElmnt const& elm, JetParticleBunch&     b ) 
{
  ::propagate_bunch(static_cast<Pinger const&>(elm),b);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

