#include "beamline.inc"

void JetQuadrupole::P_LikeTPOT( bmlnElmnt* pbe, Particle& p ) {
  ((JetQuadrupole*) pbe)->p_bml->propagate( p );
}

void JetQuadrupole::J_LikeTPOT( bmlnElmnt* pbe, JetParticle& p ) {
  ((JetQuadrupole*) pbe)->p_bml->propagate( p );
}
