/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.2
******                                    
******  File:      dlist.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <iostream>
using std::cerr;
using std::endl;

#include "dlist.h"

// ================================================================
//      Global variables
//

#ifdef OBJECT_DEBUG
int dlist::objectCount = 0;
int dlink::objectCount = 0;
int dlist_iterator::objectCount = 0;
#endif

// ================================================================
//      Constructors and the destructor ...
//


dlist::dlist(  const dlist& x )
{
  void* w;
  last = 0;
  dlist_iterator getNext( x );
  while((  w = getNext()  )) append( w );
  owner = 0;
  
  #ifdef OBJECT_DEBUG
    objectCount++;
  #endif
}

void dlink::putAbove( dlink* a ) 
{
  a -> prev = prev;
  a -> next = this;
  prev -> next = a;
  prev = a;
}

dlist& dlist::operator=( const dlist& x )
{
  dlist_iterator getNext( x );
  void* p;
  clear();
  while((  p = getNext()  )) append( p );
  return *this;
}
  
void dlink::putBelow( dlink* a ) {
  a -> prev = this;
  a -> next = next;
  next -> prev = a;
  next = a;
}

bool dlist::contains( const void* x ) const
{
  bool ret = false;
  dlist_iterator getNext( *this );
  void* q;
  while((  !ret && (q = getNext())  )) 
  { ret = ( q == x );
  }
  return ret;
}


void* dlist::remove( dlink* p ) 
{
  void* a;
  
  // NOTE:  This method assumes that *p is actually in the dlist;
  //        it does not double-check to see that this is true.
  //        This method purposely does not use dlist_iterator
  
  // If p is the first element in the list ...
  if ( p == last -> next ) {
    a = get();
    return a;
    }
  
  // If p is the last element in the list ...
  if ( p == last ) {
    // .. and there is only one element ..
    if( last->prev == last ) {
      a = get();
      return a;
      }
    // .. or there is more then one element ..
    last = p->prev;
    }
  
  // If p is neither the first nor the last element
  // in the list, or p is the last element in a list
  // with more than one element ..
  ( p -> prev ) -> next  =  p -> next;
  ( p -> next ) -> prev  =  p -> prev;
  a = p -> e;
  delete p;
  return a;
}


char dlist::remove( void* a )
{
  dlist_traversor getNext( *this );
  dlink* p;
  void* w;
  while((  p = getNext()  ))  if( p->e == a ) {
			    w = remove( p );
			    return 1;
			    }
  return 0;
} // End function char dlist::remove( void* a )


void* dlist::remove( int n )
{
  dlist_traversor getNext( *this );
  dlink* p;
  void* w;
  int i = 0;
  while((  p = getNext()  ))  if( i++ == n ) {
			    w = remove( p );
			    return w;
			    }
  return 0;
} // End function char dlist::remove( void* a )


dlist dlist::remove( void* a, void* b )
{
  dlist  ret;
  dlink* p;
  dlink* pr;
  void*  w;
  void*  wd;
  void*  y;
  dlist_traversor getNext ( *this );
  
  do { p = getNext();
       w = p->e;
     } while ( p != 0 && w != a && w != b );
  
  if( p == 0 ) {
    cerr << "\n*** WARNING ***                              \n";
    cerr <<   "*** WARNING *** dlist::remove( void*, void* )    \n";
    cerr <<   "*** WARNING *** Reached the end of the list  \n";
    cerr <<   "*** WARNING *** without finding markers.     \n";
    cerr <<   "*** WARNING ***                              \n";
    cerr << endl;
    return *this;
  }
  
  if( w == a ) y = b;
  else  {      y = a;
               if( w != b ) {
                 cerr << "\n*** WARNING*** IMPOSSIBLE ERROR!";
                 cerr << "\n*** WARNING *** dlist::remove( void*, void* )";
                 cerr << endl;
                 return *this;
               }
  }
  
  p = getNext();
  w = ( pr = p )->e;
  if( w == y || p == 0 ) {
    cerr << "\n*** WARNING ***                                     \n";
    cerr <<   "*** WARNING *** dlist::remove( void*, void* )           \n";
    cerr <<   "*** WARNING *** No elements available for removal.  \n";
    cerr <<   "*** WARNING ***                                     \n";
    cerr << endl;
    return *this;
  }
  
  do {
    p = getNext();
    wd = remove( pr );
    ret.append( w );
    w = ( pr = p )->e;
  } while ( ( p != 0 ) && ( w != y ) );
  
  if( p == 0 ) {
    cerr << "\n*** WARNING ***                                \n";
    cerr <<   "*** WARNING *** dlist::remove( void*, void* )      \n";
    cerr <<   "*** WARNING *** Reached the end of the list.   \n";
    cerr <<   "*** WARNING ***                                \n";
    cerr << endl;
  }
  
  return ret;
} // End function dlist dlist::remove( void* a, void* b )


