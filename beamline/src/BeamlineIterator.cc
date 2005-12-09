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
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

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


#include <iosetup.h>
#include <BeamlineIterator.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

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
    (*pcerr) << "*** WARNING ***                                       \n"
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

 	 
bool BeamlineIterator::isFinished() 	 
{ 	 
   return _getNext->isFinished(); 	 
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
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "DeepBeamlineIterator::DeepBeamlineIterator( const DeepBeamlineIterator& )", 
         "Copy constructor must not be called." ) );
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
    (*pcerr) << "*** WARNING ***                                       \n"
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
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "void ReverseBeamlineIterator::goBack( int n )", 
         "Not implemented." ) );
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
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "DeepReverseBeamlineIterator::DeepReverseBeamlineIterator( const DeepReverseBeamlineIterator& )", 
         "Copy constructor must not be called." ) );
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


