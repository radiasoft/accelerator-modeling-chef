/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.3
******                                    
******  File:      slist.h
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
  void*  e;

  slink( void* a, slink* p ) { 
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
  slist( void* a, char x = 0  ) { 
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

  bool contains( const void* ) const;

  void insert( void* );
  void append( void* );
  void* remove( void* );
  void* get();

  char Owns();
  void DoesOwn()    { owner = 1; }
  void DoesNotOwn() { owner = 0; }
  
  slist& operator=( const slist& );
  void* operator[]( int );
  int size() const;
  char IsEmpty() { return last == 0; }
  void clear();   // Preserves the data 
  // void zap();     // Destroys the data
  void* lastInfoPtr() const { if( last ) return last->e;
                              else       return 0;
                            }
  void* firstInfoPtr() const { if( last ) return last->next->e;
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

  inline slist_iterator( const slist& s ) { 
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
  void Terminate() { ce = 0; }

  void* operator()();
} ;


class slist_looper {
  slink* ce;
  slist* cs;
public:
  slist_looper( slist& s ) { cs = &s; ce = cs->last; }
  void* operator()();
  void Reset( const slist& s ) { cs = (slist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
} ;

class slist_traversor {
  slink* ce;
  slist* cs;
public:
  slist_traversor( const slist& s ) { cs = const_cast<slist*>(&s); ce = cs->last; }
  slink* operator()();
  void Reset( const slist& s ) { cs = (slist*) &s; ce = cs->last; }
  void Reset() { ce = cs->last; }
  void Terminate() { ce = 0; }
} ;



inline char slist::Owns() { return owner; }

#endif // SLIST_HXX
