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
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
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
#include <basic_toolkit/iosetup.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/BmlPtrList.h>
#include <basic_toolkit/PhysicsConstants.h>

using FNAL::pcout;
using FNAL::pcerr;

BmlPtrList::BmlPtrList()
{
}

BmlPtrList::BmlPtrList( const BmlPtrList& x )
: dlist( x )
{
}

BmlPtrList::~BmlPtrList()
{
}



void BmlPtrList::append( bmlnElmnt* x )
{
  if( 0 != x ) {
    this->dlist::append( (void*) x );
  }
  else {
    (*pcerr) << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** void BmlPtrList::append( bmlnElmnt* x )"
            "\n*** ERROR *** Attempt to append null pointer."
         << std::endl;
  }
}



void BmlPtrList::insert( bmlnElmnt* x )
{
  if( 0 != x ) {
    this->dlist::insert( (void*) x );
  }
  else {
    (*pcerr) << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** void BmlPtrList::insert( bmlnElmnt* x )"
            "\n*** ERROR *** Attempt to append null pointer."
         << std::endl;
  }
}


BmlPtrList& BmlPtrList::operator=( const BmlPtrList& x )
{
  this->dlist::operator=( x );
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
