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
******  REVISION HISTORY:
******
******  October 2006:   Jean-Francois Ostiguy
******                  ostiguy@fnal.gov
******             
******  - modified code to make use of new beamline embedded std::list<> member.
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


#include <typeinfo>
#include <basic_toolkit/iosetup.h>
#include <beamline/beamline.h>
#include <beamline/BeamlineIterator.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

// --------------------------------------------------------------------------
// Forward iterator
// --------------------------------------------------------------------------



BeamlineIterator::BeamlineIterator( beamline& x ) : m_bml(x) {
  m_it =  x._theList.begin();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineIterator::BeamlineIterator( BeamlineIterator const& x ): 
m_it( x.m_it ), m_bml(m_bml) { }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineIterator::~BeamlineIterator() 
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bmlnElmnt* BeamlineIterator::operator++( int )
{
  bmlnElmnt* ret = 0;

  if ( m_it == m_bml._theList.end() ) return 0;
 
  ret = *m_it;
  ++m_it;

  return ret;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineIterator::reset()
{
  m_it = m_bml._theList.begin();

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool BeamlineIterator::isFinished() 	 
{ 	 
   return ( m_it == m_bml._theList.end() );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineIterator::goBack( int n )
{
  std::advance(m_it, -n );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


DeepBeamlineIterator::DeepBeamlineIterator( beamline & x ): m_subit(0), m_bml(x){
  m_it = x._theList.begin();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


DeepBeamlineIterator::~DeepBeamlineIterator()
{ 

  if (m_subit) delete m_subit;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bmlnElmnt* DeepBeamlineIterator::operator++( int )
{

  bmlnElmnt* ret = 0;

  if( m_subit ) 
  {
    ret = (*m_subit)++; 

    if( !ret ) 
    {
      m_subit = 0;
         ret = (*this)++;
    }
  }
  else 
  {

    ret = ( m_it ==  m_bml._theList.end() ) ? 0 : *m_it; 

    if( ret ) 
    {
      ++m_it;      


      if(  typeid(*ret) == typeid(beamline) )  
      {
        m_subit = new  DeepBeamlineIterator( *static_cast<beamline*>(ret) );
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


  m_subit   = 0;
  m_it       = m_bml._theList.begin();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
// *** Reverse iterator ***
//
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


ReverseBeamlineIterator::ReverseBeamlineIterator( beamline& x ): m_bml(x) {
  m_rit = x._theList.rbegin();
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


ReverseBeamlineIterator::ReverseBeamlineIterator( ReverseBeamlineIterator const& x ): 
m_rit(x.m_rit), m_bml(x.m_bml){}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ReverseBeamlineIterator::~ReverseBeamlineIterator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bmlnElmnt* ReverseBeamlineIterator::operator++( int )
{
 
  bmlnElmnt* ret = 0;

  if ( m_rit ==  m_bml._theList.rend() ) return 0;
 
  ret = *m_rit;  ++m_rit;

  return ret;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ReverseBeamlineIterator::reset()
{
  m_rit = m_bml._theList.rbegin();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ReverseBeamlineIterator::goBack( int n )
{

  std::advance(m_rit, -n );

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DeepReverseBeamlineIterator::DeepReverseBeamlineIterator( beamline& x ): 
  m_subrit(0),   m_bml(x)
{   
    m_rit    = x._theList.rbegin();

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

DeepReverseBeamlineIterator::~DeepReverseBeamlineIterator()
{ 

      if (m_subrit) delete m_subrit;  


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bmlnElmnt* DeepReverseBeamlineIterator::operator++( int )
{
  bmlnElmnt* ret = 0;

  if( m_subrit) 
  {
    ret = (*m_subrit)++;

    if( !ret ) 
    {
      m_subrit = 0;
      ret = (*this)++;
    }
  }
  else 
  {
    ret = (m_rit == m_bml._theList.rend()) ? 0 : *m_rit;

    if( ret ) 
    {
      ++m_rit;
      if(  typeid(*ret) == typeid(beamline) ) 
      {
        m_subrit = new  DeepReverseBeamlineIterator( *static_cast<beamline*>(ret) );
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

  m_subrit = 0;
  m_rit = m_bml._theList.rbegin();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
