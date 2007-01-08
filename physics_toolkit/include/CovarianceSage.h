/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      CovarianceSage.h
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


#ifndef COVARIANCESAGE_H
#define COVARIANCESAGE_H


#include <beamline/bmlnElmnt.h>
#include <mxyzptlk/Mapping.h>
#include <basic_toolkit/MathConstants.h>
#include <physics_toolkit/Sage.h>
#include <vector>


class CovarianceSage : public Sage {

public:
  struct Info
  {
    double arcLength;
    MatrixD covariance;
    struct {
     double hor;
     double ver;
    } beta;
    struct {
     double hor;
     double ver;
    } alpha;

    Info();
    Info( Info const& );
    Info& operator=( Info const& );
  };


public:
  CovarianceSage( beamline const*, bool clonebml = false );
  CovarianceSage( beamline const&, bool clonebml = false );

  int doCalc( JetParticle&, MatrixD, beamline::Criterion& = beamline::yes ); 

  std::vector<CovarianceSage::Info> const& getCovarianceArray();

  void eraseAll();

  // Error codes returned by functions

  static const int OKAY;
  static const int NOTSQR;
  static const int OVRRUN;
  static const int NEGDET;

private:

  CovarianceSage( CovarianceSage const& );

  std::vector<Info> calcs_;  // array of calculated results
  void  deleteCalcs();
};

#endif // COVARIANCESAGE_H
