#include "beamline.inc"

void thinSeptum::propagate( Particle& p ) {
  static int i;
  static double kick;
  static double TrState[6], dummy[6];
  
  if( align ) align->misalign( p, geometry /* ??? */, TrState );
  else        for( i = 0; i < 6; i++  ) TrState[i] = p.state[i];
  
  if (TrState[0] > xWire ) kick = strengthPos/(1.0 + TrState[5]);
  else kick = strengthNeg/(1.0 + TrState[5]);
  
  TrState[3] += kick;

  if( align ) {
    for( i = 0; i < 6; i++  ) dummy[i] = TrState[i];
    align->align( dummy, geometry /* ??? */, TrState );
  }
  
  p.state[3] = TrState[3];
  p.state[4] = TrState[4];
}



void thinSeptum::propagate( JetParticle& p ) { }

