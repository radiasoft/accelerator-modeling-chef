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
