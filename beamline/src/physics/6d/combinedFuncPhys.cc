#include "beamline.inc"
#include "combinedFunction.h"

void combinedFunction::propagate( Particle& p ) {
 double TrState[6];

 if( align ) {
   align->misalign( p, geometry /* ??? */, TrState );
   p.setState(TrState);
 }

  p_bml->propagate( p );

 if( align ) {
   align->align( p, geometry /* ??? */, TrState );
   p.setState(TrState);
 }
}

void combinedFunction::propagate( JetParticle& p ) {
 Jet TrState[6];
 int i;

 if( align ) {
   align->misalign( p, geometry /* ??? */, TrState );
   for(i=0; i < 6; i++)
     ( p.state ).SetComponent( i, TrState[i] );
 }

  p_bml->propagate( p );

 if( align ) {
   align->align( p, geometry /* ??? */, TrState );
   for(i=0; i < 6; i++)
     ( p.state ).SetComponent( i, TrState[i] );
 }
}
  
