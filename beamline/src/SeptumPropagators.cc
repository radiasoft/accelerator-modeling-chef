/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SeptumPropagators.cc
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

#include <beamline/SeptumPropagators.h>
#include <beamline/septum.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

namespace {

  typedef PhaseSpaceIndexing::index index; 

  index const i_x   = Particle::i_x;
  index const i_y   = Particle::i_y;
  index const i_cdt = Particle::i_cdt;
  index const i_npx = Particle::i_npx;
  index const i_npy = Particle::i_npy;
  index const i_ndp = Particle::i_ndp;


template <typename Particle_t>
void propagate( thinSeptum const& elm, Particle_t& p );

template <>
void propagate( thinSeptum const& elm, Particle& p )
{
     typedef PropagatorTraits<Particle>::State_t       State_t;
     typedef PropagatorTraits<Particle>::Component_t   Component_t;
 
     State_t& state = p.state();

     Component_t kick = ( state[i_x] > elm.getWireX() ) ? 
                         elm.getPosStrength()/(1.0 + state[i_ndp]) :  elm.getNegStrength()/(1.0 + state[i_ndp]); 
  
     state[3] += kick;

}

template <>
void propagate( thinSeptum const& elm, JetParticle& p )
{
     typedef PropagatorTraits<JetParticle>::State_t       State_t;
     typedef PropagatorTraits<JetParticle>::Component_t   Component_t;
 
     State_t& state = p.state();

     Component_t kick = ( state[i_x].standardPart() > elm.getWireX() ) ? 
                         elm.getPosStrength()/(1.0 + state[i_ndp]) :  elm.getNegStrength()/(1.0 + state[i_ndp]); 
  
     state[3] += kick;

}

} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


thinSeptum::Propagator::Propagator()
  :BasePropagator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::Propagator::Propagator(thinSeptum const& elm)
  : BasePropagator(elm)
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::Propagator::Propagator(Propagator const& p)
  : BasePropagator(p)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinSeptum::Propagator::ctor(BmlnElmnt const& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinSeptum::Propagator::operator()( BmlnElmnt const& elm, Particle& p ) 
{
  ::propagate(static_cast<thinSeptum const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::Propagator::operator()( BmlnElmnt const& elm, JetParticle&     p ) 
{
  ::propagate(static_cast<thinSeptum const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

