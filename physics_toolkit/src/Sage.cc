/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      Sage.cc
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

#include "GenericException.h"
#include "Sage.h"

using namespace std;

Sage::Sage( const beamline* x, bool doClone )
: _verbose(0)
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
  _cloned = doClone;

  _arrayPtr = new beamline::arrayRep( _myBeamlinePtr, false );
}


Sage::~Sage()
{
  delete _arrayPtr;
  if( _cloned ) { _myBeamlinePtr->eliminate(); }
}


char Sage::no ( bmlnElmnt* )
{
  return 0;
}


char Sage::yes( bmlnElmnt* )
{
  return 1;
}
