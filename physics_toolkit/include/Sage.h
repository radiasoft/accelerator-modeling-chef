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
  Sage( const beamline* );
 ~Sage();
  
 void set_verbose();
 void unset_verbose();

 virtual void eraseAll() = 0;

 typedef char (*CRITFUNC)( bmlnElmnt* );

protected:
 beamline*  _myBeamlinePtr;
 char       _verbose;

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

