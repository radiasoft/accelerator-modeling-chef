/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      DriftPropagators.cc
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
******    in operator()() implementation
******
**************************************************************************
*************************************************************************/

#include <beamline/DriftPropagators.h>
#include <beamline/Drift.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>

namespace {
  
 typedef PhaseSpaceIndexing::index index; 

 index const i_x   = Particle::i_x;
 index const i_y   = Particle::i_y;
 index const i_cdt = Particle::i_cdt;
 index const i_npx = Particle::i_npx;
 index const i_npy = Particle::i_npy;
 index const i_ndp = Particle::i_ndp;


template<typename Particle_t>
void propagate( Drift const& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.state();

     Component_t npz = p.npz();

     Component_t xpr = state[i_npx] / npz;
     Component_t ypr = state[i_npy] / npz;

     state[i_x] += ( elm.Length() * xpr);
     state[i_y] += ( elm.Length() * ypr);

     Component_t D = elm.Length()*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 

     state[i_cdt] += ( D / p.beta() ) - elm.getReferenceTime();
}
} // namepace 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Drift::Propagator::Propagator()
 : BasePropagator() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Drift::Propagator::Propagator(Drift const& elm)
 : BasePropagator(elm) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
Drift::Propagator::Propagator(Drift::Propagator const& p )
 : BasePropagator(p)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Drift::Propagator* Drift::Propagator::clone() const
{
  return new Propagator(*this); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Drift::Propagator::operator()( BmlnElmnt const& elm, Particle& p ) 
{
 ::propagate(static_cast<Drift const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Drift::Propagator::operator()( BmlnElmnt const& elm, JetParticle&     p ) 
{
 ::propagate(static_cast<Drift const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// ****   MAD style optical propagator ***** 


namespace {

template<typename Particle_t>
void mad_propagate( Drift const& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.state();

     Component_t npz = p.npz();

     Component_t xpr = state[i_npx] / npz;
     Component_t ypr = state[i_npy] / npz;

     double const length =  elm.Length(); 

     state[i_x]   += length * state[i_npx];
     state[i_y]   += length * state[i_npy];
     state[i_cdt] += length /(p.beta()*p.gamma()*p.gamma() ) * state[i_ndp];

     state[i_cdt] -= elm.getReferenceTime();
}

} // namespace


MADDriftPropagator::MADDriftPropagator(): BasePropagator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MADDriftPropagator::MADDriftPropagator(Drift const& elm) : BasePropagator(elm)  
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MADDriftPropagator::MADDriftPropagator(MADDriftPropagator const& p) : BasePropagator(p)  
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| |||||

MADDriftPropagator* MADDriftPropagator::clone() const
{
  return new MADDriftPropagator(*this); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MADDriftPropagator::operator()( BmlnElmnt const& elm, Particle& p )  
{
 ::mad_propagate(static_cast<Drift const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MADDriftPropagator::operator()( BmlnElmnt const& elm, JetParticle& p )
{  
 ::mad_propagate(static_cast<Drift const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
