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


#include <basic_toolkit/iosetup.h>
#include <beamline/BeamlineIterator.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

// --------------------------------------------------------------------------
// Forward iterator
// --------------------------------------------------------------------------



BeamlineIterator::BeamlineIterator( beamline& x ) {
 
  _it     = x._theList.begin();
  _begin  = x._theList.begin();
  _end    = x._theList.end();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineIterator::BeamlineIterator( BeamlineIterator const& x ): 
_it( x._it ), _begin(x._begin), _end(x._end) { }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineIterator::~BeamlineIterator() 
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bmlnElmnt* BeamlineIterator::operator++( int )
{
  bmlnElmnt* ret = 0;

  if ( _it == _end ) return 0;
 
  ret = *_it;
  ++_it;

  return ret;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineIterator::reset()
{
  _it = _begin;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool BeamlineIterator::isFinished() 	 
{ 	 
   return (_it == _end);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineIterator::goBack( int n )
{
  std::advance(_it, -n );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


DeepBeamlineIterator::DeepBeamlineIterator( beamline & x ): _subit(0) {

 _it    = x._theList.begin();
 _begin = x._theList.begin();
 _end   = x._theList.end();

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


DeepBeamlineIterator::~DeepBeamlineIterator()
{ 

  if (_subit) delete _subit;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bmlnElmnt* DeepBeamlineIterator::operator++( int )
{

  bmlnElmnt* ret = 0;

  if( _subit ) 
  {
    ret = (*_subit)++; 

    if( !ret ) 
    {
      _subit = 0;
         ret = (*this)++;
    }
  }
  else 
  {

    ret = ( _it ==  _end ) ? 0 : *_it; 

    if( ret ) 
    {
      ++_it;      
      if( strcmp( ret->Type(), "beamline" ) == 0 ) 
      {
        _subit = new  DeepBeamlineIterator( *static_cast<beamline*>(ret) );
        ret = (*this)++;
      }
    }    
  }

  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DeepBeamlineIterator::reset()
{


  _subit   = 0;
  _it       = _begin;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// *** Reverse iterator ***
//
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


ReverseBeamlineIterator::ReverseBeamlineIterator( beamline& x ) {
 
  _rit    =  x._theList.rbegin();
  _rbegin =  x._theList.rbegin();
  _rend   =  x._theList.rend();

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


ReverseBeamlineIterator::ReverseBeamlineIterator( ReverseBeamlineIterator const& x ): 
_rit(x._rit), _rbegin(x._rbegin), _rend(x._rend) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ReverseBeamlineIterator::~ReverseBeamlineIterator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bmlnElmnt* ReverseBeamlineIterator::operator++( int )
{
 
  bmlnElmnt* ret = 0;

  if ( _rit ==  _rend ) return 0;
 
  ret = *_rit;  ++_rit;

  return ret;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ReverseBeamlineIterator::reset()
{
  _rit = _rbegin;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ReverseBeamlineIterator::goBack( int n )
{

  std::advance(_rit, -n );

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DeepReverseBeamlineIterator::DeepReverseBeamlineIterator( beamline& x ): 
  _subrit(0) 
{   
    _rit    = x._theList.rbegin();
    _rbegin = x._theList.rbegin();
    _rend   = x._theList.rend();

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DeepReverseBeamlineIterator::~DeepReverseBeamlineIterator()
{ 

      if (_subrit) delete _subrit;  


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bmlnElmnt* DeepReverseBeamlineIterator::operator++( int )
{
  bmlnElmnt* ret = 0;

  if( _subrit) 
  {
    ret = (*_subrit)++;

    if( !ret ) 
    {
      _subrit = 0;
      ret = (*this)++;
    }
  }
  else 
  {
    ret = (_rit == _rend) ? 0 : *_rit;

    if( ret ) 
    {
      ++_rit;
      if(  strcmp( ret->Type(), "beamline" ) == 0 ) 
      {
        _subrit = new  DeepReverseBeamlineIterator( *static_cast<beamline*>(ret) );
        ret = (*this)++;
      }
    }    
  }

  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DeepReverseBeamlineIterator::reset()
{

  _subrit = 0;
  _rit = _rbegin;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
