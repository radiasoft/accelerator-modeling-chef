/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      InsertionList.cc
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

#include <PhysicsConstants.h>
#include <bmlnElmnt.h>
#include <InsertionList.h>

using namespace std;

InsertionList::InsertionList( double p )
: smax( -1.0e-20 ), prtn( sqrt( p*p + PH_NORM_mp*PH_NORM_mp ) )
{
}

InsertionList::InsertionList( const InsertionList& x )
: dlist( (dlist&) x )
{
  smax = x.smax;
}

InsertionList::~InsertionList()
{
}


void InsertionList::Append( const InsertionListElement& x )
{
  Append( &x  );
}

void InsertionList::Append( const InsertionListElement* x )
{
  if( x->q->OrbitLength( prtn ) != 0.0 ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           "void InsertionList::Append( const InsertionListElement* x )", 
           "This version can only handle zero length elements." ) );
  }

  if( x->s > smax ) {
    dlist::append( (void*) x );
    smax = x->s;
    return;
  }
  ostringstream uic;
  uic  << "Ordering is not correct: " << smax << " < " << (x->s);
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
         "void InsertionList::Append( const InsertionListElement* x )", 
         uic.str().c_str() ) );
}

void InsertionList::Insert( const InsertionListElement& x )
{
  Insert( &x  );
}

void InsertionList::Insert( const InsertionListElement* x )
{
  if( x->q->OrbitLength( prtn ) != 0.0 ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           "void InsertionList::Insert( const InsertionListElement* x )", 
           "This version can only handle zero length elements." ) );
  }

  if( IsEmpty() ) {
    dlist::insert( (void*) x  );
    smax = x->s;
    return;
  }

  InsertionListElement* f = (InsertionListElement*) firstInfoPtr();
  if( f->s <= x->s ) {
    ostringstream uic;
    uic  << "Ordering is not correct. " << (f->s) << " <= " << (x->s);
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__,
           "void InsertionList::Insert( const InsertionListElement* x )", 
           uic.str().c_str() ) );
  }

  dlist::insert( (void*) x  );
  return;
}

void InsertionList::MergeUnique( InsertionList& x )
{
  if( x.Size() <= 0 ) return;

  dlist_iterator gx(  (dlist&) x );
  InsertionListElement* p_ile_x;

  while((  p_ile_x = (InsertionListElement*) gx()  )) {
    dlist_iterator gt( *(dlist*) this );
    InsertionListElement* p_ile_t;
    char inserted = 0;
    while((  p_ile_t = (InsertionListElement*) gt()  )) {
      if( fabs( p_ile_t->s - p_ile_x->s ) < 0.5 /* meters */ ) {
        // Do not insert
        inserted = 1;
        break;
      }
      if( p_ile_t->s > p_ile_x->s ) {
	dlist::putAbove( (void*) p_ile_t, (void*) p_ile_x );
        inserted = 1;
        break;
      }
    }
    if( !inserted ) Append( p_ile_x );
  }
}

void InsertionList::MergeAll( InsertionList& x )
{
  if( x.Size() <= 0 ) return;

  dlist_iterator gx(  (dlist&) x );
  InsertionListElement* p_ile_x;

  while((  p_ile_x = (InsertionListElement*) gx()  )) {
    dlist_iterator gt( *(dlist*) this );
    InsertionListElement* p_ile_t;
    char inserted = 0;
    while((  p_ile_t = (InsertionListElement*) gt()  ))
      if( p_ile_t->s > p_ile_x->s ) {
	dlist::putAbove( (void*) p_ile_t, (void*) p_ile_x );
        inserted = 1;
        break;
      }
    if( !inserted ) {
      dlist::append( (void*) p_ile_x );
      smax = p_ile_x->s;
    }
  }
}

InsertionList& InsertionList::operator=( const InsertionList& x )
{
  smax = x.smax;
  dlist::operator=( (dlist&) x );
  return *this;
}

int InsertionList::Size()
{
  return dlist::size();
}

void InsertionList::Clear()
{
  dlist::clear();
  smax = -1.0e-20;
}
ostream& operator<<(ostream& os, const InsertionList& x)
{
  dlist_iterator getNext((dlist&)x);
  InsertionListElement* ple;
  os << "InsertionList Dump " << endl;
  while((  ple = (InsertionListElement*)getNext()  )) {
    os << "s = " << ple->s << " " << ple->q->Name();
    os << " " << ple->q->Type() << endl;
  }
  return (os << endl);
}
