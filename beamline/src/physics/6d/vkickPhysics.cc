#include "beamline.inc"

void vkick::localPropagate( Particle& p ) {
  p.state[4] += strength;
}

void vkick::localPropagate( JetParticle& p ) {
  ( p.state ).SetComponent( 4, p.state(4) + strength );
}
