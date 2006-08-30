/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.1
******                                    
******  File:      QuadEliminator.cc
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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

/*
 * File: QuadEliminator.cc
 * 
 * Source for class QuadEliminator
 * 
 * Concatenates quads.
 * 
 * Leo Michelotti
 * March 11, 2002
 * 
 */

#include <iostream>
#include <string>

#include <basic_toolkit/GenericException.h>
#include <physics_toolkit/QuadEliminator.h>
#include <beamline/quadrupole.h>
#include <beamline/BeamlineIterator.h>

using namespace std;

QuadEliminator::QuadEliminator()
: _quadPtr(0), _bmlPtr(0)
{
}


QuadEliminator::QuadEliminator( const QuadEliminator& )
{
  throw( GenericException( __FILE__, __LINE__, 
         "QuadEliminator::QuadEliminator( const QuadEliminator& )", 
         "Cannot copy QuadEliminator object." ) );
}


QuadEliminator::~QuadEliminator()
{
  if( 0 != _quadPtr )  { delete _quadPtr;      }
  if( 0 != _bmlPtr   ) { _bmlPtr->eliminate(); }
}


void QuadEliminator::visitBeamline( const beamline* x )
{
  // Quick check
  if( 0 != _bmlPtr ) {  // Paranoid check only
    throw( GenericException( __FILE__, __LINE__, 
           "void QuadEliminator::visitBeamline( const beamline* x )", 
           "Impossible! Horrible, inexplicable error has occurred!!" ) );
  }


  // Create the new beamline
  int n = strlen(x->Name()) + strlen("_QuadMerged") + 1;
  char* nuNam = new char [n];
  strcpy( nuNam, x->Name() );
  strcat( nuNam, "_QuadMerged" );
  if( strlen(nuNam) != n-1 ) {  // More paranoia
    throw( GenericException( __FILE__, __LINE__, 
           "void QuadEliminator::visitBeamline( const beamline* x )", 
           "Impossible! Horrible, inexplicable error has occurred!!" ) );
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


  // Append final quad, if it exists.
  if( 0 != _quadPtr ) {
    _bmlPtr->append( _quadPtr->Clone() );
    delete _quadPtr;
    _quadPtr = 0;
  }

}


void QuadEliminator::visitBmlnElmnt( const bmlnElmnt* x )
{
  if( 0 != _bmlPtr )  {  // Not paranoia!
    if( 0 != _quadPtr ) {
      _bmlPtr->append( _quadPtr->Clone() );
      delete _quadPtr;
      _quadPtr = 0;
    }
    _bmlPtr->append( x->Clone() ); 
  }
}


void QuadEliminator::visitQuadrupole( const quadrupole* x )
{
  if( 0 == _quadPtr ) {
    _quadPtr = new quadrupole( *x );
  }

  else if( ( _quadPtr->Alignment() == x->Alignment() ) &&
           ( _quadPtr->Strength()  == x->Strength()  )    ) {
    int nn = strlen(_quadPtr->Name()) + strlen(x->Name()) + 3;
    char newName[nn];
    newName[0] = '\0';
    strcat( newName, _quadPtr->Name() );
    strcat( newName, "+" );
    strcat( newName, x->Name() );

    _quadPtr->Rename( newName );
    _quadPtr->setLength( x->Length() + _quadPtr->Length() );
  }

  else {
     _bmlPtr->append( _quadPtr->Clone() );
     delete _quadPtr;
     // Note: the above two lines could be replaced by
     // _bmlPtr->append( _quadPtr );
     // I'm not sure why I used Clone() here.
     _quadPtr = new quadrupole( *x );
  }
}


beamline* QuadEliminator::beamlinePtr()
{
  return _bmlPtr;
  // Invoking routine is responsible for
  // using this beamline before the 
  // QuadEliminator goes out of scope.
}


beamline* QuadEliminator::clonedBeamlinePtr()
{
  return ((beamline*) _bmlPtr->Clone());
  // Invoking routine is responsible for
  // eliminating the cloned beamline.
}

