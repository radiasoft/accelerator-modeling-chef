#include "beamline.inc"

void vkick::propagate( Particle& p ) {
  static double TrState[6], dummy[6];
  if( align ) {
    align->misalign( p, geometry /* ??? */, TrState );
    TrState[4] += strength;
    memcpy((void *)dummy, (const void *)TrState, 6 * sizeof(double));
    align->align( dummy, geometry /* ??? */, TrState );
    p.state[3] = TrState[3];
    p.state[4] = TrState[4];
  }
  else {
    p.state[4] += strength;
  }
}

void vkick::propagate( JetParticle& p ) {
  int i;
  static Jet TrState[6], dummy[6];
  if( align ) {
    align->misalign( p, geometry /* ??? */, TrState );
    TrState[4] += strength;
    for( i = 0; i < 6; i++  ) dummy[i] = TrState[i];
    align->align( dummy, geometry /* ??? */, TrState );
    ( p.state ).SetComponent( 3, TrState[3] );
    ( p.state ).SetComponent( 4, TrState[4] );
  }
  else {
    ( p.state ).SetComponent( 4, p.state(4) + strength );
  }
}
