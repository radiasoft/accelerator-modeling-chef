/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      DecapolePropagators.cc
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
******
**************************************************************************
*************************************************************************/

#include <beamline/DecapolePropagators.h>
#include <beamline/decapole.h>
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

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
void propagate( thinDecapole& elm, Particle_t & p ) 
{

 // "Strength" is B'l in Tesla

 typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
 typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

 if( elm.Strength() == 0.0 ) return;  
 
 State_t& state = p.State();

 
 double const k      = elm.Strength() / p.ReferenceBRho();

 Component_t x = state[i_x];
 Component_t y = state[i_y];
 
 Component_t  xx = x*x;  
 Component_t  xy = x*y;  
 Component_t  yy = y*y;
 
 state[i_npx] -= ( k * ( xx*xx - 6.0*xy*xy + yy*yy ) );
 state[i_npy] -= ( 4.0 * k * xy * ( xx - yy ) );
}


} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinDecapole::Propagator::operator()( thinDecapole& elm, Particle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinDecapole::Propagator::operator()( thinDecapole& elm, JetParticle&     p ) 
{
  ::propagate(elm,p);
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
