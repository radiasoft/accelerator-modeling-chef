/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      beamline.h
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab     
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
******  Author: Leo Michelotti 
******          michelotti@fnal.gov
******
******  REVISION HISTORY
******
******  October 2006: Jean-Francois Ostiguy 
******                ostiguy@fnal.gov
******
******  - beamline no longer inherits from a dlist container                                                               
******  - element container is now a private  nested std::list<> member
******  - implemented new STL compatible iterators 
******    plain iterator               ( one level )
******    plain reverse_iterator       ( one level )
******    pre_order_iterator           ( recursive )
******    reverse_pre_order_iterator   ( recursive )
******    deep_iterator                ( recursive, skips beamlines )
******    reverse_deep_iterator        ( recursive, skips beamlines )
******                                                             
**************************************************************************
*************************************************************************/

#ifndef BEAMLINE_H
#define BEAMLINE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/lattFunc.h>
#include <beamline/BmlVisitor.h>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/shared_ptr.hpp>

class BmlPtrList;
class slist;

class beamline;

beamline& operator-( beamline const& );

// --------------------------------------------------------------------------------------------------


class beamline: public bmlnElmnt {

 friend class BmlVisitor;
 friend class ConstBmlVisitor;
 friend class BeamlineIterator;
 friend class ReverseBeamlineIterator;
 friend class DeepBeamlineIterator;
 friend class DeepReverseBeamlineIterator;


public:
  enum LineMode { line, ring, unknown };

  class arrayRep
  {
  public:
    arrayRep( const beamline*, bool = false );    // Clones all elements if 2nd argument is true
   ~arrayRep();
    int size() { return _n; }
    bmlnElmnt* e( int i ) { return _element[i]; } // WARNING: For speed, this is made unsafe.
                                                  // User is responsible for staying in bounds.

  private:
    int         _n;
    bmlnElmnt** _element;
    bool        _cloned;
 
  };

  struct Criterion
  {
    virtual bool operator()( const bmlnElmnt* );
    virtual bool operator()( const bmlnElmnt& );
    virtual ~Criterion() {};

  };
  // Returns true if element matches the derived 
  // criterion; false, if not.
  struct Aye : Criterion {
    bool operator()( const bmlnElmnt* ) { return true; }
    bool operator()( const bmlnElmnt& ) { return true; }
  };
  struct Nay : Criterion {
    bool operator()( const bmlnElmnt* ) { return false; }
    bool operator()( const bmlnElmnt& ) { return false; }
  };
  static Aye yes;
  static Nay no;

  struct Action
  {
    virtual int operator()( bmlnElmnt* );
    virtual int operator()( bmlnElmnt& );
    virtual ~Action() {}
  };
  // Derived classes should return 0 if the action is 
  // performed successfully on the element. All other
  // values indicate error.


private:

  // Data

  double                  nominalEnergy;    // In GeV
  int                     numElem;          // Number of elements in the beamline
  char                    twissDone;
  LineMode               _mode;
  std::list<bmlnElmnt*>  _theList; 

  // Methods
  void   _moveRel(   int axis, double const& u,     bmlnElmnt* thePtr,             int*   errorCodePtr, BmlPtrList* recycleBinPtr, std::string invoker );
  void _rotateRel(   int axis, double const& angle, bmlnElmnt* thePtr, double pct, int*   errorCodePtr, BmlPtrList* recycleBinPtr, std::string invoker );

  std::ostream& writeTo(std::ostream&);
  friend std::istream& operator>>( std::istream&, beamline& );


public:

  // CONSTRUCTORS AND DESTRUCTOR____________________________________________________________

  beamline( const char* nm = "NONAME" );
  beamline( bmlnElmnt* );
  beamline( beamline const& );
  beamline( char*, bmlnElmnt* );
  beamline( char*, beamline* );
  beamline( FILE* );                  // Reading persistent object stored
                                      //  in a binary file.

  virtual ~beamline();                 // Notice that this destructor does not destroy
                                       // the beamline elements.  To do that, use
                                       // beamline::zap().
  void zap();                          // Destroys the beamline elements
                                       // without destroying the beamline
  void clear();                        // Returns state to empty beamline.



  // EDITING LATTICE_____________________________________________________________________

  void insert( bmlnElmnt* );
  void append( bmlnElmnt* );

