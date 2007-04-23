/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rfcavityPhysics.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
******
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  REVISION HISTORY
******
******  December 2006 ostiguy@fnal.gov 
******  -Efficiency improvements. Particle state pased by reference.
******  
******  January 4, 2007   (Tuesday)
******  Stored in cvs repository
******  after major reorganization of files.
******  - lpjm
******
******  January 13, 2007  (Friday)
******  Upgrades: preparation for use in ILC studies.
******  - lpjm
******
******  February 2, 2007  (Friday)
******  Fixed edge focusing following recommendation of Paul Lebrun
******  to use actual electric field upon entry.
******  In addition, included change to "effective length" calculation.
******  !! WARNING: THESE CHANGES ARE TENTATIVE AND BEING REVIEWED !!
******  - lpjm
******  
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <beamline/rfcavity.h>

#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

using FNAL::pcerr;
using FNAL::pcout;


void rfcavity::localPropagate( Particle& p ) 
{

  if( strength_ == 0.0 ) { // zero strength + zero loss cavity is a drift 
     ctRef_   = length_;
     bmlnElmnt::localPropagate( p ); 
     return;
  }

  double E( p.ReferenceEnergy() );

  double oldRefP, newRefP;

  Vector& state = p.getState();

  double referenceEnergyGain        = strength_*sin_phi_s_;
  double const oldLength            = length_;
  double onaxisEnergyGain           = strength_*sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c );

  // Assign focal lengths for effective kicks that
  // model first order edge focussing.

  double k1 = - 0.5*(onaxisEnergyGain/length_)/ E;
  double k2 =   0.5*(onaxisEnergyGain/length_)/(E + referenceEnergyGain);

  // Thin lens kick upon entry

  state[3] += k1*state[0];
  state[4] += k1*state[1];

  // Free space propagation through "effective half length"
  // of the cavity.

  length_  = (oldLength)/2.0;

  double w = (referenceEnergyGain/2.0) / p.ReferenceEnergy();
  if( fabs(w) > 1.0e-8 ) {
    length_ *= (log(1.+w)/w);
  }
  
  ctRef_   = length_;
  bmlnElmnt::localPropagate( p ); // propagate through a drift of length = effective half-length

  // Cavity increases energy and momentum of the particle (changes angles ! )

  E = p.Energy() + 0.5*onaxisEnergyGain;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );


  double   m  = p.Mass();
  double   pr = p.ReferenceMomentum();

  state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0;

  //----------------------------------------------------------------------------------------------------------  
  // ************************** Put transverse wake field kick here ... ********************************
  //----------------------------------------------------------------------------------------------------------  

  // Free space propagation through "effective half length"
  // of the cavity.

  length_  = (oldLength)/2.0;

   w = (referenceEnergyGain/2.0) / p.ReferenceEnergy();
   if( fabs(w) > 1.0e-8 ) {
     length_ *= (log(1.+w)/w);
  }

  ctRef_   = length_;
  bmlnElmnt::localPropagate( p );

  // Cavity increases energy and momentum of the particle
  
  E = p.Energy() + 0.5*onaxisEnergyGain;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  m  = p.Mass();
  pr = p.ReferenceMomentum();

  state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0;


  // Thin lens kick upon exit

  state[3] += k2*state[0];
  state[4] += k2*state[1];

  // Restore length before returning.

  length_ = oldLength;
  ctRef_  = length_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::localPropagate( JetParticle& p ) 
{

  if( strength_ == 0.0 ) { // zero strength + zero loss cavity is a drift 
     ctRef_   = length_;
     bmlnElmnt::localPropagate( p ); 
     return;
  }

  Jet E( p.ReferenceEnergy() );

  double oldRefP, newRefP;

  Mapping& state = p.getState();

  double referenceEnergyGain        = strength_*sin_phi_s_;
  double const oldLength            = length_;
  Jet onaxisEnergyGain              = strength_*sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c );

  // Assign focal lengths for effective kicks that
  // model first order edge focussing.

  Jet k1 = - 0.5*(onaxisEnergyGain/length_)/ E;
  Jet k2 =   0.5*(onaxisEnergyGain/length_)/(E + referenceEnergyGain);

  // Thin lens kick upon entry

  state[3] += k1*state[0];
  state[4] += k1*state[1];

  // Free space propagation through "effective half length"
  // of the cavity.

  length_  = (oldLength)/2.0;

  double w = (referenceEnergyGain/2.0) / p.ReferenceEnergy();
  if( fabs(w) > 1.0e-8 ) {
    length_ *= (log(1.+w)/w);
  }

  ctRef_   = length_;
  bmlnElmnt::localPropagate( p ); // propagate through a drift of length = effective half-length

  // Cavity increases energy and momentum of the particle

  E = p.Energy() + 0.5*onaxisEnergyGain;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  double   m  = p.Mass();
  double   pr = p.ReferenceMomentum();

  state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0;
  

  // Put transverse wake field kick here ...
  
  // Free space propagation through "effective half length"
  // of the cavity.


  length_ = (oldLength)/2.0;

   w = (referenceEnergyGain/2.0) / p.ReferenceEnergy();
   if( fabs(w) > 1.0e-8 ) {
     length_ *= (log(1.+w)/w);
  }

  ctRef_   = length_;
  bmlnElmnt::localPropagate( p );

  // Cavity increases energy and momentum of the particle

  
  E = p.Energy() + 0.5*onaxisEnergyGain;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  m  = p.Mass();
  pr = p.ReferenceMomentum();

  state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0;


  // Thin lens kick upon exit

  state[3] += k2*state[0];
  state[4] += k2*state[1];

  // Restore length before returning.

  length_ = oldLength;
  ctRef_  = length_;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::localPropagate( Particle& p ) 
{
  double denom, px, py, cs; 
  double oldRefP, newRefP;
  
  Vector& state = p.getState(); 
 
  if( 0.0 != strength_ ) {
    px = p.get_npx();
    py = p.get_npy();
    denom = 1.0 + p.get_ndp();
    cs = sqrt( 1.0 - ( ( px*px + py*py ) / ( denom*denom ) ) );
    double E = p.Energy() + ((strength_*cs)*(sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c )));

    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.ReferenceEnergy() + strength_*sin_phi_s_ );
    newRefP = p.ReferenceMomentum();

    state[3] *= ( oldRefP / newRefP );
    state[4] *= ( oldRefP / newRefP );

    double   m  = p.Mass();
    double   pr = p.ReferenceMomentum();

    state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinrfcavity::localPropagate( JetParticle& p ) 
{
  Jet__environment_ptr myEnv( p.State().Env() );
  Jet denom(myEnv), px(myEnv), py(myEnv), cs(myEnv);
  double oldRefP, newRefP;

  Mapping& state = p.getState(); 

  if( 0.0 != strength_ ) {
    px = p.get_npx();
    py = p.get_npy();
    denom = 1.0 + p.get_ndp();
    cs = sqrt( 1.0 - ( ( px*px + py*py ) / ( denom*denom ) ) );
    Jet E = p.Energy() + ((strength_*cs)*(sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c )));

    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.ReferenceEnergy() + strength_*sin_phi_s_ );
    newRefP = p.ReferenceMomentum();
    state[3] = ( oldRefP / newRefP )*state[3];
    state[4] = ( oldRefP / newRefP )*state[4];

    double   m  = p.Mass();
    double   pr = p.ReferenceMomentum();

    state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0 ;
  }
}


