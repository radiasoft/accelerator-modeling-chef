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


  static beamline* cloneLineAndInsert( double          percent,
                                       BmlPtrList&     insertions,
                                       BmlPtrList&     targets,
                                       const beamline* linePtr );
  // Creates a new beamline and returns its address. The
  //   invoking program takes responsibility for deleting the
  //   created beamline. It also takes the responsibility of 
  //   making certain that no element in <insertions> are already
  //   included in beamlines.
  // The new beamline is a clone of <linePtr> with the difference
  //   that elements from <insertions> are inserted next to
  //   elements in <targets>. As input, the elements stored
  //   in <targets> must correspond to elements in <linePtr>
  //   and be stored in the same order. 
  //   *********************
  //   NOTE WELL: the elements in <insertions> are NOT cloned.
  //   *********************
  // Reference energy and name of the new beamline will match 
  //   the old. 
  // <percent> must take on a value in [0,1] and determines where 
  //   elements are inserted. If <percent> = 0, then elements from
  //   <insertions> are put upstream of their corresponding 
  //   elements from <targets>; if <percent> = 1, then insertion
  //   is downstream; if an intermediate value, the target element
  //   is split <percent> of the distance from its upstream end
  //   and the insertion is made there.
  // If completely successful, both <insertions> and <targets>
  //   will be empty when the method has completed.
  // A clone of <linePtr> is returned if:
  //   (i) either <insertions> or <targets> is empty
  //   (ii) no element of targets matches an element in <*linePtr>
  // The method works recursively so that the hierarchical structure
  //   of <linePtr> is preserved.


  static bool isSpace( const bmlnElmnt* );
  static bool isSpace( const bmlnElmnt& );
  // Returns true if the argument refers to an element
  //   which acts like empty space: e.g. drifts, Slots, markers.


  static bool isKnown( const bmlnElmnt* );
  static bool isKnown( const bmlnElmnt& );
  // Returns true if the argument's type is recognized.


  // Message streams
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
