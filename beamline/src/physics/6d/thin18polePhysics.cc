#include "beamline.inc"

void thin18pole::localPropagate( Particle& p ) {
 double x, y, k;
 
  if(strength != 0) {
    k = strength/p.ReferenceBRho();
    x = p.state[0];
    y = p.state[1];
 
    Complex z( x, y );
    z = z*z;
    z = z*z;
    z = z*z;

    p.state[3] -= k * real(z);
    p.state[4] += k * imag(z);
  }
}

void thin18pole::localPropagate( JetParticle& p ) {
  if(strength != 0) {
    Jet  k, x, y, s;
    JetC z;
 
    k = strength/p.ReferenceBRho();
    x = p.state(0);
    y = p.state(1);
 
    z = x + complex_i*y;
    z = z*z;
    z = z*z;
    z = z*z;

    s = p.state(3) - ( k * real(z) );
    ( p.state ).SetComponent( 3, s );
    s = p.state(4) + ( k * imag(z) );
    ( p.state ).SetComponent( 4, s );
  }
}
