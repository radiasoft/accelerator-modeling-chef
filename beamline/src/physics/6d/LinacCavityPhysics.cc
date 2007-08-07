/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LinacCavityPhysics.cc
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
******  Authors:   Leo Michelotti         michelotti@fnal.gov 
******             Jean-Francois Ostiguy  ostiguy@fnal.gov                                              
******                                                                
********************************************************* **************
************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <boost/function.hpp>

#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/JetParticle.h>
#include <beamline/LinacCavity.h>
#include <beamline/OpticalStateAdaptor.h>

#include <basic_toolkit/TMatrix.h>

using FNAL::pcerr;
using FNAL::pcout;


class RSMatrixPropagator: public elm_core_access { 

public:

  void operator()( bmlnElmnt& elm,  Particle&    p ) {  propagate<Particle>( elm,p); }
  void operator()( bmlnElmnt& elm,  JetParticle& p ) {  propagate<JetParticle>( elm,p); }

  template<typename Particle_t>
  void propagate( bmlnElmnt& elm_arg, Particle_t& p ); 
};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DefaultPropagator: public elm_core_access {

public:

  void operator()( bmlnElmnt& elm,  Particle&       p )   {   //propagate_part_1<Particle>( elm, p);    
                                                              //propagate_part_2<Particle>( elm, p);
                                                              propagate<Particle>( elm, p);

                                                          }

  void operator()( bmlnElmnt& elm,  JetParticle&    p )   {   //propagate_part_1<JetParticle>( elm, p);  
                                                              //propagate_part_2<JetParticle>( elm, p);
                                                              propagate<JetParticle>( elm, p);
                                                          }

  void operator()( bmlnElmnt& elm,  ParticleBunch&  b )   {  for (ParticleBunch::iterator it = b.begin(); it != b.end(); ++it) {
                                                               propagate_part_1<Particle>( elm,*it); 
                                                             }  

  // THIS IS BROKEN !
                                                             for (ParticleBunch::iterator it = b.begin(); it != b.end(); ++it) {
                                                               propagate_part_2<Particle>( elm,*it); 
                                                             }   
                                                          }

private:


  template< typename Particle_t>
  void propagate( bmlnElmnt& elm_arg, Particle_t& p ); 

  template< typename Particle_t>
  void propagate_part_1( bmlnElmnt& elm_arg, Particle_t& p ); 

  template< typename Particle_t>
  void propagate_part_2( bmlnElmnt& elm_arg, Particle_t& p ); 

};



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::localPropagate( Particle& p ) 
{
  DefaultPropagator().operator()( *this, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::localPropagate( JetParticle& p ) 
{
  DefaultPropagator().operator()( *this, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::localPropagate( ParticleBunch& b ) 
{
  DefaultPropagator().operator()( *this, b);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template< typename Particle_t>
void DefaultPropagator::propagate( bmlnElmnt& elm_arg, Particle_t& p ) 
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
  
  boost::function<double(Component_t const& comp)> norm = &PropagatorTraits<Particle_t>::norm;

  LinacCavity& elm = static_cast<LinacCavity&>(elm_arg);

  const double strength   = elm.Strength();
  const double length     = elm.Length();
  const double ctRef      = elm.getReferenceTime();
  const double phi_s      = elm.getPhi();
  const double w_rf       = elm.getRadialFrequency();

  double const ctr       = ctRef;

  double const oldLength = length;
  double const m         = p.Mass();
 
  State_t& state = p.State();

  if( strength == 0.0) { 
    elm.setReferenceTime(0.0);
    elm.bmlnElmnt::localPropagate( p ); 
    state[2] -= ctRef; 
    elm.setReferenceTime(ctRef);
    return;
  }

  double         const referenceEnergyGain = strength*cos ( phi_s);
  Component_t    const onaxisEnergyGain    = strength*cos ( phi_s  + state[2] * w_rf / PH_MKS_c );

  Component_t    const eE_z                = onaxisEnergyGain / oldLength;

  // Thin lens kick upon entry

  Component_t k     = - ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

  state[3] += k*state[0];  
  state[4] += k*state[1];


  // Free space propagation through "effective half length"
  // of the cavity. 
 
  Component_t x_in  = state[0];
  Component_t y_in  = state[1];

  elm.setReferenceTime(0.0);
  elm.setLength( oldLength/2.0 );
  elm.bmlnElmnt::localPropagate( p );


  Component_t w = (onaxisEnergyGain/2.0) / p.Energy();
  if( norm(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                   { w = 1.0;           }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];
 

  // Cavity increases energy and momentum of the particle
  Component_t E  = p.Energy() + onaxisEnergyGain/2.0;


  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  double newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  state[5] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;
  
  //--------------------------------------
  // PUT TRANSVERSE WAKE FIELD KICK HERE...
  //--------------------------------------

  // Free space propagation through "effective half length"
  // of the cavity.
  // NOTE: THIS AFFECTS THE TIME COORDINATE !!!!

  x_in  = state[0];
  y_in  = state[1];

  elm.setLength( oldLength/2.0 );
  elm.setReferenceTime(0.0);
  elm.bmlnElmnt::localPropagate( p );

  w = (onaxisEnergyGain/2.0) / p.Energy();
  if( norm(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                   { w = 1.0;           }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];


  // Cavity increases energy and momentum of the particle
  E = p.Energy() + onaxisEnergyGain/2.0;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2.0 );
  newRefP = p.ReferenceMomentum();


  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  state[5] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;


  // Thin lens kick upon exit

  k         = ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );
  state[3] += k*state[0];
  state[4] += k*state[1];

  // Restore length

  elm.setLength(oldLength);
  elm.setReferenceTime(ctRef);

  state[2] -= ctRef;

 }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template< typename Particle_t>
void DefaultPropagator::propagate_part_1( bmlnElmnt& elm_arg, Particle_t& p ) 
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
  
  boost::function<double(Component_t const& comp)> norm = &PropagatorTraits<Particle_t>::norm;

  LinacCavity& elm = static_cast<LinacCavity&>(elm_arg);

  const double strength   = elm.Strength();
  const double length     = elm.Length();
  const double ctRef      = elm.getReferenceTime();
  const double phi_s      = elm.getPhi();
  const double w_rf       = elm.getRadialFrequency();
  const double m          = p.Mass();
 
  State_t& state = p.State();


  if( strength == 0.0) { 
    elm.setReferenceTime(0.0);
    elm.setLength(length/2.0);
    elm.bmlnElmnt::localPropagate( p ); 
    state[2] -= elm.ctRef_part_1_;
    elm.setReferenceTime(ctRef);
    elm.setLength(length);
    return;
  }

  double         const referenceEnergyGain = strength*cos ( phi_s);
  Component_t    const onaxisEnergyGain    = strength*cos ( phi_s  + state[2] * w_rf / PH_MKS_c );
  Component_t    const eE_z                = onaxisEnergyGain / length;

  //---------------------------
  // Thin lens kick upon entry
  //---------------------------

  Component_t k     = - ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

  state[3] += k*state[0];  
  state[4] += k*state[1];

  //--------------------------------------------------------------
  // Free space propagation through "effective first half length"
  // of the cavity. 
  //--------------------------------------------------------------

  Component_t x_in  = state[0];
  Component_t y_in  = state[1];

  elm.setReferenceTime(0.0);  
  elm.setLength( length/2.0 );
  elm.bmlnElmnt::localPropagate( p );
  elm.setLength(length);
  elm.setReferenceTime(ctRef);  

  state[2] -= elm.ctRef_part_1_;

  std::cout << "part1 state[2] = " << state[2] << std::endl; 

  Component_t w = (onaxisEnergyGain/2.0) / p.Energy();

  if( norm(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                   { w = 1.0;           }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];
 
  // Cavity increases energy and momentum of the particle

  Component_t E  = p.Energy() + onaxisEnergyGain/2.0;

  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  double newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  state[5] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;
  

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template< typename Particle_t>
void DefaultPropagator::propagate_part_2( bmlnElmnt& elm_arg, Particle_t& p ) 
{  
  

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
  
  boost::function<double(Component_t const& comp)> norm = &PropagatorTraits<Particle_t>::norm;

  LinacCavity& elm = static_cast<LinacCavity&>(elm_arg);

  const double strength   = elm.Strength();
  const double length     = elm.Length();
  const double ctRef      = elm.getReferenceTime();
  const double phi_s      = elm.getPhi();
  const double w_rf       = elm.getRadialFrequency();
  const double m          = p.Mass();

  State_t& state = p.State();

  if( strength == 0.0) { 
    elm.setReferenceTime(0.0);
    elm.setLength(length/2.0);
    elm.bmlnElmnt::localPropagate( p ); 
    state[2] -= elm.ctRef_part_2_;
    elm.setReferenceTime(ctRef);
    elm.setLength(length);
    return;
  }


  double         const referenceEnergyGain = strength*cos ( phi_s);
  Component_t    const onaxisEnergyGain    = strength*cos ( phi_s  + state[2] * w_rf / PH_MKS_c );
  Component_t    const eE_z                = onaxisEnergyGain / length;


  //--------------------------------------------------------------
  // Free space propagation through "effective second half length"
  // of the cavity. 
  //--------------------------------------------------------------

  Component_t x_in  = state[0];
  Component_t y_in  = state[1];

  elm.setLength( length/2.0 );
  elm.setReferenceTime(0.0);
  elm.bmlnElmnt::localPropagate( p );
  elm.setLength(length);
  elm.setReferenceTime(ctRef);

  state[2] -= elm.ctRef_part_2_;

  std::cout << "part2 state[2] = " << state[2] << std::endl; 

  Component_t w = (onaxisEnergyGain/2.0) / p.Energy();
  if( norm(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                   { w = 1.0;           }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];


  // Cavity increases energy and momentum of the particle

  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2.0 );
  double newRefP = p.ReferenceMomentum();

  Component_t E = p.Energy() + onaxisEnergyGain/2.0;

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  state[5] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;

  //-------------------------
  // Thin lens kick upon exit
  //--------------------------

  Component_t k   = ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

  state[3]  += k*state[0];
  state[4]  += k*state[1];


 }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
void RSMatrixPropagator::propagate( bmlnElmnt& elm_arg, Particle_t & p ) 
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  boost::function<double(Component_t const& )> norm = &PropagatorTraits<Particle_t>::norm;

  LinacCavity& elm = static_cast<LinacCavity&>(elm_arg);

  double const  strength   = elm.Strength();
  double const  length     = elm.Length();
  double const  ctRef      = elm.getReferenceTime();
  double const  phi_s      = elm.getPhi();
  double const  w_rf       = elm.getRadialFrequency();
  double const m           = p.Mass();

  if( 0 == strength ) { 
    elm.setReferenceTime(length/ p.ReferenceBeta());
    elm.bmlnElmnt::localPropagate( p ); 
    elm.setReferenceTime(ctRef);
    return;
  }

  OpticalStateAdaptor<Particle_t> optical_state(p);

  double const referenceEnergyGain = strength * cos( phi_s );
  Component_t    const onaxisEnergyGain    = strength * cos( phi_s + optical_state[2] * w_rf / PH_MKS_c );  

  const double eta = 1.0;

  Component_t E_i     =   p.Energy();
  Component_t p_i     =   p.Momentum();
  Component_t E_f     =   ( E_i + onaxisEnergyGain );
  Component_t E_p     =   onaxisEnergyGain/length;
  Component_t cdt_i   =   p.get_cdt();

  State_t tstate(2);  

  Component_t phi = phi_s  + ( optical_state[2]* w_rf / PH_MKS_c ); 

  /// ???????????????????? if ( phi.standardPart() < 0.0)  phi = -phi; 

  Component_t sn_phi = sin(   phi );
  Component_t cn_phi = cos(   phi );


  Component_t alpha = ( norm(phi) > 1.0e-8 ) ? sqrt( (eta*phi) / (8.0*cn_phi ) ) * log ( E_f/E_i): 0;

  Component_t sn_alpha = sin( alpha );
  Component_t cn_alpha = cos( alpha );

  Component_t term1 =    ( norm(phi) > 1.0e-8 )  ? sqrt(2.0/(eta*phi)) * sn_alpha : ( 0.5 / cn_phi) * log( E_f/E_i ); 
  Component_t term2 =    ( norm(phi) > 1.0e-8 )  ? sqrt(eta*phi/8.0) * sn_alpha / cn_phi : eta*phi/4.0*log( E_f/E_i );

  Component_t rmat00 =    cn_alpha -  cn_phi * term1;
  Component_t rmat01 =    2.0* term1 *( E_i/E_p ) * cn_phi;     
  Component_t rmat10 = -  (E_p/E_i) * ( 0.5*cn_phi*term1 + term2 );
  
  Component_t rmat11 = E_i/E_f * (cn_alpha + cn_phi *term1 );

  tstate[0] = optical_state[0];
  tstate[1] = optical_state[3];

  Component_t newstate0 = rmat00*tstate[0] + rmat01*tstate[1];
  Component_t newstate1 = rmat10*tstate[0] + rmat11*tstate[1];

  optical_state[0] = tstate[0];
  optical_state[3] = tstate[1];

  tstate[0] = optical_state[1];
  tstate[1] = optical_state[4];

  newstate0 = rmat00*tstate[0] + rmat01*tstate[1];
  newstate1 = rmat10*tstate[0] + rmat11*tstate[1];

  optical_state[1] = newstate0;
  optical_state[4] = newstate1;


  // Change the dp/p state coordinate  

  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain );
  optical_state[5]  = ( sqrt((E_f - m)*(E_f + m))/ p.ReferenceMomentum() ) - 1.0 ;

}


