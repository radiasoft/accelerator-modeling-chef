#include "beamline.inc"

void hkick::propagate( Particle& p ) {
  static double TrState[6], dummy[6];
  if( align ) {
    align->misalign( p, geometry /* ??? */, TrState );
    TrState[3] += strength;
    memcpy((void *)dummy, (const void *)TrState, 6 * sizeof(double));
    align->align( dummy, geometry /* ??? */, TrState );
    p.state[3] = TrState[3];
    p.state[4] = TrState[4];
  }
  else {
    p.state[3] += strength;
  }
}

void hkick::propagate( JetParticle& p ) {
  int i;
  static Jet TrState[6], dummy[6];
  if( align ) {
    align->misalign( p, geometry /* ??? */, TrState );
    TrState[3] += strength;
    for( i = 0; i < 6; i++  ) dummy[i] = TrState[i];
    align->align( dummy, geometry /* ??? */, TrState );
    ( p.state ).SetComponent( 3, TrState[3] );
    ( p.state ).SetComponent( 4, TrState[4] );
  }
  else {
    ( p.state ).SetComponent( 3, p.state(3) + strength );
  }
}
