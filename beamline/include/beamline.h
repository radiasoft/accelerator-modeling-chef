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
******  October 2006  ostiguy@fnal.gov
******  - beamline no longer inherits from c-style void*  dlist container
******  - element container is now a private  nested std::list<> member
******  - implemented new STL compatible iterators: 
******    iterator, pre_order_iterator. post_order_iterator, deep_iterator 
******    as well as const and reverse variants of the above
******  Jan - Mar 2007 ostiguy@fnal.gov
******  - support for reference counted elements
******  Jul 2007         ostiguy@fnal.gov
******  - eliminated nested functor predicates and action classes  
******  Dec 2007         ostiguy@fnal.gov
******  - support for JetParticle bunch
******  Mar 2008        ostiguy@fnal.gov
******  - since magnetic element scale with _momentum_ beamline now has a 
******   reference momentum attribute (rather than a reference energy). 
******  - implemented scheme to allow elements to access parent beamline.
******  Apr 2008         michelotti@fnal.gov
******  - additional argument list for beamline::InsertElementsFromList(..)
******* May 2008 ostiguy@fnal.gov
******  - proper, explicit assignment operator
******  - propagator moved (back) to base class
******  - no assumption about internal structure
******  - localPropagate/Propagate now const-correct

**************************************************************************
*************************************************************************/

#ifndef BEAMLINE_H
#define BEAMLINE_H

#include <list>
#include <stack>
#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/LatticeFunctions.h>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/reverse_iterator.hpp>

class beamline;
class BmlVisitor;
class ConstBmlVisitor;

 
// -------------------------------------------------------------------------------

class beamline: public bmlnElmnt {

private:

    template<typename U, int Dummy>
    struct iter_traits{ };

public:

   struct Node;
 
   enum LineMode { line, ring };

   template<typename held_type>
   class iter;

   typedef iter<ElmPtr>                                                   iterator;     
   typedef iter<ElmPtr const>                                       const_iterator;     
   
   template<typename held_type>
   class reverse_iter;

   typedef reverse_iter<ElmPtr>                                    reverse_iterator;     
   typedef reverse_iter<ElmPtr const>                        const_reverse_iterator;     

   template<typename held_type>
   class pre_order_iter;

   typedef pre_order_iter<ElmPtr>                               pre_order_iterator;     
   typedef pre_order_iter<ElmPtr const>                    const_pre_order_iterator;     

   template<typename held_type>
   class reverse_pre_order_iter;

   typedef reverse_pre_order_iter<ElmPtr>                reverse_pre_order_iterator;
   typedef reverse_pre_order_iter<ElmPtr const>    const_reverse_pre_order_iterator;

   template<typename held_type>
   class post_order_iter;

   typedef post_order_iter<ElmPtr>                             post_order_iterator;     
   typedef post_order_iter<ElmPtr const>                 const_post_order_iterator;     

   template<typename held_type>
   class reverse_post_order_iter;

   typedef reverse_post_order_iter<ElmPtr>               reverse_post_order_iterator;
   typedef reverse_post_order_iter<ElmPtr const>    const_reverse_post_order_iterator;

   template<typename held_type>
   class deep_iter;

   typedef deep_iter<ElmPtr>                                          deep_iterator;
   typedef deep_iter<ElmPtr const>                               const_deep_iterator;


   template<typename held_type>
   class reverse_deep_iter;    

   typedef  reverse_deep_iter<ElmPtr>                         reverse_deep_iterator;
   typedef  reverse_deep_iter<ElmPtr const>             const_reverse_deep_iterator;



   beamline( std::string const& name = "");
   beamline( beamline const& );
   beamline( FILE* );                  // Reading persistent object stored
                                      //  in a binary file.

  ~beamline();
   beamline* Clone() const;

   beamline& operator=( beamline const& rhs);


   void clear();                       // Returns state to empty beamline.

   beamline const*  parent()  const  { return parent_; } 
   beamline*        parent()         { return parent_; }    
   
   virtual void     reset()   const; 

   double getReferenceTime()                    const;     
   void   setReferenceTime( double   const& );               
   void   setReferenceTime( Particle&       );             

   void     putAbove( iterator   it, ElmPtr y ); // Insert y above (before;  upstream of) x
   iterator putBelow( iterator   it, ElmPtr y ); // Insert y below (after, downstream of) x

   iterator erase   ( iterator it );
   iterator erase   ( iterator pos1, iterator pos2 );

   void     remove( ElmPtr );

   void insert( ElmPtr    );
   void insert( bmlnElmnt const&  );

   void append( ElmPtr    );
   void append( bmlnElmnt const&  );

   void Split( double const&, ElmPtr&, ElmPtr& ) const;

   beamline*  reverse() const;                


   // std::pair<iterator,double> insertAt( iterator const& pos, double const& ds, ElmPtr elm );  


