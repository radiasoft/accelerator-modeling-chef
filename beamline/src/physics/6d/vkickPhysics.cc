#include "beamline.inc"

void vkick::propagate( Particle& p ) {
  static double TrState[6], dummy[6];
  if( align ) {
    align->misalign( p, geometry /* ??? */, TrState );
  } else  {
    memcpy((void *)TrState, (const void *)p.state, 6 * sizeof(double));
  }
  // p.state[4] += strength;
  TrState[4] += strength;

  if( align ) {
    memcpy((void *)dummy, (const void *)TrState, 6 * sizeof(double));
    align->align( dummy, geometry /* ??? */, TrState );
  }
  p.state[3] = TrState[3];
  p.state[4] = TrState[4];
}

void vkick::propagate( JetParticle& p ) {
  int i;
  static Jet TrState[6], dummy[6];
  if( align ) {
    align->misalign( p, geometry /* ??? */, TrState );
  } else {
    for( i = 0; i < 6; i++  ) TrState[i] = p.state(i);
  }

  TrState[4] += strength;
  // Jet  dummy;
  // dummy = p.state(4) + strength;
  // ( p.state ).SetComponent( 4, dummy );

  if( align ) {
    for( i = 0; i < 6; i++  ) dummy[i] = TrState[i];
    align->align( dummy, geometry /* ??? */, TrState );
  }
  ( p.state ).SetComponent( 3, TrState[3] );
  ( p.state ).SetComponent( 4, TrState[4] );
}

