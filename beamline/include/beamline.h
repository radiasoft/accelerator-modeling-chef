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
******  - beamline no longer inherits from c-style void*  dlist container
******  - element container is now a private  nested std::list<> member
******  - implemented new STL compatible iterators 
******    iterator, pre_order_iterator. post_order_iterator, deep_iterator 
******    as well as const and reverse variants of the above
******
******  Jan - Mar 2007:  Jean-Francois Ostiguy 
******                   ostiguy@fnal.gov
******  - support for reference counted elements
******
******  Jul 2007         ostiguy@fnal.gov
******  - eliminated nested functor predicate and action classes  
******
******  Dec 2007         ostiguy@fnal.gov
******  - support for JetParticle bunch
******
******  Apr 2008         michelotti@fnal.gov
******  - additional argument list for beamline::InsertElementsFromList(..)
******
**************************************************************************
*************************************************************************/

#ifndef BEAMLINE_H
#define BEAMLINE_H

#include <list>
#include <boost/utility.hpp>

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

 
// -------------------------------------------------------------------------------

class beamline: public bmlnElmnt {

public:

   enum LineMode { line, ring, unknown };

   // ... iterator forward declarations
   //-----------------------------------

   template<typename held_type>
   class iter;

   typedef iter<ElmPtr>                                                   iterator;     
   typedef iter <ElmPtr const>                                      const_iterator;     

   template<typename held_type>
   class reverse_iter;

   typedef reverse_iter<ElmPtr>                                   reverse_iterator;     
   typedef reverse_iter<ElmPtr const>                       const_reverse_iterator;     

   template<typename held_type>
   class pre_order_iter;

   typedef pre_order_iter<ElmPtr>                               pre_order_iterator;     
   typedef pre_order_iter<ElmPtr const>                   const_pre_order_iterator;     

   template<typename held_type>
   class reverse_pre_order_iter;

   typedef reverse_pre_order_iter<ElmPtr>               reverse_pre_order_iterator;
   typedef reverse_pre_order_iter<ElmPtr const>   const_reverse_pre_order_iterator;

   template<typename held_type>
   class post_order_iter;

   typedef post_order_iter<ElmPtr>                             post_order_iterator;     
   typedef post_order_iter<ElmPtr const>                 const_post_order_iterator;     


   template<typename held_type>
   class reverse_post_order_iter;

   typedef reverse_post_order_iter<ElmPtr>              reverse_post_order_iterator;
   typedef reverse_post_order_iter<ElmPtr const>  const_reverse_post_order_iterator;

   template<typename held_type>
   class deep_iter;

   typedef deep_iter<ElmPtr>                                          deep_iterator;
   typedef deep_iter<ElmPtr const>                              const_deep_iterator;


   template<typename held_type>
   class reverse_deep_iter;    

   typedef  reverse_deep_iter<ElmPtr>                         reverse_deep_iterator;
   typedef  reverse_deep_iter<ElmPtr const>             const_reverse_deep_iterator;


 // CONSTRUCTORS AND DESTRUCTOR____________________________________________________________

   beamline( const char* nm = "NONAME" );
   beamline( beamline const& );
   beamline( FILE* );                  // Reading persistent object stored
                                      //  in a binary file.

  ~beamline();
   beamline* Clone() const;

   beamline& operator=( beamline const& rhs);


   void clear();                       // Returns state to empty beamline.


   double getReferenceTime()                    const;     
   void setReferenceTime( double   const& );               
   void setReferenceTime( Particle& );             

   void setLength( double const& );

  // EDITING LATTICE_____________________________________________________________________

   void     putAbove( iterator   it, ElmPtr const&  y ); // Insert y above (before;  upstream of) x
   iterator putBelow( iterator   it, ElmPtr const&  y ); // Insert y below (after, downstream of) x

   iterator erase   ( iterator it );
   iterator erase   ( iterator pos1, iterator pos2 );

   void     remove( ElmPtr elm);

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

