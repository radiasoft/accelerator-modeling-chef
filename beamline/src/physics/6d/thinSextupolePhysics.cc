#include "beamline.inc"

void thinSextupole::propagate( Particle& p ) {
 double x, y, k;
 double TrState[6], dummy[6];
 int    i;

 if( align ) align->misalign( p, geometry /* ??? */, TrState );
 else        for( i = 0; i < 6; i++  ) 
               TrState[i] = p.state[i];

 k = strength / p.ReferenceBRho();
 x = TrState[0];
 y = TrState[1];
 
 TrState[3] -= k * ( x*x - y*y );
 TrState[4] += 2.0 * k * x*y;

 if( align ) {
   for( i = 0; i < 6; i++  ) dummy[i] = TrState[i];
   align->align( dummy, geometry /* ??? */, TrState );
 }

 p.state[3] = TrState[3];
 p.state[4] = TrState[4];
}


void thinSextupole::propagate( JetParticle& p ) {
 static Jet TrState[6], dummy[6];
 double k;
 static Jet x, y;
 static int i;
 
 if( align ) align->misalign( p, geometry /* ??? */, TrState );
 else        for( i = 0; i < 6; i++  ) 
               TrState[i] = p.state(i);

 k = strength / p.ReferenceBRho();
 x = TrState[0];
 y = TrState[1];
 
 TrState[3] -= k * ( x*x - y*y );
 TrState[4] += 2.0 * k * x*y ;

 if( align ) {
   for( i = 0; i < 6; i++  ) dummy[i] = TrState[i];
   align->align( dummy, geometry /* ??? */, TrState );
 }

 ( p.state ).SetComponent( 3, TrState[3] );
 ( p.state ).SetComponent( 4, TrState[4] );

}
