/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.3
******                                    
******  File:      slist.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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
*****
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
Implementation of the class slist, as detailed in Section 7.3.2
of The C++ Programming Language, by Stroustrup.
*/

#include <stdio.h>
#include <stdlib.h>
#include <basic_toolkit/slist.h>

// ================================================================
//      Global variables
//

#ifdef OBJECT_DEBUG
int slist::objectCount = 0;
int slink::objectCount = 0;
int slist_iterator::objectCount = 0;
#endif

// ================================================================
//      Constructors and the destructor ...
//

slist::slist( slist const& x )
{
void* w;
last = 0;
slist_iterator getNext( x );
while((  w = getNext()  )) append( w );
owner = 0;

}

void slist::insert( void* a) {
if( last )
  last->next = new slink( a, last->next );
else {
  last = new slink( a, 0 );
  last->next = last;
  }
}

void slist::append( void* a ) {
if( last )
  last = last->next = new slink( a, last->next );
else {
  last = new slink( a, 0 );
  last->next = last;
  }
}


bool slist::contains( const void* x ) const
{
  bool ret = false;
  slist_iterator getNext( *this );
  void* q;
  while((  !ret && (q = getNext())  )) 
  { ret = ( q == x );
  }
  return ret;
}


void* slist::remove( void* NeedToRemove ) {

  // If the list is not empty ...
  if ( last ) {

    // If there is only one slink ...
    if( last->next == last ) {
      if( last->e == NeedToRemove ) return get();  
      // slist::get() is a function known to work properly.
    }

    // Otherwise ...
    slink* curEl = last;
    slink* nxtEl = last;
    void*  curEn = 0;

    do {
      nxtEl = curEl->next;
      curEn = curEl ? nxtEl->e : 0;  
      // ??? Why this way???
      // ??? Why not simply: curEn = nxtEl->e;

      if ( curEn == NeedToRemove ) {
        curEl->next = nxtEl->next;
        if( nxtEl == last ) last = curEl;
        delete nxtEl;
        return curEn;
      }
      curEl        = nxtEl;
    } while ( curEl != last );

    // If the desired void* is not found, then...
    return 0;
 }

 // On the other hand, if the list is empty ...
 else return 0;
}

void* slist::get() {
if ( last == 0 ) return 0;   // In case the list is empty.
// if ( last == 0 ) { 
//   printf( "ERROR:SLIST> get from empty slist." );
//   exit(0);
//   }
slink* f = last->next;
void* r = f->e;
if ( f == last )
  last = 0;
else
  last->next = f->next;
delete f;
return r;
}

slist& slist::operator=( slist const& x )
{
slist_iterator getNext( x );
void* p;
clear();
while((  p = getNext()  )) append( p );
return *this;
}

void* slist::operator[]( int n )
{
slist_iterator getNext( *this );
int i = 0;
void* p = 0;
while( ( i++ <= n ) && ( p = getNext() ) ) ;
return p;
}

int slist::size() const
{
slist_iterator getNext( *this );
int i = 0;
const void* p;
while((  p = getNext()  )) i++;
return i;
}


void slist::clear() {
slink* l = last;
if( l == 0 ) return;
do {
  slink* ll = l;
  l = l->next;
  delete ll;
  } 
while( l != last );
last = 0;
}


// void slist::zap(){
// slink* l;
// slink* ll;
// l = last;
// if( l == 0 ) return;
// do {
//   ll = l;
//   l = l->next;
//   delete ll->e;
//   delete ll;
//   }
// while( l != last );
// delete l->e;
// delete l;
// last = 0;
// }

void* slist_iterator::operator()() {
void* ret = 0;
if( cs->last )
{ 
  ret = ce ? ( ce = ce->next )->e : 0;
  if( ce == cs->last ) ce = 0;
}
return ret;
}


void* slist_looper::operator()() {
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

slink* slist_traversor::operator()() {
slink* ret = 0;
if( cs->last )
{
  if( ce != 0 ) ret = ( ce = ce->next );
  else          ret = 0;
  if( ce == cs->last ) ce = 0;
}
return ret;
}
