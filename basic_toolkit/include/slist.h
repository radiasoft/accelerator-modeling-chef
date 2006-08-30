/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      slist.h
******  Version:   4.4
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
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
****** Revision History:
******
****** March 2005: ostiguy@fnal.gov
******  
****** Memory management improvements. Pooled allocation.
******
**************************************************************************
*************************************************************************/

/*************************************************************************
**************************************************************************
******                                                               *****
******  Leo Michelotti                                               *****
******                                                               *****
******  Taken from Stroustrup's book,                                *****
******  The C++ Programming Language                                 *****
******                                                               *****
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef SLIST_HXX
#define SLIST_HXX

#include <basic_toolkit/globaldefs.h>

#include <gms/FastPODAllocator.h>
#include <gms/FastAllocator.h>

typedef void* ent;

// Note: It may be necessary to disable private allocation.
// The allocator may trigger a bug in CHEF under Windows.
// However, it is not clear at all that the 
// allocator is really the source of the problem.
// 
// class slink: public gms::FastPODAllocator<slink> {

class DLLLOCAL slink {

private:

friend class slist;
friend class slist_iterator;
friend class slist_looper;
friend class slist_traversor;

  slink* next;
  void* e;

   slink( void* a, slink* p ): next(p), e(a) {} 
  ~slink() {}
};

class DLLEXPORT slist: public gms::FastAllocator {
private:
  slink* last;
  char   owner;
public:
  slist( char x = 0 ) : last(0), owner(x) {} 
  slist( void* a, char x = 0  ) { 
    last = new slink(a,0); 
    last->next = last; 
    owner = x; 
  }
  slist( slist const& );
  virtual ~slist() { clear(); }

  bool contains( const void* ) const;

  void insert( void* );
  void append( void* );
  void* remove( void* );
  void* get();
  void* pop();

  char Owns();
  void DoesOwn()    { owner = 1; }
  void DoesNotOwn() { owner = 0; }
  
  slist& operator=( const slist& );
  void* operator[]( int );
  int size() const;
  char IsEmpty() { return last == 0; }
  void clear();   // Preserves the data 
  void* lastInfoPtr() { if( last ) return last->e;
                      else       return 0;
                    }
  void* firstInfoPtr() { if( last ) return last->next->e;
                       else       return 0;
                     }

  friend class slist_iterator;
  friend class slist_looper;
  friend class slist_traversor;

};

class DLLEXPORT slist_iterator {
  const slist* const  cs;
  slink*              ce;

public:

  slist_iterator( slist const& s ):  cs(&s), ce( const_cast<slink*>(cs->last) ) {} 
 ~slist_iterator() { }

  void Reset()     { ce = cs->last; }
  void Terminate() { ce = 0; }

  void* operator()();
} ;


class DLLEXPORT slist_looper {
  slist* cs;
  slink* ce;
public:
  slist_looper( slist& s ):   cs(&s), ce(cs->last) { }
  void* operator()();
  void Reset( const slist& s ) { cs = (slist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
} ;

class slist_traversor {
  slist* cs;
  slink* ce;
public:
  slist_traversor( slist& s ): cs(&s), ce(cs->last) { }
  slink* operator()();
  void Reset( const slist& s ) { cs = (slist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
  void Terminate() { ce = 0; }
} ;



inline char slist::Owns() { return owner; }

inline void* slist::pop() { return this->get(); }

#endif // SLIST_HXX
