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
*******  REVISION HISTORY
******
******  October 2006: Jean-Francois Ostiguy 
******                ostiguy@fnal.gov
******
******  - beamline no longer inherits from c-style void*  dlist container
******  - element container is now a private  nested std::list<> member
******  - implemented new STL compatible iterators 
******    iterator, pre_order_iterator. post_order_iteartor, deep_iterator 
******    as well as const and reverse variants of the above
******
******  Jan - Mar 2007:  Jean-Francois Ostiguy 
******                   ostiguy@fnal.gov
******
****** - support for reference counted elements
****** 
**************************************************************************
*************************************************************************/

#ifndef BEAMLINE_H
#define BEAMLINE_H

#include <list>
#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/lattFunc.h>

#include <boost/shared_ptr.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/reverse_iterator.hpp>

class beamline;
class BmlVisitor;
class ConstBmlVisitor;

 

// --------------------------------------------------------------------------------------------------


class beamline: public bmlnElmnt {

public:
  enum LineMode { line, ring, unknown };

  struct Criterion
  {
    virtual bool operator()( bmlnElmnt const & );
    virtual ~Criterion() {};

  };
  // Returns true if element matches the derived 
  // criterion; false, if not.
  struct Aye : Criterion {
    bool operator()( bmlnElmnt const& ) { return true; }
  };
  struct Nay : Criterion {
    bool operator()( bmlnElmnt const& ) { return false; }
  };

  static Aye yes;
  static Nay no;

  struct Action
  {
    virtual int operator()( bmlnElmnt& );
    virtual ~Action() {}
  };
  // Derived classes should return 0 if the action is 
  // performed successfully on the element. All other
  // values indicate error.


public:

  // CONSTRUCTORS AND DESTRUCTOR____________________________________________________________

  beamline( const char* nm = "NONAME" );
  beamline( beamline const& );
  beamline( FILE* );                  // Reading persistent object stored
                                      //  in a binary file.

  virtual ~beamline();

  beamline* Clone() const;

  beamline& operator=( beamline const& rhs);


  void clear();                       // Returns state to empty beamline.



  // EDITING LATTICE_____________________________________________________________________

  void insert( ElmPtr    const&  );
  void insert( bmlnElmnt const&  );

  void append( ElmPtr    const&  );
  void append( bmlnElmnt const&  );

  void Split( double const&, ElmPtr&, ElmPtr& ) const;

  beamline*  reverse() const;                

  void InsertElementAt( double const& s_0, double const& s,  ElmPtr q );
                                      // Will insert q into the beamline at
                                      // OrbitLength s, assuming the beamline
                                      // begins at OrbitLength s_0.  Normally
                                      // will be invoked by a user program using
                                      // s_0 = 0, but s_0 is included so the
                                      // module can be used recursively.

  void InsertElementsFromList( Particle const& particle, double& s, std::list<std::pair<ElmPtr,double> >& inList);
                                      // Will insert elements from the list into
                                      // the beamline at locations specified in
                                      // the list.  

  int replace( ElmPtr elm1, ElmPtr elm2);
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

  int deepReplace( ElmPtr elm1, ElmPtr elm2 );
                                      // Like replace() but descends into
                                      // the hierarchy. Either argument
                                      // may be a pointer to a beamline.


