#include "beamline.inc"
#include "kick.h"

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
