/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   2.0
******                                    
******  File:      BmlUtil.h
******                                                                
******  Copyright (c) 2003  Universities Research Association, Inc.   
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
 *  File: BmlUtil.h
 *  Header file for collecting Beamline
 *  utilities as methods of a single class.
 *  
 *  Initial version.
 *  October 28, 2003
 *  - Leo Michelotti
 * 
 *  Added makeCovariance functions.
 *  October 29, 2004
 *  - Leo Michelotti
 */


// Forward declarations
class MatrixC;

#ifndef COVARIANCESAGE_H
#include "CovarianceSage.h"
#endif

class BmlUtil
{
 public: 
  static void normalize( MatrixC& E );
  // On input: the columns of E contain eigenvectors of
  //   a one-turn matrix. 
  // On output: each column has been normalized
  //   for physical interpretation.

  static int makeCovariance( CovarianceSage::Info& info,  
                             const Particle&, 
                             double = 40.0, double = 40.0, double = 40.0 );
  static int makeCovariance( CovarianceSage::Info*,
                             const Particle&, 
                             double = 40.0, double = 40.0, double = 40.0 );
  // The preceding functions use the "alpha" and "beta" information
  //   in the argument, info, to construct what would be an uncoupled 
  //   equilibrium covariance matrix at those values of alpha and beta,
  //   which is stored in info.covariance.
  // Additional arguments eps_1 and eps_2 are the two invariant
  //   "emittances," interpreted as expectation values of (scaled)
  //   action coordinates, in units of pi mm-mr.  eps_1 is (mostly)
  //   horizontal; eps_2 is (mostly) vertical. Currently, eps_3 is ignored.
  // Future upgrade: create coupled, equilibrium covariance matrix.
  //   This will require adding more data to the Info struct and
  //   a second bool argument, with default value for backwards 
  //   compatability.

  static void setErrorStream( std::ostream* );
  static void setOutputStream( std::ostream* );

  static std::ostream* _errorStreamPtr;
  static std::ostream* _outputStreamPtr;

 private: 
  static const double mlt1;

  // Error codes 
  static const int PSDERR;
  // Used by functions which assume phase space has dimension
  //   six, and some other dimension is employed.
};
