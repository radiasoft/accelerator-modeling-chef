/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      Sage.cc
******  Version:   1.1
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
 *  File: Sage.cc
 *  Base class for all sages.
 *  
 *  Leo Michelotti
 *  November 23, 1998
 */


#ifndef NO_RTTI
#include <typeinfo>
#endif

#include <basic_toolkit/GenericException.h>
#include <physics_toolkit/Sage.h>
#include <physics_toolkit/FramePusher.h>  // Used by Sage::isRing functions

double Sage::_defGapTol   = 0.005;  // = 5 mm
double Sage::_defAngleTol = 0.001;  // = 1 mrad

using namespace std;

Sage::Sage( const beamline* x, bool doClone )
: _verbose(false),
  _cloned(doClone),
  _isRing(Sage::isRing(x)),
  _errorStreamPtr( &std::cerr ),
  _outputStreamPtr( &std::cout ),
  _ringGapTolerance(_defGapTol),
  _ringAngleTolerance(_defAngleTol)
{
  // Preconditions: x is a valid pointer to a beamline
  // Requirements:
  // _arrayPtr must be deleted by destructor.

  if( 0 == x ) {
    throw( GenericException( __FILE__, __LINE__, 
           "Sage::Sage( const beamline* x, bool doClone )", 
           "Constructor invoked with null pointer." ) );
  }

#ifndef NO_RTTI
  if( typeid(*x) != typeid(beamline) ) {
    throw( GenericException( __FILE__, __LINE__, 
           "Sage::Sage( const beamline* x, bool doClone )", 
           "Constructor invoked with pointer to "
           "something other than a beamline." ) );
  }
#endif

  if( doClone ) {
    _myBeamlinePtr = (beamline*) (x->Clone());
  }
  else {
    _myBeamlinePtr = (beamline*) x;
  }

  if( beamline::unknown == _myBeamlinePtr->getLineMode() ) {
    if( _isRing ) {
      _myBeamlinePtr->setLineMode( beamline::ring );
    }
    else {
      _myBeamlinePtr->setLineMode( beamline::line );
    }
  }

  _arrayPtr = new beamline::arrayRep( _myBeamlinePtr, false );
}


Sage::Sage( const Sage& x )
: _verbose(false),
  _cloned(x._cloned),
  _isRing(x._isRing),
  _errorStreamPtr( &std::cerr ),
  _outputStreamPtr( &std::cout ),
  _ringGapTolerance(_defGapTol),
  _ringAngleTolerance(_defAngleTol)
{
  throw( GenericException( __FILE__, __LINE__, 
         "Sage::Sage( const Sage& )",
         "A Sage should never be copied." ) );
}


Sage::~Sage()
{
  delete _arrayPtr;
  if( _cloned ) { _myBeamlinePtr->eliminate(); }
}


bool Sage::no ( bmlnElmnt* )
{
  return false;
}


bool Sage::yes( bmlnElmnt* )
{
  return true;
}


bool Sage::isRing( const beamline& bml, double gap_tol, double angle_tol )
{
  // This test will return true for pathologically short lines
  //   like a beamline consisting of a single 1 mm drift.
  gap_tol = std::abs(gap_tol); 
  angle_tol = std::abs(angle_tol); 
  // Paranoia in case of excessively stupid user.

  FramePusher fp;
  bml.accept( fp );

  // Check the point of return
  Vector r(3);
  r = fp.getFrame().getOrigin();
  int i,j;
  for( i = 0; i < 3; i++ ) {
    if( gap_tol < std::abs(r(i)) ) { return false; }
  }

  // Check the angle of return
  //   Tolerance is hardwired to 1 milliradian
  MatrixD fv(3,3);
  fv = fp.getFrame().getAxes();
  for( i = 0; i < 3; i++ ) {
    for( j = 0; j < 3; j++ ) {
      if( i != j ) {
        if( angle_tol < std::abs(fv(i,j)) ) { return false; }
      }
    }
  }
  
  return true;
}


bool Sage::isRing( const beamline* bmlPtr, double gt, double at )
{
  return Sage::isRing( *bmlPtr, gt, at );
}


bool Sage::hasRing() const
{
  return Sage::isRing( _myBeamlinePtr );
}


