/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      OctupolePropagators.cc
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

#include <beamline/OctupolePropagators.h>
#include <beamline/octupole.h>
#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/Drift.h>

namespace {
 
  typedef PhaseSpaceIndexing::index index;
 
  index const i_x   = Particle::i_x;
  index const i_y   = Particle::i_y;
  index const i_cdt = Particle::i_cdt;
  index const i_npx = Particle::i_npx;
  index const i_npy = Particle::i_npy;
  index const i_ndp = Particle::i_ndp;


template<typename Particle_t>
void propagate( octupole const& elm,  Particle_t& p, BmlPtr bml )
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.state();

  for ( beamline::const_iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }

  state[i_cdt] -= elm.getReferenceTime(); 
}

template <typename Particle_t>
void propagate( thinOctupole const& elm, Particle_t & p ) 
{

 // "Strength" is B'l in Tesla

 typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
 typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

 if( elm.Strength() == 0.0 ) return;  

 State_t& state = p.state();
 
 Component_t x  = state[i_x];
 Component_t y  = state[i_y];
 Component_t xx = x*x;
 Component_t yy = y*y;

 double const k = elm.Strength() / p.refBrho();
    
 state[i_npx] -= k * x * ( xx - 3.0*yy );
 state[i_npy] -= k * y * ( yy - 3.0*xx );
 
}

} // namespace


octupole::Propagator::Propagator(int n)
  : n_(n), BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

octupole::Propagator::Propagator(octupole const& elm, int n)
  : n_(n), BasePropagator(elm)
{
  ctor(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


octupole::Propagator::Propagator( Propagator const& p)
  : n_(p.n_), BasePropagator(p)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void octupole::Propagator::ctor( BmlnElmnt const& arg)
{
  bml_ = BmlPtr( new beamline );
  bml_->append( DriftPtr( new Drift( "", 0.5*arg.Length() ) ) );
  bml_->append( ThinOctupolePtr( new thinOctupole( "", arg.Strength()*arg.Length()) ));
  bml_->append( DriftPtr( new Drift( "", 0.5*arg.Length() ) ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  octupole::Propagator::setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  ctor(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void octupole::Propagator::operator()( BmlnElmnt const& elm, Particle&  p )
{ 
  ::propagate( static_cast<octupole const&>(elm),p, bml_ );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void octupole::Propagator::operator()(  BmlnElmnt const& elm, JetParticle&  p ) 
{ 
  ::propagate( static_cast<octupole const&>(elm),p, bml_ );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinOctupole::Propagator::Propagator()
  : BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinOctupole::Propagator::Propagator( thinOctupole const& elm)
  : BasePropagator(elm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


thinOctupole::Propagator::Propagator( Propagator const& p)
  : BasePropagator(p)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinOctupole::Propagator::operator()( BmlnElmnt const& elm, Particle&  p )
{ 
  ::propagate( static_cast<thinOctupole const&>(elm),p );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinOctupole::Propagator::operator()(  BmlnElmnt const& elm, JetParticle&  p ) 
{ 
  ::propagate( static_cast<thinOctupole const&>(elm),p );
}

