/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      monitorPhysics.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include "monitor.h"

void monitor::localPropagate( Particle& p ) {
  int i;
  static double realLength;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    length *= getDriftFraction();
    
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) rgr[i] = p.state[i];
    
    if ( onOff ) {
      fprintf( outputFile, "BPM: %s ..........\n", ident );
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
      fprintf( outputFile, "BPM: %s ..........\n", ident );
      for ( i = 0; i < BMLN_dynDim; i++ ) 
	fprintf( outputFile, "%10.6lf  ", rgr[i] );
      fprintf( outputFile, "\n" );
    }
  }
}

void monitor::localPropagate( JetParticle& p ) {
  int i;
  static double realLength;
    if( length > 0.0 ) {
      // ---------------------------------------------------------------------
      realLength = length;
      length *= getDriftFraction();
      
      bmlnElmnt::localPropagate( p );   // Drift through half the length.
      
      for ( i = 0; i < BMLN_dynDim; i++ ) rgr[i] = ( p.state(i) ).standardPart();
      
      if ( onOff ) {
	fprintf( outputFile, "BPM: %s ..........\n", ident );
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
	fprintf( outputFile, "BPM: %s ..........\n", ident );
	for ( i = 0; i < BMLN_dynDim; i++ ) 
	  fprintf( outputFile, "%10.6lf  ", rgr[i] );
	fprintf( outputFile, "\n" );
      }
    }
}



void hmonitor::localPropagate( Particle& p ) {
  int i;
  static double realLength;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    length *= getDriftFraction(); 
    
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) rgr[i] = p.state[i];
    
    if ( onOff ) {
      fprintf( outputFile, "HBPM: %s ..........\n", ident );
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
      fprintf( outputFile, "HBPM: %s ..........\n", ident );
      for ( i = 0; i < BMLN_dynDim; i++ ) 
	fprintf( outputFile, "%10.6lf  ", rgr[i] );
      fprintf( outputFile, "\n" );
    }
  }
}

void hmonitor::localPropagate( JetParticle& p ) {
  int i;
  static double realLength;
    if( length > 0.0 ) {
      // ---------------------------------------------------------------------
      realLength = length;
      length *= getDriftFraction();
      
      bmlnElmnt::localPropagate( p );   // Drift through half the length.
      
      for ( i = 0; i < BMLN_dynDim; i++ ) rgr[i] = ( p.state(i) ).standardPart();
      
      if ( onOff ) {
	fprintf( outputFile, "HBPM: %s ..........\n", ident );
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
	fprintf( outputFile, "HBPM: %s ..........\n", ident );
	for ( i = 0; i < BMLN_dynDim; i++ ) 
	  fprintf( outputFile, "%10.6lf  ", rgr[i] );
	fprintf( outputFile, "\n" );
      }
    }
}


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
