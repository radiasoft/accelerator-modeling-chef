/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      BeamlineIterator.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


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
