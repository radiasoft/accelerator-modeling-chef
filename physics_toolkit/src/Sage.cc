/*
 *  File: Sage.cc
 *  Base class for all sages.
 *  
 *  Leo Michelotti
 *  November 23, 1998
 */


#include "Sage.h"

Sage::Sage( const beamline* x )
: _verbose(0)
{
  _myBeamlinePtr = (beamline*) x;
}


Sage::~Sage()
{
}


char Sage::no ( bmlnElmnt* )
{
  return 0;
}


char Sage::yes( bmlnElmnt* )
{
  return 1;
}
