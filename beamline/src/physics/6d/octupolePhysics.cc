#include "beamline.inc"

void octupole::propagate( Particle& p ) {
  p_bml->propagate( p );
}

void octupole::propagate( JetParticle& p ) {
  p_bml->propagate( p );
}