  void Split( double const&, bmlnElmnt**, bmlnElmnt** ) const;

  beamline*  reverse() const;                

  void InsertElementAt( double const& s_0, double const& s, const bmlnElmnt* q );
                                      // Will insert q into the beamline at
                                      // OrbitLength s, assuming the beamline
                                      // begins at OrbitLength s_0.  Normally
                                      // will be invoked by a user program using
                                      // s_0 = 0, but s_0 is included so the
                                      // module can be used recursively.

  void InsertElementsFromList( double& s_0, InsertionList&, slist& );
                                      // Will insert elements from the list into
                                      // the beamline at locations specified in
                                      // the list.  Removed elements are stored
                                      // in the final argument, in case they
                                      // need to be eliminated by the user module.
  // InsertElementsFromList1 is a workaround for python bindings. See
  // python-bindings/src/py-bmlnelmnt.cpp
  double InsertElementsFromList1( double const& s_0, InsertionList& il, slist& sl)
                                      // a version of InsertElementsFromList
                                      // that is callable from Python.
    {
      double tmp(s_0);
      InsertElementsFromList(tmp, il, sl);
      return tmp;
    }

  int replace( const bmlnElmnt*, const bmlnElmnt* );
                                      // Will replace the first argument with
                                      // the second. Return values:
                                      // 0 everything went as planned
                                      // 1 first argument was not found
                                      // 2 at least one argument was null
                                      // NOTE: works only at the highest
                                      // level of the tree hierarchy; does
                                      // not descend into the tree.
                                      // NOTE ALSO: will replace only the
                                      // first object found whose address
                                      // matches that of the second.

  int deepReplace( const bmlnElmnt*, const bmlnElmnt* );
                                      // Like replace() but descends into
                                      // the hierarchy. Either argument
                                      // may be a pointer to a beamline.

  //beamline& operator^( bmlnElmnt& );  // Alters the beamline
  //beamline& operator+( bmlnElmnt& );  // Does not alter the beamline
  //beamline& operator+( beamline& );
  //beamline& operator-( beamline& );

  friend beamline& operator-( beamline const& );


  int startAt( bmlnElmnt const*, // Resets the "beginning" of the
               int = 1 );               // beamline to the element given
                                        // by the argument. Should be used
                                        // only for rings. Returns non-zero
                                        // if error occurs (esp., no such
                                        // element. The optional integer
                                        // argument allows one to reset to
                                        // nth occurrence of the element.
  int startAt( const char*,             // Resets the "beginning" of the
               int = 1 );               // beamline to the element whose
                                        // name is given by the argument.


  sector* makeSector ( int, JetParticle& );
                                     // Returns a pointer to a new
                                     // sector equivalent to the entire
                                     // beamline.  The argument is the
                                     // degree of the map.

  sector* MakeSector ( bmlnElmnt const&, bmlnElmnt const&, int,  JetParticle&);                       // Returns a pointer to a new sector
                                                                                                      // equivalent to everything between
                                                                                                      // the first two arguments( exclusive).
                 


  sector* MakeSectorFromStart (bmlnElmnt const&,  int,  JetParticle& );
  sector*     MakeSectorToEnd (bmlnElmnt const&,  int,  JetParticle& );


  void     sectorize ( int, JetParticle& );                                                                         // Alters the object itself.
  beamline sectorize ( bmlnElmnt* start,  bmlnElmnt* end, int degree,  JetParticle& p,   const char* = "NONAME");   // Alters the object: everything between
                                                                                                                    // the bmlnElmnt arguments replaced by a map.
                                                                                                                    // <-- This argument is the degree of the map.
                        
                

  void putAbove( std::list<bmlnElmnt*>::iterator const& iter, bmlnElmnt* y ); // Insert y above (before;  upstream of) x
  void putBelow( std::list<bmlnElmnt*>::iterator const& iter, bmlnElmnt* y ); // Insert y below (after, downstream of) x

  beamline remove( bmlnElmnt*, bmlnElmnt* );
  void     remove( bmlnElmnt* );


  // Change geometry of the line

  bool     setAlignment( alignmentData const& );
 