dlist dlist::remove( int x, int y )     // ??? This is a kludged version. Rewrite!!
{
  void*   a;
  void*   b;
  dlist c;
  a = operator[]( x );
  b = operator[]( y );
  return remove( a, b );
}

void dlist::insert( void* a) 
{
  if( last ) {
    last->next = new dlink( a, last, last->next );
    ((last->next)->next)->prev = last->next;
    }
  else {
    last = new dlink( a, 0, 0 );
    last->next = last;
    last->prev = last;
    }
  }
  
  void dlist::append( void* a ) {
  if( last ) {
    last = last->next = new dlink( a, last, last->next );
    (last->next)->prev = last;
    }
  else {
    last = new dlink( a, 0, 0 );
    last->next = last;
    last->prev = last;
    }
}


char dlist::putAbove( const void* a, const void* b )    // Untested???
{
  char   found;
  dlink* w;
  dlink* z;
  dlist_traversor getNext( *this );
  found = 0;
  while((  w = getNext()  ))
    if( w->e == a ) {
      found = 1;
      break;
      }
  if( !found ) return 0;
  z = new dlink( ((void*) b), 0, 0 );
  w->putAbove( z );
  return 1;
}


char dlist::putBelow( const void* a, const void* b )    // Untested???
{
  char   found;
  dlink* w;
  dlink* z;
  dlist_traversor getNext( *this );
  found = 0;
  while((  w = getNext()  ))
    if( w->e == a ) {
      found = 1;
      break;
      }
  if( !found ) return 0;
  z = new dlink( ((void*) b), 0, 0 );
  w->putBelow( z );
  if( w == last ) last = z;
  return 1;
}


int dlist::replaceOne( const void* a, const void* b )
{
  // This recasting nonsense is terminally stupid!
  dlist_traversor dtr( const_cast<const dlist&>(*this) );
  dlink* q;
  while((  0 != (q = dtr())  )) {
    if( a == q->info() ) {
      q->replace( const_cast<void*>(b));
      return 0;
    }
  }
  return 1;
}


void* dlist::get() {
  if ( last == 0 ) return 0;   // In case the list is empty.
  dlink* f = last->next;
  void* r = f->e;
  if ( f == last )
    last = 0;
  else {
    last->next = f->next;
    (last->next)->prev = last;
    }
  delete f;
  return r;
}



int dlist::startAt( const void* x, int n )
{
  dlist_traversor getNext( *(dlist*) this );
  dlink* q;
  int count(0);

  while((  q = getNext()  )) 
  {
    if( q->e == x ) 
    {
      if( (++count) == n ) {
        last = q->prev;
        return 0;
      }
    }
  }

  return 1;
}

void dlist::riskStartAt( const dlink* x )
{
  // Dangerous routine!!
  last = x->prev;
}



void* dlist::operator[]( int n ) const
{
  dlist_iterator getNext( *this );
  int i = 0;
  void* p = 0;
  while( ( i++ <= n ) && ( p = getNext() ) ) ;
  return p;
}


int dlist::size() const
{
  dlist_iterator getNext( *this );
  int i = 0;
  void* p;
  while((  p = getNext()  )) i++;
  return i;
}


