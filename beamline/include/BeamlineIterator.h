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
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
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

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>



class DLLEXPORT BeamlineIterator
{
public:
  BeamlineIterator( const beamline& );
  BeamlineIterator( const beamline* );
  BeamlineIterator( const BeamlineIterator& );
  ~BeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();
  void goBack( int = 1 );
  bool isFinished();

private:
  dlist_iterator* _getNext;
};


class DLLEXPORT  DeepBeamlineIterator
{
public:
  DeepBeamlineIterator( const beamline& );
  DeepBeamlineIterator( const beamline* );
  ~DeepBeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();

  // private:
  protected:

  dlist_iterator*       _getNext;
  DeepBeamlineIterator* _subIterator;
  DeepBeamlineIterator( const DeepBeamlineIterator& ); // forbidden

};


class DLLEXPORT  ReverseBeamlineIterator
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


class DLLEXPORT  DeepReverseBeamlineIterator
{
public:
  DeepReverseBeamlineIterator( const beamline& );
  DeepReverseBeamlineIterator( const beamline* );
  ~DeepReverseBeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();

private:
  dlist_reverseIterator*       _getNext;
  DeepReverseBeamlineIterator* _subIterator;
  DeepReverseBeamlineIterator( const DeepReverseBeamlineIterator& ); // forbidden
};


#endif // BEAMLINE_ITERATOR
