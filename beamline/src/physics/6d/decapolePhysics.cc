#include "decapole.h"

void thinDecapole::localPropagate( Particle& p ) {
 double x, y, k;
 double xx, xy, yy;
 
  if(strength != 0) {
    k = strength/p.ReferenceBRho();
    x = p.state[0];
    y = p.state[1];
 
    xx = x*x;  xy = x*y;  yy = y*y;
 
    p.state[3] -=       k * ( xx*xx - 6.0*xy*xy + yy*yy );
    p.state[4] += 4.0 * k * xy * ( xx - yy );
  }
}

void thinDecapole::localPropagate( JetParticle& p ) {
  if(strength != 0) {
    Jet     k, x, y, s;;
    Jet     xx, xy, yy;
 
    k = strength/p.ReferenceBRho();
    x = p.state(0);
    y = p.state(1);
 
    xx = x*x;  xy = x*y;  yy = y*y;
 
    s = p.state(3) - ( k * ( xx*xx - 6.0*xy*xy + yy*yy ) );
    ( p.state ).SetComponent( 3, s );
    s = p.state(4) + ( 4.0 * k * xy * ( xx - yy ) );
    ( p.state ).SetComponent( 4, s );
  }
}
