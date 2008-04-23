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
*****
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
****** Sep 2006    Jean-Francois Ostiguy ostiguy@fnal.gov
******
****** - eliminated redundant (and dangerous) c-style hard casts
******   (dlist*) this  [ where this is a beamline * ]                    
****** 
****** Oct 2006:  Jean-Francois Ostiguy  ostiguy@fnal.gov
******
****** - beamline: decoupled list container from public interface
******             now using std::list<> instead of dlist                                                   
****** - introduced new iterators with stl-compatible interface                                                                
****** - eliminated all references to old-style BeamlineIterator, 
******   DeepBeamlineIterator etc ..
******
****** Jan-Mar 2007  ostiguy@fnal.gov
******
****** - added support for reference counted elements 
****** - eliminated unneeded dynamic casts             
******
****** Sep 2007      ostiguy@fnal.gov
****** - new iterator based interface for misalignments and rotations.  
******   introducing a misalignments trough an entire beamline is now 
******   a O(N) operation.
****** - refactored rotateRel(..) moveRel(..): eliminate duplicated code 
****** - eliminated find( ..):    use stl::algorithm instead 
****** - eliminated replace/deepReplace
******        
****** Mar 2008      ostiguy@fnal.gov
****** - introduced support for hierachical information ( i.e. an ElmPtr
******   can discover its parent )
**************************************************************************
*************************************************************************/

#include <basic_toolkit/iosetup.h>
#include <beamline/LatticeFunctions.h>
#include <beamline/beamline.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/combinedFunction.h>
#include <beamline/drift.h>
#include <beamline/Slot.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>
#include <beamline/sector.h>
#include <beamline/FramePusher.h>
#include <beamline/BmlVisitor.h>


#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace boost;
using FNAL::pcerr;
using FNAL::pcout;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// **************************************************
//   class beamline
// **************************************************

beamline::beamline( std::string const& nm ) 
: bmlnElmnt( nm ),  nominalMomentum_(NOTKNOWN), mode_(unknown), parent_(0), theList_()
{} 


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::beamline( beamline const& a ) 
  : bmlnElmnt(a), 
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

