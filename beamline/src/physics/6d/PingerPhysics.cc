#include "beamline.inc"
#include <pinger.h>

/* ********** Pinger propagators **********
   Note that these are also the propagators for HPinger and VPinger
   */

void Pinger::localPropagate( Particle& p ) {
  if ( isArmed() ) {
    p.state[3] += cos(_kick_direction)*strength;
    p.state[4] += sin(_kick_direction)*strength;
    disarm();
  }
}

void Pinger::localPropagate( JetParticle& p ) {
  if ( isArmed() ) {
    Jet  dummy;
    dummy = p.State(3) + cos(_kick_direction)*strength;
    ( p.state ).SetComponent( 3, dummy );
    dummy = p.State(4) + sin(_kick_direction)*strength;
    ( p.state ).SetComponent( 4, dummy );
    disarm();
  }
}

