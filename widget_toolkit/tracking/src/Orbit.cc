/*************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******        interfaces to exercise the functionality of BEAMLINE.
******                                                                
******  File:      Orbit.cc
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
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
******  REVISION HISTORY
******  ----------------
******  February, 2004.
******  - Original version of class Orbit introduced in
******  files widget_toolkit/include/Tracker.h and 
******  widget_toolkit/src/Tracker.cc.
****** 
******  May, 2008.
******  - Upgraded version of class Orbit written
******  as part of lpsDemo and stored in separate files.
******  
******  December, 2009.
******  - lpsDemo and Tracker versions merged to create
******  this one.
******  
**************************************************************************
*************************************************************************/

#include <Orbit.h>

long int Orbit::defaultMax_ = 1024;

// ---------------------------
// Implementation: class Orbit
// ---------------------------

Orbit::Orbit( Vector const& x ) 
  : mode(Orbit::points)
  , red_(1)
  , green_(1)
  , blue_(1)
  , max_(Orbit::defaultMax_)
{
  history_.push_back( new Vector( x ) );
}

Orbit::Orbit( Vector const* x ) 
  : mode(Orbit::points)
  , red_(1)
  , green_(1)
  , blue_(1)
  , max_(Orbit::defaultMax_)
{
  history_.push_back( new Vector( *x ) );
}


Orbit::Orbit( Orbit const& x ) 
  : mode(x.mode)
  , red_(x.red_)
  , green_(x.green_)
  , blue_(x.blue_)
  , max_(x.max_)
{
  for (std::list<Vector*>::const_iterator it  = x.history_.begin(); 
       it != x.history_.end(); ++it ) {
    history_.push_back( new Vector(**it) );
  }
}


Orbit::~Orbit()
{
  for (std::list<Vector*>::iterator it  = history_.begin(); 
       it != history_.end(); ++it ) {
    delete (*it);
  }
  history_.clear();
}


void Orbit::add( const Vector& x ) 
{
  if( max_ < 0 || history_.size() < max_ ) {
    history_.push_back( new Vector( x ) );
  }
}


void Orbit::add( const Vector* x ) 
{
  if( max_ < 0 || history_.size() < max_ ) {
    history_.push_back( new Vector( *x ) );
  }
}


const Vector* Orbit::lastPoint()
{
  return history_.back();
}


const Vector* Orbit::firstPoint()
{
  return history_.front();
}


void Orbit::setColor( double r, double g, double b )
{
  if( r < 0.0 )     red_ = 0.0;     // This is the OpenGL convention.
  else if( r > 1. ) red_ = 1.0;
  else              red_ = r;

  if( g < 0. )      green_ = 0.0;
  else if( g > 1. ) green_ = 1.0;
  else              green_ = g;

  if( b < 0. )      blue_ = 0.0;
  else if( b > 1. ) blue_ = 1.0;
  else              blue_ = b;
}


void Orbit::set_maxNumberOfOrbits( long int x )
{
  max_ = ( 0 <= x ? x : -x );
}


void Orbit::unset_maxNumberOfOrbits()
{
  max_ = -1;
}


long int Orbit::maxNumberOfOrbitsDefault()
{
  return Orbit::defaultMax_;
}


void Orbit::set_maxNumberOfOrbitsDefault( long int x )
{
  Orbit::defaultMax_ = ( 0 <= x ? x : -x );
}


void Orbit::unset_maxNumberOfOrbitsDefault()
{
  Orbit::defaultMax_ = -1;
}


bool Orbit::filled() const
{
  return ( 0 < max_ ? history_.size() == max_ : false );
}


// -------------------------------------
// Implementation: class Orbit::iterator
// -------------------------------------

Orbit::iterator Orbit::begin()
{
  return history_.begin();
}
 
Orbit::const_iterator Orbit::begin() const
{
   return history_.begin();
}

Orbit::iterator Orbit::end()
{
  return history_.end();
}
 
Orbit::const_iterator Orbit::end() const
{
  return history_.end();
}


