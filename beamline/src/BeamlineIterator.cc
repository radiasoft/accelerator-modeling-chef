#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      BeamlineIterator.cc
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

using namespace std;

// 
// Forward iterator
// 

BeamlineIterator::BeamlineIterator( const beamline& x )
{
  _getNext = new dlist_iterator( (dlist&) x );
}


BeamlineIterator::BeamlineIterator( const beamline* x )
{
  _getNext = new dlist_iterator( *(dlist*) x );
}


BeamlineIterator::BeamlineIterator( const BeamlineIterator& x )
{
  static char firstTime = 1;
  if( firstTime ) {
    cerr << "*** WARNING ***                                       \n"
            "*** WARNING *** BeamlineIterator::BeamlineIterator( const BeamlineIterator& ) \n"
            "*** WARNING *** Copy constructor has been called.     \n"
            "*** WARNING ***                                       \n"
            "*** WARNING *** This message appears once only.       \n"
            "*** WARNING ***                                       \n"
         << endl;
    firstTime = 0;
  }
  
  _getNext = new dlist_iterator( *(x._getNext) );
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


void BeamlineIterator::goBack( int n )
{
  _getNext->GoBack(n);
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
        _subIterator = new DeepBeamlineIterator( (beamline*) ret );
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


// 
// Reverse iterator
// 


ReverseBeamlineIterator::ReverseBeamlineIterator( const beamline& x )
{
  _getNext = new dlist_reverseIterator( (dlist&) x );
}


ReverseBeamlineIterator::ReverseBeamlineIterator( const beamline* x )
{
  _getNext = new dlist_reverseIterator( *(dlist*) x );
}


ReverseBeamlineIterator::ReverseBeamlineIterator( const ReverseBeamlineIterator& x )
{
  static char firstTime = 1;
  if( firstTime ) {
    cerr << "*** WARNING ***                                       \n"
            "*** WARNING *** ReverseBeamlineIterator::ReverseBeamlineIterator( const ReverseBeamlineIterator& ) \n"
            "*** WARNING *** Copy constructor has been called.     \n"
            "*** WARNING ***                                       \n"
            "*** WARNING *** This message appears once only.       \n"
            "*** WARNING ***                                       \n"
         << endl;
    firstTime = 0;
  }
  
  _getNext = new dlist_reverseIterator( *(x._getNext) );
}


ReverseBeamlineIterator::~ReverseBeamlineIterator()
{
  delete _getNext;
}



bmlnElmnt* ReverseBeamlineIterator::operator++( int )
{
  return (bmlnElmnt*) _getNext->operator()();
}


void ReverseBeamlineIterator::reset()
{
  _getNext->Reset();
}


void ReverseBeamlineIterator::goBack( int n )
{
  cerr << "*** ERROR ***                                           \n"
       << "*** ERROR *** void ReverseBeamlineIterator::goBack      \n"
       << "*** ERROR *** Sorry. This function is not implemented   \n"
       << "*** ERROR *** for ReverseBeamlineIterator.              \n"
       << "*** ERROR ***                                           \n"
       << endl;
  exit(1);
  // _getNext->GoBack(n);  <- not written for dlist_reverseIterator
}



DeepReverseBeamlineIterator::DeepReverseBeamlineIterator( const beamline& x )
{
  _subIterator = 0;
  _getNext = new dlist_reverseIterator( (dlist&) x );
}


DeepReverseBeamlineIterator::DeepReverseBeamlineIterator( const beamline* x )
{
  _subIterator = 0;
  _getNext = new dlist_reverseIterator( *(dlist*) x );
}


DeepReverseBeamlineIterator::DeepReverseBeamlineIterator( const DeepReverseBeamlineIterator& )
{
  cerr << "*** ERROR ***                                       \n"
          "*** ERROR *** DeepReverseBeamlineIterator::DeepReverseBeamlineIterator( const DeepReverseBeamlineIterator& ) \n"
          "*** ERROR *** Copy constructor must not be called.  \n"
          "*** ERROR ***                                       \n"
       << endl;
  exit(999);
}


DeepReverseBeamlineIterator::~DeepReverseBeamlineIterator()
{
  delete _getNext;
  if( _subIterator ) delete _subIterator;
  _subIterator = 0;
}



bmlnElmnt* DeepReverseBeamlineIterator::operator++( int )
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
        _subIterator = new DeepReverseBeamlineIterator( (beamline*) ret );
        ret = (*this)++;
      }
    }    
  }

  return ret;
}


void DeepReverseBeamlineIterator::reset()
{
  if( _subIterator ) delete _subIterator;
  _subIterator = 0;
  _getNext->Reset();
}


