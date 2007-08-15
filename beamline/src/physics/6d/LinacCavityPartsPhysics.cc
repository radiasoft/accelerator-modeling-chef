#include<iostream>
#include<basic_toolkit/PhysicsConstants.h>
#include<beamline/LinacCavityParts.h>
#include<beamline/Particle.h>
#include<beamline/ParticleBunch.h>
#include<beamline/JetParticle.h>

namespace { // anonymous namespace

template<typename Particle_t>
void propagate( bmlnElmnt& elm_arg, Particle_t& p ,bool entrance_kick, bool exit_kick ) 
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
  
  boost::function<double(Component_t const& comp)> norm = &PropagatorTraits<Particle_t>::norm;

  LCavityUpstream& elm = static_cast<LCavityUpstream&>(elm_arg);

  const double strength   = elm.Strength();
  const double length     = elm.Length();
  const double phi_s      = elm.getPhi();
  const double w_rf       = elm.getRadialFrequency();
  const double m          = p.Mass();
 
  State_t& state = p.State();

  if( strength == 0.0) { 
    elm.bmlnElmnt::localPropagate( p ); 
    return;
  }

  double         const referenceEnergyGain = strength*cos ( phi_s);
  Component_t    const onaxisEnergyGain    = strength*cos ( phi_s + state[2] * w_rf / PH_MKS_c );
  Component_t    const eE_z                = onaxisEnergyGain / length;

  //---------------------------
  // Thin lens kick upon entry
  //---------------------------

  if ( entrance_kick ) {

     Component_t k     = - ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

     state[3] += k*state[0];  
     state[4] += k*state[1];
  }

  //--------------------------------------------------------------
  // Free space propagation through "effective first half length"
  // of the cavity. Note that this an approximation for cdt, since
  // (1) the trajectory is not straight and (2) the velocity varies
  // over the length of the cavity.
  //--------------------------------------------------------------

  Component_t x_in  = state[0];
  Component_t y_in  = state[1];

  elm.bmlnElmnt::localPropagate( p );

  Component_t w = onaxisEnergyGain/ p.Energy();
 
  if( norm(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                   { w = 1.0;           }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];

  // Cavity increases energy and momentum of the particle

  Component_t E  = p.Energy() + onaxisEnergyGain;

  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain );
  double newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  state[5] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;
  
  //---------------------------
  // Thin lens kick upon exit
  //---------------------------

  if ( exit_kick ) {
 
      Component_t k   = ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

      state[3]  += k*state[0];
      state[4]  += k*state[1];
  }

}

} // anonymous namespace

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::localPropagate( ParticleBunch& b )
{
  for ( ParticleBunch::iterator it = b.begin(); it != b.end(); ++it) { 
    propagate(*it);
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityUpstream::localPropagate( Particle& p)
{
  ::propagate<Particle>(*this, p, true, false);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LCavityUpstream::localPropagate( JetParticle& p)
{
   ::propagate<JetParticle>(*this, p, true, false);  
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::localPropagate( ParticleBunch& b )
{
  for ( ParticleBunch::iterator it = b.begin(); it != b.end(); ++it) { 
    propagate(*it);
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::localPropagate( Particle& p)
{
  ::propagate<Particle>(*this, p, false, true);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LCavityDnstream::localPropagate( JetParticle& p)
{
  ::propagate<JetParticle>(*this, p, false, true);  
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


