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

using namespace std;

void monitor::localPropagate( Particle& p ) {
  int i;
  static double realLength;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    length *= getDriftFraction();
    
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = p.state[i];
    
    if ( _onOffSwitch ) {
      for ( i = 0; i < BMLN_dynDim; i++ ) {
        (*_outputStreamPtr) << _rgr[i] << "  ";
      }
      (*_outputStreamPtr) << "   BPM: " << ident << endl;
    }
    length = realLength - length;
    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length = realLength;
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

void monitor::localPropagate( JetParticle& p ) {
  int i;
  static double realLength;
    if( length > 0.0 ) {
      // ---------------------------------------------------------------------
      realLength = length;
      length *= getDriftFraction();
      
      bmlnElmnt::localPropagate( p );   // Drift through half the length.
      
      for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = ( p.state(i) ).standardPart();
      
      if ( _onOffSwitch ) {
        for ( i = 0; i < BMLN_dynDim; i++ ) {
          (*_outputStreamPtr) << _rgr[i] << "  ";
        }
        (*_outputStreamPtr) << "   BPM: " << ident << endl;
      }
      length = realLength - length;
      bmlnElmnt::localPropagate( p ); // Drift through the other half.
      
      length = realLength;
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



void hmonitor::localPropagate( Particle& p ) {
  int i;
  static double realLength;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    length *= getDriftFraction(); 
    
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = p.state[i];
    
    if ( _onOffSwitch ) {
      (*_outputStreamPtr) <<         _rgr[p.xIndex()] 
                          << "  " << _rgr[p.npxIndex()]
                          << "   HBPM: " << ident 
                          << endl;
    }
    length = realLength - length;
    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length = realLength;
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

void hmonitor::localPropagate( JetParticle& p ) {
  int i;
  static double realLength;
    if( length > 0.0 ) {
      // ---------------------------------------------------------------------
      realLength = length;
      length *= getDriftFraction();
      
      bmlnElmnt::localPropagate( p );   // Drift through half the length.
      
      for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = ( p.state(i) ).standardPart();
      
      if ( _onOffSwitch ) {
        (*_outputStreamPtr) <<         _rgr[p.xIndex()] 
                            << "  " << _rgr[p.npxIndex()]
                            << "   HBPM: " << ident 
                            << endl;
      }
      length = realLength - length;
      bmlnElmnt::localPropagate( p ); // Drift through the other half.
      
      length = realLength;
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


void vmonitor::localPropagate( Particle& p ) {
  int i;
  static double realLength;

  if( length > 0.0 ) {
    // ---------------------------------------------------------------------
    realLength = length;
    length *= getDriftFraction();
    
    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = p.state[i];
    
    if ( _onOffSwitch ) {
      (*_outputStreamPtr) <<         _rgr[p.yIndex()] 
                          << "  " << _rgr[p.npyIndex()]
                          << "   VBPM: " << ident 
                          << endl;
    }
    length = realLength - length;
    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length = realLength;
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

void vmonitor::localPropagate( JetParticle& p ) {
  int i;
  static double realLength;
    if( length > 0.0 ) {
      // ---------------------------------------------------------------------
      realLength = length;
      length *= getDriftFraction();
      
      bmlnElmnt::localPropagate( p );   // Drift through half the length.
      
      for ( i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = ( p.state(i) ).standardPart();
      
      if ( _onOffSwitch ) {
        (*_outputStreamPtr) <<         _rgr[p.yIndex()] 
                            << "  " << _rgr[p.npyIndex()]
                            << "   VBPM: " << ident 
                            << endl;
      }
      length = realLength - length;
      bmlnElmnt::localPropagate( p ); // Drift through the other half.
      
      length = realLength;
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
