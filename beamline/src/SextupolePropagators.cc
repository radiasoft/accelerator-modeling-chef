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
#include <beamline/sextupole.h>
#include <beamline/drift.h>
#include <beamline/SextupolePropagators.h>
#include <iostream>

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template<typename Particle_t>
void propagate( sextupole const& elm, Particle_t&     p )
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  BmlPtr const& bml = bmlnElmnt::core_access::get_BmlPtr(elm);

  for ( beamline::const_iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }
 
  state[i_cdt] -= elm.getReferenceTime(); 
}

template<typename Particle_t>
void propagate( thinSextupole const& elm, Particle_t& p )
{

 // "Strength" is B'l in Tesla

 typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
 typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

 State_t& state = p.State();

 if( elm.Strength() == 0.0 ) return;  
 
 double const k = elm.Strength() / p.ReferenceBRho();

 Component_t const& x = state[i_x];
 Component_t const& y = state[i_y];
 
 state[i_npx] -= k * ( x*x - y*y );
 state[i_npy] += 2.0 * k * x*y;

}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(          sextupole const& elm,    Particle& p );
template void propagate(          sextupole const& elm, JetParticle& p );
template void propagate(      thinSextupole const& elm,    Particle& p );
template void propagate(      thinSextupole const& elm, JetParticle& p );

#endif

} // namespace

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::Propagator::setup(bmlnElmnt& arg)
{ 

 ElmPtr& elm = bmlnElmnt::core_access::get_ElmPtr(arg);
 BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr(arg);


 bml = BmlPtr( new beamline );
 bml->append( DriftPtr( new drift( "", arg.Length()/ 2.0 ) ) );
 bml->append( elm =  ThinSextupolePtr( new thinSextupole( "", arg.Strength()* arg.Length()) ) );
 bml->append( DriftPtr( new drift( "", arg.Length()/ 2.0 ) ) );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::Propagator::setAttribute( bmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  setup(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::Propagator::operator()(bmlnElmnt const& elm, Particle& p)
{
  ::propagate(static_cast<sextupole const&>(elm), p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::Propagator::operator()(bmlnElmnt const& elm, JetParticle& p)
{
  ::propagate(static_cast<sextupole const&>(elm), p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::Propagator::operator()(bmlnElmnt const& elm, Particle& p)
{
  ::propagate(static_cast<thinSextupole const&>(elm), p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::Propagator::operator()(bmlnElmnt const& elm, JetParticle& p)
{
  ::propagate(static_cast<thinSextupole const&>(elm), p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

