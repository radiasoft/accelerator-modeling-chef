#include "beamline.inc"

void monitor::propagate( Particle& p ) {
 int i;
 
 for ( i = 0; i < BMLN_dynDim; i++ ) rgr[i] = p.state[i];
 
 if ( onOff ) {
   fprintf( outputFile, "BPM: %s ..........\n", ident );
   for ( i = 0; i < BMLN_dynDim; i++ ) 
     fprintf( outputFile, "%10.6lf  ", rgr[i] );
   fprintf( outputFile, "\n" );
 }
}

void monitor::propagate( JetParticle& p ) {
 int i;

 for ( i = 0; i < BMLN_dynDim; i++ ) rgr[i] = ( p.state(i) ).standardPart();
 
 if ( onOff ) {
   fprintf( outputFile, "BPM: %s ..........\n", ident );
   for ( i = 0; i < BMLN_dynDim; i++ ) 
     fprintf( outputFile, "%10.6lf  ", rgr[i] );
   fprintf( outputFile, "\n" );
 }
}
