#if HAVE_CONFIG_H
#include <config.h>
#endif
#if HAVE_CONFIG_H
#include <config.h>
#endif
#include "combinedFunction.h"

void combinedFunction::localPropagate( Particle& p ) {

  if( Propagator ) (*Propagator)   ( this, p );
  else             p_bml->propagate( p );

}

void combinedFunction::localPropagate( JetParticle& p ) {

  if( Propagator ) (*Propagator)   ( this, p );
  else             p_bml->propagate( p );

}
  
