#include "beamline.inc"

void vmonitor::localPropagate( Particle& p ) {
  int i;
  static double realLength;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    length *= getDriftFraction();
    
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) rgr[i] = p.state[i];
    
    if ( onOff ) {
      fprintf( outputFile, "VBPM: %s ..........\n", ident );
      for ( i = 0; i < BMLN_dynDim; i++ ) 
	fprintf( outputFile, "%10.6lf  ", rgr[i] );
      fprintf( outputFile, "\n" );
    }
    length = realLength - length;
    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length = realLength;
  } // ---------------------------------------------------------------------
  else {
    for ( i = 0; i < BMLN_dynDim; i++ ) rgr[i] = p.state[i];
    
    if ( onOff ) {
      fprintf( outputFile, "VBPM: %s ..........\n", ident );
      for ( i = 0; i < BMLN_dynDim; i++ ) 
	fprintf( outputFile, "%10.6lf  ", rgr[i] );
      fprintf( outputFile, "\n" );
    }
  }
}

void vmonitor::localPropagate( JetParticle& p ) {
  int i;
  static double realLength;
    if( length > 0.0 ) {
      // ---------------------------------------------------------------------
      realLength = length;
      length *= getDriftFraction();
      
      bmlnElmnt::localPropagate( p );   // Drift through half the length.
      
      for ( i = 0; i < BMLN_dynDim; i++ ) rgr[i] = ( p.state(i) ).standardPart();
      
      if ( onOff ) {
	fprintf( outputFile, "VBPM: %s ..........\n", ident );
	for ( i = 0; i < BMLN_dynDim; i++ ) 
	  fprintf( outputFile, "%10.6lf  ", rgr[i] );
	fprintf( outputFile, "\n" );
      }
      length = realLength - length;
      bmlnElmnt::localPropagate( p ); // Drift through the other half.
      
      length = realLength;
    } // ---------------------------------------------------------------------
    else {
      for ( i = 0; i < BMLN_dynDim; i++ ) rgr[i] = ( p.state(i) ).standardPart();
      
      if ( onOff ) {
	fprintf( outputFile, "VBPM: %s ..........\n", ident );
	for ( i = 0; i < BMLN_dynDim; i++ ) 
	  fprintf( outputFile, "%10.6lf  ", rgr[i] );
	fprintf( outputFile, "\n" );
      }
    }
}
