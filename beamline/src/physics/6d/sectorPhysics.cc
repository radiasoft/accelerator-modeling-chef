#if HAVE_CONFIG_H
#include <config.h>
#endif
#if HAVE_CONFIG_H
#include <config.h>
#endif
#include "sector.h"

// this is the actual ***definition** of sector::defaultPropagate

sector::JET_Prop sector::defaultPropagate;

int sector::JET_Prop::operator()( bmlnElmnt* b, Particle& p ) {

 static int    i, j; 
 static double inState[ BMLN_dynDim ];
 static double dummyState[ BMLN_dynDim ];
 
 sector* s = dynamic_cast<sector*>(b);
 
 for( i = 0; i < BMLN_dynDim; i++  ) {
   dummyState[i] = inState[i] = p.State(i);
   inState[i] = p.State(i);
 }
   
 if( s->mapType ) {

   p.set_x  ( s->myMap( p.xIndex()   )( inState ) );
   p.set_y  ( s->myMap( p.yIndex()   )( inState ) );
   p.set_cdt( s->myMap( p.cdtIndex() )( inState ) );
   p.set_npx( s->myMap( p.npxIndex() )( inState ) );
   p.set_npy( s->myMap( p.npyIndex() )( inState ) );
   p.set_ndp( s->myMap( p.ndpIndex() )( inState ) );

   // for ( j = 0; j < BMLN_dynDim; j++ ) {
   //   dummyState[j] = s->myMap(j)( inState );
   // }
   // p.setState( dummyState );
 } 
 else
 {
   for( i = 0; i < BMLN_dynDim; i++  ) {
     dummyState[i] = 0.0;
     for( j = 0; j < BMLN_dynDim; j++  ) 
       dummyState[i] += s->mapMatrix[i][j]*inState[j];
   }
   p.set_x  ( dummyState[ p.xIndex() ]   );
   p.set_y  ( dummyState[ p.yIndex() ]   );
   p.set_cdt( dummyState[ p.cdtIndex() ] );
   p.set_npx( dummyState[ p.npxIndex() ] );
   p.set_npy( dummyState[ p.npyIndex()] );
   p.set_ndp( dummyState[ p.ndpIndex()] );
   

   // for( i = 0; i < BMLN_dynDim; i++  ) {
   //   p.state[i] = 0.0;
   //   for( j = 0; j < BMLN_dynDim; j++  ) 
   //     p.state[i] += s->mapMatrix[i][j]*inState[j];
   // }

   // Longitudinal phase space kludge
   // in case there is a cavity within a sector ?
   // p.state[2] += DeltaT( p.state[5] );
 }

 return 0;
}


int sector::JET_Prop::operator()( bmlnElmnt* b, JetParticle& p ) {
 static int  i, j; 
        Jet  inState  [BMLN_dynDim];
 static Jet  outState;
 static Jet  zero;

 sector* s = dynamic_cast<sector*>(b);

 if( s->mapType )  p.setState( s->myMap( p.State() ) );
   
 else
 {
   // for( i = 0; i < BMLN_dynDim; i++  ) 
   //   inState[i] = p.state(i);
   for( i = 0; i < BMLN_dynDim; i++  ) 
     inState[i] = p.State(i);
   
   zero = 0.0;
   // ??? REMOVE zero.fixReferenceAtStart( p.state );

   for( i = 0; i < BMLN_dynDim; i++  ) {
     outState = zero;
     for( j = 0; j < BMLN_dynDim; j++  ) {
       outState = outState + s->mapMatrix[i][j]*inState[j];
     }
     // ( p.state ).SetComponent( i, outState );
     p.State(i) = outState;
   }

   // Longitudinal phase space kludge
   // outState = inState[2] + JetDeltaT( p.state(5) );
   //( p.state ).SetComponent( 2, outState );
 }
 return 0;
}
