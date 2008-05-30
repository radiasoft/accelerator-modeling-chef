////////////////////////////////////////////////////////////
// 
// File:          Orbit.cc
//                                                                
// -------------------------------------------------------------
// 
// Copyright Universities Research Association, Inc./ Fermilab    
//           All Rights Reserved                             
//
// Usage, modification, and redistribution are subject to terms          
// of the License supplied with this software.
// 
// Software and documentation created under 
// U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
// The U.S. Government retains a world-wide non-exclusive, 
// royalty-free license to publish or reproduce documentation 
// and software for U.S. Government purposes. This software 
// is protected under the U.S. and Foreign Copyright Laws.
// 
// -------------------------------------------------------------
// 
// Author:        Leo Michelotti
// 
// REVISION HISTORY
// 
// Thursday. May 29, 2008.       (this file)
// 
////////////////////////////////////////////////////////////
// 
// A variation on class Orbit appearing implemented in
// widget_toolkit/tracking/src/Tracker.cc.  After further
// development, a successor of this file will eventually
// replace that implementation.
// 
////////////////////////////////////////////////////////////

#include <Orbit.h>

using namespace AESOP_domain;

long int Orbit::_defaultMax = -1;

Orbit::Orbit( const Vector& x ) 
  : _red(1)
  , _green(1)
  , _blue(1)
  , _max(Orbit::_defaultMax)
{
  _history.push_back( new Vector( x ) );
}

Orbit::Orbit( const Vector* x ) 
  : _red(1)
  , _green(1)
  , _blue(1)
  , _max(Orbit::_defaultMax)
{
  _history.push_back( new Vector( *x ) );
}


Orbit::Orbit( Orbit const& x ) 
  : _red(x._red)
  , _green(x._green)
  , _blue(x._blue)
  , _max(x._max)
{
  for (std::list<Vector*>::const_iterator it  = x._history.begin(); 
       it != x._history.end(); ++it ) {
    _history.push_back( new Vector(**it) );
  }
}

Orbit::~Orbit()
{
  for (std::list<Vector*>::iterator it  = _history.begin(); 
       it != _history.end(); ++it ) {
    delete (*it);
  }
  _history.clear();
}

void Orbit::add( const Vector& x ) 
{
  if( _max < 0 || _history.size() < _max ) {
    _history.push_back( new Vector( x ) );
  }
}

void Orbit::add( const Vector* x ) 
{
  if( _max < 0 || _history.size() < _max ) {
    _history.push_back( new Vector( *x ) );
  }
}

const Vector* Orbit::lastPoint()
{
  return _history.back();
}

const Vector* Orbit::firstPoint()
{
  return _history.front();
}


void Orbit::setColor( double r, double g, double b )
{
  if( r < 0. )      _red = 0.;
  else if( r > 1. ) _red = 1.;
  else              _red = r;

  if( g < 0. )      _green = 0.;
  else if( g > 1. ) _green = 1.;
  else              _green = g;

  if( b < 0. )      _blue = 0.;
  else if( b > 1. ) _blue = 1.;
  else              _blue = b;
}


// -------------------------------------
// Implementation: class Orbit::Iterator
// -------------------------------------

Orbit::iterator Orbit::begin()
{
  return _history.begin();
}
 
Orbit::const_iterator Orbit::begin() const
{
   return _history.begin();
}

Orbit::iterator Orbit::end()
{
  return _history.end();
}
 
Orbit::const_iterator Orbit::end() const
{
   return _history.end();
}


void Orbit::set_maxNumberOfOrbits( long int x )
{
  _max = ( 0 <= x ? x : -x );
}


void Orbit::unset_maxNumberOfOrbits()
{
  _max = -1;
}


long int Orbit::maxNumberOfOrbitsDefault()
{
  return Orbit::_defaultMax;
}


void Orbit::set_maxNumberOfOrbitsDefault( long int x )
{
  Orbit::_defaultMax = ( 0 <= x ? x : -x );
}


void Orbit::unset_maxNumberOfOrbitsDefault()
{
  Orbit::_defaultMax = -1;
}


bool Orbit::filled()
{
  return ( 0 < _max ? _history.size() == _max : false );
}
