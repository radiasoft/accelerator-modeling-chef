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
******  Mar 2007      ostiguy@fnal.gov
******
******  - controls Matrix returned by reference !
******  - support for reference counted elements
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
#include <beamline/quadrupole.h>

class TuneAdjuster : public Sage {

public:

  TuneAdjuster( BmlPtr bml     ); // shared
  TuneAdjuster( beamline const&); // beamline is cloned

 ~TuneAdjuster();

  void addCorrector( quadrupole     const&,  double, double );
  void addCorrector( QuadrupolePtr,          double, double );

  void addCorrector( thinQuad       const&,  double, double );
  void addCorrector( ThinQuadPtr,            double, double );

  void addCorrector( ElmPtr,          double, double );      // Form used by GUI code. Needs to go away ! 



  int numberOfCorrectors() const;

  int          changeTunesBy ( double, double,  JetParticle const& );
  int changeHorizontalTuneBy ( double,          JetParticle const& );
  int   changeVerticalTuneBy ( double,          JetParticle const& );

  MatrixD& getControls();

  void eraseAll();

private:

  bool        isQuadLike  ( bmlnElmnt const& ) const;
  bool        isaThinQuad ( bmlnElmnt const& ) const;
  bool        isaQuad     ( bmlnElmnt const& ) const;
  bool        slotFound();

  std::vector<ElmPtr> correctors_;

  MatrixD     f_;
  MatrixD     c_;

  void        addCorrector_private( ElmPtr, double, double );

};

#endif // TUNEADJUST_H
