/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.3                  
******                                    
******  File:      dlist.h
******                                                                
******  Copyright (c) Universities Research Association, Inc / Fermilab.    
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
******  Revision History:
******
******  March 2005 : ostiguy@fnal.gov
******  
******  - introduced specialized pool allocators.
******                                                              
**************************************************************************
*************************************************************************/

#ifndef DLIST_HXX
#define DLIST_HXX

#include <basic_toolkit/globaldefs.h>

#include <gms/FastAllocator.h>
#include <gms/FastPODAllocator.h>

typedef char (*DLIST_CRITFUNC)( void* );

class DLLLOCAL dlink : public gms::FastPODAllocator<dlink> {
private:
  dlink* next;
  dlink* prev;
  void* e;
public:
  dlink( void* a, dlink* p, dlink* q ):  next(q), prev(p), e(a) { }
 ~dlink () {
  }

  void putAbove( dlink* );
  void putBelow( dlink* );
  dlink* nextPtr() {return next;}
  dlink* prevPtr() {return prev;}
  void* info() {return e;}

  void replace( void* x ) { e = x; }

  friend class dlist;
  friend class dlist_iterator;
  friend class dlist_traversor;
  friend class dlist_looper;
  friend class dlist_reverseIterator;

};

class DLLEXPORT dlist: public gms::FastAllocator {

private:
  dlink* last;
  char   owner;

public:
  dlist( char x = 0 ): last(0), owner(x) {} 

  dlist( void* a, char x = 0   ) { 
    last = new dlink(a,0,0); 
    last->prev = last; 
    last->next = last; 
    owner = x; 
  }

  dlist( const dlist& );

  virtual ~dlist() { clear();}

  bool contains( const void* ) const;

  void* remove( dlink* );            // Removes the single argument.
  char remove( void* );              // Ditto 
  void* remove( int );               // Returns the ent removed from the dlist
                                   // Failure -> returns null pointer
  dlist remove( void*, void* );        // Removes everything between its arguments
                                   // (exclusively) and returns the removed chain.
  dlist remove( int, int );
  void insert( void* a );
  void append( void* a );
  char putAbove( const void* a, const void* b );
                                   // Installs b above (before) a in the list
  char putBelow( const void* a, const void* b );
                                   // Installs b below (after)  a in the list
  int replaceOne( const void* a, const void* b );
                                   // Replaces a with b. Will do so even
                                   // if b is void. Performs a level one
                                   // search: that is, will not search
                                   // through sublists.
                                   // Return values:
                                   // 0 everything went as planned
                                   // 1 first argument was not found
                                   
  void* get();

  int  startAt( const void*, int=1 );
  void riskStartAt( const dlink* );
                                   // Does not check whether the argument
                                   // is in the list.
  char Owns();
  void DoesOwn()    { owner = 1; }
  void DoesNotOwn() { owner = 0; }

  void* operator[]( int ) const;
  int size() const;
  char IsEmpty() { return last == 0; }
  void clear();
  // void zap();

  char SetLast( void* );
  char SetFirst( void* );
  char SetLast( DLIST_CRITFUNC );
  char SetFirst( DLIST_CRITFUNC );
  dlink* lastPtr() {return last;}
  void* lastInfoPtr() { if( last ) return last->e;
                      else       return 0;
                    }
  void* firstInfoPtr() { if( last ) return last->next->e;
                       else       return 0;
                    }
  dlist& operator=( const dlist& );

  // Friends ............
  friend class dlist_iterator;
  friend class dlist_traversor;
  friend class dlist_looper;
  friend class dlist_reverseIterator;
} ;

class DLLEXPORT dlist_iterator {
  dlink* ce;
  dlist* cs;

public:
  dlist_iterator( const dlist& s ) { 
    cs = (dlist*) &s; 
    ce = cs->last; 
  }

  // A copy constructor. Use with caution.
  dlist_iterator( const dlist_iterator& x ) {
    ce = x.ce;
    cs = x.cs;
  }

  ~dlist_iterator () { }

  void* operator()();
  void* current();  
  void  Reset( const dlist& s ) { cs = const_cast<dlist*>(&s); ce = cs->last; }
  void  Reset() { ce = cs->last; }
  void  GoBack( int = 1 );
  void  Terminate() { ce = 0; }
  bool  isFinished() { return ( 0 == ce ); }
} ;

class DLLEXPORT dlist_looper {
  dlink* ce;
  dlist* cs;
public:
  dlist_looper( dlist& s ) { cs = &s; ce = cs->last; }
  dlist_looper( const dlist_looper& x ) { cs = x.cs; ce = x.ce; }
  void* operator()();
  void Reset( const dlist& s ) { cs = const_cast<dlist*>(&s); ce = cs->last; }
  void Reset() { ce = cs->last; }
} ;

class DLLEXPORT dlist_traversor {
  dlink* ce;
  dlist* cs;
public:
  dlist_traversor( const dlist& s ) { cs = const_cast<dlist*>(&s); ce = cs->last; }
  dlist_traversor( const dlist_traversor& x ) { cs = x.cs; ce = x.ce; }
  dlink* operator()();
  void Reset( const dlist& s ) { cs = (dlist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
  void GoBack( int = 1 );
  void Terminate() { ce = 0; }
} ;

class DLLEXPORT dlist_reverseIterator {
  dlink* ce;
  dlist* cs;
public:
  dlist_reverseIterator( dlist const& s )                 { cs = const_cast<dlist*>(&s); ce = cs->last; }
  dlist_reverseIterator( dlist_reverseIterator const& x ) { cs = x.cs; ce = x.ce; }
  void* operator()();
  void Reset( const dlist& s ) { cs = const_cast<dlist*>(&s); ce = cs->last; }
  void Reset() { ce = cs->last; }
  void Terminate() { ce = 0; }
} ;

inline char dlist::Owns() { return owner; }

#endif // DLIST_HXX
