#include "beamline.inc"

void hmonitor::propagate( Particle& p ) {
 for ( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = p.state[i];
 
 if ( onOff ) {
   fprintf( outputFile, "HBPM: %s ......... ", ident );
   fprintf( outputFile, "%10.6lf   %10.6lf\n", p.state[0], p.state[3] );
 }
}

void hmonitor::propagate( JetParticle& p ) {
 for ( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = ( p.state(i) ).standardPart();
 
 if ( onOff ) {
   fprintf( outputFile, "HBPM: %s ......... ", ident );
   fprintf( outputFile, "%10.6lf   %10.6lf\n", ( p.state(0) ).standardPart(), 
				       ( p.state(3) ).standardPart() );
 }
}
