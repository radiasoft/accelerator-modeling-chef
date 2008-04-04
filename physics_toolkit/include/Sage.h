/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      Sage.h
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
****** Mar 2007     ostiguy@fnal.gov
******
****** - support for reference counted elements
****** - Pass particles by reference 
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

#include <basic_toolkit/globaldefs.h>
#include <beamline/beamline.h>
#include <beamline/LatticeFunctions.h>
#include <vector>

namespace sqlite { class connection; }

class Sage  {

public:

  Sage( BmlPtr,          sqlite::connection& db);
  Sage( beamline const&, sqlite::connection& db);

  virtual ~Sage();
  
  void set_verbose();
  void unset_verbose();

  virtual void eraseAll() = 0;

  typedef bool (*CRITFUNC)( ConstElmPtr );

  static bool no ( ConstElmPtr );
  static bool yes( ConstElmPtr );

 //-------------------------------------------------------------------
 // gap_tol, angle_tol are repectively the maximum position and angle 
 // discontinuity allowed when  testing fiducial orbit closure.
 // These tests will return true for pathologically short lines
 // like a beamline consisting of a single 1 mm drift.
 //-------------------------------------------------------------------

  static bool isRing( BmlPtr,          double const& gap_tol = defGapTol_, double const& angle_tol = defAngleTol_ );
  static bool isRing( beamline const&, double const& gap_tol = defGapTol_, double const& angle_tol = defAngleTol_ );
 
  bool hasRing() const;
  bool isTreatedAsRing() const;

  void     treatAsRing( bool );
  void     setGapTolerance( double );
  double   getGapTolerance()            const;
  void     setAngleTolerance( double );
  double   getAngleTolerance()          const;

  static std::vector<double> compute_tunes( Matrix const& oneturn );

protected:

  BmlPtr              bml_;
  int                 nelms_;   
  bool                verbose_;
  bool                isRing_;
  double              ringGapTolerance_;
  double              ringAngleTolerance_;
  sqlite::connection& db_;

private:

  static double defGapTol_;
  static double defAngleTol_;
 
 Sage( Sage const&); // copy forbidden 

};


#endif // SAGE_H
