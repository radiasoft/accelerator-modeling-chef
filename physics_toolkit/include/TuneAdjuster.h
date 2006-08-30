/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      TuneAdjuster.h
******  Version:   2.0
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
 *  File: TuneAdjuster.h
 *  
 *  Implementation for a sage that adjusts
 *  the tunes given a collection of "correction"
 *  quadrupoles.
 *  
 *  Leo Michelotti
 *  December 17, 1998
 *
 *  Put in check for Slots and use of new 
 *  LattFuncSage functions.
 *
 *  Leo Michelotti
 *  December 16, 1999
 *
 *  Added functions to control only horizontal
 *  or vertical tunes.
 *
 *  Leo Michelotti
 *  January 19, 2001
 *
 */

#ifndef TUNEADJUST_H
#define TUNEADJUST_H

#include <beamline/beamline.h>
#include <physics_toolkit/Sage.h>

class TuneAdjuster : public Sage
{
public:
  TuneAdjuster( const beamline*, bool = false );
  TuneAdjuster( const beamline&, bool = false );
  // Second argument is used by class Sage
  // to control cloning. (See Sage.h)
  ~TuneAdjuster();

  void addCorrector( const quadrupole&, double, double );
  void addCorrector( const quadrupole*, double, double );
  void addCorrector( const thinQuad&,   double, double );
  void addCorrector( const thinQuad*,   double, double );

  int  numberOfCorrectors() const;

  int changeTunesBy ( double, double, const JetParticle& );
  int changeHorizontalTuneBy ( double, const JetParticle& );
  int changeVerticalTuneBy ( double, const JetParticle& );
  MatrixD getControls();

  void eraseAll();

private:
  bmlnElmnt** _correctors;
  int         _numberOfCorrectors;
  MatrixD*    _f;
  MatrixD     _c;
  void        _addCorrector( const bmlnElmnt*, double, double );

  char        _isQuadLike  ( const bmlnElmnt* ) const;
  char        _isaThinQuad ( const bmlnElmnt* ) const;
  char        _isaQuad     ( const bmlnElmnt* ) const;
};


#include <physics_toolkit/TuneAdjuster.icc>


#endif // LATTFUNCSAGE_H
