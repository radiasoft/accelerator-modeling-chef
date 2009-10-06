/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SextupolePropagators.cc
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

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/beamline.h>
#include <beamline/Drift.h>
#include <beamline/sextupole.h>
#include <beamline/SextupolePropagators.h>
#include <iostream>

namespace {

  typedef PhaseSpaceIndexing::index index;

 index const i_x   = Particle::i_x;
 index const i_y   = Particle::i_y;
 index const i_cdt = Particle::i_cdt;
 index const i_npx = Particle::i_npx;
 index const i_npy = Particle::i_npy;
 index const i_ndp = Particle::i_ndp;


template<typename Particle_t>
void propagate( sextupole const& elm, Particle_t& p, BmlPtr bml)
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.state();

  for ( beamline::const_iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }
 
  state[i_cdt] -= elm.getReferenceTime(); 
}

//-----------------------------------------------------------------------------

template<typename Particle_t>
void propagate( thinSextupole const& elm, Particle_t& p )
{

 // "Strength" is B'l in Tesla

 typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
 typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

 State_t& state = p.state();

 if( elm.Strength() == 0.0 ) return;  
 
 double const k = elm.Strength() / p.refBrho();

 Component_t const& x = state[i_x];
 Component_t const& y = state[i_y];
 
 state[i_npx] -= k * ( x*x - y*y );
 state[i_npy] += 2.0 * k * x*y;

}

} // namespace

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::Propagator::Propagator ( int n)
  : n_(n), BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::Propagator::Propagator ( sextupole const& elm, int n)
  : n_(n), BasePropagator(elm)
{
  ctor(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::Propagator::Propagator ( Propagator const& p)
 : n_(p.n_), BasePropagator(p)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
void sextupole::Propagator::ctor(BmlnElmnt const& arg)
{ 

 bml_ = BmlPtr( new beamline );
 bml_->append( DriftPtr( new Drift( "", arg.Length()/ 2.0 ) ) );
 bml_->append( ThinSextupolePtr( new thinSextupole( "", arg.Strength()* arg.Length()) ) );
 bml_->append( DriftPtr( new Drift( "", arg.Length()/ 2.0 ) ) );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::Propagator::setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  ctor(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::Propagator::operator()(BmlnElmnt const& elm, Particle& p)
{
  ::propagate(static_cast<sextupole const&>(elm), p, bml_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::Propagator::operator()(BmlnElmnt const& elm, JetParticle& p)
{
  ::propagate(static_cast<sextupole const&>(elm), p, bml_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSextupole::Propagator::Propagator ()
  : BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSextupole::Propagator::Propagator ( thinSextupole const& elm)
  : BasePropagator(elm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSextupole::Propagator::Propagator ( Propagator const& p)
 : BasePropagator(p)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::Propagator::operator()(BmlnElmnt const& elm, Particle& p)
{
  ::propagate(static_cast<thinSextupole const&>(elm), p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::Propagator::operator()(BmlnElmnt const& elm, JetParticle& p)
{
  ::propagate(static_cast<thinSextupole const&>(elm), p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

