/*
 *  File: Sage.cc
 *  Base class for all sages.
 *  
 *  Leo Michelotti
 *  November 23, 1998
 */


#include <typeinfo>
#include "Sage.h"

Sage::Sage( const beamline* x, bool doClone )
: _verbose(0)
{
  // Preconditions: x is a valid pointer to a beamline
  // Requirements:
  // _arrayPtr must be deleted by destructor.

  if( 0 == x ) {
    cerr << "*** ERROR ***                             \n"
            "*** ERROR *** Sage::Sage                  \n"
            "*** ERROR *** Constructor invoked with    \n"
            "*** ERROR *** null pointer.               \n"
            "*** ERROR ***                             \n"
         << endl;
    exit(1);
  }

  if( typeid(*x) != typeid(beamline) ) {
    cerr << "*** ERROR ***                             \n"
            "*** ERROR *** Sage::Sage                  \n"
            "*** ERROR *** Constructor invoked with    \n"
            "*** ERROR *** pointer to something other  \n"
            "*** ERROR *** than a beamline.            \n"
            "*** ERROR ***                             \n"
         << endl;
    exit(1);
  }


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
