/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                ****
******  DLIST: Doubly linked lists. Based on slist, found in          ****
******         Stroustrup's C++ Programming Language                  ****
******                                                                ****
******  Copyright (c) 1990  Universities Research Association, Inc.   ****
******                All Rights Reserved                             ****
******                                                                ****
******  Author:    Leo Michelotti                                     ****
******                                                                ****
******             Fermilab                                           ****
******             P.O.Box 500                                        ****
******             Mail Stop 345                                      ****
******             Batavia, IL   60510                                ****
******                                                                ****
******             Phone: (708) 840 4956                              ****
******             Email: MICHELOTTI@FNALAD                           ****
******                    MICHELOTTI@ADCALC.FNAL.GOV                  ****
******                    ALMOND::MICHELOTTI                          ****
******                                                                ****
******  Release    Version 1.0 : May 20, 1990                         ****
******  Dates:                                                        ****
**************************************************************************
**************************************************************************
******                                                                ****
******  This material resulted from work developed under a            ****
******  Government Contract and is subject to the following           ****
******  license: The Government retains a paid-up, nonexclusive,      ****
******  irrevocable worldwide license to reproduce, prepare           ****
******  derivative works, perform publicly and display publicly       ****
******  by or for the Government, including the right to distribute   ****
******  to other Government contractors.  Neither the United          ****
******  States nor the United States Department of Energy, nor        ****
******  any of their employees, makes any warranty, express or        ****
******  implied, or assumes any legal liability or responsibility     ****
******  for the accuracy, completeness or usefulness of any           ****
******  information, apparatus, product, or process disclosed, or     ****
******  represents that its use would not infringe privately owned    ****
******  rights.                                                       ****
******                                                                ****
******  These files are made avaliable for use by specific            ****
******  individuals or a specific group.  They are not to be          ****
******  reproduced or redistributed.                                  ****
******                                                                ****
******  These files are provided "as is" with no warranties of any    ****
******  kind including the warranties of design,                      ****
******  merchantibility and fitness for a particular purpose,         ****
******  or arising from a course of dealing, usage or trade           ****
******  practice.                                                     ****
******                                                                ****
******  These files are provided with no support or obligation on     ****
******  the part of U.R.A. or Fermilab to assist in their use,        ****
******  correction, modification or enhancement.                      ****
******                                                                ****
******  Neither U.R.A. nor Fermilab shall be liable with              ****
******  respect to the infringement of copyrights, trade              ****
******  secrets or any patents by these files or any part             ****
******  thereof.                                                      ****
******                                                                ****
******  In no event will U.R.A., Fermilab, or the author(s) be        ****
******  liable for any lost revenue or profits or other               ****
******  special, indirect and consequential damages, even if          ****
******  they have been advised of the possibility of such             ****
******  damages.                                                      ****
******                                                                ****
**************************************************************************
*************************************************************************/

#ifndef DLIST_HXX
#define DLIST_HXX

typedef void* ent;
typedef char (*DLIST_CRITFUNC)( ent );

class dlink {
private:
  dlink* next;
  dlink* prev;
  ent e;
public:
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

  dlink( ent a, dlink* p, dlink* q ) { 
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
  ent info() {return e;}

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

  dlist( ent a, char x = 0   ) { 
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

  ent remove( dlink* );            // Removes the single argument.
  char remove( ent );              // Ditto 
  ent remove( int );               // Returns the ent removed from the dlist
                                   // Failure -> returns null pointer
  dlist remove( ent, ent );        // Removes everything between its arguments
                                   // (exclusively) and returns the removed chain.
  dlist remove( int, int );
  void insert( ent a );
  void append( ent a );
  char putAbove( ent a, ent b );   // Installs b above a in the list
  char putBelow( ent a, ent b );   // Installs b below a in the list
  ent get();

  int  startAt( const ent, int=1 );
  void riskStartAt( const dlink* );
                                   // Does not check whether the argument
                                   // is in the list.
  char Owns();
  void DoesOwn()    { owner = 1; }
  void DoesNotOwn() { owner = 0; }

  ent operator[]( int ) const;
  int size() const;
  char IsEmpty() { return last == 0; }
  void clear();
  void zap();

  char SetLast( ent );
  char SetFirst( ent );
  char SetLast( DLIST_CRITFUNC );
  char SetFirst( DLIST_CRITFUNC );
  dlink* lastPtr() {return last;}
  ent lastInfoPtr() { if( last ) return last->e;
                      else       return 0;
                    }
  ent firstInfoPtr() { if( last ) return last->next->e;
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

  ~dlist_iterator () {
#ifdef OBJECT_DEBUG
  objectCount--;
#endif
  }

  ent operator()();
  void Reset( const dlist& s ) { cs = (dlist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
  void GoBack( int = 1 );
} ;

class dlist_looper {
  dlink* ce;
  dlist* cs;
public:
  dlist_looper( dlist& s ) { cs = &s; ce = cs->last; }
  ent operator()();
  void Reset( const dlist& s ) { cs = (dlist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
} ;

class dlist_traversor {
  dlink* ce;
  dlist* cs;
public:
  dlist_traversor( dlist& s ) { cs = &s; ce = cs->last; }
  dlink* operator()();
  void Reset( const dlist& s ) { cs = (dlist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
} ;

class dlist_reverseIterator {
  dlink* ce;
  dlist* cs;
public:
  dlist_reverseIterator( dlist& s ) { cs = &s; ce = cs->last; }
  ent operator()();
  void Reset( const dlist& s ) { cs = (dlist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
} ;

inline char dlist::Owns() { return owner; }

#endif // DLIST_HXX
