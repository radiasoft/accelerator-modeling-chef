#include "beamline.inc"

void thinQuad::propagate( Particle& p ) {
 // "Strength" is B'l in Tesla
 static double TrState[6], dummy[6];
 static double k;

 if( align ) {
   align->misalign( p, geometry /* ??? */, TrState );
 } else  {
   memcpy((void *)TrState, (const void *)p.state, 6 * sizeof(double));
 }

 k = strength / p.ReferenceBRho();
 TrState[3] += - k * TrState[0];
 TrState[4] +=   k * TrState[1];

 if( align ) {
   memcpy((void *)dummy, (const void *)TrState, 6 * sizeof(double));
   align->align( dummy, geometry /* ??? */, TrState );
 }

 p.state[3] = TrState[3];
 p.state[4] = TrState[4];

}

void thinQuad::propagate( JetParticle& p ) {
 static Jet TrState[6], dummy[6];
 static int i;
 static double k;

 if( align ) align->misalign( p, geometry /* ??? */, TrState );
 else        for( i = 0; i < 6; i++  ) 
               TrState[i] = p.state(i);

 k = strength / p.ReferenceBRho();
 TrState[3] += - k * TrState[0];
 TrState[4] +=   k * TrState[1];

 if( align ) {
   for( i = 0; i < 6; i++  ) dummy[i] = TrState[i];
   align->align( dummy, geometry /* ??? */, TrState );
 }

 ( p.state ).SetComponent( 3, TrState[3] );
 ( p.state ).SetComponent( 4, TrState[4] );

}
