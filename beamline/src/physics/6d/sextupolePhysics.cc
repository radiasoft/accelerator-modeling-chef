#if HAVE_CONFIG_H
#include <config.h>
#endif
#if HAVE_CONFIG_H
#include <config.h>
#endif
#include "sextupole.h"

void sextupole::localPropagate( Particle& p ) {
  p_bml->propagate( p );
}

void sextupole::localPropagate( JetParticle& p ) {
  p_bml->propagate( p );
}


void thinSextupole::localPropagate( Particle& p ) {
 double x, y, k;
 double TrState[6], dummy[6];
 int    i;

 if( strength != 0.0 ) 
 {
   for( i = 0; i < 6; i++  ) {
     TrState[i] = p.state[i];
   }

   k = strength / p.ReferenceBRho();
   x = TrState[0];
   y = TrState[1];
 
   TrState[3] -= k * ( x*x - y*y );
   TrState[4] += 2.0 * k * x*y;

   p.state[3] = TrState[3];
   p.state[4] = TrState[4];
 }
}


void thinSextupole::localPropagate( JetParticle& p ) {
 static Jet TrState[6], dummy[6];
 double k;
 static Jet x, y;
 static int i;
 
 if( strength != 0.0 ) 
 {
   for( i = 0; i < 6; i++  ) {
     TrState[i] = p.state(i);
   }

   k = strength / p.ReferenceBRho();
   x = TrState[0];
   y = TrState[1];
 
   TrState[3] -= k * ( x*x - y*y );
   TrState[4] += 2.0 * k * x*y ;

   ( p.state ).SetComponent( 3, TrState[3] );
   ( p.state ).SetComponent( 4, TrState[4] );
 }
}