  int startAt( ConstElmPtr const&,      // Resets the "beginning" of the
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

  sector* MakeSector ( ElmPtr start, ElmPtr finish, int,  JetParticle&);          // Returns a pointer to a new sector
                                                                                  // equivalent to everything between
                                                                                  // the first two arguments( exclusive).
                 


  sector* MakeSectorFromStart (ElmPtr,  int,  JetParticle& );
  sector*     MakeSectorToEnd (ElmPtr,  int,  JetParticle& );


  void     sectorize ( int, JetParticle& );                                                                         // Alters the object itself.
  beamline sectorize ( ElmPtr start,  ElmPtr finish, int degree,  JetParticle& p,   const char* = "NONAME");   // Alters the object: everything between
                                                                                                                    // the bmlnElmnt arguments replaced by a map.
                                                                                                                    // <-- This argument is the degree of the map.
                        
                

  void putAbove( std::list<ElmPtr>::iterator const& iter, ElmPtr  y ); // Insert y above (before;  upstream of) x
  void putBelow( std::list<ElmPtr>::iterator const& iter, ElmPtr  y ); // Insert y below (after, downstream of) x

  beamline remove( ElmPtr start_elm, ElmPtr end_elm);
  void     remove( ElmPtr elm);


  // Change geometry of the line

  bool     setAlignment( alignmentData const& );
 
  std::list<ElmPtr>   moveRelX( ElmPtr, double const&, int* = 0 );
  std::list<ElmPtr>   moveRelY( ElmPtr, double const&, int* = 0 );
  std::list<ElmPtr>   moveRelZ( ElmPtr, double const&, int* = 0 );
  std::list<ElmPtr>      pitch( ElmPtr, double const&, double const&, int* );
  std::list<ElmPtr>        yaw( ElmPtr, double const&, double const&, int* );
  std::list<ElmPtr>       roll( ElmPtr, double const&, double const&, int* );


  // PROPAGATE PARTICLES


  void localPropagate( Particle& );          
  void localPropagate( ParticleBunch& );          
  void localPropagate( JetParticle& );          

  void enterLocalFrame( Particle&    )    const;   
  void enterLocalFrame( JetParticle&    ) const;   

  void leaveLocalFrame( Particle&    )    const;   
  void leaveLocalFrame( JetParticle&    ) const;   




  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const ;


  void propLattFunc( );
  void propLattFunc( FILE* );
  void propLattFunc( char* );

  void realignAllElements();  // WRITE
  void markAllPins();         // WRITE

  // EDIT PROPERTIES________________________________________________________________

  inline beamline::LineMode getLineMode() const
  { return mode_; }
  inline void setLineMode( beamline::LineMode x )
  { mode_ = x; }

  //    mode_ doesn't affect behavior at the beamline level
  //    but carries information for higher level code,
  //    like dataHook_.

  void setEnergy( double const&  nominalEnergyGeV );
  void unTwiss();

  void eraseBarnacles( const char* = 0 );

  // ANALYSIS_________________________________________________________________________


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


  ElmPtr      firstElement();
  ElmPtr      firstElement() const;

  ElmPtr      lastElement();
  ElmPtr      lastElement()  const;

  bool   twissIsDone()  const;

  void  setTwissIsDone();
  void unsetTwissIsDone();

  double Energy() const; 

  bool                empty() const;
  void                peekAt( double& s, Particle const& ) const;
  lattFunc     whatIsLattice( int );                                                // After element n, 0 <= n.
  lattFunc     whatIsLattice( std::string name );                                    
  int                howMany()                           const { return numElem_; } // WARNING: not reliable!
  int           countHowMany() const;
  int     countHowManyDeeply() const;
  int           countHowMany( CRITFUNC, std::list<ElmPtr>& ) const;
  int     countHowManyDeeply( CRITFUNC, std::list<ElmPtr>& ) const;
  int                  depth()                               const;                 // Returns -1 if beamline is empty.
                                                                                    // Returns  0 if beamline is flat
                                                                                    //            or all its subbeamlines are empty.
                                                                                    // Otherwise returns 1 + largest
                                                                                    // depth of all subbeamlines.

  int    contains( ElmPtr &) const;     // Returns the number of times the argument appears.

  bool find( ElmPtr&  u, ElmPtr&  v, ElmPtr&  w ) const;

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





  const char*  Type()                           const;
  bool         isMagnet()                       const;
  double       OrbitLength( Particle const& );
  bool         isFlat()                         const;
  lattRing     whatIsRing();


  // STORING_______________________________________________________________________

   beamline flatten() const;     //   Produces a flattened version of itself.

   void   writeLattFunc( );
   void   writeLattFunc( FILE* );
   void   writeLattFunc( char* );


   // ITERATORS______________________________
   // 
   // new-style STL-compatible iterators 
   

  public:


#include <beamline/beamline_iterators.h> 


private:

  // Data

  double                  nominalEnergy_;    // In GeV
  int                     numElem_;          // Number of elements in the beamline
  char                    twissDone_;
  LineMode                mode_;
  std::list<ElmPtr>       theList_; 

  // Methods
  void   moveRel(   int axis, double const& u,     ElmPtr thePtr,             int*   errorCodePtr, std::list<ElmPtr>& replaced_list, std::string invoker );
  void rotateRel(   int axis, double const& angle, ElmPtr thePtr, double pct, int*   errorCodePtr, std::list<ElmPtr>& replaced_list, std::string invoker );

  std::ostream& writeTo(std::ostream&);
  friend std::istream& operator>>( std::istream&, beamline& );


}; 


#endif // BEAMLINE_H
