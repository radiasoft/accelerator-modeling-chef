#ifndef BEAMLINECONTEXT_H
#define BEAMLINECONTEXT_H

class beamline;
class LattFuncSage;
class ClosedOrbitSage;
class ChromaticityAdjuster;
class TuneAdjuster;

class BeamlineContext
{
  public:
    BeamlineContext( beamline*, bool=false );
    // If second argument is true, the
    // beamline is cloned, and therefore
    // owned by the context.
    ~BeamlineContext();
  private:
    beamline*             _p_bml;
    LattFuncSage*         _p_lfs;
    ClosedOrbitSage*      _p_cos;
    ChromaticityAdjuster* _p_ca;
    TuneAdjuster*         _p_ta;

    bool                  _isCloned;
};

#endif // BEAMLINECONTEXT_H
