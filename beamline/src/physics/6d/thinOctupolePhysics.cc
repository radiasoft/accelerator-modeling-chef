#include "beamline.inc"

void thinOctupole::propagate( Particle& p ) {
 static double x, y, k;
 static double xx, yy;
 static double TrState[6], dummy[6];
 static int    i;
 
  if(strength != 0) {
    if( align ) align->misalign( p, geometry /* ??? */, TrState );
    else        for( i = 0; i < 6; i++  ) 
      TrState[i] = p.state[i];

    k = strength / p.ReferenceBRho();
    x = TrState[0];
    y = TrState[1];
 
    xx = x*x;  yy = y*y;
 
    TrState[3] -= k * x * ( xx - 3.0*yy );
    TrState[4] -= k * y * ( yy - 3.0*xx );

    if( align ) {
      for( i = 0; i < 6; i++  ) dummy[i] = TrState[i];
      align->align( dummy, geometry /* ??? */, TrState );
    }
    
    p.state[3] = TrState[3];
    p.state[4] = TrState[4];
  }
}

void thinOctupole::propagate( JetParticle& p ) {
  if(strength != 0) {
    Jet     TrState[6], dummy[6];
    double  k;
    Jet     x, y;
    Jet     xx, yy;
    int     i;
 
    if( align ) align->misalign( p, geometry /* ??? */, TrState );
    else        for( i = 0; i < 6; i++  ) 
      TrState[i] = p.state(i);

    k = strength / p.ReferenceBRho();
    x = TrState[0];
    y = TrState[1];
    
    xx = x*x;  yy = y*y;

    TrState[3] -= k * x * ( xx - 3.0*yy );
    TrState[4] -= k * y * ( yy - 3.0*xx );

    if( align ) {
      for( i = 0; i < 6; i++  ) dummy[i] = TrState[i];
      align->align( dummy, geometry /* ??? */, TrState );
    }

    ( p.state ).SetComponent( 3, TrState[3] );
    ( p.state ).SetComponent( 4, TrState[4] );
  }
}
