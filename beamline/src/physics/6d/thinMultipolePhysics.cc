#include "beamline.inc"

void thinMultipole::localPropagate( Particle& p ) {
 static char firstCall = 1;
 if( firstCall ) {
  printf( "\n" );
  printf( "*** SORRY ***                               \n" );
  printf( "*** SORRY *** thinMultipole::localPropagate      \n" );
  printf( "*** SORRY *** Thin multipoles are being     \n" );
  printf( "*** SORRY *** treated as drifts.            \n" );
  printf( "*** SORRY ***                               \n" );
  firstCall = 0;
 }
 bmlnElmnt::localPropagate( p );
}

void thinMultipole::localPropagate( JetParticle& p ) {
 static char firstCall = 1;
 if( firstCall ) {
  printf( "\n" );
  printf( "*** SORRY ***                               \n" );
  printf( "*** SORRY *** thinMultipole::localPropagate      \n" );
  printf( "*** SORRY *** Thin multipoles are being     \n" );
  printf( "*** SORRY *** treated as drifts.            \n" );
  printf( "*** SORRY ***                               \n" );
  firstCall = 0;
 }
 bmlnElmnt::localPropagate( p );
}
