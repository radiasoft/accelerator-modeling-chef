#include "beamline.inc"
#include "FDPolarity.h"
void JetthinSext::localPropagate( Particle& p ) {
 double k;
 double    inState  [BMLN_dynDim];
 double    outState  [BMLN_dynDim];
 int i;

 FDPolarity* pol = (FDPolarity*)dataHook.find("Polarity");
 if(pol != 0)
   k = pol->polarity * strength / p.bRho;
 else
   k = strength / p.bRho;

  p.getState( inState );

  outState[0] = inState[0];
  outState[1] = inState[1];
  outState[2] = inState[2];
  outState[3] = inState[3];
  outState[4] = inState[4];
  outState[3] += k * ( inState[0]*inState[0] - inState[1]*inState[1] );
  outState[4] -= 2.0 * k * inState[0]*inState[1];
  outState[5] = inState[5];


  p.setState( outState );

}

void JetthinSext::localPropagate( JetParticle& p ) {
  Jet k;
  Jet    zero;
  Jet    inState  [BMLN_dynDim];
  Jet    outState  [BMLN_dynDim];
  int i;
  
  zero = 0.0;
  // ??? REMOVE zero.fixReferenceAtStart( p.state );

  k = K2L / p.BRho();

  p.getState( inState );

  outState[0] = inState[0];
  outState[1] = inState[1];
  outState[2] = inState[2];
  outState[3] = inState[3] + k * ( inState[0]*inState[0] - 
				  inState[1]*inState[1] );
  outState[4] = inState[4] - 2.0 * k * inState[0]*inState[1];
  outState[5] = inState[5];

  for( i = 0; i < BMLN_dynDim; i++  ) {
    ( p.state ).SetComponent( i, outState[i] );
  }
}
