/*
 *  File: TuneAdjuster.h
 *  
 *  Implementation for a sage that adjusts
 *  the tunes given a collection of "correction"
 *  quadrupoles.
 *  
 *  Leo Michelotti
 *  December 17, 1998
 */

#ifndef TUNEADJUST_H
#define TUNEADJUST_H

#ifndef BEAMLINE_H
#include "beamline.h"
#endif

#include "Sage.h"

class TuneAdjuster : public Sage
{
public:
  TuneAdjuster( const beamline* );
  TuneAdjuster( const beamline& );
  ~TuneAdjuster();

  void addCorrector( const quadrupole&, double, double );
  void addCorrector( const quadrupole*, double, double );
  void addCorrector( const thinQuad&,   double, double );
  void addCorrector( const thinQuad*,   double, double );

  int  numberOfCorrectors() const;

  int changeTunesBy ( double, double, const JetProton& );
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


#ifndef TUNEADJUST_ICC
#include "TuneAdjuster.icc"
#endif

#endif // LATTFUNCSAGE_H
