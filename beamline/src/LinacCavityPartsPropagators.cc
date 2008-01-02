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

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template<typename Particle_t>
void driftpropagate( double const& length, bmlnElmnt& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.State();

     Component_t npz = p.get_npz();

     Component_t xpr = state[i_npx] / npz;
     Component_t ypr = state[i_npy] / npz;

     state[i_x] += length* xpr;
     state[i_y] += length* ypr;

    // NOTE : The reference time is adjusted by LinacCavityPropagator . 

     state[i_cdt ] += length*sqrt( 1.0 + xpr*xpr + ypr*ypr )/p.Beta(); 

     return;     
}


enum Position_t { upstream, downstream }; 

template<typename Element_t, typename Particle_t, Position_t position>
void propagate( Element_t& elm, Particle_t& p ) 
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
  
  boost::function<double(Component_t const& comp)> norm = &PropagatorTraits<Particle_t>::norm;


  const double strength   = elm.Strength();
  const double length     = elm.Length();
  const double phi_s      = elm.getPhi();
  const double w_rf       = elm.getRadialFrequency();
  const double m          = p.Mass();

 
  State_t& state = p.State();

  if( strength == 0.0) { 

  ::driftpropagate( length, elm, p ); 
  state[i_cdt] -= elm.getReferenceTime();
 
   return;
  }

  double         const referenceEnergyGain = strength*cos ( phi_s);
  Component_t    const onaxisEnergyGain    = strength*cos ( phi_s + state[i_cdt] * w_rf / PH_MKS_c );
  Component_t    const eE_z                = onaxisEnergyGain / length;

  if ( position == upstream ) {

    //----------------------------------------------
    // Thin lens kick upon entry for upstream part
    //----------------------------------------------

     Component_t k     = - ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

     state[i_npx] += k*state[i_x];  
     state[i_npy] += k*state[i_y];
  }
  else if ( position == downstream ) {

   //----------------------------------------------------
   // Cavity energy and momentum kick for downstream part
   //----------------------------------------------------

    Component_t E  = p.Energy() + onaxisEnergyGain;

     double oldRefP = p.ReferenceMomentum();
     p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain );
     double newRefP = p.ReferenceMomentum();

     state[i_npx] *= ( oldRefP / newRefP );
     state[i_npy] *= ( oldRefP / newRefP );

     state[i_ndp] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;
   }

  //--------------------------------------------------------------
  // Free space propagation through "effective half length"
  // of the cavity. Note that this an approximation for cdt, since
  // (1) the trajectory is not straight and (2) the velocity varies
  // over the length of the cavity.
  //--------------------------------------------------------------
  
  Component_t x_in  = state[i_x];
  Component_t y_in  = state[i_y];

  ::driftpropagate( length, elm, p ); 

  Component_t w = onaxisEnergyGain/ p.Energy();
 
  if( norm(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                   { w = 1.0;           }

  state[i_x] = ( 1.0 - w )*x_in + w*state[i_x];
  state[i_y] = ( 1.0 - w )*y_in + w*state[i_y];


  if ( position == upstream ) {

   //----------------------------------------------------
   // Cavity energy and momentum kick for upstream part
   //----------------------------------------------------

     Component_t E  = p.Energy() + onaxisEnergyGain;

     double oldRefP = p.ReferenceMomentum();
     p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain );
     double newRefP = p.ReferenceMomentum();

     state[i_npx] *= ( oldRefP / newRefP );
     state[i_npy] *= ( oldRefP / newRefP );

     state[i_ndp] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;
  }  
  
  else if ( position == downstream ) {

   //----------------------------------------------
   // Thin lens kick upon exit for downstream part
   //----------------------------------------------

      Component_t k   = ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

      state[i_npx]  += k*state[i_x];
      state[i_npy]  += k*state[i_y];
  }

  state[i_cdt] -= elm.getReferenceTime();

}

} // anonymous namespace

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::Propagator::setup(  LCavityUpstream& elm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::Propagator::operator()( LCavityUpstream& elm, Particle& p )
{
  ::propagate<LCavityUpstream, Particle, upstream>(elm, p);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::Propagator::operator()( LCavityUpstream& elm, JetParticle& p )
{
  ::propagate<LCavityUpstream, JetParticle, upstream>(elm, p);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::Propagator::setup(  LCavityDnstream& elm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::Propagator::operator()( LCavityDnstream& elm, Particle& p )
{
  ::propagate<LCavityDnstream, Particle, downstream>(elm, p);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::Propagator::operator()( LCavityDnstream& elm, JetParticle& p )
{
  ::propagate<LCavityDnstream, JetParticle, downstream>(elm, p);  
}
