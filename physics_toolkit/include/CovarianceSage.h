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


#ifndef COVARIANCESAGE_H
#define COVARIANCESAGE_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif
#ifndef MAP_HXX
#include "Mapping"
#endif
#ifndef MATHCONS_H
#include "MathConstants.h"
#endif
#ifndef SAGE_H
#include "Sage.h"
#endif

class CovarianceSage : public Sage
{
public:
  struct Info : BarnacleData 
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
    Info( const Info& );
    ~Info(){}
  };


public:
  CovarianceSage( const beamline*, bool = false );
  CovarianceSage( const beamline&, bool = false );
  ~CovarianceSage();

  int doCalc( JetParticle*, MatrixD, beamline::Criterion& = beamline::yes ); 

  const CovarianceSage::Info* getInfoPtr( int );

  void eraseAll();

  // Error codes returned by functions
  static const int CovarianceSage::OKAY;
  static const int CovarianceSage::NOTSQR;
  static const int CovarianceSage::OVRRUN;
  static const int CovarianceSage::NEGDET;

private:
  Info** _calcs;  // array of calculated results
  int    _n;      // size of the _calcs array

  void _deleteCalcs();
};

#endif // COVARIANCESAGE_H
