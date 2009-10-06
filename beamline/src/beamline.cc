/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      beamline.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
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
******
****** Sep 2006    ostiguy@fnal.gov
****** - eliminated redundant (and dangerous) c-style hard casts
******   (dlist*) this  [ where this is a beamline * ]                    
****** Oct 2006:   ostiguy@fnal.gov
****** - beamline: decoupled list container from public interface
******             now using std::list<> instead of dlist                                                   
****** - introduced new iterators with stl-compatible interface                                                                
****** - eliminated all references to old-style BeamlineIterator, 
******   DeepBeamlineIterator etc ..
****** Jan-Mar 2007 ostiguy@fnal.gov
****** - added support for reference counted elements 
****** - eliminated unneeded dynamic casts             
****** Sep 2007     ostiguy@fnal.gov
****** - new iterator based interface for misalignments and rotations.  
******   introducing a misalignments trough an entire beamline is now 
******   a O(N) operation.
****** - refactored rotateRel(..) moveRel(..): eliminate duplicated code 
****** - eliminated find( ..):    use stl::algorithm instead 
****** - eliminated replace/deepReplace
****** Mar 2008      ostiguy@fnal.gov
****** - introduced support for hierachical information ( i.e. an ElmPtr
******   can discover its parent )
****** Apr 2008     michelotti@fnal.gov
****** - additional argument list for beamline::InsertElementsFromList(..)
******
**************************************************************************
*************************************************************************/

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/LatticeFunctions.h>
#include <beamline/beamline.h>
#include <beamline/BeamlinePropagators.h>
#include <beamline/BmlnElmnt.h>
#include <beamline/combinedFunction.h>
#include <beamline/Drift.h>
#include <beamline/Slot.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>
#include <beamline/sector.h>
#include <beamline/FramePusher.h>
#include <beamline/BmlVisitor.h>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace boost;
using FNAL::pcerr;
using FNAL::pcout;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::beamline( std::string const& nm ) 
: BmlnElmnt( nm ),  nominalMomentum_(0.0), mode_(line), parent_(0), theList_()
{
  propagator_ = PropagatorPtr( new Propagator(*this) ); 
} 


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::beamline( beamline const& a ) 
  : BmlnElmnt(a), 
    nominalMomentum_(a.nominalMomentum_), 
    mode_(a.mode_), 
    parent_(0), 
    theList_() 
{

 // Recursively clone all the beamlines but NOT the elements.

 for ( const_iterator it = a.begin(); it != a.end(); ++it) {
       append(*it);
 }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::~beamline() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline* beamline::clone() const {

 beamline*  bml  = new beamline("");

 bml->BmlnElmnt::operator=(*this); // copy BmlnElmnt state
 
 bml->mode_            = mode_;  
 bml->nominalMomentum_ = nominalMomentum_; 

 // Recursively clone ALL beamlines AND ALL elements.

 for ( beamline::const_iterator it = begin(); it != end(); ++it) {
    bml->append( ElmPtr( (*it)->clone() ) );
 }
 
 return bml;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline& beamline::operator=( beamline const& rhs) {

    if ( &rhs == this) return (*this);  
    
    BmlnElmnt::operator=(*this);

    mode_            = rhs.mode_;  
    nominalMomentum_ = rhs.nominalMomentum_; 
    theList_.clear();

    // Recursively clone all the beamlines objects but NOT the elements.

    for ( const_iterator it = rhs.begin(); it != rhs.end(); ++it) {
       append(*it);
    }
    return *this;  

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::registerReference( Particle const& p, bool scaling )
{
  Particle particle(p);
  propagateReference( particle, p.refBrho(), scaling );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::propagateReference( Particle& particle, double initialBRho, bool scaling ) 
{
  double cdt = 0.0;
     
  for ( iterator it=begin(); it != end(); ++it) {
      (*it)->propagateReference( particle,initialBRho, scaling );
       cdt +=(*it)->getReferenceTime();          
  }

  setReferenceTime( cdt );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char*  beamline::Type() const 
{ 
  return "beamline"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    beamline::isMagnet()  const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    beamline::isThin()  const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    beamline::isPassive()  const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    beamline::isDriftSpace()  const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr& beamline::front() 
{ 
  return   theList_.front();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr const& beamline::front() const
{ 
  return   theList_.front();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr&  beamline::back() 
{ 
   return   theList_.back(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr const& beamline::back() const
{ 
   return   theList_.back(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double beamline::Momentum() const 
{ 
   return nominalMomentum_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double beamline::Length() const
{
  double sum = 0.0;

  for ( const_iterator it  =  begin(); 
                       it !=  end(); ++it ) {
   sum += (*it)->Length(); 
  }
  return sum;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double beamline::OrbitLength( Particle const& x ) const
{
 double s = 0.0;

 for ( beamline::const_iterator it = begin(); it != end(); ++it) {
  s += (*it)->OrbitLength( x );
 }
 return s;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::clear() 
{
  theList_.clear();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::reset() const
{
  std::cout << "beamline::reset() hast been called" << std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::setMomentum( double const& pc ) 
{
 nominalMomentum_ = pc;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::insert( ElmPtr q ) {

  if ( !q) throw GenericException( __FILE__, __LINE__,  "beamline::insert(ElmPtr const&)", 
                                                        "Error: Attempt to insert a null ElmPtr");  

  BmlPtr bml = boost::dynamic_pointer_cast<beamline>( q );

  if (bml)  { 
    bml = BmlPtr( new beamline(*bml) ); 
    theList_.push_front( bml );  
  } 
  else {      
    theList_.push_front( q);  
  }

  if (bml)  bml->parent_ = this; 
  
  length_ += bml ? bml->BmlnElmnt::Length() : q->Length();

}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::insert( BmlnElmnt const& elm ) {

  insert( ElmPtr( elm.clone() ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::append( ElmPtr q ) {

  if ( !q ) throw GenericException( __FILE__, __LINE__, "beamline::append( ElmPtr const& q )", 
                                                        "Error: Attempt to append a null ElmPtr.");  

 BmlPtr bml = boost::dynamic_pointer_cast<beamline>( q );

 if (bml) { 
    bml = BmlPtr( new beamline(*bml) ); 
    theList_.push_back( bml);
 }
 else {
    theList_.push_back(q);
 }

 if (bml)  bml->parent_ = this; 
 length_ += bml ? bml->BmlnElmnt::Length() : q->Length();


} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::append( BmlnElmnt const& elm ) 
{
  append( ElmPtr( elm.clone() ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::InsertElementAt( double const& s_0, double const& s, ElmPtr q )
{
  std::list<std::pair<ElmPtr,double> >  aList;
  std::pair<ElmPtr,double> aPair(q,s);
  aList.push_back( aPair );
  double ss = s_0;
  InsertElementsFromList( ss, aList );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::InsertElementsFromList( double& s, std::list<std::pair<ElmPtr,double> >& inList , boost::function<double(ElmPtr)> Length )
{
  
 static const ElmPtr null;

 beamline::iterator bml_iter = begin();

 ElmPtr  p_be   = ( bml_iter == end() ) ? null : *bml_iter;

 std::pair<ElmPtr,double>  p_ile = inList.front();  // top element; not removed

 bool   firstWarning = true;

 if( !p_be ) {
  (*pcerr) << "\n*** WARNING *** "
              "\n*** WARNING *** beamline::InsertElementsFromList(...)"
              "\n*** WARNING *** The beamline is empty! "
              "\n*** WARNING *** Nothing will be done. "
              "\n*** WARNING *** "
           << endl;
  return;
 }

 if( ! (p_ile.first) ) {
  (*pcerr) << "\n*** WARNING *** "
              "\n*** WARNING *** beamline::InsertElementsFromList(...)"
              "\n*** WARNING *** The list is empty! "
              "\n*** WARNING *** Nothing will be done. "
              "\n*** WARNING *** "
       << endl;
  return;
 }


 double localLength = 0;

 while( p_be && (p_ile.first) ) {

  localLength = Length(p_be);

  if( p_be->isBeamline() )  {

    boost::static_pointer_cast<beamline>(p_be)->InsertElementsFromList( s, inList, Length );

    p_ile = inList.front();   // this may have changed
    ++bml_iter; 
    p_be = (bml_iter == end()) ? null : *bml_iter; 
  }

  else if ( s + localLength <= p_ile.second )  {
    s += localLength;
    ++bml_iter; 
    p_be = (bml_iter == end()) ? null : *bml_iter;     
  }

  else if ( s == p_ile.second ) {
    putAbove( bml_iter, p_ile.first );
    inList.pop_front();             // removes top element
    if (inList.empty() ) break;
    p_ile = inList.front();         // accesses new top element

  }

  else if (  typeid(*p_be) == typeid(combinedFunction)  ) {
    
#if 0
    BmlnElmnt::core_access::get_BmlPtr(*p_be)->InsertElementsFromList( s, inList, Length );

    p_ile = inList.front();     // this may have changed

    ++bml_iter; 
    p_be = (bml_iter == end()) ? null : *bml_iter;     

#endif
    if( firstWarning ) {
      (*pcerr) << "\n*** WARNING: FIXME !                           *** "
                  "\n*** WARNING: Insertion into a combinedFunction *** "
                  "\n*** WARNING: element will hide what is being   *** "
                  "\n*** WARNING: inserted.                         *** "
                  "\n*** WARNING:                                   *** "
               << endl;
      firstWarning = false;
    }

  }

  else if ( ( p_ile.second > s ) && ( p_ile.second < s + localLength ) )  {
    
    std::pair<ElmPtr,ElmPtr> selms = p_be->split( ( p_ile.second - s )/localLength );

    ElmPtr p_be_a =  selms.first;
    ElmPtr p_be_b =  selms.second;

    putAbove( bml_iter, p_be_a      );
    putAbove( bml_iter, p_ile.first );
    putAbove( bml_iter, p_be_b      );

    s +=  Length(p_be_a) + Length(p_ile.first);

    p_be = p_be_b;

    bml_iter = erase( bml_iter );    // bml_iter now points to element downstream of p_be_b !

    inList.pop_front();                       // removes top element
    if (inList.empty() ) break;
    p_ile = inList.front();                   // accesses new top element

    --bml_iter;                               // now points to p_be_b 
  }

  else {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** beamline::InsertElementsFromList( s, list ) "
                "\n*** WARNING *** The impossible has happened at "
                "\n*** WARNING *** s = " << s
             << "\n*** WARNING *** and element: " << p_be->Type() << "  " 
                                                  << p_be->Name()
             << "\n*** WARNING *** "
             << endl;
    (*pcerr) << "Here are the tests:\n";
    (*pcerr) << "else if ( s + localLength <= p_ile.second )\n"
         << "else if ( " << setprecision(10) << ( s + localLength )
         << " <= "       << setprecision(10) << ( p_ile.second )
         << " )\n";
    (*pcerr) << "else if ( s == p_ile->second )\n"
         << "else if ( " << setprecision(10) << s 
         << " == "       << setprecision(10) << p_ile.second 
         << " )\n";
    (*pcerr) << "else if (  typeid(*p_be) == typeid(combinedFunction)  )\n"
         << "else if (  "
         << typeid(*p_be).name()
         << " == "
         << typeid(combinedFunction).name()
         << "  )\n";
    (*pcerr) << "else if ( ( s < p_ile->second ) && ( p_ile->second < s + localLength ) )\n"
         << "else if ( ( " << setprecision(10) << s 
         << " < "          << setprecision(10) << p_ile.second 
         << " ) && ( "     << setprecision(10) << p_ile.second 
         << " < "          << setprecision(10) << ( s + localLength )
         << " ) )\n";

    s += localLength;
    ++bml_iter;
    p_be =  ( bml_iter == end() ) ? null: *bml_iter;
  }
 }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

namespace { 
  double OrbitLength( ElmPtr elm, Particle& p) { return elm->OrbitLength(p); } 
  double Length( ElmPtr elm)                   { return elm->Length();       } 
}

void beamline::InsertElementsFromList( Particle const& particle, double& s, std::list<std::pair<ElmPtr,double> >& inList )
{ 
  InsertElementsFromList(s, inList, boost::bind<double>( ::OrbitLength, _1, particle) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::InsertElementsFromList( double& s, std::list<std::pair<ElmPtr,double> >& inList )
{
  InsertElementsFromList( s, inList, boost::function<double(ElmPtr)>(&::Length) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline* beamline::reverse() const {

 std::string theName = std::string("REVERSE_") + std::string( Name() );
 beamline* result = new beamline( theName.c_str() );

  for ( beamline::const_reverse_iterator rit = rbegin(); rit != rend(); ++rit) {

    if( typeid(**rit) == typeid(beamline) ) 
    {
      result->append(  BmlPtr( dynamic_cast<beamline const&>(**rit).reverse() ) );
    }
    else
    {                      
      result->append(  ElmPtr( (*rit)->clone() ) ); 
    }

  } // for

 return result;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::pair<ElmPtr,ElmPtr> beamline::split( double const& ) const
{
  throw(  GenericException( __FILE__, __LINE__, 
         "beamline::split( double const& )", 
         "This method should not be invoked by a beamline object." ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::putAbove( beamline::iterator it, ElmPtr elm ) 
{
 // Insert y above (before; upstream of) iterator it in the beamline
 // UPON RETURN: it points to the same element, i.e. the position downstream of the 
 // inserted element.  

 std::list<ElmPtr>::iterator ite = it.base();

 BmlPtr bml = boost::dynamic_pointer_cast<beamline>( elm );

 if (bml)  { 
    bml = BmlPtr( new beamline(*bml) );  
    theList_.insert( ite, bml);
 }
 else { 
    theList_.insert( ite, elm);
 }

 length_ += bml ? bml->Length() : elm->Length();

 if (bml)  bml->parent_ = this; 
 
 return it;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::putBelow( beamline::iterator  iter, ElmPtr elm) 
{

 //------------------------------------------------------------------
 // Insert y below (after; downstream of) x in the beamline.
 // UPON RETURN: iter points to the newly inserted element's position. 
 //-------------------------------------------------------------------

 ++iter;

 std::list<ElmPtr>::iterator itx = iter.base(); 

 BmlPtr bml = boost::dynamic_pointer_cast<beamline>( elm );

 if (bml)  { 
    bml = BmlPtr( new beamline(*bml) );  
    theList_.insert( itx, bml);
 }
 else {
    theList_.insert( itx, elm);
 }

 length_ += bml ? bml->Length() : elm->Length();

 if (bml)  bml->parent_ = this; 
 
 return iter;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline beamline::flatten() const {

 beamline r;

 for (beamline::const_deep_iterator it = deep_begin(); it != deep_end(); ++it ) {  
     r.append( (*it) );
 }

 r.setMomentum( this->Momentum() );
 r.rename( this->Name() );
 r.setLineMode( mode_ );

 return r;
} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::startAt( ConstElmPtr const& x, int n ) {

  int count = 0;

  std::list<ElmPtr> newList;

  iterator pos;

  for ( iterator it = begin();  it != end(); ++it) { 
       if ( *it == x )   ++count;            
       if ( count == n ) {  
          pos = it;
          break;
       }
  }
       
  if  ( pos == end() ) return 1; // new starting element not found  
 
  newList.push_back( *theList_.begin() );

  for ( iterator it = pos;  it != end(); ++it) { 
    newList.push_back( *it ) ;
  }

  for ( iterator it = begin();  it != pos; ++it) { 
    newList.push_back( *it );
  }

  theList_ = newList;

  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::startAt( char const* s, int n ) {

  int count = 0;

  std::list<ElmPtr> newList;
  iterator pos;

  for ( iterator it = begin();  it != end(); ++it) { 
       if ( std::string( (*it)->Name() ) == std::string( s ) )   ++count;            
       if ( count == n ) {  
          pos = it;
          break;
       }
  }
       
  if  ( pos == end()) return 1; // new starting element not found  

  newList.push_back( *theList_.begin() );

  for ( iterator it = pos;  it != end(); ++it) { 
    newList.push_back( *it );
  }

  for ( iterator it = begin();  it != pos; ++it) { 
    newList.push_back( *it );
  }

  theList_ = newList;

  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector* beamline::makeSector ( beamline::iterator pos1, beamline::iterator pos2, int deg, JetParticle& jp )  const
{

 //------------------------------------------------------------------------------------
 // Assumes that the argument jp has been initialized as desired by the calling program.  
 // This routine does NOT initialize the state of jp.
 //-------------------------------------------------------------------------------------

 if (pos1 == end() ) return 0;

 Particle particle(jp);
 double s = 0;

 for (beamline::const_iterator it = pos1; it != pos2; ++it) { 
    (*it)->propagate( jp);
    s += (*it)->OrbitLength( particle );
 }

 // FIXME: it is not clear how those attribues should be set 

 //s->length_       = length_;
 //s->strength_     = strength_;
 //s->align_        = align_;
 //s->pAperture_    = pAperture_;

 return new sector( "", jp.state().filter( 0, deg ), s );

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::sectorize( beamline::iterator pos1,  beamline::iterator pos2, int degree, JetParticle& pd, char const* sectorName ) 
{
  
  // sectorize the interval [ pos1, pos2 )
 
  SectorPtr s ( makeSector( pos1, pos2, degree, pd ) );
  s->rename( sectorName );
  
  iterator pos = erase( pos1, pos2 );  
   
  putAbove( pos, s );

  // FIXME: it is not clear what should be done with the beamline attributes 

  // length_       = 0.0;
  // strength_     = 0.0;
  // align_        = 0;
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool beamline::empty() const 
{
  return theList_.empty();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::countHowMany() const 
{
 return theList_.size();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::countHowManyDeeply() const {

 int count = 0;
 ConstBmlPtr bml;
 bool isbml = false;

 for (beamline::const_iterator it  = begin(); 
                               it != end(); ++it ) {

   if ( ( isbml = (*it)->isBeamline() )) bml = dynamic_pointer_cast<beamline const>(*it);  

   count +=  isbml ? bml->countHowManyDeeply() : 1; 
 }

 return count;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::countHowMany(  boost::function<bool(BmlnElmnt const&)> query,  std::list<ElmPtr>& elmlist ) const 
{
  elmlist.clear();
  int ret = 0;

  for (beamline::const_iterator it = begin(); it != end(); ++it) {

     if( !query(**it) )  continue; 

     ++ret; elmlist.push_back(*it);
  }


 return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::countHowManyDeeply( boost::function<bool(BmlnElmnt const&)> query, std::list<ElmPtr>& elmlist ) const 
{

  elmlist.clear();

  int ret = 0;

  for (beamline::const_deep_iterator it = deep_begin(); it != deep_end(); ++it) {
   
     if (!query(**it) ) continue;

     elmlist.push_back(*it); ++ret;  

  }
   
 return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::depth() const
{
  // Returns -1 if beamline is empty.
  // Returns  0 if beamline is flat
  //   or all its subbeamlines are empty.
  // Otherwise returns 1 + largest
  //   depth of all subbeamlines.

  int ret = -1;
  int maxSubDepth = -1;
  

  for (beamline::const_iterator it = begin(); it != end(); ++it) {
    ret = 0;
    if(  typeid(**it) == typeid(beamline) ) {
      int subDepth =  boost::dynamic_pointer_cast<beamline const>(*it)->depth();
      if( maxSubDepth < subDepth ) { maxSubDepth = subDepth; }
    }
  }

  if( 0 == ret ) { ret = maxSubDepth + 1; }
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::erase( beamline::iterator pos1, beamline::iterator pos2 ) 
{
//--------------------------------------------------
// NOTE: erase the range [ pos1, pos2 )
//--------------------------------------------------
 std::list<ElmPtr>::iterator pos1x = pos1.base();
 std::list<ElmPtr>::iterator pos2x = pos2.base();
 
 return iterator( this, theList_.erase( pos1x, pos2x) ); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::moveRelX( beamline::iterator ipos, double const& u )
{
  return moveRel( Frame::xAxisIndex(), u, ipos, string("beamline::moveRelX") );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::moveRelY(  beamline::iterator ipos, double const& u )
{
  return moveRel( Frame::yAxisIndex(), u, ipos, string("beamline::moveRelX") );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::moveRelZ(  beamline::iterator ipos, double const& u )
{
  return moveRel( Frame::zAxisIndex(), u, ipos, string("beamline::moveRelX") );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::moveRel( int axis, double const& u, beamline::iterator ipos, string invoker )
{
  //----------------------------------------------------------------------------------------------------
  // Upon entry: axis            = index of axis along displacement
  //             u          [m]  = displacement
  //             ipos            = iterator pointing at element to be translated
  //             invoker         = name of calling routine (used in error messages)
  //             
  // Upon exit:  The element pointed at  will have been displaced in the direction
  //             of its local x coordinate by adjusting its neighboring
  //             free-space elements. 
  //             The element is not altered; only its
  //             neighbors are. altered. 
  //             The iterator is modified, but still points 
  //             to the same element as upon entry.
  //             errorCode  = 0, nothing wrong
  //                          1, errorCode, ret, or thePtr is null on entry
  //                          2, i != 0, 1, or 2
  //                          3, |u| < 1 nanometer displacement
  //-----------------------------------------------------------------------------------------------------  

  //-----------------------------------------------------------------------------------------------------
  //
  //          ------------- ---------- ------------
  //          UPSTREAM ELM |   ELM   | DWSTREAM ELM
  //          ------------- ---------- ------------
  //         F0            F1        F2           F3
  //-----------------------------------------------------------------------------------------------------


  // Argument filter

  int errorCode = 0;
 
  ElmPtr thePtr = *ipos; 

  if( !thePtr ) { errorCode = 1; return ipos;}

  if( axis < 0 || 2 < axis ) {
    errorCode = 2;
    return ipos;
  }

  // Will not displace anything less than 1 nanometer
  if( std::abs(u) < 1.0e-9 ) { 
    (*pcerr) << "\n*** ERROR *** "
         << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** Called by " << invoker
         << "\n*** ERROR *** Unable to perform operation on "
         << thePtr->Type() << "  " << thePtr->Name() << "."
         << "\n*** ERROR *** Requested displacement, " 
         << (1.e9*u) 
         << ", nanometers is too small."
            "\n*** ERROR *** Must be at least 1 nanometer."
            "\n*** ERROR *** "
         << endl;
    errorCode = 3;
    return ipos;
  }

  iterator it  = ipos;
 
  ElmPtr  downStreamPtr    = ( ++it != end()    ) ?  *it :  ElmPtr();
  it = ipos;
  ElmPtr  upStreamPtr      = ( it   == begin()  ) ? ElmPtr() : *(--it);
  it = ipos;

  
  Frame const frameZero; 
  FramePusher fp(frameZero); 

  Frame frameOne   =    upStreamPtr ? upStreamPtr->accept(fp),   fp.getFrame() : fp.getFrame();
  Frame frameTwo   =  ( thePtr->accept( fp ),  fp.getFrame() );
  Frame frameThree =  downStreamPtr ? downStreamPtr->accept(fp), fp.getFrame() : fp.getFrame();

  if( !upStreamPtr) {
     (*pcerr) << "\n*** WARNING *** "
              << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
              << "\n*** WARNING *** Called by " << invoker
              << "\n*** WARNING *** Will displace using downstream end of "
              << thePtr->Type() << "  " << thePtr->Name() << "."
              << "\n*** WARNING *** There is no upstream neighbor."
              << "\n*** WARNING *** "
              << endl;
  }

  Frame pinnedFrameOne = ( (thePtr->pinnedFrames_).upStream()   ).patchedOnto( frameOne );
  Frame pinnedFrameTwo = ( (thePtr->pinnedFrames_).downStream() ).patchedOnto( frameTwo );


  //.............................................................................. 
  // !!! The next lines can be modified (maybe) to do pinned-referenced movements
  //..............................................................................

  Vector displacement(u*frameOne.getAxis(axis));

  frameOne.translate( displacement );
  frameTwo.translate( displacement );

  (thePtr->pinnedFrames_).upStream(   pinnedFrameOne.relativeTo( frameOne ) );
  (thePtr->pinnedFrames_).downStream( pinnedFrameTwo.relativeTo( frameTwo ) );

  SlotPtr sp;

  if( typeid(*upStreamPtr) == typeid(Drift) ) {
      SlotPtr slotPtr( new Slot(upStreamPtr->Name(), frameOne ) );
      slotPtr->setReferenceTime( upStreamPtr->getReferenceTime() );
      slotPtr->pinnedFrames_.downStream( (thePtr->pinnedFrames_).upStream() );
      it = erase( --it );
      putAbove( it, slotPtr ); 
  } 
  else if( sp = boost::dynamic_pointer_cast<Slot>(upStreamPtr) ) {
    sp->setInFrame ( frameZero );
    sp->setOutFrame( frameOne );
    sp->pinnedFrames_.downStream ( (thePtr->pinnedFrames_).upStream() );  // ??? This is not quite right.
  } 
 
  
  if( typeid(*downStreamPtr) == typeid(Drift) ) {
      SlotPtr slotPtr( new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) ) );
      slotPtr->setReferenceTime( downStreamPtr->getReferenceTime() );
      slotPtr->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() );
      it =  erase( ++it );
      putAbove( it, slotPtr ); 
      --it; --it;
   }
   else if( sp = boost::dynamic_pointer_cast<Slot>(downStreamPtr) ) {
      sp->setInFrame( frameZero );
      sp->setOutFrame( frameThree.relativeTo(frameTwo) );
      sp->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() ); // ??? This is not quite right.
   }
   
  return it; 

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 beamline::iterator beamline::pitch(  beamline::iterator pos, double const& angle, double const& pct )
{
  return rotateRel( Frame::xAxisIndex(), angle, pos, pct, string("beamline::pitch") );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::yaw(   beamline::iterator pos, double const& angle, double const& pct )
{
  return rotateRel( Frame::yAxisIndex(), angle, pos, pct, string("beamline::yaw"));
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::roll(   beamline::iterator pos, double const& angle, double const& pct )
{
  return rotateRel( Frame::zAxisIndex(), angle, pos, pct, string("beamline::roll") );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::rotateRel(   int axis, double const& angle, iterator ipos, double pct, string invoker )
{
  //-----------------------------------------------------------------------------------------------
  // Upon entry: axis            = rotation direction, expressed a unit vector
  //             angle [radians] = displacement 
  //             pos             = iterator pointing to element to be translated
  //             pct             = percentage downstream of element to serve
  //                               as fixed point of the rotation
  //             invoker         = name of calling routine (used in error messages)
  //             
  // Upon exit:  element at pos will have been rotated along the direction "axis"
  //             by adjusting its neighboring free-space elements. 
  //             The element at pos is not changed; only its neighbors are altered.
  //             
  //             errorCode  = 0, nothing wrong
  //                          2, |angle| < 1 nanoradian displacement
  //                          3, the element pointed at by the iterator is a sector element
  //-------------------------------------------------------------------------------------------------  

  //-----------------------------------------------------------------------------------------------------
  //
  //          ------------- ---------- ------------
  //          UPSTREAM ELM |   ELM   | DWSTREAM ELM
  //          ------------- ---------- ------------
  //         F0            F1        F2           F3
  //-----------------------------------------------------------------------------------------------------


  // Argument filter

  int errorCode = 0;
  ElmPtr thePtr = *ipos; 

  if( pct < 0.0 || pct > 1.0 )       { pct = 0.5; }
  if( std::abs(pct) < 1.0e-8 )       { pct = 0.0; }
  if( std::abs(1.0 - pct) < 1.0e-8 ) { pct = 1.0; }

  // Will not rotate anything less than 1 nanoradian.
  if( std::abs(angle) < 1.0e-9 ) { 
    (*pcerr) << "\n*** ERROR *** "
         << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** Called by " << invoker
         << "\n*** ERROR *** Unable to perform operation on "
         << thePtr->Type() << "  " << thePtr->Name() << "."
         << "\n*** ERROR *** Requested rotation angle, " 
         << (1.0e9*angle) 
         << ", nanoradians is too small."
            "\n*** ERROR *** Must be at least 1 nanoradian."
            "\n*** ERROR *** "
         << endl;
    errorCode = 2;
    return ipos;
  }

  // Check for a valid element
  if( typeid(*thePtr) == typeid(sector) ) {
    (*pcerr) << "\n*** ERROR *** "
         << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** Called by " << invoker
         << "\n*** ERROR *** Unable to perform rotation on "
         << thePtr->Type() << "  " << thePtr->Name() << "."
         << endl;
    errorCode = 3;
    return ipos;
  }

  beamline::iterator it  = ipos;
 
  ElmPtr  downStreamPtr    = ( ++it != end()    ) ?  *it :  ElmPtr();
  it = ipos;
  ElmPtr  upStreamPtr      = ( it   == begin()  ) ? ElmPtr() : *(--it);
  it = ipos;

  SlotPtr sp; 

  Frame const frameZero;

  FramePusher fp( frameZero );

  Frame frameOne   =  upStreamPtr   ?  ( upStreamPtr->accept(fp),  fp.getFrame() )  : fp.getFrame();
  Frame frameTwo   =  ( thePtr->accept( fp ),  fp.getFrame() );
  Frame frameThree =  downStreamPtr ?  ( downStreamPtr->accept(fp), fp.getFrame() ) : fp.getFrame();
 
  Frame pinnedFrameOne = ( (thePtr->pinnedFrames_).upStream()   ).patchedOnto( frameOne );
  Frame pinnedFrameTwo = ( (thePtr->pinnedFrames_).downStream() ).patchedOnto( frameTwo );

  //................................................... 
  // Construct a Frame in between frameOne and frameTwo
  //...................................................

  Frame midFrame;
  if     ( 0.0 == pct ) { 
      midFrame = frameOne; 
  } else if( 1.0 == pct ){ 
      midFrame = frameTwo; 
  } else if(    typeid( *thePtr )  == typeid(Slot) || typeid( *thePtr )  == typeid(beamline) ) {
      midFrame = Frame::tween( frameOne, frameTwo, pct );
  } else {
      std::pair<ElmPtr,ElmPtr> selms = thePtr->split(pct );
      ElmPtr usHalfPtr = selms.first;
      ElmPtr dsHalfPtr = selms.second;
      FramePusher fp2( frameOne );
      usHalfPtr->accept(fp2);
      midFrame = fp2.getFrame();
  }

  //................................................... 
  // !!! The next lines can be modified (maybe) to do pinned-referenced movements
  // Do the rotation
  //................................................... 
 
   Vector rotationAxis( midFrame.getAxis(axis) );

   Frame uFrame( frameOne.relativeTo(midFrame) );
   Frame dFrame( frameTwo.relativeTo(midFrame) );

   uFrame.rotate( angle, rotationAxis, true );
   dFrame.rotate( angle, rotationAxis, true );

   frameOne = uFrame.patchedOnto(midFrame);
   frameTwo = dFrame.patchedOnto(midFrame);

   (thePtr->pinnedFrames_).upStream(   pinnedFrameOne.relativeTo( frameOne ) );
   (thePtr->pinnedFrames_).downStream( pinnedFrameTwo.relativeTo( frameTwo ) );

   // Reset upstream and downstream elements

   if(  sp = boost::dynamic_pointer_cast<Slot>( upStreamPtr ) ) {
        sp->setInFrame( frameZero );
        sp->setOutFrame( frameOne );
        sp->pinnedFrames_.downStream( (thePtr->pinnedFrames_).upStream() );
   }
   if( sp = boost::dynamic_pointer_cast<Slot>( downStreamPtr ) ) {
        sp->setInFrame( frameZero );
        sp->setOutFrame( frameThree.relativeTo(frameTwo) );
        sp->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() );
   }
   if( typeid(*upStreamPtr) == typeid(Drift) ) {
        SlotPtr slotPtr( new Slot(upStreamPtr->Name(), frameOne ) );
        slotPtr->setReferenceTime( upStreamPtr->getReferenceTime() );
        slotPtr->pinnedFrames_.downStream ((thePtr->pinnedFrames_).upStream() );
        it = erase( --it );
        putAbove( it, slotPtr ); 
   }
   if( typeid(*downStreamPtr) == typeid(Drift) ) {
       SlotPtr slotPtr( new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) ) );
       slotPtr->setReferenceTime( downStreamPtr->getReferenceTime() );
       slotPtr->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() );
       it = erase( ++it );
       putAbove( it, slotPtr ); 
       --it; --it;
   }

   return it;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool beamline::setAlignment( Alignment const& al ) {

  // Propagate alignment data of entire  beamline to each individual element

  for (beamline::iterator it = begin() ; it != end(); ++it) {

    if( !(*it)->setAlignment(al)  ) {
      (*pcerr) << "\n*** ERROR *** "
           << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
           << "\n*** ERROR *** bool beamline::setAlignment( Alignment const& al )"
              "\n*** ERROR *** Unable to perform operation on "
           << (*it)->Type() << "  " << (*it)->Name()
           << "\n*** ERROR *** without affecting its neighbors."
              "\n*** ERROR *** The beamline has been left in a corrupted state."
              "\n*** ERROR *** "
           << endl;
      return false;
    }
  }
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::remove( ElmPtr elm ) 
{ 
  theList_.remove( elm ); 
  return; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& beamline::writeTo(ostream& os) {

#if 0
  double momentum = Momentum();

  os <<  OSTREAM_DOUBLE_PREC << momentum << endl;
  for (beamline::iterator it = begin() ; it != end(); ++it) {
    os << (**it);
  }
  os << "beamline_END " << Name() << " 0 0 0 0 0\n";
#endif
  return os;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool beamline::isFlat() const
{

  for (beamline::const_iterator it = begin() ; it != end(); ++it) {

    if( typeid(**it) == typeid(beamline) ) return false; 
  }

  return true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::accept( BmlVisitor& v )            
{  
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::accept( ConstBmlVisitor& v ) const 
{  
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


beamline::iterator beamline::erase( beamline::iterator it)
{
  std::list<ElmPtr>::iterator lit = it.base();  
 
  lit = theList_.erase( lit );    

  return beamline::iterator( this, lit);
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  beamline::isBeamline() const 
{ 
  return true; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::size() const 
{
  return theList_.size();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//-------------------------------------------------------------------------------------------
// Member functions related to iterators.
//-------------------------------------------------------------------------------------------

beamline::const_iterator beamline::begin() const                           
{ 
  return  const_iterator(  this,  theList_.begin() );         
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator    beamline::begin()                                 
{ 
  return        iterator(  this,  theList_.begin() );         
}       

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::const_iterator beamline::end()   const
{ 
  return  const_iterator( this, theList_.end() );             
}   
    
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::end()                                         
{ 
  return  iterator( this, theList_.end());              
}   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
beamline::const_reverse_iterator beamline::rbegin() const
{ 
  return   const_reverse_iterator(end());                     
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::reverse_iterator beamline::rbegin()                              
{ 
  return reverse_iterator(end());                     
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::const_reverse_iterator beamline::rend()   const
{ 
 return  const_reverse_iterator( begin() );                  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::reverse_iterator       beamline::rend()
{ 
 return  reverse_iterator( begin() );                  
}
   
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::const_pre_order_iterator beamline::pre_begin() const
{ 
  return  const_pre_order_iterator( begin() );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::pre_order_iterator       beamline::pre_begin()
{ 
  return pre_order_iterator( begin() );
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::const_pre_order_iterator beamline::pre_end() const
{ 
  return  const_pre_order_iterator( end() );
}   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::pre_order_iterator       beamline::pre_end()
{ 
  return pre_order_iterator( end() );
}   
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


beamline::const_reverse_pre_order_iterator beamline::rpre_begin()  const
{ 
 return const_reverse_pre_order_iterator(  pre_end() );      
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::reverse_pre_order_iterator       beamline::rpre_begin()
{ 
  return       reverse_pre_order_iterator(  pre_end() );      
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::const_reverse_pre_order_iterator beamline::rpre_end() const
{ 
  return const_reverse_pre_order_iterator( pre_begin() );     
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::reverse_pre_order_iterator       beamline::rpre_end()
{ 
 return       reverse_pre_order_iterator( pre_begin() );     
}   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


beamline::const_post_order_iterator beamline::post_begin() const
{ 
  return  const_post_order_iterator( begin() );               
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::post_order_iterator       beamline::post_begin()
{ 
 return        post_order_iterator( begin() );
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::const_post_order_iterator beamline::post_end()  const
{ 
  return  const_post_order_iterator( end() );                 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::post_order_iterator beamline::post_end()
{ 
  return        post_order_iterator( end() );                 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


beamline::const_reverse_post_order_iterator beamline::rpost_begin() const
{ 
  return  const_reverse_post_order_iterator( post_end() );    
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::reverse_post_order_iterator beamline::rpost_begin() 
{ 
  return        reverse_post_order_iterator( post_end() );    
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::const_reverse_post_order_iterator beamline::rpost_end() const
{ 
  return  const_reverse_post_order_iterator( post_begin() );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::reverse_post_order_iterator beamline::rpost_end()
{  
  return reverse_post_order_iterator( post_begin() );  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::const_deep_iterator beamline::deep_begin() const
{ 
   if ( begin() == end() ) return  const_deep_iterator( begin() ); 
   const_iterator it = begin();
   BmlPtr bml; 
   while ( bml = boost::dynamic_pointer_cast<beamline>(*it) ) { it = bml->begin(); } 
   return  const_deep_iterator(it );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::deep_iterator       beamline::deep_begin()
{ 
  if ( begin() == end() ) return   deep_iterator( begin() ); 
  iterator it = begin();
  BmlPtr bml; 
  while ( bml = boost::dynamic_pointer_cast<beamline>(*it) ) { it = bml->begin(); } 
  return  deep_iterator(it);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::const_deep_iterator  beamline::deep_end() const
{ 
  if ( begin() == end() ) return   const_deep_iterator( end() ); 
  return   const_deep_iterator(  end() );                     
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::deep_iterator beamline::deep_end() 
{ 
   if ( begin() == end() ) return   deep_iterator( end() ); 
   return   deep_iterator(  end() );                     
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::const_reverse_deep_iterator beamline::rdeep_begin() const 
{ 
  return  const_reverse_deep_iterator( deep_end() );          
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::reverse_deep_iterator beamline::rdeep_begin()
{ 
    return        reverse_deep_iterator( deep_end() );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::const_reverse_deep_iterator beamline::rdeep_end() const
{ 
  return  const_reverse_deep_iterator( deep_begin() );        
}   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::reverse_deep_iterator beamline::rdeep_end()
{  
  return        reverse_deep_iterator( deep_begin() );        
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

