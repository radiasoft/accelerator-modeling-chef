#include "BeamlineContext.h"

BeamlineContext::BeamlineContext( beamline* x, bool doClone )
: _p_bml(x), 
  _p_lfs(0), _p_cos(0), 
  _p_ca(0), _p_ta(0),
  _isCloned(doClone)
{
  if( _isCloned ) {
    _p_bml = x->Clone();
  }
}

BeamlineContext::~BeamlineContext()
{
  if( _isCloned ) _p_bml->eliminate;

  if( _p_lfs ) delete _p_lfs;
  if( _p_cos ) delete _p_cos;
  if( _p_ca  ) delete _p_ca;
  if( _p_ta  ) delete _p_ta;
}
