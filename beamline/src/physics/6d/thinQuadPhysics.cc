#include "beamline.inc"

void thinQuad::localPropagate( Particle& p ) {
 // "Strength" is B'l in Tesla
 static double TrState[6], dummy[6];
 static double k;

 if( strength != 0.0 ) 
 {
   memcpy((void *)TrState, (const void *)p.state, 6 * sizeof(double));

   k = strength / p.ReferenceBRho();
   TrState[3] += - k * TrState[0];
   TrState[4] +=   k * TrState[1];

   p.state[3] = TrState[3];
   p.state[4] = TrState[4];

 }
}

void thinQuad::localPropagate( JetParticle& p ) {
 static Jet TrState[6], dummy[6];
 static int i;
 static double k;

 if( strength != 0.0 ) 
 {
   for( i = 0; i < 6; i++  ) {
     TrState[i] = p.state(i);
   }

   k = strength / p.ReferenceBRho();
   TrState[3] += - k * TrState[0];
   TrState[4] +=   k * TrState[1];

   ( p.state ).SetComponent( 3, TrState[3] );
   ( p.state ).SetComponent( 4, TrState[4] );
 }
}
