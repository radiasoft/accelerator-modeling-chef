/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.3
******                                    
******  File:      dlist.h
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


#ifndef DLIST_HXX
#define DLIST_HXX

// REMOVE: typedef void* ent;
typedef char (*DLIST_CRITFUNC)( void* );

class dlink {
private:
  dlink* next;
  dlink* prev;
  void*  e;
public:
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

  dlink( void* a, dlink* p, dlink* q ) { 
    e=a; 
    prev=p; 
    next=q; 
#ifdef OBJECT_DEBUG
  objectCount++;
#endif
  }

  ~dlink () {
#ifdef OBJECT_DEBUG
  objectCount--;
#endif
  }

  void putAbove( dlink* );
  void putBelow( dlink* );
  dlink* nextPtr() {return next;}
  dlink* prevPtr() {return prev;}
  void* info() {return e;}

  friend class dlist;
  friend class dlist_iterator;
  friend class dlist_traversor;
  friend class dlist_looper;
  friend class dlist_reverseIterator;

/*
 *   Pool functions written by Michael Allen
 *   February, 1993
 */
#ifdef POOLED
  inline void* operator new(size_t);
  inline void operator delete(void*, size_t);
#endif // POOLED
} ;


/*
 *   Pool functions written by Michael Allen
 *   February, 1993
 */
#ifdef POOLED
class _dlink_Shell
{
friend class _dlink_Pool;

 private:
 
  union
  {
    _dlink_Shell* next;
    char  dummy[sizeof(dlink)];
  };
};

#define _dlink_BLOCK_COUNT 1024

class _dlink_Pool
{
friend class dlink;
  
 private:
  _dlink_Pool();
  
  inline static void* alloc();
  inline static void free(void*);
  
  _dlink_Shell slot[_dlink_BLOCK_COUNT];
  static _dlink_Shell* top;
  static int num_made;
};

inline void*
dlink::operator new(size_t)
{
  return _dlink_Pool::alloc();
}

inline void
dlink::operator delete(void* p, size_t)
{
  _dlink_Pool::free(p);
}

inline void*
_dlink_Pool::alloc()
{
  if (0 == top)
    {
      new _dlink_Pool;
    }
  void* ans = top;
  top = top->next;
  return ans;
}

inline void
_dlink_Pool::free(void* p)
{
  ((_dlink_Shell*)p)->next = top;
  top = (_dlink_Shell*)p;
}

#endif  // POOLED

class dlist {

private:
  dlink* last;
  char   owner;

public:
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

  dlist( char x = 0 ) { 
    last = 0; 
    owner = x; 
#ifdef OBJECT_DEBUG
  objectCount++;
#endif
  }

  dlist( void* a, char x = 0   ) { 
    last = new dlink(a,0,0); 
    last->prev = last; 
    last->next = last; 
    owner = x; 
#ifdef OBJECT_DEBUG
  objectCount++;
#endif
  }

  dlist( const dlist& );

  virtual ~dlist() { 
    clear(); 
#ifdef OBJECT_DEBUG
  objectCount--;
#endif
  }

  bool contains( const void* ) const;

  void* remove( dlink* );          // Removes the single argument.
  char remove( void* );            // Ditto 
  void* remove( int );             // Returns the void* removed from the dlist
                                   // Failure -> returns null pointer
  dlist remove( void*, void* );    // Removes everything between its arguments
                                   // (exclusively) and returns the removed chain.
  dlist remove( int, int );
  void insert( void* a );
  void append( void* a );
  char putAbove( const void* a, const void* b );
                                   // Installs b above (before) a in the list
  char putBelow( const void* a, const void* b );
                                   // Installs b below (after)  a in the list
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
  const dlink* lastPtr() {return last;}
  void* lastInfoPtr() const { if( last ) return last->e;
                              else       return 0;
                    }
  void* firstInfoPtr() const { if( last ) return last->next->e;
                               else       return 0;
                    }
  dlist& operator=( const dlist& );

  // Friends ............
  friend class dlist_iterator;
  friend class dlist_traversor;
  friend class dlist_looper;
  friend class dlist_reverseIterator;
} ;

class dlist_iterator {
  dlink* ce;
  dlist* cs;

public:
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

  dlist_iterator( const dlist& s ) { 
    cs = (dlist*) &s; 
    ce = cs->last; 
    #ifdef OBJECT_DEBUG
    objectCount++;
    #endif
  }

  // A copy constructor. Use with caution.
  dlist_iterator( const dlist_iterator& x ) {
    ce = x.ce;
    cs = x.cs;
  }

  ~dlist_iterator () {
    #ifdef OBJECT_DEBUG
    objectCount--;
    #endif
  }

  void* operator()();
  void Reset( const dlist& s ) { cs = (dlist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
  void GoBack( int = 1 );
  void Terminate() { ce = 0; }
  bool isFinished() { return ( 0 == ce ); }
} ;

class dlist_looper {
  dlink* ce;
  dlist* cs;
public:
  dlist_looper( dlist& s ) { cs = &s; ce = cs->last; }
  dlist_looper( const dlist_looper& x ) { cs = x.cs; ce = x.ce; }
  void* operator()();
  void Reset( const dlist& s ) { cs = (dlist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
} ;

class dlist_traversor {
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

class dlist_reverseIterator {
  dlink* ce;
  dlist* cs;
public:
  dlist_reverseIterator( dlist& s ) { cs = &s; ce = cs->last; }
  dlist_reverseIterator( const dlist_reverseIterator& x ) { cs = x.cs; ce = x.ce; }
  void* operator()();
  void Reset( const dlist& s ) { cs = (dlist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
  void Terminate() { ce = 0; }
} ;

inline char dlist::Owns() { return owner; }

#endif // DLIST_HXX
