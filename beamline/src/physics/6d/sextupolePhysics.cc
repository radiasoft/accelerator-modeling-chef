#include "beamline.inc"

void sextupole::localPropagate( Particle& p ) {
  p_bml->propagate( p );
}

void sextupole::localPropagate( JetParticle& p ) {
  p_bml->propagate( p );
}