  BmlPtrList   moveRelX( bmlnElmnt*, double const&, int* = 0 );
  BmlPtrList   moveRelY( bmlnElmnt*, double const&, int* = 0 );
  BmlPtrList   moveRelZ( bmlnElmnt*, double const&, int* = 0 );
  BmlPtrList      pitch( bmlnElmnt*, double const&, double const&, int* );
  BmlPtrList        yaw( bmlnElmnt*, double const&, double const&, int* );
  BmlPtrList       roll( bmlnElmnt*, double const&, double const&, int* );


  // PROPAGATE PARTICLES


  void localPropagate( Particle& );          
  void localPropagate( ParticleBunch& );          
  void localPropagate( JetParticle& );          

  void enterLocalFrame( Particle&    )    const;   
  void enterLocalFrame( JetParticle&    ) const;   

  void leaveLocalFrame( Particle&    )    const;   
  void leaveLocalFrame( JetParticle&    ) const;   




  void accept( BmlVisitor& v )            {  v.visitBeamline( this ); }
  void accept( ConstBmlVisitor& v ) const {  v.visitBeamline( this ); }


  void propLattFunc( );
  void propLattFunc( FILE* );
  void propLattFunc( char* );

  void realignAllElements();  // WRITE
  void markAllPins();         // WRITE

  // EDIT PROPERTIES________________________________________________________________

  inline beamline::LineMode getLineMode() const
  { return _mode; }
  inline void setLineMode( beamline::LineMode x )
  { _mode = x; }

  //   _mode doesn't affect behavior at the beamline level
  //    but carries information for higher level code,
  //    like _dataHook.

  void setEnergy( double const&  nominalEnergyGeV );
  void unTwiss();

  void eraseBarnacles( const char* = 0 );

  // ANALYSIS_________________________________________________________________________

  // twiss no-longer virtual to eliminate
  // circular library dependencies - FO

  int twiss( JetParticle&, double const& dpp = 0.00001, int  attachFlag = 1 );
                           // Computes lattice functions all the
                           // way around the ring.
                           // Attaches a lattRing Barnacle labelled 
                           // "Ring" to the beamline, and a lattFunc
                           // Barnacle labelled "Twiss" to every element
                           // in the line.

  int twiss( char,         JetParticle& );
                           // Only computes lattice functions at
                           // the beginning of the ring.
                           // Uses the same method as MAD.

  int twiss( lattFunc&,    JetParticle&,  int attachFlag = 1);
                           // Computes lattice functions for a
                           // non-periodic beamline.
                           // Uses the same method as MAD.


  // QUERIES _________________________________________________________________________________

  // REMOVE: void   peekAt( double& s, Particle* = 0 );

  void                peekAt( double& s, const Particle& ) const;
  lattFunc     whatIsLattice( int );                                                // After element n, 0 <= n.
  lattFunc     whatIsLattice( char* n );                                            // n is name of element 
  int                howMany()                           const { return numElem; }  // WARNING: not reliable!
  int           countHowMany( CRITFUNC = 0, slist* = 0 ) const;
  int     countHowManyDeeply( CRITFUNC = 0, slist* = 0 ) const;
  int                  depth()                           const;                     // Returns -1 if beamline is empty.
                                                                                    // Returns  0 if beamline is flat
                                                                                    //            or all its subbeamlines are empty.
                                                                                    // Otherwise returns 1 + largest
                                                                                    // depth of all subbeamlines.

  int    contains( const bmlnElmnt*) const;     // Returns the number of times the argument appears.

  bool find( bmlnElmnt*& u, bmlnElmnt*& v, bmlnElmnt*& w ) const;

  // Upon entry: u and w should have null value but can, in fact
  //               be anything. 
  //               WARNING: they will be reset, so don't use addresses
  //               of valid elements.
  //             *v is the element to be searched for.
  // Upon exit:  u points to the element upstream of v
  //             w points to the element downstream of v
  //               Return value of u or w can be null (i.e. 0) if
  //               this beamline is not treated as a ring and *v is
  //               either the first or last element, respectively.
  //
  //             Value returned: true,  if *v is found
  //                             false, if *v is not found
  //               If false is returned, then return values of 
  //               u and w are unspecified.
  // 
  // Comments:   Only one instance, the first instance, of *v is found.
  //             This routine is meant to work on lines containing
  //               unique elements. For some cases, it may work with
  //               multiple instances of the same element.
  // 
  // Example:    beamline A; ...
  //             bmlnElmnt* u = 0; bmlnElmnt* w = 0;
  //             rbend B; ...
  //             if( A.find( u, &B, w ) ) { ... }
  // 



