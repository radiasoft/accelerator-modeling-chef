/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      ClosedOrbitSage.h
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
 *  File: ClosedOrbitSage.h
 *  The "Sage" for using FPSolver.
 *  
 *  Leo Michelotti
 *  November 23, 1998
 */

#ifndef CLORB_SAGE_H
#define CLORB_SAGE_H

#ifndef SAGE_H
#include "Sage.h"
#endif
#ifndef BEAMLINE_H
#include "beamline.h"
#endif

class ClosedOrbitSage : public Sage
{
 public:
  ClosedOrbitSage( const beamline*, bool = false );
  // Second argument is used by class Sage
  // to control cloning. (See Sage.h)
  ~ClosedOrbitSage();
  
  int findClosedOrbit( JetParticle* );

  void eraseAll();
  void setForcedCalc();
  void unsetForcedCalc();
  void setIgnoreErrors( bool );

 private:
  bool _forced;
  bool _ignoreErrors;

  int  _invokeFPSolver( JetParticle* );
  
};


inline void ClosedOrbitSage::setForcedCalc()
{
  _forced = true;
}

inline void ClosedOrbitSage::unsetForcedCalc()
{
  _forced = false;
}

inline void ClosedOrbitSage::setIgnoreErrors( bool x )
{
  _ignoreErrors = x;
}


#endif // CLORB_SAGE_H
