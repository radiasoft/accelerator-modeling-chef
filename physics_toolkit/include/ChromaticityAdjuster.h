/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      ChromaticityAdjuster.h
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
****** REVISION HISTORY
******
******  Mar 2007      ostiguy@fnal.gov
******
******  - controls Matrix returned by reference !
******  - support for reference counted elements
**************************************************************************
*************************************************************************/


/*
 *  File: ChromaticityAdjuster.h
 *  
 *  Implementation for a sage that adjusts
 *  chromaticities given a collection of "correction"
 *  sextupoles
 *  
 *  Leo Michelotti
 *  February 1, 1999
 */

#ifndef CHROMADJUST_H
#define CHROMADJUST_H

#include <beamline/beamline.h>
#include <physics_toolkit/Sage.h>

class ChromaticityAdjuster : public Sage {

public:

  ChromaticityAdjuster( BmlPtr );
  ChromaticityAdjuster( beamline const& );


 ~ChromaticityAdjuster();

  void addCorrector( sextupole     const&, double, double );
  void addCorrector( SextupolePtr,         double, double );

  void addCorrector( thinSextupole const&, double, double );
  void addCorrector( ThinSextupolePtr,     double, double );

  void  addCorrector(  ElmPtr x, double a, double b ); // This form is used by the GUI code. It MUST go away !

  int numberOfCorrectors()  const;
  int changeChromaticityBy ( double, double, JetParticle const& );

  MatrixD& getControls();

  void eraseAll();

private:

  void  addCorrector_private(  ElmPtr x, double a, double b );
  bool  isaThinSextupole(  bmlnElmnt const& ) const;

  std::vector<ElmPtr> correctors_;

  MatrixD     f_;
  MatrixD     c_;

};

#endif // LATTFUNCSAGE_H
