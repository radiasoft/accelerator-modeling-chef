#include "beamline.inc"

// OLD FORM  
// OLD FORM  void rfcavity::propagate( Proton& p) {
// OLD FORM   double phi;
// OLD FORM  
// OLD FORM   phi = phi_s + p.state[2] * w_rf;
// OLD FORM   p.state[5] += eV * ( sin(phi) - sin_phi_s );
// OLD FORM  }
// OLD FORM  
// OLD FORM  void rfcavity::propagate( JetProton& p) {
// OLD FORM   Jet phi, u;
// OLD FORM   
// OLD FORM   phi = phi_s + p.state[2] * w_rf;
// OLD FORM   u = p.state[5] + eV * ( sin(phi) - sin_phi_s );
// OLD FORM   ( p.state ).setVariable( u, 5 );
// OLD FORM  }
// OLD FORM  


#ifndef MADPHYSICS

void thinrfcavity::propagate( Particle& p ) 
{
 static double E;
 E = p.Energy() + eV*sin( phi_s + p.state[2] * w_rf / PH_MKS_c );
 p.SetReferenceEnergy( p.E + eV*sin(phi_s) );
 p.state[5] = ( sqrt((E - p.m)*(E + p.m)) 
              - sqrt((p.E-p.m)*(p.E+p.m))) / p.p;
}

void thinrfcavity::propagate( JetParticle& p ) 
{
 static Jet E;
 E = p.Energy() + eV*sin( phi_s + p.state(2) * w_rf / PH_MKS_c );
 p.SetReferenceEnergy( p.E + eV*sin(phi_s) );
 ( p.state ).SetComponent( 5, ( sqrt((E - p.m)*(E + p.m)) 
                              - sqrt((p.E-p.m)*(p.E+p.m)) ) / p.p 
                         );
}

#endif

// ================================================================

#ifdef MADPHYSICS

void thinrfcavity::propagate( Particle& p) 
{
 static char firstCall = 1;
 if( firstCall ) {
  cout << "*** WARNING ***  thinrfcavity::propagate( Particle& ): This\n"
       << "*** WARNING ***  routine was written using a linear\n"
       << "*** WARNING ***  approximation.  Eventually it will be\n"
       << "*** WARNING ***  fixed so as to represent an RF kick more\n"
       << "*** WARNING ***  correctly." << endl;
  firstCall = 0;
 }

 double kick;
 kick = eV*2.0*M_PI*w_rf/p.Beta()*cos(phi_s)/p.Momentum()/PH_MKS_c/1.0e9;
 p.state[5] += kick*p.state[2];
}

void thinrfcavity::propagate( JetParticle& p) 
{
 static char firstCall = 1;
 if( firstCall ) {
  cout << "*** WARNING ***  thinrfcavity::propagate( JetParticle& ): This\n"
       << "*** WARNING ***  routine was written using a linear\n"
       << "*** WARNING ***  approximation.  Eventually it will be\n"
       << "*** WARNING ***  fixed so as to represent an RF kick more\n"
       << "*** WARNING ***  correctly." << endl;
  firstCall = 0;
 }

 Jet phi, u;
 Jet kick;
 kick = eV*2.0*M_PI*w_rf/p.Beta()*cos(phi_s)/p.Momentum()/PH_MKS_c/1.0e9;
 u = p.state(5) + kick *p.state(2);
 ( p.state ).SetComponent( 5, u );
}

#endif
