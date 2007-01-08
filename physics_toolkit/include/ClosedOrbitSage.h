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
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
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


#include <physics_toolkit/Sage.h>
#include <beamline/beamline.h>


class ClosedOrbitSage : public Sage {

 public:

  ClosedOrbitSage( const beamline*, bool clonebml = false );
  
  int findClosedOrbit( JetParticle& );

  void eraseAll();
  void setForcedCalc();
  void unsetForcedCalc();
  void setIgnoreErrors( bool );

  static int ERR_NOTRING;

 private:

  ClosedOrbitSage( ClosedOrbitSage const&);

  int  invokeFPSolver( JetParticle& );

  bool  forced_;
  bool  ignoreErrors_;

};


#endif // CLORB_SAGE_H
