#include "septum.h"

void thinSeptum::localPropagate( Particle& p ) {
  static int i;
  static double kick;
  static double TrState[6], dummy[6];
  
  for( i = 0; i < 6; i++  ) TrState[i] = p.state[i];
  
  if (TrState[0] > xWire ) kick = strengthPos/(1.0 + TrState[5]);
  else kick = strengthNeg/(1.0 + TrState[5]);
  
  TrState[3] += kick;

  p.state[3] = TrState[3];
  p.state[4] = TrState[4];
}



void thinSeptum::localPropagate( JetParticle& p ) { 
// This is taken from the thinSeptaPhysics.cc file. AJB 2/20/98

  static int i;
  Jet TrState[6], dummy[6];
  Jet JetxWire;
  Jet kick;

  for( i = 0; i < 6; i++  ) TrState[i] = p.state(i);

  JetxWire = xWire;
  if (TrState[0].standardPart() > JetxWire.standardPart() ) kick = strengthPos/(1.0 + TrState[5]);
  else kick = strengthNeg/(1.0 + TrState[5]);
  
 TrState[3] += kick;
  
  ( p.state ).SetComponent( 3, TrState[3] );
  ( p.state ).SetComponent( 4, TrState[4] );
}

