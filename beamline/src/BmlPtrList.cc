/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                 
******                                    
******  File:      BmlPtrList.cc
******                                                                
******  Copyright (c) 2003 Universities Research Association, Inc.    
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


#include "bmlnElmnt.h"
#include "PhysicsConstants.h"

BmlPtrList::BmlPtrList()
{
}

BmlPtrList::BmlPtrList( const BmlPtrList& x )
: dlist( (dlist&) x )
{
}

BmlPtrList::~BmlPtrList()
{
}


void BmlPtrList::append( bmlnElmnt& x )
{
  this->BmlPtrList::append( &x  );
}

void BmlPtrList::append( bmlnElmnt* x )
{
  if( 0 != x ) {
    this->dlist::append( (void*) x );
  }
  else {
    cerr << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** void BmlPtrList::append( bmlnElmnt* x )"
            "\n*** ERROR *** Attempt to append null pointer."
         << endl;
  }
}


void BmlPtrList::insert( bmlnElmnt& x )
{
  this->BmlPtrList::insert( &x  );
}


void BmlPtrList::insert( bmlnElmnt* x )
{
  if( 0 != x ) {
    this->dlist::insert( (void*) x );
  }
  else {
    cerr << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** void BmlPtrList::insert( bmlnElmnt* x )"
            "\n*** ERROR *** Attempt to append null pointer."
         << endl;
  }
}


BmlPtrList& BmlPtrList::operator=( const BmlPtrList& x )
{
  this->dlist::operator=( (dlist&) x );
  return *this;
}


bmlnElmnt* BmlPtrList::get()
{
  return (bmlnElmnt*) (this->dlist::get());
}


bmlnElmnt* BmlPtrList::operator()( const int& n ) const
{
  return (bmlnElmnt*) (this->dlist::operator[]( n ));
}


int BmlPtrList::size() const
{
  return this->dlist::size();
}


void BmlPtrList::clear()
{
  this->dlist::clear();
}
