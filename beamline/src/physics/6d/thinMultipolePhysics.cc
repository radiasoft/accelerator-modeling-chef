#include "beamline.inc"

void thinMultipole::propagate( Particle& p ) {
 static char firstCall = 1;
 if( firstCall ) {
  printf( "\n" );
  printf( "*** SORRY ***                               \n" );
  printf( "*** SORRY *** thinMultipole::propagate      \n" );
  printf( "*** SORRY *** Thin multipoles are being     \n" );
  printf( "*** SORRY *** treated as drifts.            \n" );
  printf( "*** SORRY ***                               \n" );
  firstCall = 0;
 }
 bmlnElmnt::propagate( p );
}

void thinMultipole::propagate( JetParticle& p ) {
 static char firstCall = 1;
 if( firstCall ) {
  printf( "\n" );
  printf( "*** SORRY ***                               \n" );
  printf( "*** SORRY *** thinMultipole::propagate      \n" );
  printf( "*** SORRY *** Thin multipoles are being     \n" );
  printf( "*** SORRY *** treated as drifts.            \n" );
  printf( "*** SORRY ***                               \n" );
  firstCall = 0;
 }
 bmlnElmnt::propagate( p );
}
