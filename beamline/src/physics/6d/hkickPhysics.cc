#include "beamline.inc"

void hkick::localPropagate( Particle& p ) {
  p.state[3] += strength;
}

void hkick::localPropagate( JetParticle& p ) {
  ( p.state ).SetComponent( 3, p.state(3) + strength );
}
