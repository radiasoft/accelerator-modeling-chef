#if HAVE_CONFIG_H
#include <config.h>
#endif
#if HAVE_CONFIG_H
#include <config.h>
#endif
#include "kick.h"

// =================================================
// vkick propagators
// =================================================

void vkick::localPropagate( Particle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    p.state[4] += strength;
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    p.state[4] += strength;
  }
}


void vkick::localPropagate( JetParticle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    ( p.state ).SetComponent( 4, p.state(4) + strength );
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    ( p.state ).SetComponent( 4, p.state(4) + strength );
  }
}


// =================================================
// hkick propagators
// =================================================

void hkick::localPropagate( Particle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    p.state[3] += strength;
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    p.state[3] += strength;
  }
}


void hkick::localPropagate( JetParticle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    ( p.state ).SetComponent( 3, p.state(3) + strength );
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    ( p.state ).SetComponent( 3, p.state(3) + strength );
  }
}

// =================================================
// kick propagators
// =================================================

void kick::localPropagate( Particle& p ) 
{
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    p.state[3] += horizontalKick;
    p.state[4] += verticalKick;
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    p.state[3] += horizontalKick;
    p.state[4] += verticalKick;
  }
}

void kick::localPropagate( JetParticle& p ) 
{
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    (p.state).SetComponent( 3, p.state(3) + horizontalKick );
    (p.state).SetComponent( 4, p.state(4) + verticalKick   );
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    (p.state).SetComponent( 3, p.state(3) + horizontalKick );
    (p.state).SetComponent( 4, p.state(4) + verticalKick   );
  }
}
