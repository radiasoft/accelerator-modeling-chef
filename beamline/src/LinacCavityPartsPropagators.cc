/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.
******  File:      LinacCavityPartsPropagators.cc
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Authors:    Jean-Francois Ostiguy                                     
******              ostiguy@fnal.gov                         
******             
******              Leo Michelotti        
******              michelotti@fnal.gov
******
****** The propagation algorithm is based on code 
****** originally written by LM.    
******
****** REVISION HISTORY:
******
****** Dec 2008 ostiguy@fnal.gov
******
****** Restructured code to eliminate duplication. Ensure that
****** the entrance kick is applied correctly; this was done 
****** correctly in LM's original code.
******
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation
******
******
****** PLEASE NOTE:
****** ------------
****** This propagator assumes 
******
****** (1) v/c ~ 1    (relativistic approximation)
****** (2) px/pz << 1 (paraxial approximation)
******
**************************************************************************
**************************************************************************
*************************************************************************/

#include<iostream>
#include<basic_toolkit/PhysicsConstants.h>
#include<beamline/LinacCavityPartsPropagators.h>
#include<beamline/LinacCavityParts.h>
#include<beamline/Particle.h>
#include<beamline/ParticleBunch.h>
#include<beamline/JetParticle.h>

using namespace PhysicsConstants;

namespace { // anonymous namespace

  typedef PhaseSpaceIndexing::index index;

  index const i_x   = Particle::i_x;
  index const i_y   = Particle::i_y;
  index const i_cdt = Particle::i_cdt;
  index const i_npx = Particle::i_npx;
  index const i_npy = Particle::i_npy;
  index const i_ndp = Particle::i_ndp;

  enum Position_t { upstream, downstream }; 

  template<typename Component_t>
  inline double toDouble( Component_t const& val );

  template<>
  inline double toDouble( Jet const& val ) { return val.standardPart(); }

  template<>
  inline double toDouble( double const& val ) { return val; }


// ------------------------------------------------------------------------------


template<typename Particle_t>
void driftpropagate( typename PropagatorTraits<Particle_t>::Component_t const& length, BmlnElmnt const& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.state();

     Component_t npz = p.npz();

     Component_t xpr = state[i_npx] / npz ;
     Component_t ypr = state[i_npy] / npz ;

     state[i_x] += length* xpr;
     state[i_y] += length* ypr;

     state[i_cdt ] += length*sqrt( 1.0 + xpr*xpr + ypr*ypr )/p.beta(); 

     return;      
}

// ------------------------------------------------------------------------------

template<typename Particle_t>
void edge_focusing_kick( Particle_t& p,  typename PropagatorTraits<Particle_t>::Component_t const& eE_z, double const& sign )
{
 
  //------------------------------------------------------
  // sign : entrance kick is negative (toward     axis)
  //        exit     kick is positive (away from  axis)
  //------------------------------------------------------

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.state();

  Component_t k   = sign * eE_z/p.refMomentum() / ( 2.0*p.betaZ() );

  state[i_npx] += k*state[i_x];  
  state[i_npy] += k*state[i_y];

}

// ------------------------------------------------------------------------------

template<typename Particle_t>
void acceleration_kick( Particle_t& p,  typename PropagatorTraits<Particle_t>::Component_t const& onaxisEnergyGain, double const& referenceEnergyGain )
{
   
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  double const m          = p.mass();

  State_t& state = p.state();

  Component_t E  = p.energy() + onaxisEnergyGain;

  double oldRefP = p.refMomentum();
  p.setRefEnergy( p.refEnergy() + referenceEnergyGain );
  double newRefP = p.refMomentum();

  state[i_npx] *= ( oldRefP / newRefP );
  state[i_npy] *= ( oldRefP / newRefP );

  Component_t one_over_gamma2 = (m*m)/(E*E);
  Component_t beta  = sqrt(1.0 - one_over_gamma2 );

  state[i_ndp] = ( ( E * beta) / newRefP ) - 1.0;

}

// ------------------------------------------------------------------------------

template<typename Element_t, typename Particle_t, Position_t position>
void propagate( Element_t const& elm, Particle_t& p ) 
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
  
