#include "beamline.inc"

void vmonitor::localPropagate( Particle& p ) {
 for ( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = p.state[i];
 
 if ( onOff ) {
   fprintf( outputFile, "VBPM: %s ......... ", ident );
   fprintf( outputFile, "%10.6lf   %10.6lf\n", p.state[1], p.state[4] );
 }
}

void vmonitor::localPropagate( JetParticle& p ) {
 for ( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = ( p.state(i) ).standardPart();
 
 if ( onOff ) {
   fprintf( outputFile, "VBPM: %s ......... ", ident );
   fprintf( outputFile, "%10.6lf   %10.6lf\n", ( p.state(1) ).standardPart(), 
				       ( p.state(4) ).standardPart() );
 }
}
