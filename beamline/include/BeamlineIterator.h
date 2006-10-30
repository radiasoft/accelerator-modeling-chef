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
******  REVISION HISTORY:
******
******  October 2006: Jean-Francois Ostiguy ostiguy@fnal.gov
******
******  - class beamline contains std::list  
******  - beamline iterators now use STL iterators internally
******    Public interface unchanged, old behavior emulated. 
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

class beamline;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT BeamlineIterator
{

public:

  BeamlineIterator( beamline              & );
  BeamlineIterator( BeamlineIterator const& );
 ~BeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();
  void goBack( int = 1 );
  bool isFinished();

private:
  
  bool                            m_reset;  // needed to emulate position before begin()
  std::list<bmlnElmnt*>::iterator m_it;
  beamline&                       m_bml;
};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



class DLLEXPORT  DeepBeamlineIterator
{
public:

  DeepBeamlineIterator( beamline & );
 ~DeepBeamlineIterator();

  bmlnElmnt* operator++( int );            // postfix increment
  void reset();

  protected:

  bool                            m_reset;  // needed to emulate position before begin()
  std::list<bmlnElmnt*>::iterator m_it;
  beamline&                       m_bml;
  DeepBeamlineIterator*           m_subit;

 private:

  DeepBeamlineIterator( DeepBeamlineIterator const& ); // forbidden

};


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


class DLLEXPORT  ReverseBeamlineIterator
{
public:

  ReverseBeamlineIterator( beamline & );
  ReverseBeamlineIterator( ReverseBeamlineIterator const& );
 ~ReverseBeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();
  void goBack( int = 1 );

private:

  bool                                    m_reset;  // needed to emulate position before begin()
  std::list<bmlnElmnt*>::reverse_iterator m_rit;
  beamline&                               m_bml;

};

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

class DLLEXPORT  DeepReverseBeamlineIterator
{
public:

  DeepReverseBeamlineIterator( beamline & );
 ~DeepReverseBeamlineIterator();

  bmlnElmnt* operator++( int );    // postfix increment
  void reset();

private:

  bool                                    m_reset;  // needed to emulate position before begin()
  std::list<bmlnElmnt*>::reverse_iterator m_rit;
  beamline&                               m_bml;
  DeepReverseBeamlineIterator*            m_subrit;
  DeepReverseBeamlineIterator( DeepReverseBeamlineIterator const& ); 
};


#endif // BEAMLINE_ITERATOR
