/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      monitorPhysics.cc
******  Version:   3.0
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <Particle.h>
#include <monitor.h>

using namespace std;

void monitor::localPropagate( Particle& p )
{
  int i;
  double realLength, realCt;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    realCt     = _ctRef;
    length *= getDriftFraction();
    _ctRef *= getDriftFraction();
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) { _rgr[i] = p.state[i]; }
    
    if ( _onOffSwitch ) {
      for ( i = 0; i < BMLN_dynDim; i++ ) {
        (*_outputStreamPtr) << _rgr[i] << "  ";
      }
      (*_outputStreamPtr) << "   BPM: " << ident << endl;
    }

    length = realLength - length;
    _ctRef = realCt - _ctRef;
    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length = realLength;
    _ctRef = realCt;
  } // ---------------------------------------------------------------------

  else {
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = p.state[i];
    
    if ( _onOffSwitch ) {
      for ( i = 0; i < BMLN_dynDim; i++ ) {
        (*_outputStreamPtr) << _rgr[i] << "  ";
      }
      (*_outputStreamPtr) << "   BPM: " << ident << endl;
    }
  }
}


void monitor::localPropagate( JetParticle& p ) 
{
  int i;
  double realLength, realCt;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    realCt     = _ctRef;
    length *= getDriftFraction();
    _ctRef *= getDriftFraction();
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = ( p.state(i) ).standardPart();
    
    if ( _onOffSwitch ) {
      for ( i = 0; i < BMLN_dynDim; i++ ) {
        (*_outputStreamPtr) << _rgr[i] << "  ";
      }
      (*_outputStreamPtr) << "   BPM: " << ident << endl;
    }

    length = realLength - length;
    _ctRef = realCt - _ctRef;
    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length = realLength;
    _ctRef = realCt;
  } // ---------------------------------------------------------------------

  else {
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = ( p.state(i) ).standardPart();
    
    if ( _onOffSwitch ) {
      for ( i = 0; i < BMLN_dynDim; i++ ) {
        (*_outputStreamPtr) << _rgr[i] << "  ";
      }
      (*_outputStreamPtr) << "   BPM: " << ident << endl;
    }
  }
}


void hmonitor::localPropagate( Particle& p ) 
{
  int i;
  double realLength, realCt;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    realCt     = _ctRef;
    length *= getDriftFraction(); 
    _ctRef *= getDriftFraction();
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = p.state[i];
    
    if ( _onOffSwitch ) {
      (*_outputStreamPtr) <<         _rgr[p.xIndex()] 
                          << "  " << _rgr[p.npxIndex()]
                          << "   HBPM: " << ident 
                          << endl;
    }

    length = realLength - length;
    _ctRef = realCt - _ctRef;
    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length = realLength;
    _ctRef = realCt;
  } // ---------------------------------------------------------------------

  else {
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = p.state[i];
    
    if ( _onOffSwitch ) {
      (*_outputStreamPtr) <<         _rgr[p.xIndex()] 
                          << "  " << _rgr[p.npxIndex()]
                          << "   HBPM: " << ident 
                          << endl;
    }
  }
}


void hmonitor::localPropagate( JetParticle& p ) 
{
  int i;
  double realLength, realCt;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    realCt     = _ctRef;
    length *= getDriftFraction();
    _ctRef *= getDriftFraction();
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = ( p.state(i) ).standardPart();
    
    if ( _onOffSwitch ) {
      (*_outputStreamPtr) <<         _rgr[p.xIndex()] 
                          << "  " << _rgr[p.npxIndex()]
                          << "   HBPM: " << ident 
                          << endl;
    }

    length = realLength - length;
    _ctRef = realCt - _ctRef;
    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length = realLength;
    _ctRef = realCt;
  } // ---------------------------------------------------------------------

  else {
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = ( p.state(i) ).standardPart();
    
    if ( _onOffSwitch ) {
      (*_outputStreamPtr) <<         _rgr[p.xIndex()] 
                          << "  " << _rgr[p.npxIndex()]
                          << "   HBPM: " << ident 
                          << endl;
    }
  }
}


void vmonitor::localPropagate( Particle& p ) 
{
  int i;
  double realLength, realCt;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    realCt     = _ctRef;
    length *= getDriftFraction();
    _ctRef *= getDriftFraction();
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = p.state[i];
    
    if ( _onOffSwitch ) {
      (*_outputStreamPtr) <<         _rgr[p.yIndex()] 
                          << "  " << _rgr[p.npyIndex()]
                          << "   VBPM: " << ident 
                          << endl;
    }

    length = realLength - length;
    _ctRef = realCt - _ctRef;
    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length = realLength;
    _ctRef = realCt;
  } // ---------------------------------------------------------------------

  else {
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = p.state[i];
    
    if ( _onOffSwitch ) {
      (*_outputStreamPtr) <<         _rgr[p.yIndex()] 
                          << "  " << _rgr[p.npyIndex()]
                          << "   VBPM: " << ident 
                          << endl;
    }
  }
}


void vmonitor::localPropagate( JetParticle& p ) 
{
  int i;
  double realLength, realCt;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    realCt     = _ctRef;
    length *= getDriftFraction();
    _ctRef *= getDriftFraction();
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = ( p.state(i) ).standardPart();
    
    if ( _onOffSwitch ) {
      (*_outputStreamPtr) <<         _rgr[p.yIndex()] 
                          << "  " << _rgr[p.npyIndex()]
                          << "   VBPM: " << ident 
                          << endl;
    }

    length = realLength - length;
    _ctRef = realCt - _ctRef;
    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length = realLength;
    _ctRef = realCt;
  } // ---------------------------------------------------------------------

  else {
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = ( p.state(i) ).standardPart();
    
    if ( _onOffSwitch ) {
      (*_outputStreamPtr) <<         _rgr[p.yIndex()] 
                          << "  " << _rgr[p.npyIndex()]
                          << "   VBPM: " << ident 
                          << endl;
    }
  }
}
