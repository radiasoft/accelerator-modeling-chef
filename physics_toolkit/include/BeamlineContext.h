#ifndef BEAMLINECONTEXT_H
#define BEAMLINECONTEXT_H

#ifndef _IOSTREAM_H
#include <iostream.h>
#endif

class beamline;
class ClosedOrbitSage;
class ChromaticityAdjuster;
class TuneAdjuster;
class ConstBmlVisitor;
class Particle;
class JetProton;
class bmlnElmnt;
class BeamlineIterator;
class DeepBeamlineIterator;
class ReverseBeamlineIterator;
class DeepReverseBeamlineIterator;

#ifndef LATTFUNCSAGE_H
#include "LattFuncSage.h"
#endif

class BeamlineContext
{
  public:
    BeamlineContext( bool = false, beamline* = 0 );
    BeamlineContext( const BeamlineContext& );
    // If first argument is true, the
    // beamline is cloned, and therefore
    // owned by the context.
    ~BeamlineContext();

    int assign( /* const */ beamline* );

    void accept( ConstBmlVisitor& ) const;
    void setClonedFlag( bool );
    bool getClonedFlag();

    void writeTree();

    // Beamline functions ...
    const char* name() const;
    void peekAt( double& s, Particle* = 0 ) const;
    double sumLengths() const;
    int setLength( bmlnElmnt*, double );
    double getEnergy() const;
    int countHowManyDeeply() const;


    // Sage operations
    double getHorizontalFracTune();
    double getVerticalFracTune();
    const LattFuncSage::lattFunc* getLattFuncPtr( int );


    // Iterator functions
    int beginIterator();
    int beginDeepIterator();
    int beginReverseIterator();
    int beginDeepReverseIterator();

    void endIterator();
    void endDeepIterator();
    void endReverseIterator();
    void endDeepReverseIterator();

    const bmlnElmnt* i_next();
    const bmlnElmnt* di_next();
    const bmlnElmnt* ri_next();
    const bmlnElmnt* dri_next();

    void i_reset();
    void di_reset();
    void ri_reset();
    void dri_reset();

    const beamline* cheatBmlPtr() const;

    // Stream functions
    friend ostream& operator<<( ostream&, const BeamlineContext& );
    friend istream& operator>>( istream&,       BeamlineContext& );


  private:
    beamline*             _p_bml;
    LattFuncSage*         _p_lfs;
    ClosedOrbitSage*      _p_cos;
    ChromaticityAdjuster* _p_ca;
    TuneAdjuster*         _p_ta;

    JetProton*            _p_jp;
    // once created, its state always contains
    // one traversal of the beamline.

    LattFuncSage::tunes*  _tunes;

    bool                  _isCloned;
    bool                  _normalLattFuncsCalcd;

    BeamlineIterator*            _p_bi;
    DeepBeamlineIterator*        _p_dbi;
    ReverseBeamlineIterator*     _p_rbi;
    DeepReverseBeamlineIterator* _p_drbi;

    void _createLFS();
    void _deleteLFS();
    void _createTunes();
};

#endif // BEAMLINECONTEXT_H
