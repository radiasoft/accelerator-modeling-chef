#include "rfcavity.h"
#include "PhysicsConstants.h"

#ifndef MADPHYSICS

void thinrfcavity::localPropagate( Particle& p ) 
{
 static double E;
 static double oldRefP, newRefP;
 E = p.Energy() + strength*sin( phi_s + p.state[2] * w_rf / PH_MKS_c );
 oldRefP = p.ReferenceMomentum();
 p.SetReferenceEnergy( p.E + strength*sin(phi_s) );
 newRefP = p.ReferenceMomentum();
 p.state[3] *= ( oldRefP / newRefP );
 p.state[4] *= ( oldRefP / newRefP );
 p.state[5] = ( sqrt((E - p.m)*(E + p.m)) 
              - sqrt((p.E-p.m)*(p.E+p.m))) / p.p;
}

void thinrfcavity::localPropagate( JetParticle& p ) 
{
 static Jet E;
 static double oldRefP, newRefP;
 E = p.Energy() + strength*sin( phi_s + p.state(2) * w_rf / PH_MKS_c );
 oldRefP = p.ReferenceMomentum();
 p.SetReferenceEnergy( p.E + strength*sin(phi_s) );
 newRefP = p.ReferenceMomentum();
 ( p.state ).SetComponent( 3, ( oldRefP / newRefP )*p.State(3) );
 ( p.state ).SetComponent( 4, ( oldRefP / newRefP )*p.State(4) );
 ( p.state ).SetComponent( 5, ( sqrt((E - p.m)*(E + p.m)) 
                              - sqrt((p.E-p.m)*(p.E+p.m)) ) / p.p 
                         );
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
