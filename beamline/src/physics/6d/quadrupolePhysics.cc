#include "beamline.inc"

void quadrupole::P_LikeTPOT( bmlnElmnt* pbe, Particle& p ) {
  ((quadrupole*) pbe)->p_bml->propagate( p );
}

void quadrupole::J_LikeTPOT( bmlnElmnt* pbe, JetParticle& p ) {
  ((quadrupole*) pbe)->p_bml->propagate( p );
}
