/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      rfcavityPhysics.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include "rfcavity.h"
#include "PhysicsConstants.h"

#ifndef MADPHYSICS

void rfcavity::localPropagate( Particle& p ) 
{
  static double E;
  static double oldRefP, newRefP;
  static double oldLength;
  static double referenceEnergyGain;
  static double w;
  static double k1, k2;

  // Scale length to "effective" length.
  referenceEnergyGain = strength*sin_phi_s;
  oldLength = this->length;

  // Assign focal lengths for effective kicks that
  // model first order edge focussing.
  k1 = - 0.5*(referenceEnergyGain/oldLength)/(p.E);
  k2 =   0.5*(referenceEnergyGain/oldLength)/(p.E + referenceEnergyGain);

  // Thin lens kick upon entry
  p.state[3] += k1*p.state[0];
  p.state[4] += k1*p.state[1];

  // Free space propagation through "effective half length"
  // of the cavity.
  this->length = (oldLength)/2.0;
  w = (referenceEnergyGain/2.0) / p.E;
  if( fabs(w) > 1.0e-8 ) {
    this->length *= (log(1.+w)/w);
  }
  bmlnElmnt::localPropagate( p );

  // Cavity increases energy and momentum of the particle
  if( 0.0 != this->strength ) {
    E = p.Energy() + 0.5*strength*sin( phi_s + p.state[2] * w_rf / PH_MKS_c );
    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.E + referenceEnergyGain/2. );
    newRefP = p.ReferenceMomentum();
    p.state[3] *= ( oldRefP / newRefP );
    p.state[4] *= ( oldRefP / newRefP );
    p.state[5] = ( sqrt((E - p.m)*(E + p.m)) 
                 - sqrt((p.E-p.m)*(p.E+p.m))) / p.p;
  }

  // Put transverse wake field kick here ...

  // Free space propagation through "effective half length"
  // of the cavity.
  this->length = (oldLength)/2.0;
  w = (referenceEnergyGain/2.0) / p.E;
  if( fabs(w) > 1.0e-8 ) {
    this->length *= (log(1.+w)/w);
  }
  bmlnElmnt::localPropagate( p );

  // Cavity increases energy and momentum of the particle
  if( 0.0 != this->strength ) {
    E = p.Energy() + 0.5*strength*sin( phi_s + p.state[2] * w_rf / PH_MKS_c );
    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.E + referenceEnergyGain/2. );
    newRefP = p.ReferenceMomentum();
    p.state[3] *= ( oldRefP / newRefP );
    p.state[4] *= ( oldRefP / newRefP );
    p.state[5] = ( sqrt((E - p.m)*(E + p.m)) 
                 - sqrt((p.E-p.m)*(p.E+p.m))) / p.p;
  }

  // Thin lens kick upon exit
  p.state[3] += k2*p.state[0];
  p.state[4] += k2*p.state[1];

  // Restore length before returning.
  this->length = oldLength;
}


void rfcavity::localPropagate( JetParticle& p ) 
{
  static Jet E;
  static double oldRefP, newRefP;
  static double oldLength;
  static double referenceEnergyGain;
  static double w;
  static double k1, k2;

  // Scale length to "effective" length.
  referenceEnergyGain = strength*sin_phi_s;
  oldLength = this->length;

  // Assign focal lengths for effective kicks that
  // model first order edge focussing.
  k1 = - 0.5*(referenceEnergyGain/oldLength)/(p.E);
  k2 =   0.5*(referenceEnergyGain/oldLength)/(p.E + referenceEnergyGain);

  // Thin lens kick upon entry
  ( p.state ).SetComponent( 3, p.State(3) + k1*p.State(0) );
  ( p.state ).SetComponent( 4, p.State(4) + k1*p.State(1) );

  // Free space propagation through "effective half length"
  // of the cavity.
  this->length = (oldLength)/2.0;
  w = (referenceEnergyGain/2.0) / p.E;
  if( fabs(w) > 1.0e-8 ) {
    this->length *= (log(1.+w)/w);
  }
  bmlnElmnt::localPropagate( p );

  // Cavity increases energy and momentum of the particle
  if( 0.0 != this->strength ) {
    E = p.Energy() + 0.5*strength*sin( phi_s + p.state(2) * w_rf / PH_MKS_c );
    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.E + referenceEnergyGain/2.0 );
    newRefP = p.ReferenceMomentum();
    ( p.state ).SetComponent( 3, ( oldRefP / newRefP )*p.State(3) );
    ( p.state ).SetComponent( 4, ( oldRefP / newRefP )*p.State(4) );
    ( p.state ).SetComponent( 5, ( sqrt((E - p.m)*(E + p.m)) 
                                 - sqrt((p.E-p.m)*(p.E+p.m)) ) / p.p );
  }

  // Put transverse wake field kick here ...

  // Free space propagation through "effective half length"
  // of the cavity.
  this->length = (oldLength)/2.0;
  w = (referenceEnergyGain/2.0) / p.E;
  if( fabs(w) > 1.0e-8 ) {
    this->length *= (log(1.+w)/w);
  }
  bmlnElmnt::localPropagate( p );

  // Cavity increases energy and momentum of the particle
  if( 0.0 != this->strength ) {
    E = p.Energy() + 0.5*strength*sin( phi_s + p.state(2) * w_rf / PH_MKS_c );
    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.E + referenceEnergyGain/2.0 );
    newRefP = p.ReferenceMomentum();
    ( p.state ).SetComponent( 3, ( oldRefP / newRefP )*p.State(3) );
    ( p.state ).SetComponent( 4, ( oldRefP / newRefP )*p.State(4) );
    ( p.state ).SetComponent( 5, ( sqrt((E - p.m)*(E + p.m)) 
                                 - sqrt((p.E-p.m)*(p.E+p.m)) ) / p.p );
  }

  // Thin lens kick upon exit
  ( p.state ).SetComponent( 3, p.State(3) + k2*p.State(0) );
  ( p.state ).SetComponent( 4, p.State(4) + k2*p.state(1) );

  // Restore length
  this->length = oldLength;
}



