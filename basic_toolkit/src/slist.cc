/* 
Implementation of the class slist, as detailed in Section 7.3.2
of The C++ Programming Language, by Stroustrup.
*/

#include <stdio.h>
#include <stdlib.h>
#include "slist.h"

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

slist::slist( const slist& x )
{
ent w;
last = 0;
slist_iterator getNext( x );
while((  w = getNext()  )) append( w );
owner = 0;

#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}

void slist::insert( ent a) {
if( last )
  last->next = new slink( a, last->next );
else {
  last = new slink( a, 0 );
  last->next = last;
  }
}

void slist::append( ent a ) {
if( last )
  last = last->next = new slink( a, last->next );
else {
  last = new slink( a, 0 );
  last->next = last;
  }
}

ent slist::remove( ent NeedToRemove ) {

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
    ent    curEn = 0;

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

    // If the desired ent is not found, then...
    return 0;
 }

 // On the other hand, if the list is empty ...
 else return 0;
}

ent slist::get() {
if ( last == 0 ) return 0;   // In case the list is empty.
// if ( last == 0 ) { 
//   printf( "ERROR:SLIST> get from empty slist." );
//   exit(0);
//   }
slink* f = last->next;
ent r = f->e;
if ( f == last )
  last = 0;
else
  last->next = f->next;
delete f;
return r;
}

slist& slist::operator=( const slist& x )
{
slist_iterator getNext( x );
ent p;
clear();
while((  p = getNext()  )) append( p );
return *this;
}

ent slist::operator[]( int n )
{
slist_iterator getNext( *this );
int i = 0;
ent p = 0;
while( ( i++ <= n ) && ( p = getNext() ) ) ;
return p;
}

int slist::size() const
{
slist_iterator getNext( *this );
int i = 0;
ent p;
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


void slist::zap(){
slink* l;
slink* ll;
l = last;
if( l == 0 ) return;
do {
  ll = l;
  l = l->next;
  delete ll->e;
  delete ll;
  }
while( l != last );
delete l->e;
delete l;
last = 0;
}

ent slist_iterator::operator()() {
ent ret = 0;
if( cs->last )
{ 
  ret = ce ? ( ce = ce->next )->e : 0;
  if( ce == cs->last ) ce = 0;
}
return ret;
}


ent slist_looper::operator()() {
ent ret = 0;
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
