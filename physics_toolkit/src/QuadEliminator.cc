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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


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

#include "QuadEliminator.h"
#include "quadrupole.h"
#include "BeamlineIterator.h"

using namespace std;

QuadEliminator::QuadEliminator()
: _quadPtr(0), _bmlPtr(0)
{
}


QuadEliminator::QuadEliminator( const QuadEliminator& )
{
  cerr << "*** ERROR ***                                 \n"
          "*** ERROR *** Never, never, never copy an     \n"
          "*** ERROR *** QuadEliminator object!          \n"
          "*** ERROR ***                                 \n"
       << endl;
  exit(1);
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
    cerr << "\n*** ERROR ***                                 "
            "\n*** ERROR *** File: " << __FILE__ << "  Line: " << __LINE__ 
         << "\n*** ERROR ***  QuadEliminator::visitBeamline  "
            "\n*** ERROR ***  A horrible, inexplicable       "
            "\n*** ERROR ***  error has occurred!!           "
            "\n*** ERROR ***                                 "
         << endl;
    exit(-1);
  }


  // Create the new beamline
  int n = strlen(x->Name()) + strlen("_QuadMerged") + 1;
  char* nuNam = new char [n];
  strcpy( nuNam, x->Name() );
  strcat( nuNam, "_QuadMerged" );
  if( strlen(nuNam) != n-1 ) {  // More paranoia
    cerr << "*** ERROR ***                             \n"
            "*** ERROR ***  QuadEliminator::visitBeamline  \n"
            "*** ERROR ***  An inexplicable, horrible  \n"
            "*** ERROR ***  error has occurred!!       \n"
            "*** ERROR ***                             \n"
         << endl;
    exit(-1);
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

