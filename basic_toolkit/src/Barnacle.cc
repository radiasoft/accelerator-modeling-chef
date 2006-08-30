/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      Barnacle.cc
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <basic_toolkit/Barnacle.h>


BarnacleList::BarnacleList() {}

BarnacleList::~BarnacleList() {
 dlist_iterator getNext( theList );
 Barnacle* p;
 while( 0 != (  p = (Barnacle*) getNext() )) delete p;
}
 
void BarnacleList::append( const Barnacle* x ) {
 theList.append( (void*) x );
}


void BarnacleList::insert( const Barnacle* x ) {
 theList.insert( (void*) x );
}


void BarnacleList::append( const char* ident, const BarnacleData* w ) {
 theList.append( (void*) new Barnacle( ident, w ) );
}


void BarnacleList::insert( const char* ident, const BarnacleData* w ) {
 theList.insert( (void*) new Barnacle( ident, w ) );
}


char BarnacleList::eraseFirst( const char* ident ) {
 dlist_iterator getNext( theList );
 Barnacle* p;
 while( 0 != (  p = (Barnacle*) getNext()  )) {
  if( strcmp( ident, p->id ) == 0 ) {
   theList.remove( (void*) p );   // ??? This is REALLY stupid!
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
    theList.remove( (void*) p );   // ??? This is REALLY stupid!
    delete p;
   }
  }
 }
 else {
  while( 0 != (  p = (Barnacle*) getNext()  )) {
   getNext.GoBack();
   theList.remove( (void*) p );   // ??? This is REALLY stupid!
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

Barnacle* BarnacleList::lift( const char* ident, int n ) {
 dlist_iterator getNext( theList );
 int i = 1;
 Barnacle* p;
 while( 0 != (  p = (Barnacle*) getNext()  )) 
  if( strcmp( ident, p->id ) == 0 ) {
   if( n == i++ ) {
    theList.remove( (void*) p );   // ??? This is REALLY stupid!
    return p;
   }
  }
 return 0;
}

Barnacle::Barnacle( const char* s, const BarnacleData* e ) {
 id = new char[ strlen(s) + 1 ];
 strcpy( id, s );
 info = (BarnacleData*) e;

}

Barnacle::~Barnacle() {
 delete [] id;
 delete info;

}
