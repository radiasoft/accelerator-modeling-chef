#include "Barnacle.h"
#include <string.h>
#include <iostream.h>

// ================================================================
//      Global variables
//

#ifdef OBJECT_DEBUG
int Barnacle::objectCount = 0;
int BarnacleList::objectCount = 0;
#endif

// ================================================================

BarnacleList::BarnacleList() {
#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}

BarnacleList::~BarnacleList() {
 dlist_iterator getNext( theList );
 Barnacle* p;
 while( 0 != (  p = (Barnacle*) getNext() )) delete p;
#ifdef OBJECT_DEBUG
  objectCount--;
#endif
}
 
void BarnacleList::append( const Barnacle* x ) {
 theList.append( (ent) x );
}


void BarnacleList::insert( const Barnacle* x ) {
 theList.insert( (ent) x );
}


void BarnacleList::append( const char* ident, const BarnacleData* w ) {
 theList.append( (ent) new Barnacle( ident, w ) );
}


void BarnacleList::insert( const char* ident, const BarnacleData* w ) {
 theList.insert( (ent) new Barnacle( ident, w ) );
}


char BarnacleList::eraseFirst( const char* ident ) {
 dlist_iterator getNext( theList );
 Barnacle* p;
 while( 0 != (  p = (Barnacle*) getNext()  )) {
  if( strcmp( ident, p->id ) == 0 ) {
   theList.remove( (ent) p );   // ??? This is REALLY stupid!
   delete p;
   return 0;   // All OK
  }
 }
 return 1;     // Not OK
}


char BarnacleList::eraseAll( const char* ident ) {
 dlist_iterator getNext( theList );
 Barnacle* p;
 if( ident != 0 ) {
  while( 0 != (  p = (Barnacle*) getNext()  )) {
   if( strcmp( ident, p->id ) == 0 ) {
    getNext.GoBack();
    theList.remove( (ent) p );   // ??? This is REALLY stupid!
    delete p;
   }
  }
 }
 else {
  while( 0 != (  p = (Barnacle*) getNext()  )) {
   getNext.GoBack();
   theList.remove( (ent) p );   // ??? This is REALLY stupid!
   delete p;
  }
 }
 return 0;
}


BarnacleData* BarnacleList::find( const char* ident, int n ) const {
 dlist_iterator getNext( theList );
 int i = 1;
 Barnacle* p;
 while( 0 != (  p = (Barnacle*) getNext()  )) 
  if( strcmp( ident, p->id ) == 0 )
    if( n == i++ ) 
      return p->info;
 return 0;
}

Barnacle* BarnacleList::lift( const char* ident ) {
 dlist_iterator getNext( theList );
 Barnacle* p;
 while( 0 != (  p = (Barnacle*) getNext()  )) 
  if( strcmp( ident, p->id ) == 0 ) {
   theList.remove( (ent) p );   // ??? This is REALLY stupid!
   return p;
  }
 return 0;
}

Barnacle::Barnacle( const char* s, const BarnacleData* e ) {
 id = new char[ strlen(s) + 1 ];
 strcpy( id, s );
 info = (BarnacleData*) e;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

Barnacle::~Barnacle() {
 delete [] id;
 delete info;

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}
