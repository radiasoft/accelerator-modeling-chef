/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      EdgePropagators.tcc
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
******
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
******             Jean-Francois Ostiguy  ostiguy@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******  
******  Apr 2015            michelotti@fnal.gov
******  - added option of using dynamically calculated entry and exit
******    angles in the edge propagators, instead of the angles 
******    "hard-wired" by RefRegVisitor. To enable this option,
******    pass a -DNO_FIXED_ENTRY_ANGLE macro definition to the compiler.
******
**************************************************************************
*************************************************************************/

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/beamline.h>
#include <beamline/Edge.h>
#include <beamline/EdgePropagators.h>
#include <beamline/drift.h>
#include <iostream>

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template <typename Particle_t>
void propagate( Edge& elm, Particle_t & p ) 
{

 // "Strength" is B'l in Tesla

 typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
 typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

 State_t& state = p.State();

 if( elm.Strength() == 0.0 ) return; 
 
 #ifdef NO_FIXED_ENTRY_ANGLE
 Component_t const k = ( p.Charge() > 0.0 ) ? (  (p.get_npx()/p.get_npz())*elm.Strength() / p.ReferenceBRho() ) 
                                            : ( -(p.get_npx()/p.get_npz())*elm.Strength() / p.ReferenceBRho() ) ;
 #else
 double const k = ( p.Charge() > 0.0 ) ? ( elm.Strength() / p.ReferenceBRho()) : (-elm.Strength() / p.ReferenceBRho() ) ;
 #endif

 state[i_npy] -=  k * state[i_y];

 
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate( Edge& elm,    Particle& p );
template void propagate( Edge& elm, JetParticle& p );

#endif
//-----------------------------------------------------------------------------------

} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Edge::Propagator::operator()( Edge& elm, Particle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Edge::Propagator::operator()( Edge& elm, JetParticle&     p ) 
{
  ::propagate(elm,p);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

