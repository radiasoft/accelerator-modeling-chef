#include "beamline.inc"

void octupole::localPropagate( Particle& p ) {
  p_bml->propagate( p );
}

void octupole::localPropagate( JetParticle& p ) {
  p_bml->propagate( p );
}
