/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      RSMatrixPropagator.cc
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
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov      
******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/

//------------------------------------------------------------------------ 
//
//  Propagator based on the Rosensweig - Serafini Matrix  
//  Use as alternate propagator for LinacCavity
//
//-------------------------------------------------------------------------

#include <boost/bind.hpp>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/RSMatrixPropagator.h>
#include <beamline/OpticalStateAdaptor.h>
#include <beamline/LinacCavityParts.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/JetParticle.h>


using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

namespace { // anonymous namespace

enum Position_t { upstream, downstream }; 

template <typename Particle_t, Position_t position >
void propagate( bmlnElmnt& elm_arg, Particle_t & p ) 
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  boost::function<double(Component_t const& )> norm = &PropagatorTraits<Particle_t>::norm;

  LCavityUpstream& elm = static_cast<LCavityUpstream&>(elm_arg);

  double const  strength   = elm.Strength();
  double const  length     = elm.Length();
  double const  ctRef      = elm.getReferenceTime();
  double const  phi_s      = elm.getPhi();
  double const  w_rf       = elm.getRadialFrequency();
  double const m           = p.Mass();

  if( 0 == strength ) { 
    elm.bmlnElmnt::localPropagate( p ); 
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

  Component_t term1 =    ( norm(phi) > 1.0e-8 )  ? sqrt(2.0/(eta*phi)) * sn_alpha          : ( 0.5 / cn_phi) * log( E_f/E_i ); 
  Component_t term2 =    ( norm(phi) > 1.0e-8 )  ? sqrt(eta*phi/8.0)   * sn_alpha / cn_phi : eta*phi/4.0*log( E_f/E_i );

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

} // anonymous namespace
