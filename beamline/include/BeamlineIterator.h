/*
**
** File: BeamlineIterator.h
** 
** Two classes are defined: BeamlineIterator and DeepBeamlineIterator.
** The former works only on the top level of the beamline while
** the latter goes through all sub-beamlines.
** 
** --- Leo Michelotti
** --- November 17, 1998
**
*/


#ifndef BEAMLINE_ITERATOR
#define BEAMLINE_ITERATOR


#ifndef BEAMLINE_H
#include "beamline.h"
#endif


class BeamlineIterator
{
public:
  BeamlineIterator( const beamline& );
  BeamlineIterator( const beamline* );
  BeamlineIterator( const BeamlineIterator& );
  ~BeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();

private:
  dlist_iterator* _getNext;
};


class DeepBeamlineIterator
{
public:
  DeepBeamlineIterator( const beamline& );
  DeepBeamlineIterator( const beamline* );
  DeepBeamlineIterator( const DeepBeamlineIterator& );
  ~DeepBeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();

private:
  dlist_iterator*       _getNext;
  DeepBeamlineIterator* _subIterator;
};


#endif // BEAMLINE_ITERATOR
