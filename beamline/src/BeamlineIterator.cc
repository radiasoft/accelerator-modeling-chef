/*
**
** File: BeamlineIterator.cc
** 
** Two classes are implemented: BeamlineIterator and DeepBeamlineIterator.
** The former works only on the top level of the beamline while
** the latter goes through all sub-beamlines.
** 
** --- Leo Michelotti
** --- November 17, 1998
**
*/


#include "BeamlineIterator.h"


BeamlineIterator::BeamlineIterator( const beamline& x )
{
  _getNext = new dlist_iterator( (dlist&) x );
}


BeamlineIterator::BeamlineIterator( const beamline* x )
{
  _getNext = new dlist_iterator( *(dlist*) x );
}


BeamlineIterator::BeamlineIterator( const BeamlineIterator& )
{
  cerr << "*** ERROR ***                                       \n"
          "*** ERROR *** BeamlineIterator::BeamlineIterator( const BeamlineIterator& ) \n"
          "*** ERROR *** Copy constructor must not be called.  \n"
          "*** ERROR ***                                       \n"
       << endl;
  exit(999);
}


BeamlineIterator::~BeamlineIterator()
{
  delete _getNext;
}



bmlnElmnt* BeamlineIterator::operator++( int )
{
  return (bmlnElmnt*) _getNext->operator()();
}


void BeamlineIterator::reset()
{
  _getNext->Reset();
}




DeepBeamlineIterator::DeepBeamlineIterator( const beamline& x )
{
  _subIterator = 0;
  _getNext = new dlist_iterator( (dlist&) x );
}


DeepBeamlineIterator::DeepBeamlineIterator( const beamline* x )
{
  _subIterator = 0;
  _getNext = new dlist_iterator( *(dlist*) x );
}


DeepBeamlineIterator::DeepBeamlineIterator( const DeepBeamlineIterator& )
{
  cerr << "*** ERROR ***                                       \n"
          "*** ERROR *** DeepBeamlineIterator::DeepBeamlineIterator( const DeepBeamlineIterator& ) \n"
          "*** ERROR *** Copy constructor must not be called.  \n"
          "*** ERROR ***                                       \n"
       << endl;
  exit(999);
}


DeepBeamlineIterator::~DeepBeamlineIterator()
{
  delete _getNext;
  if( _subIterator ) delete _subIterator;
  _subIterator = 0;
}



bmlnElmnt* DeepBeamlineIterator::operator++( int )
{
  bmlnElmnt* ret = 0;

  if( _subIterator ) 
  {
    ret = (*_subIterator)++;
    if( ret == 0 ) 
    {
      delete _subIterator;
      _subIterator = 0;
      ret = (*this)++;
    }
  }
  else 
  {
    ret = (bmlnElmnt*) _getNext->operator()();
    if( ret != 0 ) 
    {
      if( 0 == strcmp( ret->Type(), "beamline" ) ) 
      {
        _subIterator = new DeepBeamlineIterator( ret );
        ret = (*this)++;
      }
    }    
  }

  return ret;
}


void DeepBeamlineIterator::reset()
{
  if( _subIterator ) delete _subIterator;
  _subIterator = 0;
  _getNext->Reset();
}
