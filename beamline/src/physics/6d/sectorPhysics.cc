#include "beamline.inc"

void sector::localPropagate( Particle& p ) {
 static int    i, j; 
 static double inState[ BMLN_dynDim ];
 
 for( i = 0; i < BMLN_dynDim; i++  ) 
   inState[i] = p.state[i];
   
 if( mapType ) 
   for ( j = 0; j < BMLN_dynDim; j++ ) 
     p.state[j] = myMap(j)( inState );
 
 else
 {
   for( i = 0; i < BMLN_dynDim; i++  ) {
     p.state[i] = 0.0;
     for( j = 0; j < BMLN_dynDim; j++  ) 
       p.state[i] += mapMatrix[i][j]*inState[j];
   }

   // Longitudinal phase space kludge
   p.state[2] += DeltaT( p.state[5] );
 }

}

void sector::localPropagate( JetParticle& p ) {
 static int  i, j; 
        Jet  inState  [BMLN_dynDim];
 static Jet  outState;
 static Jet  zero;
 
 if( mapType )  p.setState( myMap( p.State() ) );
   
 else
 {
   for( i = 0; i < BMLN_dynDim; i++  ) 
     inState[i] = p.state(i);
   
   zero = 0.0;
   // ??? REMOVE zero.fixReferenceAtStart( p.state );

   for( i = 0; i < BMLN_dynDim; i++  ) {
     outState = zero;
     for( j = 0; j < BMLN_dynDim; j++  ) 
       outState = outState + mapMatrix[i][j]*inState[j];
     ( p.state ).SetComponent( i, outState );
   }

   // Longitudinal phase space kludge
   outState = inState[2] + JetDeltaT( p.state(5) );
   ( p.state ).SetComponent( 2, outState );
 }
}
