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
#include <beamline/drift.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::i_x;
  Particle::PhaseSpaceIndex const& i_y   = Particle::i_y;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::i_cdt;
  Particle::PhaseSpaceIndex const& i_npx = Particle::i_npx;
  Particle::PhaseSpaceIndex const& i_npy = Particle::i_npy;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::i_ndp;


template<typename Particle_t>
void propagate( drift const& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.State();

     Component_t npz = p.get_npz();

     Component_t xpr = state[i_npx] / npz;
     Component_t ypr = state[i_npy] / npz;

     state[i_x] += ( elm.Length() * xpr);
     state[i_y] += ( elm.Length() * ypr);

     Component_t D = elm.Length()*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 

     state[i_cdt] += ( D / p.Beta() ) - elm.getReferenceTime();
}

template<typename Particle_t>
void mad_propagate( drift const& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.State();

     Component_t npz = p.get_npz();

     Component_t xpr = state[i_npx] / npz;
     Component_t ypr = state[i_npy] / npz;

     double const length =  elm.Length(); 

     state[i_x]   += length * state[i_npx];
     state[i_y]   += length * state[i_npy];
     state[i_cdt] += length /(p.Beta()*p.Gamma()*p.Gamma() ) * state[i_ndp];

     state[i_cdt] -= elm.getReferenceTime();
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(     drift const& elm,    Particle& p );
template void propagate(     drift const& elm, JetParticle& p );
template void mad_propagate( drift const& elm,    Particle& p );
template void mad_propagate( drift const& elm, JetParticle& p );

#endif
//-----------------------------------------------------------------------------------
} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

drift::Propagator* drift::Propagator::Clone() const
{
  return new Propagator(*this); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void drift::Propagator::operator()( bmlnElmnt const& elm, Particle& p ) 
{
 ::propagate(static_cast<drift const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void drift::Propagator::operator()( bmlnElmnt const& elm, JetParticle&     p ) 
{
 ::propagate(static_cast<drift const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// ****   MAD style optical propagator ***** 


MADDriftPropagator* MADDriftPropagator::Clone() const
{
  return new MADDriftPropagator(*this); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MADDriftPropagator::operator()( bmlnElmnt const& elm, Particle& p )  
{
 ::mad_propagate(static_cast<drift const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MADDriftPropagator::operator()( bmlnElmnt const& elm, JetParticle& p )
{  
 ::mad_propagate(static_cast<drift const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
