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


#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
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
  void goBack( int = 1 );

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


class ReverseBeamlineIterator
{
public:
  ReverseBeamlineIterator( const beamline& );
  ReverseBeamlineIterator( const beamline* );
  ReverseBeamlineIterator( const ReverseBeamlineIterator& );
  ~ReverseBeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();
  void goBack( int = 1 );

private:
  dlist_reverseIterator* _getNext;
};


class DeepReverseBeamlineIterator
{
public:
  DeepReverseBeamlineIterator( const beamline& );
  DeepReverseBeamlineIterator( const beamline* );
  DeepReverseBeamlineIterator( const DeepReverseBeamlineIterator& );
  ~DeepReverseBeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();

private:
  dlist_reverseIterator*       _getNext;
  DeepReverseBeamlineIterator* _subIterator;
};


#endif // BEAMLINE_ITERATOR
