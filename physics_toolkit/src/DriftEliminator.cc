/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      DriftEliminator.cc
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
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


/*
 * File: DriftEliminator.cc
 * 
 * Source for class DriftEliminator
 * 
 * Concatenates drifts and removes passive
 * elements of zero length.
 * 
 * Leo Michelotti
 * May 17, 2001
 * 
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <string>

#include "DriftEliminator.h"

#include "GenericException.h"
#include "drift.h"
#include "marker.h"
#include "monitor.h"
#include "Slot.h"

#include "BeamlineIterator.h"

using namespace std;

DriftEliminator::DriftEliminator()
: _driftPtr(0), _bmlPtr(0)
{
}


DriftEliminator::DriftEliminator( const DriftEliminator& )
{
  throw( GenericException( __FILE__, __LINE__, 
         "DriftEliminator::DriftEliminator( const DriftEliminator& )", 
         "Copying object not permitted." ) );
}


DriftEliminator::~DriftEliminator()
{
  if( 0 != _driftPtr ) { delete _driftPtr; }
  if( 0 != _bmlPtr   ) { _bmlPtr->eliminate(); }
}


void DriftEliminator::visitBeamline( const beamline* x )
{
  // Quick check
  if( 0 != _bmlPtr ) {  // Paranoid check only
    throw( GenericException( __FILE__, __LINE__, 
           "void DriftEliminator::visitBeamline( const beamline* x )", 
           "A horrible, inexplicable error has occurred!! Null _bmlPtr." ) );
  }


  // Create the new beamline
  int n = strlen(x->Name()) + strlen("_Condensed") + 1;
  char* nuNam = new char [n];
  strcpy( nuNam, x->Name() );
  strcat( nuNam, "_Condensed" );
  if( strlen(nuNam) != n-1 ) {  // More paranoia
    throw( GenericException( __FILE__, __LINE__, 
           "void DriftEliminator::visitBeamline( const beamline* x )", 
           "An inexplicable, horrible error has occurred!!" ) );
  }

  _bmlPtr = new beamline( nuNam );
  _bmlPtr->setEnergy( x->Energy() );

  delete [] nuNam;


  // Process the argument
  DeepBeamlineIterator dbi( x );
  bmlnElmnt* q;
  while( 0 != ( q = dbi++ ) ) {
    q->accept( *this );
  }


  // Append final drift, if it exists.
  if( 0 != _driftPtr ) {
    if( 0.0 < _driftPtr->Length() ) {
      _bmlPtr->append( _driftPtr->Clone() );
    }
    delete _driftPtr;
    _driftPtr = 0;
  }

}


void DriftEliminator::visitBmlnElmnt( const bmlnElmnt* x )
{
  if( 0 != _bmlPtr ) {  // Not paranoia!
    if( 0.0 != x->Strength() ) {
      if( 0 != _driftPtr ) {
        if( 0.0 < _driftPtr->Length() ) {
          _bmlPtr->append( _driftPtr->Clone() );
	}
        delete _driftPtr;
        _driftPtr = 0;
      }
      _bmlPtr->append( x->Clone() ); 
    }
    else {
      _handlePassiveElement( x );
    }
  }
}


void DriftEliminator::visitSlot( const Slot* x )
{
  if( 0 != _bmlPtr ) {  // Not paranoia!
    if( 0 != _driftPtr ) {
      if( 0.0 < _driftPtr->Length() ) {
        _bmlPtr->append( _driftPtr->Clone() );
      }
      delete _driftPtr;
      _driftPtr = 0;
    }
    _bmlPtr->append( x->Clone() ); 
  }
}


void DriftEliminator::_handlePassiveElement( const bmlnElmnt* x )
{
  if( 0 == _driftPtr ) {
    _driftPtr = new drift( (char*) "concat_drift", x->Length() );
  }
  else {
    _driftPtr->setLength( x->Length() + _driftPtr->Length()  );
  }
}


void DriftEliminator::visitMonitor( const monitor* x )
{
  if( 0.0 == x->Strength() ) {
    _handlePassiveElement(x);
  } 
  else {
    this->visitBmlnElmnt( (const bmlnElmnt*) x );
  }
}

void DriftEliminator::visitMarker( const marker* x )
{
  // Do nothing
}

void DriftEliminator::visitDrift( const drift* x )
{
  _handlePassiveElement(x);
}


beamline* DriftEliminator::beamlinePtr()
{
  return _bmlPtr;
  // Invoking routine is responsible for
  // using this beamline before the 
  // DriftEliminator goes out of scope.
}


beamline* DriftEliminator::clonedBeamlinePtr()
{
  return ((beamline*) _bmlPtr->Clone());
  // Invoking routine is responsible for
  // eliminating the cloned beamline.
}