  boost::function<double(Component_t const& comp)> norm = &PropagatorTraits<Particle_t>::norm;


  double const strength   = elm.Strength();
  double const length     = elm.Length();
  double const phi_s      = elm.phi();
  double const w_rf       = elm.radialFrequency();
 
  State_t& state = p.state();
  
  if( strength == 0.0) { 
    ::driftpropagate( length, elm, p ); 
    state[i_cdt] -= elm.getReferenceTime();
    return;
  }

  double         const referenceEnergyGain = strength*cos ( phi_s);
  Component_t    const onaxisEnergyGain    = strength*cos ( phi_s + state[i_cdt] * w_rf / PH_MKS_c );
  Component_t    const eE_z                = onaxisEnergyGain / length;

  // this defined here and used later for ct correction  
 
  Component_t    const E_i                 = p.energy();
  Component_t    const pz_i                = p.npz() * p.refMomentum();


  if ( position == upstream ) { ::edge_focusing_kick( p, eE_z, -1.0 ); }

  Component_t xp0 = state[i_npx]/p.npz();
  Component_t yp0 = state[i_npy]/p.npz();

  Component_t w = onaxisEnergyGain/ p.energy();

  w = ( norm(w) > 1.0e-10 ) ? log(1.0 + w )/ w : 1.0;

  ::acceleration_kick( p, onaxisEnergyGain, referenceEnergyGain );

  //--------------------------------------------------------------
  // Free space propagation through "effective half length"
  // of the cavity. Note that this an approximation for cdt, since
  // (1) the trajectory is not straight and (2) the velocity varies
  // over the length of the cavity.
  //--------------------------------------------------------------
  

  // drift through "effective" length  w*length

  state[ i_x ] +=  xp0 * w * length; 
  state[ i_y ] +=  yp0 * w * length; 


  if ( position == downstream ) { ::edge_focusing_kick( p, eE_z, 1.0); }

  // Fix the time calculation

  state[i_cdt] +=  (length * (( p.npz() * p.refMomentum() - pz_i ) / (p.energy() - E_i) ) - elm.getReferenceTime() );
 

}

} // anonymous namespace

LCavityUpstream::Propagator::Propagator()
  :BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityUpstream::Propagator::Propagator(LCavityUpstream const& elm)
  :BasePropagator(elm)
{
  ctor(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityUpstream::Propagator::Propagator(Propagator const& p)
  :BasePropagator(p)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::Propagator::ctor(  BmlnElmnt const& elm )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::Propagator::setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  ctor(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::Propagator::operator()(  BmlnElmnt const& elm, Particle& p )
{
  ::propagate<LCavityUpstream,  Particle, upstream>( static_cast<LCavityUpstream const&>(elm), p );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p )
{
  ::propagate<LCavityUpstream, JetParticle, upstream>( static_cast<LCavityUpstream const&>(elm), p);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LCavityDnstream::Propagator::Propagator()
  :BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityDnstream::Propagator::Propagator(LCavityDnstream const& elm)
  : BasePropagator(elm)
{
  ctor(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LCavityDnstream::Propagator::Propagator(Propagator const& p)
 : BasePropagator(p)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::Propagator::ctor(  BmlnElmnt const& elm )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::Propagator::setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  ctor(elm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::Propagator::operator()( BmlnElmnt const& elm, Particle& p )
{
  ::propagate<LCavityDnstream, Particle, downstream>( static_cast<LCavityDnstream const&>(elm), p);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p )
{
  ::propagate<LCavityDnstream, JetParticle, downstream>( static_cast<LCavityDnstream const&>(elm), p);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