void dlist::clear() 
{
  dlink* l = last;
  if( l == 0 ) return;
  do {
    dlink* ll = l;
    l = l->next;
    delete ll;
    }
  while( l != last );
  last = 0;
}

// void dlist::zap()
// {
//   dlink* l;
//   dlink* ll;
//   l = last;
//   if( l == 0 ) return;
//   do {
//     ll = l;
//     l = l->next;
//     delete ll->e;
//     delete ll;
//     }
//   while( l != last );
//   delete l->e;
//   delete l;
//   last = 0;
// }


char dlist::SetLast( void* x ) {   // Warning: not tested!!
  dlink* ce = last;
  char found = 0;
  char done  = 0;
  while( !found && !done ) {
    ce = ce->next;
    found = ( ce->e == x );
    done = ( ce == last );
  }
  if( found ) last = ce;
  return found;
}

char dlist::SetLast( DLIST_CRITFUNC f ) {   // Warning: not tested!!
  dlink* ce = last;
  char found = 0;
  char done  = 0;
  while( !found && !done ) {
    ce = ce->next;
    found = (*f)( ce->e );
    done = ( ce == last );
  }
  if( found ) last = ce;
  return found;
}

char dlist::SetFirst( void* x ) {   // Warning: not tested!!
  dlink* ce = last;
  char found = 0;
  char done  = 0;
  while( !found && !done ) {
    ce = ce->next;
    found = ( ce->e == x );
    done = ( ce == last );
  }
  if( found ) last = ce->prev;
  return found;
}

char dlist::SetFirst( DLIST_CRITFUNC f ) {   // Warning: not tested!!
  dlink* ce = last;
  char found = 0;
  char done  = 0;
  while( !found && !done ) {
    ce = ce->next;
    found = (*f)( ce->e );
    done = ( ce == last );
  }
  if( found ) last = ce->prev;
  return found;
}



void* dlist_iterator::operator()() 
{
  void* ret = 0;
  if( cs->last )
  { 
    ret = ce ? ( ce = ce->next )->e : 0;
    if( ce == cs->last ) ce = 0;
  }
  return ret;
}


void dlist_iterator::GoBack( int n ) 
{
  static int j;
  
  if( n < 1 ) n = 1;
  if( ce == 0 ) ce = cs->last;  // Cancels interference from 
				// dlist_iterator::operator()
  for( j = 0; j < n; j++ ) ce = ce->prev;
}


void* dlist_looper::operator()() 
{
  void* ret = 0;
  if( cs->last )
  {
    ret = ce;
    if( ret != 0 ) {
      ret = ( ce = ce->next )->e;
      if( ce == cs->last ) ce = 0;
      }
    else {
      ce = cs->last;
      }
  }
  return ret;
}

dlink* dlist_traversor::operator()() 
{
  dlink* ret = 0;
  if( cs->last )
  {
    if( ce != 0 ) ret = ( ce = ce->next );
    else          ret = 0;
    if( ce == cs->last ) ce = 0;
  }
  return ret;
}

void dlist_traversor::GoBack( int n ) 
{
  static int j;
  
  if( n < 1 ) n = 1;
  if( ce == 0 ) ce = cs->last;  // Cancels interference from 
				// dlist_traversor::operator()
  for( j = 0; j < n; j++ ) ce = ce->prev;
}


void* dlist_reverseIterator::operator()() 
{
  if( ce == 0 ) return 0;
  void* ret = 0;
  if( cs->last )
  {
    ret = ce->e;
    if( ( ce = ce->prev ) == cs->last ) ce = 0;
  }
  return ret;
}


/*
 *   Pool functions written by Michael Allen
 *   February, 1993
 */

#ifdef POOLED

_dlink_Shell* _dlink_Pool::top = 0;
_dlink_Pool::_dlink_Pool()
{
  register int i = _dlink_BLOCK_COUNT;
  _dlink_Shell* p = slot;
  _dlink_Shell* q;
  while (--i)
    {
      q = p++;
      q->next = p;
    }
  p->next = top;
  top = slot;
}

#endif //  POOLED