  inline bmlnElmnt* firstElement() const
  { return   _theList.front(); }

  inline bmlnElmnt* lastElement() const
  { return   _theList.back(); }

  inline char twissIsDone()
  { return twissDone; }

  inline void setTwissIsDone()
  { twissDone = 1; }

  inline void unsetTwissIsDone()
  { twissDone = 0; }

  double Energy() const 
  { return nominalEnergy; }

  const char*  Type() const;
  bool isFlat();

  beamline* Clone() const;

  double OrbitLength( Particle const& );
  lattRing whatIsRing();


  // CLONING AND STORING_______________________________________________________________________

   beamline* flatten() const;    //   Produces a flattened version of itself.
                                 //   WARNING: the elements are not cloned.
                                 //   Thus the flattened line contains the
                                 //   same objects as its original.

   void   writeLattFunc( );
   void   writeLattFunc( FILE* );
   void   writeLattFunc( char* );


   // ITERATORS______________________________
   // 
   // new-style STL-compatible iterators 
   

  public:


    class iterator;
    class reverse_iterator;

    class const_iterator;
    class const_reverse_iterator;

    class pre_order_iterator;
    class reverse_pre_order_iterator;

    class const_pre_order_iterator;
    class const_reverse_pre_order_iterator;

    class post_order_iterator;
    class reverse_post_order_iterator;

    class const_post_order_iterator;
    class const_reverse_post_order_iterator;

    class deep_iterator;
    class reverse_deep_iterator;    

    class const_deep_iterator;
    class const_reverse_deep_iterator;


    beamline::iterator begin() const
                      { beamline* self = const_cast<beamline*>(this);
                        return beamline::iterator(self, self->_theList.begin());       
                      }
    beamline::iterator end()   const
                      { beamline* self = const_cast<beamline*>(this);
                        return beamline::iterator(self, self->_theList.end()  );      
                      }   
  
    beamline::reverse_iterator rbegin() const        
                      { beamline* self = const_cast<beamline*>(this);
                        return beamline::iterator(self, self->_theList.end()   );     
                      } 
    beamline::reverse_iterator rend()   const               
                      { beamline* self = const_cast<beamline*>(this);
                        return beamline::iterator(self, self->_theList.begin() );     
                      }   
   
    beamline::pre_order_iterator pre_begin()          
                      { return beamline::pre_order_iterator(beamline::iterator(this, _theList.begin() )); } 
    beamline::pre_order_iterator pre_end()            
                      { return beamline::pre_order_iterator(beamline::iterator(this, _theList.end()   )); }   
 
    beamline::reverse_pre_order_iterator rpre_begin()  
                      { return beamline::pre_order_iterator(beamline::iterator(this, _theList.end()   )); } 
    beamline::reverse_pre_order_iterator rpre_end()   
                      { return beamline::pre_order_iterator(beamline::iterator(this, _theList.begin() )); }   

    beamline::post_order_iterator post_begin() 
                      { return beamline::post_order_iterator(beamline::iterator(this, _theList.begin()   )); } 
    beamline::post_order_iterator post_end()     
                      { return beamline::post_order_iterator(beamline::iterator(this, _theList.end()     )); }   

    beamline::reverse_post_order_iterator rpost_begin()  
                      { return beamline::post_order_iterator(beamline::iterator(this, _theList.end()   )); } 
    beamline::reverse_post_order_iterator rpost_end()   
                      { return beamline::post_order_iterator(beamline::iterator(this, _theList.begin() )); }   

    beamline::deep_iterator deep_begin() 
                      { return beamline::deep_iterator(beamline::pre_order_iterator(beamline::iterator(this, _theList.begin() ))); } 
    beamline::deep_iterator deep_end()   
                      { return beamline::deep_iterator(beamline::pre_order_iterator(beamline::iterator(this, _theList.end()   ))); }   

    beamline::reverse_deep_iterator rdeep_begin()  
                      { return beamline::deep_iterator( beamline::pre_order_iterator(beamline::iterator(this, _theList.end()  ))); } 
    beamline::reverse_deep_iterator rdeep_end()    
                      { return beamline::deep_iterator( beamline::pre_order_iterator(beamline::iterator(this, _theList.begin() ))); }   



#include <beamline/beamline_iterators.h> 

}; 


#endif // BEAMLINE_H
