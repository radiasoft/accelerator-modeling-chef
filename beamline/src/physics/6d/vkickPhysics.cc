#include "beamline.inc"

void vkick::localPropagate( Particle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    p.state[4] += strength;
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    p.state[4] += strength;
  }
}


void vkick::localPropagate( JetParticle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    ( p.state ).SetComponent( 4, p.state(4) + strength );
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    ( p.state ).SetComponent( 4, p.state(4) + strength );
  }
}
