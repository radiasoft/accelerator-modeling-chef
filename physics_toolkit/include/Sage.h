/*
 *  File: Sage.h
 *  Base class for all sages.
 *  
 *  Leo Michelotti
 *  November 23, 1998
 */

#ifndef SAGE_H
#define SAGE_H

#ifndef BEAMLINE_H
#include "beamline.h"
#endif

#ifndef MATHCONS_H
#include "MathConstants.h"     // to get bool
#endif

class Sage 
{
public:
  Sage( const beamline*, bool = false );
  // Second argument controls cloning.
  // If true, then the Sage will clone
  // its own copy of the beamline from
  // the first argument, completely
  // disconnecting it from the calling
  // routine. By default, it just stores
  // the pointer.
 ~Sage();
  
 void set_verbose();
 void unset_verbose();

 virtual void eraseAll() = 0;

 typedef char (*CRITFUNC)( bmlnElmnt* );

protected:
 beamline*           _myBeamlinePtr;
 beamline::arrayRep* _arrayPtr;
 char                _verbose;
 bool                _cloned;
 bmlnElmnt**         _element;

public:
 static char no ( bmlnElmnt* );
 static char yes( bmlnElmnt* );

};


inline void Sage::set_verbose()
{
  _verbose = 1;
}

inline void Sage::unset_verbose()
{
  _verbose = 0;
}


#endif // SAGE_H