   void InsertElementsFromList(                  double& s, std::list<std::pair<ElmPtr,double> >& inList );
   void InsertElementsFromList( Particle const&, double& s, std::list<std::pair<ElmPtr,double> >& inList );
                                       // Both methods insert elements from the list into
                                       // the beamline (in place) at locations specified in
                                       // the list.  The only difference in behavior arises
                                       // from using bmlnElmmnt::OrbitLength(Particle const&)
                                       // rather than  bmlnElmnt::Length() in the second form
                                       // when distributing the elements.

  int startAt( ConstElmPtr const&,     // Resets the "beginning" of the
               int = 1 );              // beamline to the element given
                                       // by the argument. Should be used
                                       // only for rings. Returns non-zero
                                       // if error occurs (esp., no such
                                       // element. The optional integer
                                       // argument allows one to reset to
                                       // nth occurrence of the element.
  int startAt( const char*,            // Resets the "beginning" of the
               int = 1 );              // beamline to the element whose
                                       // name is given by the argument.


  sector*  makeSector ( iterator pos1, iterator pos2,  int degree, JetParticle& )  const;  //  sector equivalent to [ pos1, pos2 )

  void     sectorize  ( iterator pos1,  iterator pos2, int degree, JetParticle& p, const char* = "NONAME");   


  // Change geometry of the line

  bool     setAlignment( alignmentData const& );
 
  iterator moveRelX( iterator pos, double const& dx);
  iterator moveRelY( iterator pos, double const& dy);
  iterator moveRelZ( iterator pos, double const& dz);
  iterator    pitch( iterator pos, double const& angle, double const& rpos);
  iterator      yaw( iterator pos, double const& angle, double const& rpos);
  iterator     yaw2( iterator pos, double const& angle, double const& rpos);
  iterator     roll( iterator pos, double const& angle, double const& rpos);


  // PROPAGATE PARTICLES


  void localPropagate( Particle& );          
  void localPropagate( ParticleBunch& );          
  void localPropagate( JetParticle& );          
  void localPropagate( JetParticleBunch& );          

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

  int twiss( LattFunc&,    JetParticle&,  int attachFlag = 1);
                           // Computes lattice functions for a
                           // non-periodic beamline.
                           // Uses the same method as MAD.


  // QUERIES _________________________________________________________________________________


  ElmPtr&        firstElement();
  ElmPtr const&  firstElement() const;

  ElmPtr&        lastElement();
  ElmPtr const&  lastElement()  const;

  bool   twissIsDone()  const;

  void  setTwissIsDone();
  void unsetTwissIsDone();

  double Energy() const; 

  bool           isBeamline() const;
  bool                empty() const;
  void                peekAt( double& s, Particle const& ) const;
  LattFunc     whatIsLattice( int );                                                // After element n, 0 <= n.
  LattFunc     whatIsLattice( std::string name );                                    
  int                howMany() const; 
  int           countHowMany() const;
  int     countHowManyDeeply() const;

  int           countHowMany( boost::function<bool(bmlnElmnt const&)>, std::list<ElmPtr>& ) const;
  int     countHowManyDeeply( boost::function<bool(bmlnElmnt const&)>, std::list<ElmPtr>& ) const;

  int                  depth()                               const;                 // Returns -1 if beamline is empty.
                                                                                    // Returns  0 if beamline is flat
                                                                                    //            or all its subbeamlines are empty.
                                                                                    // Otherwise returns 1 + largest
                                                                                    // depth of all subbeamlines.

  const char*  Type()                           const;
  bool         isMagnet()                       const;
  double       OrbitLength( Particle const& ) const;
  bool         isFlat()                         const;
  LattRing     whatIsRing();


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

  iterator    moveRel(   int axis, double const& u,     iterator pos,             std::string invoker );
  iterator  rotateRel(   int axis, double const& angle, iterator pos, double pct, std::string invoker );
  iterator rotateRel2(   int axis, double const& angle, iterator pos, double pct, std::string invoker );

  std::ostream&           writeTo(std::ostream&);
  friend std::istream& operator>>( std::istream&, beamline& );

  double                  nominalEnergy_;    // In GeV
  bool                    twissDone_;
  LineMode                mode_;
  std::list<ElmPtr>       theList_; 


}; 
#endif // BEAMLINE_H
