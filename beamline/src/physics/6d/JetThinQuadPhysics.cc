#include "beamline.inc"

void JetthinQuad::propagate( Particle& p ) {
 // "Strength" is B'l in Tesla
 static double xpr, ypr, p3divpbar, state5p1;

 state5p1 = 1.0 + p.state[5];
 p3divpbar = sqrt( state5p1 * state5p1
                 - p.state[3]*p.state[3] 
                 - p.state[4]*p.state[4] );
 xpr = p.state[3] / p3divpbar; 
 ypr = p.state[4] / p3divpbar; 

 xpr += - strength * p.state[0] / p.BRho();
 ypr +=   strength * p.state[1] / p.BRho();

 p3divpbar = state5p1 / sqrt( 1.0 + xpr*xpr + ypr*ypr );

 p.state[3] = xpr*p3divpbar;
 p.state[4] = ypr*p3divpbar;
}

void JetthinQuad::propagate( JetParticle& p ) {
 Jet dummy;
 Jet xpr, ypr, p3divpbar;

 p3divpbar = sqrt( pow( 1.0 + p.state(5), 2.0 )
                 - p.state(3)*p.state(3) 
                 - p.state(4)*p.state(4) );
 xpr = p.state(3) / p3divpbar; 
 ypr = p.state(4) / p3divpbar; 

 xpr += - KL * p.state(0) / p.BRho();
 ypr +=   KL * p.state(1) / p.BRho();

 p3divpbar = ( 1.0 + p.state(5) ) / sqrt( 1.0 + xpr*xpr + ypr*ypr );

 dummy = xpr*p3divpbar;
 ( p.state ).SetComponent( 3, dummy );
 dummy = ypr*p3divpbar;
 ( p.state ).SetComponent( 4, dummy );
}

