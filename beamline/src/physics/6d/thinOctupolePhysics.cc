#include "beamline.inc"

void thinOctupole::localPropagate( Particle& p ) {
 static double x, y, k;
 static double xx, yy;
 
  if(strength != 0) {
    k = strength / p.ReferenceBRho();
    x = p.state[0];
    y = p.state[1];
 
    xx = x*x;  
    yy = y*y;
 
    p.state[3] -= k * x * ( xx - 3.0*yy );
    p.state[4] -= k * y * ( yy - 3.0*xx );
  }
}

void thinOctupole::localPropagate( JetParticle& p ) {
  if(strength != 0) {
    double  k;
    Jet     x( p.State(0) );
    Jet     y( p.State(1) );
    Jet     xx( x.Env() );
    Jet     yy( y.Env() );
 
    k = strength / p.ReferenceBRho();
    
    xx = x*x;  
    yy = y*y;

    p.State(3) -= k * x * ( xx - 3.0*yy );
    p.State(4) -= k * y * ( yy - 3.0*xx );
  }
}
