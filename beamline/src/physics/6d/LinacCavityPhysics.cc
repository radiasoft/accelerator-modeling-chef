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
************************************************************************
************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/LinacCavity.h>
#include <beamline/OpticalStateAdaptor.h>

#include <basic_toolkit/TMatrix.h>

using FNAL::pcerr;
using FNAL::pcout;


class LocalPropagateRSMatrix: public elm_core_access { 

public:

  void operator()( bmlnElmnt& elm,  Particle&    p );
  void operator()( bmlnElmnt& elm,  JetParticle& p );

};


class LocalPropagateCHEF: public elm_core_access {

public:

  void operator()( bmlnElmnt& elm,  Particle&     p ); 
  void operator()( bmlnElmnt& elm,  JetParticle&  p ); 

};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::localPropagate( Particle& p ) 
{

  LocalPropagateRSMatrix().operator()( *this, p);

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::localPropagate( JetParticle& p ) 
{
  LocalPropagateRSMatrix().operator()( *this, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void LocalPropagateCHEF::operator()( bmlnElmnt& elm_arg, Particle& p ) 
{
 
  LinacCavity& elm = static_cast<LinacCavity&>(elm_arg);

  
  double const strength   = elm.Strength();
  double const length     = elm.Length();
  double const ctRef      = elm.getReferenceTime();
  double const phi_s      = elm.getPhi();
  double const w_rf       = elm.getRadialFrequency();

  if( strength == 0.0 ) { 
    elm.setReferenceTime( length/ p.ReferenceBeta() );
    elm.bmlnElmnt::localPropagate( p ); 
    elm.setReferenceTime(ctRef);
    return;
  }

  double const oldLength = length;
  double const m         = p.Mass();
  double const p_i       = p.get_npz() * p.ReferenceMomentum();
  double const E_i       = p.Energy();
  double const cdt_i     = p.get_cdt();;

  Vector& state   = p.getState();

  double const referenceEnergyGain = strength * cos ( phi_s );
  double const onaxisEnergyGain    = strength * cos ( phi_s + state[2] * w_rf / PH_MKS_c );  
  double const eE_z                = onaxisEnergyGain / oldLength;

  // Thin lens kick upon entry
  double k  = - ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

  state[3] += k*state[0];  
  state[4] += k*state[1];

  // Free space propagation through "effective half length"
  // of the cavity.

  double x_in  = state[0];
  double y_in  = state[1];

  //--------------------------------- 
  elm.setReferenceTime(0.0);
  elm.setLength( oldLength/2.0 );
  elm.bmlnElmnt::localPropagate( p );
  //---------------------------------

   double w = (onaxisEnergyGain/2.0) / p.Energy();
  if( std::abs(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                       { w = 1.0;           }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];

  // Cavity increases energy and momentum of the particle

  double E  = p.Energy() + onaxisEnergyGain/2.0;

  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  double newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );
  state[5]  = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;

  //--------------------------------------
  // PUT TRANSVERSE WAKE FIELD KICK HERE...
  //--------------------------------------


  x_in  = state[0];
  y_in  = state[1];

  //--------------------------------
  elm.setReferenceTime(0.0);
  elm.setLength( oldLength/2.0 );
  elm.bmlnElmnt::localPropagate( p );
  //-----------------------------------


  w = (onaxisEnergyGain/2.0) / p.Energy();
  if( std::abs(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                       { w = 1.0;           }

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

  // Restore parameters

  elm.setLength(oldLength);
  elm.setReferenceTime(ctRef);

  state[2] -= ctRef;
  

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LocalPropagateCHEF::operator()( bmlnElmnt& elm_arg, JetParticle& p ) 
{

  LinacCavity& elm = static_cast<LinacCavity&>(elm_arg);

  const double strength   = elm.Strength();
  const double length     = elm.Length();
  const double ctRef      = elm.getReferenceTime();
  const double phi_s      = elm.getPhi();
  const double w_rf       = elm.getRadialFrequency();

  double const ctr       = ctRef;

  if( strength == 0.0) { 
    elm.setReferenceTime( length/p.ReferenceBeta() );
    elm.bmlnElmnt::localPropagate( p ); 
    elm.setReferenceTime(ctRef);
    return;
  }

  double const oldLength = length;
  double const m         = p.Mass();
  Jet    const p_i       = p.get_npz() * p.ReferenceMomentum();
  Jet    const E_i       = p.Energy();
  Jet    const cdt_i     = p.get_cdt();
 
  Mapping& state = p.getState();

  double const referenceEnergyGain = strength*cos ( phi_s);
  Jet    const onaxisEnergyGain    = strength*cos ( phi_s  + state[2] * w_rf / PH_MKS_c );
  Jet    const eE_z                = onaxisEnergyGain / oldLength;

  // Thin lens kick upon entry

  Jet k     = - ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

  state[3] += k*state[0];  
  state[4] += k*state[1];


  // Free space propagation through "effective half length"
  // of the cavity. 
 
  Jet x_in  = state[0];
  Jet y_in  = state[1];

  elm.setReferenceTime(0.0);
  elm.setLength( oldLength/2.0 );
  elm.bmlnElmnt::localPropagate( p );

  Jet w = (onaxisEnergyGain/2.0) / p.Energy();
  if( std::abs(w.standardPart()) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                                      { w = 1.0;           }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];
 

  // Cavity increases energy and momentum of the particle
  Jet E  = p.Energy() + onaxisEnergyGain/2.0;

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
  if( std::abs(w.standardPart()) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                                      { w = 1.0;           }

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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LocalPropagateRSMatrix::operator()( bmlnElmnt& elm_arg, Particle& p ) 
{

  LinacCavity& elm = static_cast<LinacCavity&>(elm_arg);

  double const  strength   = elm.Strength();
  double const  length     = elm.Length();
  double const  ctRef      = elm.getReferenceTime();
  double const  phi_s      = elm.getPhi();
  double const  w_rf       = elm.getRadialFrequency();

  if( 0 == strength ) { 
    elm.setReferenceTime( length/ p.ReferenceBeta() );
    elm.bmlnElmnt::localPropagate( p ); 
    elm.setReferenceTime(ctRef);
    return;
  }

   
  double const oldLength = length;
  double const m         = p.Mass();
  double const E_i       = p.Energy();
  double const p_i       = p.Momentum();
  double const cdt_i     = p.get_cdt();

  OpticalStateAdaptor optical_state(p);

  double const referenceEnergyGain = strength * cos( phi_s );
  double const onaxisEnergyGain    = strength * cos( phi_s  + optical_state[2] * w_rf / PH_MKS_c );  // ??? Improve!
  double const eta = 1.0;

  double E_f = ( E_i + onaxisEnergyGain );
  double E_p = onaxisEnergyGain /length;

  Vector  tstate(2);  
  Matrix rmat(2,2);
  
  double phi = phi_s  + ( optical_state[2]* w_rf / PH_MKS_c ); 
  if ( phi < 0.0) phi = -phi; 

  double sn_phi = sin( phi  + optical_state[2] * w_rf / PH_MKS_c  );
  double cn_phi = cos( phi  + optical_state[2] * w_rf / PH_MKS_c  );

  double alpha = sqrt( (eta*phi) / (8.0*cn_phi ) ) * log ( E_f/E_i);

  double sn_alpha = sin( alpha );
  double cn_alpha = cos( alpha );


  double term1 =    ( std::abs(phi) > 1.0e-8 )  ? sqrt(2.0/(eta*phi)) * sn_alpha : ( 0.5 / cn_phi) * log( E_f/E_i ); 
  double term2 =    ( std::abs(phi) > 1.0e-8 )  ? sqrt(eta*phi/8.0)   * sn_alpha / cn_phi : eta*phi/4.0*log( E_f/E_i );

  rmat(0,0) =    cn_alpha -  cn_phi * term1;
  rmat(0,1) =    2.0* term1 *( E_i/E_p ) * cn_phi;     
  rmat(1,0) = -  (E_p/E_i) * ( 0.5*cn_phi*term1 + term2 );
  
  rmat(1,1) = E_i/E_f * (cn_alpha + cn_phi *term1 );


  tstate[0] = optical_state[0];
  tstate[1] = optical_state[3];

  tstate = rmat*tstate;

  optical_state[0] = tstate[0];
  optical_state[3] = tstate[1];

  tstate[0] = optical_state[1];
  tstate[1] = optical_state[4];

  tstate = rmat*tstate;
  
  optical_state[1] = tstate[0];
  optical_state[4] = tstate[1];


  // Change the dp/p state coordinate  

  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain );
  optical_state[5]  = ( sqrt((E_f - m)*(E_f + m))/ p.ReferenceMomentum() ) - 1.0 ;


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LocalPropagateRSMatrix::operator()( bmlnElmnt& elm_arg, JetParticle& p ) 
{

  LinacCavity& elm = static_cast<LinacCavity&>(elm_arg);

  double const& strength   = elm.Strength();
  double const& length     = elm.Length();
  double const& ctRef      = elm.getReferenceTime();
  double const& phi_s      = elm.getPhi();
  double const& w_rf       = elm.getRadialFrequency();

  double const ctr       = ctRef;
  if( 0 == strength ) { 
    elm.setReferenceTime(length/ p.ReferenceBeta());
    elm.bmlnElmnt::localPropagate( p ); 
    elm.setReferenceTime(ctr);
    return;
  }

  double const oldLength = length;
  double const m         = p.Mass();

  JetOpticalStateAdaptor optical_state(p);


  double const referenceEnergyGain = strength * cos( phi_s );
  Jet    const onaxisEnergyGain    = strength * cos( phi_s + optical_state[2] * w_rf / PH_MKS_c );  

  const double eta = 1.0;

  Jet E_i     =   p.Energy();
  Jet p_i     =   p.Momentum();
  Jet E_f     =   ( E_i + onaxisEnergyGain );
  Jet E_p     =   onaxisEnergyGain/length;
  Jet cdt_i   =   p.get_cdt();

  Mapping tstate(2);  

  Jet phi = phi_s  + ( optical_state[2]* w_rf / PH_MKS_c ); 

  if ( phi.standardPart() < 0.0)  phi = -phi; 

  Jet sn_phi = sin(   phi );
  Jet cn_phi = cos(   phi );


  Jet alpha = ( std::abs(phi.standardPart()) > 1.0e-8 ) ? sqrt( (eta*phi) / (8.0*cn_phi ) ) * log ( E_f/E_i): 0;

  Jet sn_alpha = sin( alpha );
  Jet cn_alpha = cos( alpha );

  Jet term1 =    ( std::abs(phi.standardPart()) > 1.0e-8 )  ? sqrt(2.0/(eta*phi)) * sn_alpha : ( 0.5 / cn_phi) * log( E_f/E_i ); 
  Jet term2 =    ( std::abs(phi.standardPart()) > 1.0e-8 )  ? sqrt(eta*phi/8.0) * sn_alpha / cn_phi : eta*phi/4.0*log( E_f/E_i );

  Jet rmat00 =    cn_alpha -  cn_phi * term1;
  Jet rmat01 =    2.0* term1 *( E_i/E_p ) * cn_phi;     
  Jet rmat10 = -  (E_p/E_i) * ( 0.5*cn_phi*term1 + term2 );
  
  Jet rmat11 = E_i/E_f * (cn_alpha + cn_phi *term1 );

  tstate[0] = optical_state[0];
  tstate[1] = optical_state[3];

  Jet newstate0 = rmat00*tstate[0] + rmat01*tstate[1];
  Jet newstate1 = rmat10*tstate[0] + rmat11*tstate[1];

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


