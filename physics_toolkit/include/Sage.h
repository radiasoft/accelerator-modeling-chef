/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      Sage.h
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


/*
 *  File: Sage.h
 *  Base class for all sages.
 *  
 *  November 23, 1998
 *  Original code
 *  Leo Michelotti
 * 
 *  April 30, 2004
 *  Writing to user-specified error and output streams.
 *  Leo Michelotti
 */

#ifndef SAGE_H
#define SAGE_H

#ifndef BEAMLINE_H
#include "beamline.h"
#endif

class Sage 
{
public:
  Sage( const beamline*, bool = false );
  // Second argument controls cloning.
  // If true, then the Sage will clone
  //   its own copy of the beamline from
  //   the first argument, completely
  //   disconnecting it from the calling
  //   routine. 
  // By default, it just stores the pointer.
  Sage( const Sage& );
 ~Sage();
  
 void set_verbose();
 void unset_verbose();

 virtual void eraseAll() = 0;

 typedef bool (*CRITFUNC)( bmlnElmnt* );

 static bool no ( bmlnElmnt* );
 static bool yes( bmlnElmnt* );

 void setErrorStream( ostream* );
 void setOutputStream( ostream* );

protected:
 beamline*           _myBeamlinePtr;
 beamline::arrayRep* _arrayPtr;
 bool                _verbose;
 bool                _cloned;
 ostream*            _errorStreamPtr;
 ostream*            _outputStreamPtr;
};


inline void Sage::set_verbose()
{
  _verbose = true;
}

inline void Sage::unset_verbose()
{
  _verbose = false;
}

inline void Sage::setErrorStream( ostream* x )
{
  _errorStreamPtr = x;
}

inline void Sage::setOutputStream( ostream* x )
{
  _outputStreamPtr = x;
}


#endif // SAGE_H

