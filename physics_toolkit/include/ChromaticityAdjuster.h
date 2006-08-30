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

class ChromaticityAdjuster : public Sage
{
public:
  ChromaticityAdjuster( const beamline*, bool = false );
  ChromaticityAdjuster( const beamline&, bool = false );
  // Second argument is used by class Sage
  // to control cloning. (See Sage.h)
  ~ChromaticityAdjuster();

  void addCorrector( const sextupole&, double, double );
  void addCorrector( const sextupole*, double, double );
  void addCorrector( const thinSextupole&, double, double );
  void addCorrector( const thinSextupole*, double, double );

  int numberOfCorrectors() const;
  int changeChromaticityBy ( double, double, const JetParticle& );
  MatrixD getControls();

  void eraseAll();

private:
  bmlnElmnt** _correctors;
  int         _numberOfCorrectors;
  MatrixD*    _f;
  MatrixD     _c;
  void        _addCorrector( const bmlnElmnt*, double, double );
  char        _isaThinSextupole( const bmlnElmnt* ) const;
};


#include <physics_toolkit/ChromaticityAdjuster.icc>

#endif // LATTFUNCSAGE_H
