/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      LBSage.h
******  Version:   1.0
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
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

#ifndef LBSAGE_H
#define LBSAGE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <mxyzptlk/Mapping.h>
#include <basic_toolkit/MathConstants.h>
#include <physics_toolkit/Sage.h>


class LBSage : public Sage
{
public:
  struct Info : BarnacleData 
  {
    double arcLength;
    double beta_1x;
    double beta_1y;
    double beta_2x;
    double beta_2y;
    double alpha_1x;
    double alpha_1y;
    double alpha_2x;
    double alpha_2y;
    double u1;
    double u2;
    double u3;
    double u4;
    double nu_1;
    double nu_2;

    Info();
    Info( const Info& );
    ~Info(){}
  };


private:
  Info** _calcs;  // array of calculated results
  int    _n;      // size of the _calcs array

  void _deleteCalcs();
  void _createCalcs();


public:
  LBSage( const beamline*, bool = false );
  LBSage( const beamline&, bool = false );
  virtual ~LBSage();

  // vestigial: need to retain???
  int doCalc( const JetParticle*, beamline::Criterion& = beamline::yes ); 
      // PRECONDITION: The JetParticle must be on the closed
      //   orbit with the identity mapping for its state.
      //               Its Jet environment's reference point 
      //   should be the closed orbit. It is not reset.
      // POSTCONDITION: The JetParticle has the one-turn
      //   mapping for its state.
      // If default value is used for second argument, 
      //   information is attached to all elements.

  const LBSage::Info* getInfoPtr( int );

  void eraseAll();
};

#endif // LBSAGE_H
