/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      FPSolver.h
******  Version:   1.5
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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - Pass particles by reference 
******                                                                
**************************************************************************
*************************************************************************/


#ifndef FPSOLVER_H
#define FPSOLVER_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlPtr.h>

typedef bool (*FP_CRITFUNC)( ConstElmPtr );

struct FPinfo {
  FPinfo( double const&, Vector const& );
  double arclength;
  Vector state;
};


class FPSolver {

 public:

  FPSolver( BmlPtr bml = BmlPtr(), int ndim=6);
 ~FPSolver();
  
  double& JumpScale( int ); 
  double& ZeroScale( int );

  int  operator()( Particle&,    const char*, FP_CRITFUNC = 0 );
  int  operator()( JetParticle&, const char*, FP_CRITFUNC = 0 );   
      // PRECONDITIONS:  1st argument: address of Particle whose
      //   closed orbit is to be found. Phase space dimensions
      //   of Particle must match that of the FPSolver object.
      //   In fact, it had better be 6.
      //                 2nd argument: value is ignored, but
      //   its presence means that the transverse (four-dimensional)
      //   closed orbit is to be calculated. 
      //                 3rd argument: closed orbit Barnacles will
      //   be attached to all elements that match the criterion.
      //   Default: attach to no elements.
      // POSTCONDITIONS: 1st argument: [Jet}Particle's state is on
      //   the closed orbit.
      //                 Return value 0: normal exit
      //                              1: Newton iterates stopped
      //                                 after 200 iterations
      // 
      //                 NOTE: the Jet::Environment is not reset
      //   to the closed orbit. Thus all derivatives are assumed
      //   evaluated at the initial reference point, even though
      //   the JetParticle is "on" the closed orbit.
  int  operator()( Particle&, FP_CRITFUNC = 0 );     // FixedPoint
  void operator()( JetParticle&, FP_CRITFUNC = 0 );  // FixedPoint
  // REMOVE: void operator()( double* );                      // fixedPoint
  // If default value is used for FP_CRITFUNC argument, then no 
  // fixed point information is attached to the elements.

 void eraseAll();

 private:
  int       dimension_;
  double*   jumpScale_;
  double*   zeroScale_;
  int*      l_;
  BmlPtr    bmLine_;   // Not owned; not deleted.



};

#endif // FPSOLVER_H