void thinrfcavity::localPropagate( Particle& p ) 
{
  static double E;
  static double oldRefP, newRefP;

  if( 0.0 != this->strength ) {
    E = p.Energy() + strength*sin( phi_s + p.state[2] * w_rf / PH_MKS_c );
    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.E + strength*sin_phi_s );
    newRefP = p.ReferenceMomentum();
    p.state[3] *= ( oldRefP / newRefP );
    p.state[4] *= ( oldRefP / newRefP );
    p.state[5] = ( sqrt((E - p.m)*(E + p.m)) 
                 - sqrt((p.E-p.m)*(p.E+p.m))) / p.p;
  }
}

void thinrfcavity::localPropagate( JetParticle& p ) 
{
  static Jet E;
  static double oldRefP, newRefP;

  if( 0.0 != this->strength ) {
    E = p.Energy() + strength*sin( phi_s + p.state(2) * w_rf / PH_MKS_c );
    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.E + strength*sin_phi_s );
    newRefP = p.ReferenceMomentum();
    ( p.state ).SetComponent( 3, ( oldRefP / newRefP )*p.State(3) );
    ( p.state ).SetComponent( 4, ( oldRefP / newRefP )*p.State(4) );
    ( p.state ).SetComponent( 5, ( sqrt((E - p.m)*(E + p.m)) 
                                 - sqrt((p.E-p.m)*(p.E+p.m)) ) / p.p 
                            );
  }
}

#endif

// ================================================================

#ifdef MADPHYSICS

void thinrfcavity::localPropagate( Particle& p) 
{
 static char firstCall = 1;
 if( firstCall ) {
  cout << "*** WARNING ***  thinrfcavity::localPropagate( Particle& ): This\n"
       << "*** WARNING ***  routine was written using a linear\n"
       << "*** WARNING ***  approximation.  Eventually it will be\n"
       << "*** WARNING ***  fixed so as to represent an RF kick more\n"
       << "*** WARNING ***  correctly." << endl;
  firstCall = 0;
 }

 double kick;
 kick = strength*M_TWOPI*w_rf/p.Beta()*cos(phi_s)/p.Momentum()/PH_MKS_c/1.0e9;
 p.state[5] += kick*p.state[2];
}

void thinrfcavity::localPropagate( JetParticle& p) 
{
 static char firstCall = 1;
 if( firstCall ) {
  cout << "*** WARNING ***  thinrfcavity::localPropagate( JetParticle& ): This\n"
       << "*** WARNING ***  routine was written using a linear\n"
       << "*** WARNING ***  approximation.  Eventually it will be\n"
       << "*** WARNING ***  fixed so as to represent an RF kick more\n"
       << "*** WARNING ***  correctly." << endl;
  firstCall = 0;
 }

 Jet phi, u;
 Jet kick;
 kick = strength*M_TWOPI*w_rf/p.Beta()*cos(phi_s)/p.Momentum()/PH_MKS_c/1.0e9;
 u = p.state(5) + kick *p.state(2);
 ( p.state ).SetComponent( 5, u );
}

#endif
