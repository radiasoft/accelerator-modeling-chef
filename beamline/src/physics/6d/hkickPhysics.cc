#include "beamline.inc"

void hkick::localPropagate( Particle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    p.state[3] += strength;
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    p.state[3] += strength;
  }
}


void hkick::localPropagate( JetParticle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    ( p.state ).SetComponent( 3, p.state(3) + strength );
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    ( p.state ).SetComponent( 3, p.state(3) + strength );
  }
}
