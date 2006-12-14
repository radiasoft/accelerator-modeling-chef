/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rfcavityPhysics.cc
******  Version:   3.0
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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/rfcavity.h>

#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

using FNAL::pcerr;
using FNAL::pcout;


// REMOVE: #ifndef MADPHYSICS

void rfcavity::localPropagate( Particle& p ) 
{
  double E( p.ReferenceEnergy() );

  double oldRefP, newRefP;
  double oldLength;
  double referenceEnergyGain;
  double w;
  double k1, k2;

  Vector& state = p.getState();

  // Scale length to "effective" length.
  referenceEnergyGain = strength*sin_phi_s;
  oldLength = this->length;

  // Assign focal lengths for effective kicks that
  // model first order edge focussing.

  k1 = - 0.5*(referenceEnergyGain/oldLength)/ E;
  k2 =   0.5*(referenceEnergyGain/oldLength)/(E + referenceEnergyGain);

  // Thin lens kick upon entry
  state[3] += k1*state[0];
  state[4] += k1*state[1];


  // Propagate through the inner structures
  double x_in = state[0];
  double y_in = state[1];
  w = (referenceEnergyGain/2.0) / E;
  if( std::abs(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                       { w = 1.0;           }

  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*(x++))->localPropagate( p );
  }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];


  // Thin lens kick upon exit
  state[3] += k2*state[0];
  state[4] += k2*state[1];

  // Restore length before returning.
  this->length = oldLength;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::localPropagate( JetParticle& p ) 
{
  double E( p.ReferenceEnergy() ); 

  double oldRefP, newRefP;
  double oldLength;
  double referenceEnergyGain;
  double w;
  double k1, k2;

  Mapping& state = p.getState();

  // Scale length to "effective" length.
  referenceEnergyGain = strength*sin_phi_s;
  oldLength = this->length;

  // Assign focal lengths for effective kicks that
  // model first order edge focussing.
  k1 = - 0.5*(referenceEnergyGain/oldLength)/ E;
  k2 =   0.5*(referenceEnergyGain/oldLength)/(E + referenceEnergyGain);

  // Thin lens kick upon entry

   state[3] = state[3] + k1*state[0];
   state[4] = state[4] + k1*state[1];


  // Propagate through the inner structures
  Jet x_in( state[0] );
  Jet y_in( state[1] );
  w = (referenceEnergyGain/2.0) / E;

  if( std::abs(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                       { w = 1.0;           }

  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*(x++))->localPropagate( p );
  }

   state[0] = ( 1.0 - w )*x_in + w*state[0];
   state[1] = ( 1.0 - w )*y_in + w*state[1];

  // Thin lens kick upon exit
   state[3] =  state[3] + k2*state[0];
   state[4] =  state[4] + k2*state[1];

  // Restore length
  this->length = oldLength;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::localPropagate( Particle& p ) 
{
  
  double denom, px, py, cs; 
  double oldRefP, newRefP;
  
  Vector& state = p.getState(); 
 
  if( 0.0 != this->strength ) {
    px = p.get_npx();
    py = p.get_npy();
    denom = 1.0 + p.get_ndp();
    cs = sqrt( 1.0 - ( ( px*px + py*py ) / ( denom*denom ) ) );
    double E = p.Energy() + ((strength*cs)*(sin( phi_s + state[2] * w_rf / PH_MKS_c )));

    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.ReferenceEnergy() + strength*sin_phi_s );
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

  if( 0.0 != this->strength ) {
    px = p.get_npx();
    py = p.get_npy();
    denom = 1.0 + p.get_ndp();
    cs = sqrt( 1.0 - ( ( px*px + py*py ) / ( denom*denom ) ) );
    Jet E = p.Energy() + ((strength*cs)*(sin( phi_s + state(2) * w_rf / PH_MKS_c )));

    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.ReferenceEnergy() + strength*sin_phi_s );
    newRefP = p.ReferenceMomentum();
    state[3] = ( oldRefP / newRefP )*state[3];
    state[4] = ( oldRefP / newRefP )*state[4];

    // REMOVE: ( state ).SetComponent( 5, ( sqrt((E - p.m)*(E + p.m)) 
    // REMOVE:                              - sqrt((p.E-p.m)*(p.E+p.m)) ) / p.p 
    // REMOVE:                         );

     double   m  = p.Mass();
     double   pr = p.ReferenceMomentum();

     state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0 ;
  }
}
// REMOVE: 
// REMOVE: #endif
// REMOVE: 
// REMOVE: // ================================================================
// REMOVE: 
// REMOVE: #ifdef MADPHYSICS
// REMOVE: 
// REMOVE: void thinrfcavity::localPropagate( Particle& p) 
// REMOVE: {
// REMOVE:  static char firstCall = 1;
// REMOVE:  if( firstCall ) {
// REMOVE:   (*pcout) << "*** WARNING ***  thinrfcavity::localPropagate( Particle& ): This\n"
// REMOVE:        << "*** WARNING ***  routine was written using a linear\n"
// REMOVE:        << "*** WARNING ***  approximation.  Eventually it will be\n"
// REMOVE:        << "*** WARNING ***  fixed so as to represent an RF kick more\n"
// REMOVE:        << "*** WARNING ***  correctly." << endl;
// REMOVE:   firstCall = 0;
// REMOVE:  }
// REMOVE: 
// REMOVE:  double kick;
// REMOVE:  kick = strength*M_TWOPI*w_rf/p.Beta()*cos(phi_s)/p.Momentum()/PH_MKS_c/1.0e9;
// REMOVE:  p.state[5] += kick*p.state[2];
// REMOVE: }
// REMOVE: 
// REMOVE: void thinrfcavity::localPropagate( JetParticle& p) 
// REMOVE: {
// REMOVE:  static char firstCall = 1;
// REMOVE:  if( firstCall ) {
// REMOVE:   (*pcout) << "*** WARNING ***  thinrfcavity::localPropagate( JetParticle& ): This\n"
// REMOVE:        << "*** WARNING ***  routine was written using a linear\n"
// REMOVE:        << "*** WARNING ***  approximation.  Eventually it will be\n"
// REMOVE:        << "*** WARNING ***  fixed so as to represent an RF kick more\n"
// REMOVE:        << "*** WARNING ***  correctly." << endl;
// REMOVE:   firstCall = 0;
// REMOVE:  }
// REMOVE: 
// REMOVE:  Jet phi, u;
// REMOVE:  Jet kick;
// REMOVE:  kick = strength*M_TWOPI*w_rf/p.Beta()*cos(phi_s)/p.Momentum()/PH_MKS_c/1.0e9;
// REMOVE:  u = p.state(5) + kick *p.state(2);
// REMOVE:  ( p.state ).SetComponent( 5, u );
// REMOVE: }
// REMOVE: 
// REMOVE: #endif
