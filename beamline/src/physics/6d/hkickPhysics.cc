#include "beamline.inc"

void hkick::propagate( Particle& p ) {
 p.state[3] += strength;
}

void hkick::propagate( JetParticle& p ) {
 Jet  dummy;
 dummy = p.state(3) + strength;
 ( p.state ).SetComponent( 3, dummy );
}
