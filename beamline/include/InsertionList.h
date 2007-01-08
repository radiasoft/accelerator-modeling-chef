/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      InsertionList.h
******  Version:   2.1
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


// Note: This file has been removed from bmlnElmnt.h
// in order to improve separability.   - lpjm  12/9/03

#ifndef INSERTIONLIST_H
#define INSERTIONLIST_H

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/dlist.h>
#include <beamline/Particle.h>


struct DLLEXPORT InsertionListElement {
  double     s;  // Position (design orbit length)
  bmlnElmnt* q;  // Element to be inserted.
  InsertionListElement( double     x = 0.0, /* s */
                        bmlnElmnt* p = 0    /* q */ 
                      ) { s = x; q = p; }
};


class DLLEXPORT InsertionList : private dlist {
private:
  double    _smax;
  Particle* _pPtr;

public:
  InsertionList( double /* momentum [GeV/c] */ = 1000.0 );
  // By default, a proton will be used as the internal particle.
  InsertionList( const Particle& );
  // Argument must have the correct momentum and, ideally,
  // be on the fiducial reference orbit.
  InsertionList( const InsertionList& );
  ~InsertionList();

  void Append( const InsertionListElement& );
  void Append( const InsertionListElement* );
  void Insert( const InsertionListElement& );
  void Insert( const InsertionListElement* );

  void MergeUnique( InsertionList& );
  void MergeAll   ( InsertionList& );

  InsertionListElement* Get();   // Iterator; removes elements from list
  InsertionListElement* operator()( const int& ) const;
  InsertionList& operator=( const InsertionList& );
  int Size();
  void Clear();   // Preserves the InsertionListElements

  Particle* clonedParticlePtr();
  // Will instatiate a particle on the heap. 
  // Invoker must take responsibility for deleting it.

  friend std::ostream& operator<<( std::ostream&, const InsertionList& );
};


inline Particle* InsertionList::clonedParticlePtr()
{ 
  return _pPtr->Clone();
}


inline InsertionListElement* InsertionList::Get()
{
  return (InsertionListElement*) dlist::get();
}


inline InsertionListElement* InsertionList::operator()( const int& n ) const
{
  return (InsertionListElement*) dlist::operator[]( n );
}

#endif // INSERTIONLIST_H
