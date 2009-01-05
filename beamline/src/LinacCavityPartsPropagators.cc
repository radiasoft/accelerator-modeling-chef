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

namespace { // anonymous namespace

  Particle::PhaseSpaceIndex const& i_x   = Particle::i_x;
  Particle::PhaseSpaceIndex const& i_y   = Particle::i_y;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::i_cdt;
  Particle::PhaseSpaceIndex const& i_npx = Particle::i_npx;
  Particle::PhaseSpaceIndex const& i_npy = Particle::i_npy;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::i_ndp;


  enum Position_t { upstream, downstream }; 


  template<typename Component_t>
  double toDouble( Component_t const& val );

  template<>
  double toDouble( Jet const& val ) { return val.standardPart(); }

  template<>
  double toDouble( double const& val ) { return val; }


// ------------------------------------------------------------------------------


template<typename Particle_t>
void driftpropagate( typename PropagatorTraits<Particle_t>::Component_t const& length, bmlnElmnt const& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.State();

     Component_t npz = p.get_npz();

     Component_t xpr = state[i_npx] / npz ;
     Component_t ypr = state[i_npy] / npz ;

     state[i_x] += length* xpr;
     state[i_y] += length* ypr;

     state[i_cdt ] += length*sqrt( 1.0 + xpr*xpr + ypr*ypr )/p.Beta(); 

     return;      
}

// ------------------------------------------------------------------------------

template<typename Particle_t>
void edge_focusing_kick( Particle_t& p,  typename PropagatorTraits<Particle_t>::Component_t const& eE_z, double const& sign )
{
 
  // sign : entrance kick is negative (toward     axis)
  //        exit     kick is positive (away from  axis)

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  Component_t k   = sign*( eE_z/p.Momentum() ) / ( 2.0*p.BetaZ() );

  state[i_npx] += k*state[i_x];  
  state[i_npy] += k*state[i_y];

}

// ------------------------------------------------------------------------------

template<typename Particle_t>
void acceleration_kick( Particle_t& p,  typename PropagatorTraits<Particle_t>::Component_t const& onaxisEnergyGain, double const& referenceEnergyGain )
{
   
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  double const m          = p.Mass();

  State_t& state = p.State();

  Component_t E  = p.Energy() + onaxisEnergyGain;

  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain );
  double newRefP = p.ReferenceMomentum();

  state[i_npx] *= ( oldRefP / newRefP );
  state[i_npy] *= ( oldRefP / newRefP );

  state[i_ndp] = ( sqrt((E - m)*(E + m)) / newRefP ) - 1.0;

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
 
  State_t& state = p.State();
  
  if( strength == 0.0) { 
    ::driftpropagate( length, elm, p ); 
    state[i_cdt] -= elm.getReferenceTime();
    return;
  }

  double         const referenceEnergyGain = strength*cos ( phi_s);
  Component_t    const onaxisEnergyGain    = strength*cos ( phi_s + state[i_cdt] * w_rf / PH_MKS_c );
  Component_t    const eE_z                = onaxisEnergyGain / length;

  // this defined here and used later for ct correction  
 
  Component_t    const E_i                 = p.Energy();
  Component_t    const p_i                 = p.get_npz() * p.ReferenceMomentum();


  if ( position == upstream ) { ::edge_focusing_kick( p, eE_z, -1.0 ); }

  Component_t xp0 = state[i_npx]/p.get_npz();
  Component_t yp0 = state[i_npy]/p.get_npz();

  ::acceleration_kick( p, onaxisEnergyGain, referenceEnergyGain );

  //--------------------------------------------------------------
  // Free space propagation through "effective half length"
  // of the cavity. Note that this an approximation for cdt, since
  // (1) the trajectory is not straight and (2) the velocity varies
  // over the length of the cavity.
  //--------------------------------------------------------------
  
  Component_t w = onaxisEnergyGain/ p.Energy();
  
   w = ( norm(w) > 1.0e-10 ) ? log(1.0 +w )/w : 1.0;

  // drift through "effective" length  w*length

  state[ i_x ] += xp0 * w * length; 
  state[ i_y ] += yp0 * w * length; 

  if ( position == downstream ) { ::edge_focusing_kick( p, eE_z, 1.0); }


  // Fix the time calculation

  state[i_cdt] =  length * (( p.get_npz() * p.ReferenceMomentum() - p_i ) / (p.Energy() - E_i) ) - elm.getReferenceTime();

}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void driftpropagate( Component_t length, bmlnElmnt& elm, Particle& p );
template void driftpropagate( Component_t length, bmlnElmnt& elm, JetParticle& p );

template void propagate<LCavityUpstream, Particle,      upstream>( LCavityUpstream& elm,    Particle& p );
template void propagate<LCavityUpstream, JetParticle,   upstream>( LCavityUpstream& elm, JetParticle& p );
template void propagate<LCavityDnstream, Particle,    downstream>( LCavityDnstream& elm,    Particle& p );
template void propagate<LCavityDnstream, JetParticle, downstream>( LCavityDnstream& elm, JetParticle& p );
template void edge_focusing_kick<Particle>   (    Particle& p,  typename PropagatorTraits<Particle>::Component_t const& eE_z, 
                                                  double const& sign );
template void edge_focusing_kick<JetParticle>( JetParticle& p,  typename PropagatorTraits<JetParticle>::Component_t const& eE_z, 
                                                  double const& sign );
template void acceleration_kick<Particle>( Particle& p,  typename PropagatorTraits<Particle>::Component_t const& onaxisEnergyGain, 
                                          double const& referenceEnergyGain )
template void acceleration_kick<JetParticle>( JetParticle& p,  typename PropagatorTraits<JetParticle>::Component_t const& onaxisEnergyGain, 
                                          double const& referenceEnergyGain )

#endif
//-----------------------------------------------------------------------------------


} // anonymous namespace

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::Propagator::setup(  bmlnElmnt& elm )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::Propagator::setAttribute( bmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  setup(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::Propagator::operator()(  bmlnElmnt const& elm, Particle& p )
{
  ::propagate<LCavityUpstream,  Particle, upstream>( static_cast<LCavityUpstream const&>(elm), p );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::Propagator::operator()( bmlnElmnt const& elm, JetParticle& p )
{
  ::propagate<LCavityUpstream, JetParticle, upstream>( static_cast<LCavityUpstream const&>(elm), p);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void  LCavityDnstream::Propagator::setup(  bmlnElmnt& elm )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::Propagator::setAttribute( bmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  setup(elm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::Propagator::operator()( bmlnElmnt const& elm, Particle& p )
{
  ::propagate<LCavityDnstream, Particle, downstream>( static_cast<LCavityDnstream const&>(elm), p);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::Propagator::operator()( bmlnElmnt const& elm, JetParticle& p )
{
  ::propagate<LCavityDnstream, JetParticle, downstream>( static_cast<LCavityDnstream const&>(elm), p);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


