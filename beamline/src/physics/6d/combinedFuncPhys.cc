#include "beamline.inc"
#include "combinedFunction.h"

void combinedFunction::localPropagate( Particle& p ) {

  p_bml->propagate( p );

}

void combinedFunction::localPropagate( JetParticle& p ) {

  p_bml->propagate( p );

}
  