beamline* beamline::Clone() const {

 beamline*  bml  = new beamline("");

 bml->bmlnElmnt::operator=(*this); // copy bmlnElmnt state
 
 bml->mode_          = mode_;  
 bml->nominalMomentum_ = nominalMomentum_; 

 // Recursively clone ALL beamlines AND ALL elements.

 for ( beamline::const_iterator it = begin(); it != end(); ++it) {
    bml->append( ElmPtr( (*it)->Clone() ) );
 }
 
 return bml;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline& beamline::operator=( beamline const& rhs) {

    if ( &rhs == this) return (*this);  
    
    bmlnElmnt::operator=(*this);

    mode_            = rhs.mode_;  
    nominalMomentum_ = rhs.nominalMomentum_; 
    theList_.clear();

    // Recursively clone all the beamlines objects but NOT the elements.

    for ( const_iterator it = begin(); it != end(); ++it) {
       append(*it);
    }
    return *this;  

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

ElmPtr& beamline::firstElement() 
{ 
  return   *begin(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr const& beamline::firstElement() const
{ 
    return   *begin();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr&  beamline::lastElement() 
{ 
   return   theList_.back(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr const& beamline::lastElement() const
{ 
   return   theList_.back(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double beamline::Momentum() const 
{ 
   return nominalMomentum_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double beamline::OrbitLength( Particle const& x ) 
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

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::localPropagate( Particle& x ) 
{
 for ( beamline::iterator it = begin(); it != end();  ++it ) { 

   (*it)->propagate( x );
 }
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::localPropagate( ParticleBunch& x ) 
{
 for (beamline::iterator it = begin(); it != end();  ++it ) { 
   (*it) -> propagate( x );
 }
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::localPropagate( JetParticle& x ) 
{

 for (beamline::iterator it = begin(); it != end();  ++it ) { 
 
   (*it)->propagate( x );

 }
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::localPropagate( JetParticleBunch& x ) 
{
 for (beamline::iterator it = begin(); it != end();  ++it ) { 
   (*it) -> propagate( x );
 }
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::setMomentum( double const& pc ) 
{
 nominalMomentum_ = pc;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::eraseBarnacles( char const* s )
{

 for (beamline::deep_iterator it = deep_begin(); it != deep_end(); ++it ) { 
     (*it)->dataHook.eraseAll( s );
  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LattRing beamline::whatIsRing() 
{
  LattRing errRet;

  BarnacleList::iterator it = dataHook.find( "Ring" ); 


 if( it == dataHook.end() ) {
    (*pcout) << endl;
    (*pcout) << "*** WARNING ***                            \n"
         << "*** WARNING *** beamline::whatIsRing       \n"
         << "*** WARNING *** Entry was not found.       \n"
         << "*** WARNING *** Meaningless value being    \n"
         << "*** WARNING *** returned.                  \n"
         << "*** WARNING ***                            \n" << endl;
  return errRet;
 }
 
  return any_cast<LattRing>(it->info);
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
  
  length_ += bml ? bml->Length() : q->Length();

}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::insert( bmlnElmnt const& elm ) {

  insert( ElmPtr( elm.Clone() ) );
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
 length_ += bml ? bml->Length() : q->Length();


} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::append( bmlnElmnt const& elm ) 
{
  append( ElmPtr( elm.Clone() ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void beamline::InsertElementsFromList( Particle const& particle, double& s, std::list<std::pair<ElmPtr,double> >& inList)
{

 static const ElmPtr null;

 beamline::iterator bml_iter = begin();

 ElmPtr  p_be   = ( bml_iter == end() ) ? null : *bml_iter;

 ElmPtr p_be_a;
 ElmPtr p_be_b;

 std::pair<ElmPtr,double>  p_ile = inList.front();  // top element; not removed

 bool   firstWarning = true;

 if( !p_be ) {
  (*pcerr) << "\n"
          "*** WARNING ***                                     \n"
          "*** WARNING *** beamline::InsertElementsFromList( s_0, list ) \n"
          "*** WARNING *** The beamline is empty!              \n"
          "*** WARNING *** Nothing will be done.               \n"
          "*** WARNING ***                                     \n"
       << endl;
  return;
 }

 if( ! (p_ile.first) ) {
  (*pcerr) << "\n"
          "*** WARNING ***                                     \n"
          "*** WARNING *** beamline::InsertElementsFromList( s_0, list ) \n"
          "*** WARNING *** The list is empty!                  \n"
          "*** WARNING *** Nothing will be done.               \n"
          "*** WARNING ***                                     \n"
       << endl;
  return;
 }


 Particle lparticle(particle);

 while( p_be && (p_ile.first) ) {

  if( typeid(*p_be) == typeid(beamline) ) {

    boost::static_pointer_cast<beamline>(p_be)->InsertElementsFromList( lparticle, s, inList );

    p_ile = inList.front();   // this may have changed
    ++bml_iter; 
    p_be = (bml_iter == end()) ? null : *bml_iter; 
  }

  else if ( s + p_be->OrbitLength( lparticle ) <= p_ile.second ) {

    s += p_be->OrbitLength( lparticle );
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

    p_be->bml_->InsertElementsFromList( lparticle, s, inList );
    p_ile = inList.front();     // this may have changed

    ++bml_iter; 
    p_be = (bml_iter == end()) ? null : *bml_iter;     

    if( firstWarning ) {
      (*pcerr) << "\n*** WARNING:                                   *** "
                   "\n*** WARNING: Insertion into a combinedFunction *** "
                   "\n*** WARNING: element will hide what is being   *** "
                   "\n*** WARNING: inserted.                         *** "
                   "\n*** WARNING:                                   *** "
               << endl;
      firstWarning = false;
    }
  }

  else if ( ( p_ile.second > s ) && ( p_ile.second < s + p_be->OrbitLength( lparticle ) ) )  {

    p_be->Split( ( p_ile.second - s )/p_be->OrbitLength( lparticle ), p_be_a,  p_be_b );


    putAbove( bml_iter, p_be_a      );
    putAbove( bml_iter, p_ile.first );
    putAbove( bml_iter, p_be_b      );

    s += ( p_be_a->OrbitLength( lparticle ) + p_ile.first->OrbitLength( lparticle ) );

  
    p_be = p_be_b;

    bml_iter = erase( bml_iter );             // bml_iter now points to element downstream of p_be_b !

    inList.pop_front();                       // removes top element
    if (inList.empty() ) break;
    p_ile = inList.front();                   // accesses new top element

    --bml_iter;                               // now points to p_be_b 

  }

  else {
    (*pcerr) << "\n"
            "*** WARNING ***                                     \n"
            "*** WARNING *** beamline::InsertElementsFromList( s, list ) \n"
            "*** WARNING *** The impossible has happened at      \n"
            "*** WARNING ***                                     \n"
            "*** WARNING *** s = " << s << "                     \n"
            "*** WARNING ***                                     \n"
            "*** WARNING *** and element: " << p_be->Type() << "  " 
                                            << p_be->Name() << " \n"
            "*** WARNING ***                                     \n"
         << endl;

    (*pcerr) << "Here are the tests:\n";
   (*pcerr) << "else if ( s + p_be->OrbitLength( lparticle ) <= p_ile.second )\n"
         << "else if ( " << setprecision(10) << ( s + p_be->OrbitLength( lparticle ) )
         << " <= "       << setprecision(10) << ( p_ile.second )
         << " )\n";
    (*pcerr) << "else if ( s == p_ile->second )\n"
         << "else if ( " << setprecision(10) << s 
         << " == "       << setprecision(10) << p_ile.second 
         << " )\n";
    (*pcerr) << "else if ( p_be->bml_ )\n"
         << "else if ( " << p_be->bml_ << " )\n";
    (*pcerr) << "else if ( ( s < p_ile->second ) && ( p_ile->second < s + p_be->OrbitLength( lparticle ) ) )\n"
         << "else if ( ( " << setprecision(10) << s 
         << " < "          << setprecision(10) << p_ile.second 
         << " ) && ( "     << setprecision(10) << p_ile.second 
         << " < "          << setprecision(10) << ( s + p_be->OrbitLength( lparticle ) )
         << " ) )\n";


    s += p_be->OrbitLength( lparticle );

    ++bml_iter;
    p_be =  ( bml_iter == end() ) ? null: *bml_iter;

  }
 }

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
      result->append(  ElmPtr( (*rit)->Clone() ) ); 
    }

  } // for

 return result;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::Split( double const&, ElmPtr&, ElmPtr& ) const
{
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "void beamline::Split( double const&, bmlnElmnt&, bmlnElmnt& )", 
         "This method should not be invoked by a beamline object." ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::putAbove( beamline::iterator it, ElmPtr elm ) 
{
 // Insert y above (before; upstream of) iter in the beamline
 // UPON RETURN: iter points to the same element, i.e. the position downstream of the 
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



}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::putBelow( beamline::iterator  iter, ElmPtr elm) 
{

 //------------------------------------------------------------------
 // Insert y below (after; downstream of) x in the beamline.
 // UPON RETURN: iter points to the newly inserted element's position. 
 //-------------------------------------------------------------------

 if ( iter == end() ) {
   (*pcout) << "*** WARNING **** : attempt to insert an element downstream of a beamline's end." << std::endl;
   (*pcout) << "                   beamline name :" << Name()  << std::endl;
   return iter;
 }

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


    // ++++++++++++ End:   Insert and append functions ++++++++++++++++++


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

 return new sector( "", jp.State().filter( 0, deg ), s );

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

void beamline::peekAt( double& s, Particle const& prt ) const
{

  bmlnElmnt* p;

  (*pcout) << "\nBegin beamline::peekat() -- Address of beamline: "
       << ident_ << " = " << (int) this 
       << endl;

 for (beamline::const_iterator it = begin(); it != end(); ++it )  {

    if( typeid( **it ) == typeid(beamline) ) 
      boost::static_pointer_cast<beamline>(*it)->peekAt( s, prt );
    else (*it)->peekAt( s, prt );
  }

  (*pcout) << "End beamline::peekat() -- Address of beamline: "
       << ident_ << " = " << (int) this 
       << endl;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool beamline::empty() const {

  return theList_.empty();

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::countHowMany() const {

 int count = 0;

 for (beamline::const_iterator it = begin(); it != end(); ++it, ++count);
 
 return count; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::countHowManyDeeply() const {

 int count = 0;

 for (beamline::const_deep_iterator it  = deep_begin(); 
                                    it != deep_end(); ++it, ++count); 

 return count;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::countHowMany(  boost::function<bool(bmlnElmnt const&)> query,  std::list<ElmPtr>& elmlist ) const 
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

int beamline::countHowManyDeeply( boost::function<bool(bmlnElmnt const&)> query, std::list<ElmPtr>& elmlist ) const 
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

  if( typeid(*upStreamPtr) == typeid(drift) ) {
      SlotPtr slotPtr( new Slot(upStreamPtr->Name().c_str(), frameOne ) );
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
 
  
  if( typeid(*downStreamPtr) == typeid(drift) ) {
      SlotPtr slotPtr( new Slot(downStreamPtr->Name().c_str(), frameThree.relativeTo(frameTwo) ) );
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
  Frame frameOne, frameTwo, frameThree;
  Frame pinnedFrameOne, pinnedFrameTwo; 

  FramePusher fp( frameZero );

  frameOne   =  upStreamPtr ? upStreamPtr->accept(fp),  fp.getFrame() : fp.getFrame();
  frameTwo   =  ( thePtr->accept( fp ),  fp.getFrame() );
  frameThree =  downStreamPtr ? downStreamPtr->accept(fp), fp.getFrame() : fp.getFrame();
 
  pinnedFrameOne = ( (thePtr->pinnedFrames_).upStream()   ).patchedOnto( frameOne );
  pinnedFrameTwo = ( (thePtr->pinnedFrames_).downStream() ).patchedOnto( frameTwo );

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
      ElmPtr usHalfPtr, dsHalfPtr;
      thePtr->Split( pct, usHalfPtr, dsHalfPtr );
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
   if( typeid(*upStreamPtr) == typeid(drift) ) {
        SlotPtr slotPtr( new Slot(upStreamPtr->Name().c_str(), frameOne ) );
        slotPtr->setReferenceTime( upStreamPtr->getReferenceTime() );
        slotPtr->pinnedFrames_.downStream ((thePtr->pinnedFrames_).upStream() );
        it = erase( --it );
        putAbove( it, slotPtr ); 
   }
   if( typeid(*downStreamPtr) == typeid(drift) ) {
       SlotPtr slotPtr( new Slot(downStreamPtr->Name().c_str(), frameThree.relativeTo(frameTwo) ) );
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

bool beamline::setAlignment( alignmentData const& al ) {
  // Propogate alignment data of entire  beamline to each individual element


  for (beamline::iterator it = begin() ; it != end(); ++it) {

    if( !(*it)->setAlignment(al)  ) {
      (*pcerr) << "\n*** ERROR *** "
           << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
           << "\n*** ERROR *** bool beamline::setAlignment( const alignmentData& al )"
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

  double momentum = Momentum();

  os <<  OSTREAM_DOUBLE_PREC << momentum << endl;
  for (beamline::iterator it = begin() ; it != end(); ++it) {
    os << (**it);
  }
  os << "beamline_END " << Name() << " 0 0 0 0 0\n";
  return os;
}


// **************************************************
//   Frame functions
// **************************************************


void beamline::enterLocalFrame( Particle& p ) const
{

  // Check for bends


  for (beamline::const_iterator it = begin() ; it != end(); ++it) {

    if( (typeid( *(*it) ) == typeid(sbend) ) ||
        (typeid( *(*it) ) == typeid(rbend) ) ) {    

      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void beamline::enterLocalFrame( Particle& p ) const", 
             "Not implemented for beamlines containing bends." ) );
    }
  }
  
  bmlnElmnt::enterLocalFrame( p );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::enterLocalFrame( JetParticle& p ) const
{
  // Check for bends

  for (beamline::const_iterator it = begin() ; it != end(); ++it) {

    if( (typeid( *(*it) ) == typeid(sbend) )||
        (typeid( *(*it) ) == typeid(rbend) )  ) {
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void beamline::enterLocalFrame( JetParticle& p ) const", 
             "Not implemented for beamlines containing bends." ) );
    }
  }
  
  bmlnElmnt::enterLocalFrame( p );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::leaveLocalFrame( Particle& p ) const
{
  bmlnElmnt::leaveLocalFrame( p );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::leaveLocalFrame( JetParticle& p ) const
{
  bmlnElmnt::leaveLocalFrame( p );
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

double beamline::getReferenceTime()  const
{

  double ct = 0.0; 
  for ( beamline::const_iterator it = begin(); it != end(); ++it ) {

    ct += (*it)->getReferenceTime();

  } 
  return ct; 
}     

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::setReferenceTime( double const& ct)
{ 
  (*pcerr) << "*** WARNING ****: Attempt to explicitly set the reference time attribute of a beamline. " << std::endl;
  (*pcerr) << "*** WARNING ****: This is most likely an error." << std::endl; 
  (*pcerr) << "*** WARNING ****: Continuing, nontheless... " << std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::setLength( double const& )
{ 
  (*pcerr) <<   "*** WARNING ****: "
              "\n*** WARNING ****: Attempt to explicitly set the length of a beamline. "
              "\n*** WARNING ****: This is most likely an error.  Will not comply."
              "\n*** WARNING ****: Continuing, nonetheless... "
              "\n*** WARNING ****: " 
           << std::endl;
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

int beamline::howMany() const 
{
  return theList_.size();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::setReferenceTime( Particle& particle) 
{

#if 0

 std::vector<RFCavityPtr>     rfcavities;  
 std::vector<ThinRFCavityPtr> thinrfcavities;  
 RFCavityPtr                  rfcavity_elm; 
 ThinRFCavityPtr              thinrfcavity_elm; 

 double cumulativeCdt   = 0.0;
 double initialMomentum = particle.ReferenceMomentum();

 for ( iterator it  = begin(); it != end(); ++it) {
    
     double momentum = particle.ReferenceMomentum();
       
        if(   ( initialMomentum != momentum ) && ( (*it)->isMagnet() ) ) { 
             (*it)->setStrength( ((*it)->Strength())*(momentum/initialMomentum_) );
        }

        (*it)->setReferenceTime(particle);
 
        cumulativeCdt +=(*it)->getReferenceTime();          

        if (  boost::dynamic_pointer_cast<LinacCavity>(*it) ) { std::cout << (*it)->getReferenceTime() << std::endl; }

        if ( rfcavity_elm     = boost::dynamic_pointer_cast<rfcavity>(*it) )      rfcavities.push_back(rfcavity_elm);
        if ( thinrfcavity_elm = boost::dynamic_pointer_cast<thinrfcavity>(*it) )  thinrfcavities.push_back(thinrfcavity_elm);
       
     }

    //-------------------------------------------------------------
    // Store the total time of traversal as a frequency. 
    // Not used if the beamline is not a ring. 
    // NOTE: revolutionFrequency_ is meaningless if rfcavity 
    // elements with an accelerating phase are present.
    //---------------------------------------------------------------

    revolutionFrequency_ = PH_MKS_c/cumulativeCdt ;

    // -------------------------------------------------------------------
    // set the frequency in rfcavity elements when a   
    // revolution harmonic has been specified. 
    //--------------------------------------------------------------------
  
    for ( std::vector<RFCavityPtr>::iterator it = rfcavities.begin();  it != rfcavities.end(); ++it ) {
      (*it)->setFrequencyRelativeTo( revolutionFrequency_ );
    }

    for ( std::vector<ThinRFCavityPtr>::iterator it = thinrfcavities.begin();  it != thinrfcavities.end(); ++it ) {
      (*it)->setFrequencyRelativeTo( revolutionFrequency_ );
    }

#endif

}
