#include "beamline.inc"

void sextupole::propagate( Particle& p ) {
  p_bml->propagate( p );
}

void sextupole::propagate( JetParticle& p ) {
  p_bml->propagate( p );
}
