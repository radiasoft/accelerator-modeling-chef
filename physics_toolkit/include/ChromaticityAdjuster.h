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

#ifndef BEAMLINE_H
#include "beamline.h"
#endif

#include "Sage.h"

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
  int changeChromaticityBy ( double, double, const JetProton& );
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


#ifndef CHROMADJUST_ICC
#include "ChromaticityAdjuster.icc"
#endif

#endif // LATTFUNCSAGE_H
