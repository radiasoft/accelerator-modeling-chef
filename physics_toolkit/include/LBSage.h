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

#ifndef LBSAGE_H
#define LBSAGE_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif
#ifndef MAP_HXX
#include "Mapping.h"
#endif
#ifndef MATHCONS_H
#include "MathConstants.h"
#endif
#ifndef SAGE_H
#include "Sage.h"
#endif

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
