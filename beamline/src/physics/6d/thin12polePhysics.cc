#include "beamline.inc"

void thin12pole::localPropagate( Particle& p ) {
 double x, y, k;
 
 if(strength != 0) {
   k = strength/p.ReferenceBRho();
   x = p.state[0];
   y = p.state[1];
   
   Complex z( x, y );
   Complex z2 = z*z;
   z = z2*z2*z;
   
   p.state[3] -= k * real(z);
   p.state[4] += k * imag(z);
 }
}

void thin12pole::localPropagate( JetParticle& p ) {
  if(strength != 0) {
    Jet  k, x, y, s;
    JetC z, z2;
    
    k = strength/p.ReferenceBRho();
    x = p.state(0);
    y = p.state(1);
    
    z = x + complex_i*y;
    z2 = z*z;
    z = z2*z2*z;

    s = p.state(3) - ( k * real(z) );
    ( p.state ).SetComponent( 3, s );
    s = p.state(4) + ( k * imag(z) );
    ( p.state ).SetComponent( 4, s );
  }
}
