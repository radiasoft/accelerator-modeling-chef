#include "beamline.inc"
#include "combinedFunction.h"

void combinedFunction::localPropagate( Particle& p ) {

  if( Propagator ) (*Propagator)   ( this, p );
  else             p_bml->propagate( p );

}

void combinedFunction::localPropagate( JetParticle& p ) {

  if( Propagator ) (*Propagator)   ( this, p );
  else             p_bml->propagate( p );

}
  
