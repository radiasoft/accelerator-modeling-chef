#include "MappPropFunc.h"

int MappPropFunc::operator() ( bmlnElmnt* b, Particle& p ) 
{
  p.setState( _myMap( p.State() ) );
  return 0;
}


int MappPropFunc::operator() ( bmlnElmnt* b, JetParticle& p ) 
{
  p.setState( _myMap( p.State() ) );
  return 0;
}


MappPropFunc::MappPropFunc( JetParticle& p, bmlnElmnt& b )
{
  // This assumes the p has already been set 
  // up correctly for the in-state.
  b.propagate( p );
  _myMap.CopyFrom( p.State() );
}


MappPropFunc::MappPropFunc( const Mapping& map )
: _myMap( map )
{
}

MappPropFunc::MappPropFunc( const MappPropFunc& x )
: _myMap( x._myMap )
{
}

void MappPropFunc::setMapping( const Mapping& x )
{
  _myMap.CopyFrom( x );
}

