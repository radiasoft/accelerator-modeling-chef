/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      Sage.h
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

