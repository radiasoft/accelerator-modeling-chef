#include "beamline.inc"

void vkick::propagate( Particle& p ) {
 p.state[4] += strength;
}

void vkick::propagate( JetParticle& p ) {
 Jet  dummy;
 dummy = p.state(4) + strength;
 ( p.state ).SetComponent( 4, dummy );
}

