/*************************************************************************
**************************************************************************
******                                                               *****
******  Leo Michelotti                                               *****
******                                                               *****
******  Taken from Stroustrup's book,                                *****
******  The C++ Programming Language                                 *****
******                                                               *****
******  5/15/96  Added "owner"    (LM)                               *****
******                                                               *****
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef SLIST_HXX
#define SLIST_HXX

typedef void* ent;

class slink {
public:
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

private:
friend class slist;
friend class slist_iterator;
friend class slist_looper;
friend class slist_traversor;
  slink* next;
  ent e;

  slink( ent a, slink* p ) { 
    e=a; 
    next=p; 
#ifdef OBJECT_DEBUG
  objectCount++;
#endif
  }

  ~slink() {
#ifdef OBJECT_DEBUG
  objectCount--;
#endif
  }
} ;

class slist {
private:
  slink* last;
  char   owner;
public:
  slist( char x = 0 ) { 
    last = 0; 
    owner = x; 
#ifdef OBJECT_DEBUG
  objectCount++;
#endif
  }
  slist( ent a, char x = 0  ) { 
    last = new slink(a,0); 
    last->next = last; 
    owner = x; 
#ifdef OBJECT_DEBUG
  objectCount++;
#endif
  }
  slist( const slist& );
  virtual ~slist() { 
    clear();
#ifdef OBJECT_DEBUG
  objectCount--;
#endif
  }

  bool contains( const ent ) const;

  void insert( ent );
  void append( ent );
  ent remove( ent );
  ent get();

  char Owns();
  void DoesOwn()    { owner = 1; }
  void DoesNotOwn() { owner = 0; }
  
  slist& operator=( const slist& );
  ent operator[]( int );
  int size() const;
  char IsEmpty() { return last == 0; }
  void clear();   // Preserves the data 
  void zap();     // Destroys the data
  ent lastInfoPtr() { if( last ) return last->e;
                      else       return 0;
                    }
  ent firstInfoPtr() { if( last ) return last->next->e;
                       else       return 0;
                     }

  friend class slist_iterator;
  friend class slist_looper;
  friend class slist_traversor;

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
} ;

class slist_iterator {
  slink* ce;
  slist* cs;

public:
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

  slist_iterator( const slist& s ) { 
    cs = (slist*) &s; 
    ce = cs->last; 
#ifdef OBJECT_DEBUG
  objectCount++;
#endif
  }

  ~slist_iterator() { 
#ifdef OBJECT_DEBUG
  objectCount--;
#endif
  }

  void Reset    ( const slist& s ) { cs = (slist*) &s; ce = cs->last; }
  void Reset()  { ce = cs->last; }

  ent operator()();
} ;


class slist_looper {
  slink* ce;
  slist* cs;
public:
  slist_looper( slist& s ) { cs = &s; ce = cs->last; }
  ent operator()();
  void Reset( const slist& s ) { cs = (slist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
} ;

class slist_traversor {
  slink* ce;
  slist* cs;
public:
  slist_traversor( slist& s ) { cs = &s; ce = cs->last; }
  slink* operator()();
  void Reset( const slist& s ) { cs = (slist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
} ;



inline char slist::Owns() { return owner; }

#endif // SLIST_HXX