   void InsertElementAt( double const& s_0, double const& s,  ElmPtr q ); 
   void InsertElementsFromList(                  double& s, std::list<std::pair<ElmPtr,double> >& inList );
   void InsertElementsFromList( Particle const&, double& s, std::list<std::pair<ElmPtr,double> >& inList );
                                       // Both methods insert elements from the list into
                                       // the beamline (in place) at locations specified in
                                       // the list.  The only difference in behavior arises
                                       // from using bmlnElmmnt::OrbitLength(Particle const&)
                                       // rather than  bmlnElmnt::Length() in the second form
                                       // when distributing the elements.


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


  sector*  makeSector ( iterator pos1, iterator pos2,  int degree, JetParticle& )  const;  //  sector equivalent to [ pos1, pos2 )

  void     sectorize  ( iterator pos1,  iterator pos2, int degree, JetParticle& p, const char* = "NONAME");   


  // Change geometry of the line

  bool     setAlignment( alignmentData const& );
 
  iterator moveRelX( iterator pos, double const& dx);
  iterator moveRelY( iterator pos, double const& dy);
  iterator moveRelZ( iterator pos, double const& dz);
  iterator    pitch( iterator pos, double const& angle, double const& rpos);
  iterator      yaw( iterator pos, double const& angle, double const& rpos);
  iterator     roll( iterator pos, double const& angle, double const& rpos);


  void localPropagate( Particle&         ) const;          
  void localPropagate( ParticleBunch&    ) const;          
  void localPropagate( JetParticle&      ) const;          
  void localPropagate( JetParticleBunch& ) const;          

  void enterLocalFrame( Particle&        ) const;   
  void enterLocalFrame( JetParticle&     ) const;   

  void leaveLocalFrame( Particle&        ) const;   
  void leaveLocalFrame( JetParticle&     ) const;   


  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const ;

  void realignAllElements();  // WRITE
  void markAllPins();         // WRITE


  inline beamline::LineMode getLineMode() const
  { return mode_; }
  inline void setLineMode( beamline::LineMode x )
  { mode_ = x; }

  //    mode_ doesn't affect behavior at the beamline level
  //    but carries information for higher level code,
  //    like dataHook_.

  void setMomentum( double const&  nominalMomentumGeV );

  void eraseBarnacles( const char* = 0 );


  ElmPtr&        firstElement();
  ElmPtr const&  firstElement() const;

  ElmPtr&        lastElement();
  ElmPtr const&  lastElement()  const;

  double Momentum() const; 

  bool           isBeamline() const;
  bool                empty() const;
  void                peekAt( double& s, Particle const& ) const;
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
  bool         isThin()                         const;
  bool         isPassive()                      const;

  double       OrbitLength( Particle const& )   const;
  double       Length()                         const;
  bool         isFlat()                         const;
  LattRing     whatIsRing();



  beamline flatten() const;     //   Produces a flattened version of itself.

  // STL-compatible iterators ...  
   

  public:

    const_iterator begin() const;                           
          iterator       begin();                                 
         

    const_iterator end()   const;                           
          iterator end();                                         


    const_reverse_iterator rbegin() const;                  
          reverse_iterator rbegin();                              


    const_reverse_iterator rend()   const;                  
          reverse_iterator       rend();                          


    const_pre_order_iterator pre_begin() const;             
          pre_order_iterator       pre_begin();                   


    const_pre_order_iterator pre_end() const;               
          pre_order_iterator       pre_end();                


    const_reverse_pre_order_iterator rpre_begin()  const;   
          reverse_pre_order_iterator       rpre_begin();          


    const_reverse_pre_order_iterator rpre_end() const;      
          reverse_pre_order_iterator       rpre_end();            


    const_post_order_iterator post_begin() const;           
          post_order_iterator post_begin();                 


    const_post_order_iterator post_end()  const;            
          post_order_iterator post_end();                         


    const_reverse_post_order_iterator rpost_begin() const;  
          reverse_post_order_iterator rpost_begin();              


    const_reverse_post_order_iterator rpost_end() const;    
          reverse_post_order_iterator rpost_end();                


    const_deep_iterator deep_begin() const;                 
          deep_iterator       deep_begin();                       

    const_deep_iterator deep_end() const;                   
          deep_iterator deep_end();                               

    const_reverse_deep_iterator rdeep_begin() const;        
          reverse_deep_iterator rdeep_begin();                    


    const_reverse_deep_iterator rdeep_end() const;          
          reverse_deep_iterator rdeep_end();                      

private:

  void  InsertElementsFromList( double& s, std::list<std::pair<ElmPtr,double> >& inList, boost::function<double(ElmPtr)> LengthFunctor );

  iterator   moveRel(   int axis, double const& u,     iterator pos,             std::string invoker );
  iterator rotateRel(   int axis, double const& angle, iterator pos, double pct, std::string invoker );

  std::ostream&           writeTo(std::ostream&);
  friend std::istream& operator>>( std::istream&, beamline& );

  double                  nominalMomentum_;    // In GeV
  LineMode                mode_;
  beamline*               parent_;
  std::list<ElmPtr>       theList_; 

}; 


#include <beamline/beamline_iterators.h> 



#endif // BEAMLINE_H