#if 0 
=============================================================================================================================
The code that follows is a simplified rfcavity propagator meant for debugging. 
=============================================================================================================================

void rfcavity::localPropagate( Particle& p ) 
{

  if( strength_ == 0.0 ) return;

  double E( p.ReferenceEnergy() );

  double oldRefP, newRefP;

  Vector& state = p.getState();

  double referenceEnergyGain        = strength_*sin_phi_s_;
  double const oldLength            = length_;
  double onaxisEnergyGain           = strength_*sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c );

  // Assign focal lengths for effective kicks that
  // model first order edge focussing.

  double k1 = - 0.5*(onaxisEnergyGain/length_)/ E;
  double k2 =   0.5*(onaxisEnergyGain/length_)/(E + referenceEnergyGain);

  // Thin lens kick upon entry

  state[3] += k1*state[0];
  state[4] += k1*state[1];

  // Free space propagation through "effective half length"
  // of the cavity.

  length_  = (oldLength)/2.0;

  double w = (referenceEnergyGain/2.0) / p.ReferenceEnergy();
  if( fabs(w) > 1.0e-8 ) {
    length_ *= (log(1.+w)/w);
  }

  bmlnElmnt::localPropagate( p ); // propagate through a drift of length = effective half-length

  // Cavity increases energy and momentum of the particle

  E = p.Energy() + 0.5*onaxisEnergyGain;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  double   m  = p.Mass();
  double   pr = p.ReferenceMomentum();

  state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0;
  

  // Put transverse wake field kick here ...
  
  // Free space propagation through "effective half length"
  // of the cavity.


  length_ = (oldLength)/2.0;

   w = (referenceEnergyGain/2.0) / p.ReferenceEnergy();
   if( fabs(w) > 1.0e-8 ) {
     length_ *= (log(1.+w)/w);
  }

  bmlnElmnt::localPropagate( p );

  // Cavity increases energy and momentum of the particle

  
  E = p.Energy() + 0.5*onaxisEnergyGain;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  m  = p.Mass();
  pr = p.ReferenceMomentum();

  state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0;


  // Thin lens kick upon exit

  state[3] += k2*state[0];
  state[4] += k2*state[1];

  // Restore length before returning.

  length_ = oldLength;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::localPropagate( JetParticle& p ) 
{

  if( strength_ == 0.0 ) return;

  Jet E( p.ReferenceEnergy() );

  double oldRefP, newRefP;

  Mapping& state = p.getState();

  double referenceEnergyGain        = strength_*sin_phi_s_;
  double const oldLength            = length_;
  Jet onaxisEnergyGain              = strength_*sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c );

  // Assign focal lengths for effective kicks that
  // model first order edge focussing.

  Jet k1 = - 0.5*(onaxisEnergyGain/length_)/ E;
  Jet k2 =   0.5*(onaxisEnergyGain/length_)/(E + referenceEnergyGain);

  // Thin lens kick upon entry

  state[3] += k1*state[0];
  state[4] += k1*state[1];

  // Free space propagation through "effective half length"
  // of the cavity.

  length_  = (oldLength)/2.0;

  double w = (referenceEnergyGain/2.0) / p.ReferenceEnergy();
  if( fabs(w) > 1.0e-8 ) {
    length_ *= (log(1.+w)/w);
  }

  bmlnElmnt::localPropagate( p ); // propagate through a drift of length = effective half-length

  // Cavity increases energy and momentum of the particle

  E = p.Energy() + 0.5*onaxisEnergyGain;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  double   m  = p.Mass();
  double   pr = p.ReferenceMomentum();

  state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0;
  

  // Put transverse wake field kick here ...
  
  // Free space propagation through "effective half length"
  // of the cavity.


  length_ = (oldLength)/2.0;

   w = (referenceEnergyGain/2.0) / p.ReferenceEnergy();
   if( fabs(w) > 1.0e-8 ) {
     length_ *= (log(1.+w)/w);
  }

  bmlnElmnt::localPropagate( p );

  // Cavity increases energy and momentum of the particle

  
  E = p.Energy() + 0.5*onaxisEnergyGain;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  m  = p.Mass();
  pr = p.ReferenceMomentum();

  state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0;


  // Thin lens kick upon exit

  state[3] += k2*state[0];
  state[4] += k2*state[1];

  // Restore length before returning.

  length_ = oldLength;

}

#endif
