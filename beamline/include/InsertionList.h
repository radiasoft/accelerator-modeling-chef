/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   1.0
******                                    
******  File:      InsertionList.h
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


// Note: This file has been removed from bmlnElmnt.h
// in order to improve separability.   - lpjm  12/9/03

#ifndef INSERTIONLIST_H
#define INSERTIONLIST_H

#include "Particle.h"

struct InsertionListElement {
  double     s;  // Position (design orbit length)
  bmlnElmnt* q;  // Element to be inserted.
  InsertionListElement( double     x = 0.0, /* s */
                        bmlnElmnt* p = 0    /* q */ 
                      ) { s = x; q = p; }
};


class InsertionList : private dlist
{
private:
  double smax;
  Proton prtn;
public:
  InsertionList( double /* momentum [GeV/c] */ = 1000.0 );
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
  Proton GetParticle() { return prtn; }
  friend std::ostream& operator<<( std::ostream&, const InsertionList& );
};

inline 
InsertionListElement* 
InsertionList::Get()
{
  return (InsertionListElement*) dlist::get();
}

inline 
InsertionListElement* 
InsertionList::operator()( const int& n ) const
{
  return (InsertionListElement*) dlist::operator[]( n );
}

#endif // INSERTIONLIST_H
