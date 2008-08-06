/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BBLensPropagators.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******  Authors:  Leo   Michelotti  michelotti@fnal.gov
******            J.-F. Ostiguy     ostiguy@fnal.gov
******                                                                
****** REVISION HISTORY:
******
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation
******
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/BBLensPropagators.h>
#include <beamline/BBLens.h>
#include <basic_toolkit/PhysicsConstants.h>

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template<typename Particle_t>
void propagate( BBLens const& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 

  if( elm.Strength() == 0.0 ) return;

  State_t& state = p.State();
  
  double num = elm.getDistCharge();
 
  Component_t x = state[i_x];
  Component_t y = state[i_y];
 
  State_t E  = elm.NormalizedEField( x, y );

  State_t p_beta = p.VectorBeta();

  State_t K  = elm.Beta()^E;
          K  = p_beta^K;
          K  += E; 
 
  Component_t pn =  p.Beta()*p.Gamma();

  K = - num*PH_MKS_rp*K / ( p.Beta()*pn );

  if( p.Charge()*num < 0.0 ) K = -K;  // ??? Check this!

  state[i_npx] += K[0];
  state[i_npy] += K[1];

}

//-----------------------------------------------------------------
// Workaround for gcc < 4.2 bug 
//-----------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate( BBLens const& elm,    Particle& p );
template void propagate( BBLens const& elm, JetParticle& p );

#endif
//-----------------------------------------------------------------

} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BBLens::Propagator* BBLens::Propagator::Clone() const
{ 
  return new Propagator(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::Propagator::setup( bmlnElmnt& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::Propagator::operator()( bmlnElmnt const& elm, Particle& p ) 
{
  ::propagate( static_cast<BBLens const&>(elm), p );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::Propagator::operator()( bmlnElmnt const& elm, JetParticle& p ) 
{
  ::propagate( static_cast<BBLens const&>(elm), p );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

