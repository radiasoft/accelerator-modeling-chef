/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      beamline.cc
******  Version:   3.1
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
******                                                                
****** - eliminated all references to old-style BeamlineIterator, DeepBeamlineIterator etc ..
******
****** Jan-Mar 2007
******
****** - added support for reference counted elements 
****** - eliminated unneeded dynamic casts             
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef NO_RTTI 
#include <typeinfo>
#endif
#include <string>

#include <basic_toolkit/iosetup.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/combinedFunction.h>
#include <beamline/Slot.h>
#include <beamline/sector.h>
#include <beamline/FramePusher.h>
#include <beamline/beamline.h>
#include <beamline/BmlVisitor.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>

#include <iomanip>
#include <algorithm>

using namespace std;
using namespace boost;
using FNAL::pcerr;
using FNAL::pcout;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#define PREC setprecision(4)

ostream& operator<<(ostream& os, const lattFunc& x) {
  os << PREC << x.alpha.hor << " " << x.beta.hor << " " << x.psi.hor;
  os << " " << x.alpha.ver << " " << x.beta.ver << " " << x.psi.ver;
  os << " " << x.dispersion.hor << " " << x.dPrime.hor;
  os << " " << x.dispersion.ver << " " << x.dPrime.ver;
  return (os << endl);
}

istream& operator>>(istream& is, lattFunc& x) {
  is >> x.alpha.hor;
  is >> x.beta.hor; 
  is >> x.psi.hor ;
  is >> x.alpha.ver; 
  is >> x.beta.ver; 
  is >> x.psi.ver; 
  is >> x.dispersion.hor;
  is >> x.dPrime.hor;
  is >> x.dispersion.ver;
  is >> x.dPrime.ver;
  return is;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<(ostream& os, const lattRing& x) {
  os << x.tune.hor << " " << x.tune.ver << " ";
  os << " " << x.chromaticity.hor << " " << x.chromaticity.ver;
  return (os << endl);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************
//   class beamline
// **************************************************

beamline::beamline( const char* nm ) 
: bmlnElmnt( nm ), theList_() 
{
 mode_           = unknown;
 numElem_        = 0;
 nominalEnergy_  = NOTKNOWN;
 twissDone_      = 0;
} 


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::beamline( beamline const& a ) 
  : bmlnElmnt(a), 
    mode_(a.mode_),  
    nominalEnergy_(a.nominalEnergy_), 
    numElem_(a.numElem_), 
    twissDone_(0), 
    theList_( a.theList_) 
{}


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
 bml->nominalEnergy_ = nominalEnergy_; 
 bml->numElem_       = numElem_; 
 bml->twissDone_     = 0; 

 // Recursively clone all the beamlines and all the elements.

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

    mode_          = rhs.mode_;  
    nominalEnergy_ = rhs.nominalEnergy_; 
    numElem_       = rhs.numElem_; 
    twissDone_     = rhs.twissDone_;
    theList_       = rhs.theList_;

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

ElmPtr beamline::firstElement() 
{ 
    return   theList_.front(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr beamline::firstElement() const
{ 
    return   theList_.front(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr beamline::lastElement() 
{ 
   return   theList_.back(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr beamline::lastElement() const

{ 
   return   theList_.back(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool beamline::twissIsDone() const
{  
   return twissDone_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::setTwissIsDone()
{ 
   twissDone_ = true; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::unsetTwissIsDone()
{ 
   twissDone_ = false; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double beamline::Energy() const 
{ 
   return nominalEnergy_; 
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
  numElem_ = 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void beamline::localPropagate( Particle& x ) {
 for ( beamline::iterator it = begin(); it != end();  ++it ) { 

   (*it)->propagate( x );

 }
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::localPropagate( ParticleBunch& x ) {
 for (beamline::iterator it = begin(); it != end();  ++it ) { 
   (*it) -> propagate( x );
 }
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::localPropagate( JetParticle& x ) {

 for (beamline::iterator it = begin(); it != end();  ++it ) { 
 
   (*it)->propagate( x );

 }
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::setEnergy( double const& E ) {
 nominalEnergy_ = E;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::unTwiss() {

 if  (!twissDone_ ) return;

 dataHook.eraseFirst( "Ring" );
 dataHook.eraseFirst( "Twiss" );

 for ( beamline::iterator it = begin(); it != end();  ++it ) { 
   (*it)->dataHook.eraseFirst( "Twiss" );
 }

 twissDone_ = false;   

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::eraseBarnacles( const char* s )
{

 for (beamline::deep_iterator it = deep_begin(); it != deep_end(); ++it ) { 
    (*it)->dataHook.eraseAll( s );
  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

lattRing beamline::whatIsRing() {

  lattRing errRet;

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
 
  return any_cast<lattRing>(it->info);
}

lattFunc beamline::whatIsLattice( int n ) {
 lattFunc errRet;

 if ( ( n < 0 ) || ( numElem_ <= n ) ){
    ostringstream uic;
    uic  << "Argument n = " << n 
         << " lies outside [0," 
         << (numElem_-1) << "].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "lattFunc beamline::whatIsLattice( int n ) {", 
           uic.str().c_str() ) );
 }

int count = 0;
for (beamline::iterator it = begin(); it != end(); ++it ) { 
  if( n == count++ ) 
  return any_cast<lattFunc>( (*it)->dataHook.find( "Twiss")->info ) ;
}

 (*pcout) << endl;
 (*pcout) << "*** WARNING ***                               \n"
      << "*** WARNING *** beamline::whatIsLattice       \n"
      << "*** WARNING *** Entry was not found.          \n"
      << "*** WARNING *** Meaningless value being       \n"
      << "*** WARNING *** returned.                     \n"
      << "*** WARNING ***                               \n" << endl;
 return errRet;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

lattFunc beamline::whatIsLattice( std::string n ) {

  lattFunc errRet;
  
  for (beamline::iterator it = begin(); it != end(); ++it ) { 

    if( (*it)->Name() == n  ) 
      return any_cast<lattFunc>( (*it)->dataHook.find( "Twiss" )->info );
  } 

    return errRet;  
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::insert( ElmPtr const& q ) {

  if ( !q) throw GenericException( __FILE__, __LINE__,  "beamline::insert( bmlnElmnt* q )", "Error: Attempt to insert a null bmlnElmnt*.");  

 theList_.push_front(q);
 
 if( twissDone_ ) unTwiss();

 length_ += q -> length_;

 if ( beamline const* bml = dynamic_cast<beamline const*>(q.get()) )  
      numElem_ += bml->numElem_;
 else numElem_++;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::insert( bmlnElmnt const& elm ) {

  insert( ElmPtr( elm.Clone() ) );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::append( ElmPtr const& q ) {

  if ( !q ) throw GenericException( __FILE__, __LINE__, "beamline::append( bmlnElmnt* q )", "Error: Attempt to append a null bmlnElmnt*.");  

 theList_.push_back( q );

 if( twissDone_ ) unTwiss();

 length_ += q->length_;

 if(  beamline const* bml = dynamic_cast<beamline const*>(q.get()) )  
      numElem_ += bml->numElem_;
 else 
      numElem_++;
} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::append( bmlnElmnt const& elm ) {

  append( ElmPtr( elm.Clone() ) );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void beamline::InsertElementsFromList( Particle const& particle, double& s, std::list<std::pair<ElmPtr,double> >& inList)
{

 ElmPtr null;

 std::list<ElmPtr>::iterator bml_iter = theList_.begin();

 ElmPtr  p_be   = ( bml_iter == theList_.end() ) ? null : *bml_iter;

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

  if( typeid(*p_be.get()) == typeid(beamline) ) {

    static_cast<beamline*>( p_be.get() )->InsertElementsFromList(particle, s, inList );

    p_ile = inList.front();   // this may have changed
    ++bml_iter; 
    p_be = (bml_iter == theList_.end()) ? null : *bml_iter; 
  }

  else if ( s + p_be->OrbitLength( lparticle ) <= p_ile.second ) {
    s += p_be->OrbitLength( lparticle );
    ++bml_iter; 
    p_be = (bml_iter == theList_.end()) ? null : *bml_iter;     
  }

  else if ( s == p_ile.second ) {

    putAbove( bml_iter, p_ile.first );
    inList.erase( inList.begin() ); // removes top element
    p_ile = inList.front();         // accesses new top element

  }

  else if (  typeid(*p_be.get()) == typeid(combinedFunction)  ) {

    p_be->p_bml_->InsertElementsFromList( particle, s, inList );
    p_ile = inList.front();     // this may have changed
    ++bml_iter; 
    p_be = (bml_iter == theList_.end()) ? null : *bml_iter;     

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

  else if ( ( s < p_ile.second ) && ( p_ile.second < s + p_be->OrbitLength( lparticle ) ) )  {

    p_be->Split( ( p_ile.second - s )/p_be->OrbitLength( lparticle ), p_be_a, p_be_b );

    putAbove( bml_iter, p_be_a      );
    putAbove( bml_iter, p_ile.first );
    putAbove( bml_iter, p_be_b      );

    s += ( p_be_a->OrbitLength( lparticle ) + p_ile.first->OrbitLength( lparticle ) );

  
    // removedElements.append( (void*) p_be );

    p_be = p_be_b;
    inList.erase( inList.begin() );           // removes top element
    p_ile = inList.front();                   // accesses new top element

    bml_iter = theList_.erase( bml_iter ); // bml_iter now points to element downstream of p_be_b !
    --bml_iter;                            // now points to p_be_b 

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
    (*pcerr) << "else if ( p_be->p_bml_ )\n"
         << "else if ( " << p_be->p_bml_ << " )\n";
    (*pcerr) << "else if ( ( s < p_ile->second ) && ( p_ile->second < s + p_be->OrbitLength( lparticle ) ) )\n"
         << "else if ( ( " << setprecision(10) << s 
         << " < "          << setprecision(10) << p_ile.second 
         << " ) && ( "     << setprecision(10) << p_ile.second 
         << " < "          << setprecision(10) << ( s + p_be->OrbitLength( lparticle ) )
         << " ) )\n";


    s += p_be->OrbitLength( lparticle );

    ++bml_iter;
    p_be =  ( bml_iter == theList_.end() ) ? null: *bml_iter;

  }
 }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int beamline::replace( ElmPtr a, ElmPtr b ) 
{

 
  std::list<ElmPtr>::iterator it =  std::find( theList_.begin(), theList_.end(), a );   
 
  if ( it == theList_.end() ) return 1; // not found 

  (*it) = b;

  return  0;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int beamline::deepReplace( ElmPtr a, ElmPtr b ) 
{

  if( !a || !b ) { return 2; }


  for (std::list<ElmPtr>::iterator it = theList_.begin(); it != theList_.end(); ++it ) {

    if( (*it) == a ) {

      (*it) = b; 
      return 0; 
    }

    else if( typeid(*(*it).get() ) == typeid(beamline) ) {

      if( dynamic_cast<beamline*>((*it).get())->deepReplace( a, b ) == 0  ) { return 0; }
    }

  }
  return 1;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline* beamline::reverse() const {

 std::string theName = std::string("REVERSE_") + std::string( Name() );
 beamline* result = new beamline( theName.c_str() );

  for ( beamline::const_reverse_iterator rit = rbegin(); rit != rend(); ++rit) {

    if( typeid(**rit) == typeid(beamline) ) 
    {
      result->append(  BmlPtr( dynamic_cast<beamline&>(**rit).reverse() ) );
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

void beamline::putAbove( std::list<ElmPtr>::iterator const& iter, ElmPtr  y ) 
{
 // Insert y above (before; upstream of) iter in the beamline
 // upon return, iter points Insert y above (before; upstream of) iter in the beamline

 unTwiss();

 theList_.insert( iter, y );

 length_ += y->length_;

 numElem_++;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::putBelow( std::list<ElmPtr>::iterator const& iter, ElmPtr y ) 
{
 // Insert y below (after; downstream of) x in the beamline

 unTwiss();

 std::list<ElmPtr>::iterator tmp_iter = iter; 

 if ( iter == theList_.end() ) {
   (*pcout) << "*** WARNING **** : attempt to insert an element downstream of a beamline's end." << std::endl;
   (*pcout) << "                   beamline name :" << Name()  << std::endl;
   return;
 }

 ++tmp_iter;

 theList_.insert( tmp_iter, y );
 
 length_ += y->length_;
 
 ++numElem_;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


    // ++++++++++++ End:   Insert and append functions ++++++++++++++++++


beamline beamline::flatten() const {

 beamline r;

 for (beamline::const_deep_iterator it = deep_begin(); it != deep_end(); ++it ) {  
     r.append( (*it) );
 }

 r.setEnergy( this->Energy() );
 r.rename( this->Name() );
 r.setLineMode( mode_ );

 return r;
} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int beamline::startAt( ConstElmPtr const& x, int n ) {

  int count = 0;

  std::list<ElmPtr> newList;
  std::list<ElmPtr>::iterator pos;

  for ( std::list<ElmPtr>::iterator it =theList_.begin();  it != theList_.end(); ++it) { 
       if ( *it == x )   ++count;            
       if ( count == n ) {  
          pos = it;
          break;
       }
  }
       
  if  ( pos == theList_.end()) return 1; // new starting element not found  

  for ( std::list<ElmPtr>::iterator it = pos;  it != theList_.end(); ++it) { 
    newList.push_back(*it);
  }

  for ( std::list<ElmPtr>::iterator it = theList_.begin();  it != pos; ++it) { 
    newList.push_back(*it);
  }

  theList_ = newList;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::startAt( char const* s, int n ) {

  int count = 0;

  std::list<ElmPtr> newList;
  std::list<ElmPtr>::iterator pos;

  for ( std::list<ElmPtr>::iterator it =theList_.begin();  it != theList_.end(); ++it) { 
       if ( std::string( (*it)->Name() ) == std::string( s ) )   ++count;            
       if ( count == n ) {  
          pos = it;
          break;
       }
  }
       
  if  ( pos == theList_.end()) return 1; // new starting element not found  

  for ( std::list<ElmPtr>::iterator it = pos;  it != theList_.end(); ++it) { 
    newList.push_back(*it);
  }

  for ( std::list<ElmPtr>::iterator it = theList_.begin();  it != pos; ++it) { 
    newList.push_back(*it);
  }

  theList_ = newList;

  return 1;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector* beamline::MakeSector (ElmPtr be_1, ElmPtr be_2, int deg, JetParticle& jp ) {

 // This assumes that the argument jp has been initialized as
 // desired by the calling program.  This routine does NOT
 // initialize the state of jp.

 bool       firstFound  = false;
 bool       secondFound = false;
 Particle   prt(jp);
 double     s           = 0.0;

 for (beamline::deep_iterator it = deep_begin(); it != deep_end(); ++it) {

  if( (*it) == be_2 ) {
   (*pcout) << "*** WARNING ***                                      \n" 
        << "*** WARNING *** beamline::MakeSector                 \n" 
        << "*** WARNING *** Second element found first.          \n" 
        << "*** WARNING *** Returning zero.                      \n" 
        << "*** WARNING ***                                      \n" 
        << endl;
   return 0;
  }

  else if( (*it) == be_1 ) { 
    firstFound = true;
    break;
  }
 }

 if( !firstFound ) {
  (*pcout) << "*** WARNING ***                                      \n" 
       << "*** WARNING *** beamline::MakeSector                 \n" 
       << "*** WARNING *** Unable to find first element.        \n" 
       << "*** WARNING *** Returning zero.                      \n" 
       << "*** WARNING ***                                      \n" 
       << endl;
   return 0;
 }
 
 for (beamline::deep_iterator it = deep_begin(); it != deep_end(); ++it) { // Notice: we do not propagate through

  if( (*it) == be_2 ) {                                                    // be_1 and be_2
    secondFound = true;
    break;
  }
  else {
    (*it)->propagate( jp );
    s += (*it)->OrbitLength( prt );
  }
 }

 if( !secondFound ) {
  (*pcout) << "*** WARNING ***                                      \n" 
       << "*** WARNING *** beamline::MakeSector                 \n" 
       << "*** WARNING *** Unable to find second element.       \n" 
       << "*** WARNING *** Returning zero.                      \n" 
       << "*** WARNING ***                                      \n" 
       << endl;
  return 0;
 }
 

 return new sector( jp.State().filter( 0, deg ), s );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector* beamline::MakeSectorFromStart ( ElmPtr be_1, int deg, JetParticle& jp ) {

 // This assumes that the argument jp has been initialized as
 // desired by the calling program.  This routine does NOT
 // initialize the state of jp.

 bool       firstFound  = false;
 double     s           = 0.0;
 
 Particle prt(jp);

 // Check first element against the argument ------------

 beamline::iterator it = begin();

 ElmPtr p_be = *it;

 if( !p_be ) {
  (*pcout) << "*** WARNING ***                                      \n" 
       << "*** WARNING *** beamline::MakeSectorFromStart        \n" 
       << "*** WARNING *** The beamline was empty!!             \n" 
       << "*** WARNING *** Returning zero.                      \n" 
       << "*** WARNING ***                                      \n" 
       << endl;
   return 0;
 }

 if( p_be == be_1 ) {
   return 0;
 }

 // Propagate the JetParticle through first element -----------
 p_be->propagate( jp );
 s += p_be->OrbitLength( prt );


 // Find element that matches argument ------------------
 for ( beamline::iterator it = begin(); it != end(); ++it) {
  
   p_be = *it; 
 
  if( p_be == be_1 ) {     // Notice: we do not propagate through be_1
    firstFound = true;
    break;
  }
  else {
    p_be->propagate( jp );
    s += p_be->OrbitLength( prt );
  }
 }

 if( !firstFound ) {
  (*pcout) << "*** WARNING ***                                      \n" 
       << "*** WARNING *** beamline::MakeSectorFromStart        \n" 
       << "*** WARNING *** Unable to find element.              \n" 
       << "*** WARNING *** Returning zero.                      \n" 
       << "*** WARNING ***                                      \n" 
       << endl;
  return 0;
 }
 
 // Construct the map and return the sector -----------------------------------

 return new sector( jp.State().filter( 0, deg ), s );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


sector* beamline::MakeSectorToEnd ( ElmPtr be_1, int deg, JetParticle& jp ) {

 // This assumes that the argument jp has been initialized as
 // desired by the calling program.  This routine does NOT
 // initialize the state of jp.

 ElmPtr     p_be;
 bool       firstFound  = false;
 double     s           = 0.0;

 Particle prt(jp);

 // Find the element that matches argument ---------------------------------

 beamline::iterator it = begin(); 

 for (  ; it != end(); ++it) {
  if( (*it) == be_1 ) { 
    firstFound = true;
    break;
  }
 }

 if( !firstFound ) {
  (*pcout) << "*** WARNING ***                                      \n" 
       << "*** WARNING *** beamline::MakeSectorToEnd            \n" 
       << "*** WARNING *** Unable to find first element.        \n" 
       << "*** WARNING *** Returning zero.                      \n" 
       << "*** WARNING ***                                      \n" 
       << endl;
  return 0;
 }
 
 // Check that it is not the last element --------------------------------
 p_be = *( ++it );

 if( p_be ) {
    return 0;
 }
 else {
    p_be->propagate( jp );
    s += p_be->OrbitLength( prt );
 }

 // Construct the map and return sector ------------------------------------

 for ( ; it != end(); ++it) {
   p_be = (*it);
   p_be->propagate( jp );
   s += p_be->OrbitLength( prt );
 }

 return new sector( jp.State().filter( 0, deg ), s );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector* beamline::makeSector( int degree, JetParticle& pd ) {
 Vector zero(BMLN_dynDim );

 pd.setState ( zero );
 propagate   ( pd );
 Mapping zd = pd.getState();

 for( int i = 0; i < BMLN_dynDim; ++i)
   zd[i] = zd[i].filter( 0, degree );

 sector* s        = new sector ( ident_.c_str(), zd, length_ );
 s->length_       = length_;
 s->strength_     = strength_;
 s->align_        = align_;
 s->pAperture_    = pAperture_;

 return s;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::sectorize( int degree, JetParticle& pd ) {

 sector* s = makeSector( degree, pd );

 theList_.clear();
 unTwiss();
 numElem_      = 0;
 length_       = 0.0;
 strength_     = 0.0;
 align_        = 0;

 append( SectorPtr(s) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline beamline::sectorize( ElmPtr x, ElmPtr y, int degree, JetParticle& pd, char const* sectorName ) {

  beamline a = remove( x, y ); // remove should peharps somehow return the iterator position for x

  SectorPtr s ( a.makeSector( degree, pd ) );

  s->rename( sectorName );

  std::list<ElmPtr>::iterator xpos = std::find( theList_.begin(), theList_.end(), y ); 
   
  putAbove( xpos, s );

  return a;
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

    if( typeid( *(*it).get() ) == typeid(beamline) ) 
      static_cast<beamline*>( (*it).get() )->peekAt( s, prt );
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
    if(  typeid(*(*it).get()) == typeid(beamline) ) {
      int subDepth =  dynamic_cast<const beamline*>((*it).get())->depth();
      if( maxSubDepth < subDepth ) { maxSubDepth = subDepth; }
    }
  }

  if( 0 == ret ) { ret = maxSubDepth + 1; }
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::contains( ElmPtr& x ) const
{

  int ret = 0;

  for (beamline::const_deep_iterator it = deep_begin(); it != deep_end(); ++it) {
    if( (*it) == x ) { ++ret; }
  }
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline beamline::remove( ElmPtr x, ElmPtr y ) {

 // remove the elements in the interval  ] x, y [  and return them in a new beamline

 unTwiss();

 beamline a;

 if (x == y ) return a; // empty

 std::list<ElmPtr>::iterator pos1 = std::find(theList_.begin(), theList_.end(), x); 
 std::list<ElmPtr>::iterator pos2 = std::find(pos1,             theList_.end(), y);  

 if ( (pos1 == theList_.end()) || (pos2 == theList_.end() ) ) 
   return a; // one of the element not found. return an empty line  

 ++pos1; --pos2; // exclude the boundary elements

 ++pos2;

 for ( std::list<ElmPtr>::iterator it = pos1; it != pos2;  ++it ) { 
   a.append(*it);
 }

 --pos2; 

 theList_.erase( pos1, pos2 ); 

 numElem_ = numElem_ - a.numElem_; 

 return a;


}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool beamline::find( ElmPtr& u, ElmPtr& v, ElmPtr& w ) const
{
  // Upon entry: u and w should have null value but can, in fact
  //               be anything. 
  //               WARNING: they will be reset, so don't use addresses
  //               of valid elements.
  //             v points to the element to be searched for.
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

  // There should be at least three elements in the line

  int elementCount = countHowManyDeeply();
 
  if( 0 == elementCount ) { 
    return false; 
  }

  if( 1 == elementCount ) { 
    if( v == firstElement() ) {
      u = w = ElmPtr();
      return true;
    }
    return false;
  }
 
  if( 2 == elementCount ) { 
    if( v == firstElement() ) {
      u = ElmPtr();
      w = lastElement();
      return true;
    }
    if( v == lastElement() ) {
      u = firstElement();
      w = ElmPtr();
      return true;
    }
    return false;
  }

  // Setup iterator


   beamline::const_deep_iterator dbi = deep_begin();

   ElmPtr q;
     
  // Check for possibility that *v is the first element.

  if( v == this->firstElement() ) {
    if( beamline::ring == getLineMode() ) {
        u = lastElement();
      } else {
        u = ElmPtr();
      }
  
    w = *(++dbi);
    return true;
  } 

  // Continuing ...
  q = *(++dbi);
  u = q;
  q = *(++dbi);

  for ( ; dbi != deep_end(); ++dbi ) {

    q = (*dbi);

    if( v == q ) {
      if( v == lastElement() ) {
        if( beamline::ring == getLineMode() ) {
          w = firstElement();
        }
        else {
          w = ElmPtr();
        }
      }
      else {
        w = *(++dbi);
      }
      return true;
    }

    u = q;
  }

  // Target element was never found.
  return false;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::list<ElmPtr> beamline::moveRelX(   ElmPtr thePtr, double const& u, int* errorCodePtr )
{
  // Upon entry: thePtr      = pointer to element to be translated
  //                           longitudinally
  //             u      [m]  = displacement
  // 
  // Upon exit:  *thePtr will have been displaced in the direction
  //               of its local z coordinate by adjusting its neighboring
  //               free-space elements. 
  // 
  // Returned      BmlPtrList: appended zero, one, or two
  //               neighboring free-space elements that have been replaced.
  // 

  int dummyErr;
  std::list<ElmPtr> replaced_list;

  if( 0 == errorCodePtr ) {
    moveRel( Frame::xAxisIndex(), u, thePtr, &dummyErr, replaced_list, string("beamline::moveRelX") );
  }
  else {
    moveRel( Frame::xAxisIndex(), u, thePtr, errorCodePtr, replaced_list, string("beamline::moveRelX") );
  }

  return replaced_list;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::list<ElmPtr> beamline::moveRelY(   ElmPtr thePtr, double const& u, int* errorCodePtr )
{

  int dummyErr;
  std::list<ElmPtr> replaced_list;

  if( 0 == errorCodePtr ) {
     moveRel( Frame::yAxisIndex(), u, thePtr, &dummyErr, replaced_list, 
                    string("beamline::moveRelX") );
  }
  else {
     moveRel( Frame::yAxisIndex(), u, thePtr, errorCodePtr, replaced_list, 
                    string("beamline::moveRelX") );
  }

  return replaced_list;

}




//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::list<ElmPtr> beamline::moveRelZ(  ElmPtr thePtr, double const& u, int* errorCodePtr )
{


  int dummyErr;
  std::list<ElmPtr> replaced_list;

  if( 0 == errorCodePtr ) {
    moveRel( Frame::zAxisIndex(), u, thePtr, &dummyErr,    replaced_list, 
                    string("beamline::moveRelX") );
  }
  else {
    moveRel( Frame::zAxisIndex(), u, thePtr, errorCodePtr, replaced_list, 
                    string("beamline::moveRelX") );
  }

  return replaced_list;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::moveRel(   int axis, double const& u, ElmPtr thePtr, int* errorCodePtr, std::list<ElmPtr>& replaced_list, string invoker )
{
  // Upon entry: axis            = index of axis along displacement
  //             u          [m]  = displacement
  //             thePtr          = pointer to element to be translated
  //             errorCode       = pointer to a valid int 
  //             recycleBinPtr   = pointer to a valid BmlPtrList
  //             invoker         = name of calling routine (used in error messages)
  //             
  // Upon exit:  *thePtr will have been displaced in the direction
  //               of its local x coordinate by adjusting its neighboring
  //               free-space elements. in fact, *thePtr is not changed
  //               at all. Only its neighbors are altered. In fact, they
  //               may have been replaced.
  //             *bmlPtrList: appended zero, one, or two
  //               neighboring free-space elements that have been replaced.
  //             *errorCode  = 0, nothing wrong
  //                           1, errorCode, ret, or thePtr is null on entry
  //                           2, i != 0, 1, or 2
  //                           3, |u| < 1 nanometer displacement
  // 
  //             NOTE WELL: if the returned list is not empty, it means
  //               that Slots have been created and installed in the line
  //               for which the calling program must take responsibility.
  //               This could be a serious problem. We need smart pointers!
  // 

  // #error *** WARNING ***
  // #error *** WARNING ***  beamline::moveRel must be reviewed and tested!!
  // #error *** WARNING ***

  int dummyError;
  int* errPtr;

  // Set the error pointer ...
  if( 0 == errorCodePtr ) { errPtr = &dummyError;  }
  else                    { errPtr = errorCodePtr; }
  *errPtr = 0;

  // Argument filter
  if( !thePtr ) { *errPtr = 1; return;}

  if( axis < 0 || 2 < axis ) {
    *errPtr = 2;
    return;
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
    *errPtr = 3;
    return;
  }


  // Continue program ...

  ElmPtr upStreamPtr;
  ElmPtr downStreamPtr;

  Frame frameZero, frameOne, frameTwo, frameThree;
  Frame pinnedFrameOne, pinnedFrameTwo;
  // Note: frameZero never changes; it remains the identity.

  if( this->find( upStreamPtr, thePtr, downStreamPtr ) )
  {
    if( (0 != upStreamPtr) && (0 != downStreamPtr ) ) {
      FramePusher fp( frameZero );
      upStreamPtr->accept( fp );
      frameOne = fp.getFrame();
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();

      pinnedFrameOne = ( (thePtr->pinnedFrames_).upStream()   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->pinnedFrames_).downStream() ).patchedOnto( frameTwo );

      // !!! The next lines can be modified (maybe) to do pinned-referenced movements
      Vector displacement(u*frameOne.getAxis(axis));
      frameOne.translate( displacement );
      frameTwo.translate( displacement );

      (thePtr->pinnedFrames_).upStream(   pinnedFrameOne.relativeTo( frameOne ) );
      (thePtr->pinnedFrames_).downStream( pinnedFrameTwo.relativeTo( frameTwo ) );

      // Reset upstream and downstream elements
      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        SlotPtr sp = boost::dynamic_pointer_cast<Slot>( upStreamPtr );
        sp->setInFrame( frameZero );
        sp->setOutFrame( frameOne );
        sp->pinnedFrames_.downStream ( (thePtr->pinnedFrames_).upStream() );
        // ??? This is not quite right.
      }
      if( 0 == strcmp("Slot", downStreamPtr->Type() ) ) {
        SlotPtr sp = boost::dynamic_pointer_cast<Slot>( downStreamPtr );
        sp->setInFrame( frameZero );
        sp->setOutFrame( frameThree.relativeTo(frameTwo) );
        sp->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() );
        // ??? This is not quite right.
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {

        SlotPtr slotPtr( new Slot(upStreamPtr->Name().c_str(), frameOne ) );
        slotPtr->setReferenceTime( upStreamPtr->getReferenceTime() );
        slotPtr->pinnedFrames_.downStream( (thePtr->pinnedFrames_).upStream() );
        putAbove( std::find(theList_.begin(), theList_.end(), thePtr),  slotPtr ); // !!!! TERRIBLY INEFFICIENT !
        remove( upStreamPtr );
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        SlotPtr slotPtr( new Slot(downStreamPtr->Name().c_str(), frameThree.relativeTo(frameTwo) ) );
        slotPtr->setReferenceTime( downStreamPtr->getReferenceTime() );
        slotPtr->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() );
        putBelow( std::find(theList_.begin(), theList_.end(), thePtr), slotPtr ); // !!!! TERRIBLY INNEFFICIENT  !
        remove( downStreamPtr );
        replaced_list.push_back( downStreamPtr );
      }
    }

    else if( (0 == upStreamPtr) && (0 != downStreamPtr ) ) {
      frameOne = frameZero;
      FramePusher fp( frameOne );
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();

      pinnedFrameOne = ( (thePtr->pinnedFrames_).upStream()   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->pinnedFrames_).downStream() ).patchedOnto( frameTwo );

      Vector displacement(u*frameTwo.getAxis(axis));
      frameOne.translate( displacement );
      frameTwo.translate( displacement );

      (thePtr->pinnedFrames_).upStream(   pinnedFrameOne.relativeTo( frameOne ) );
      (thePtr->pinnedFrames_).downStream( pinnedFrameTwo.relativeTo( frameTwo ) );

      (*pcerr) << "\n*** WARNING *** "
           << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
           << "\n*** WARNING *** Called by " << invoker
           << "\n*** WARNING *** Will displace using downstream end of "
           << thePtr->Type() << "  " << thePtr->Name() << "."
           << "\n*** WARNING *** There is no upstream neighbor."
              "\n*** WARNING *** "
           << endl;

      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", downStreamPtr->Type() ) ) {
        SlotPtr sp = boost::dynamic_pointer_cast<Slot>( downStreamPtr );
        sp->setInFrame( frameZero );
        sp->setOutFrame( frameThree.relativeTo(frameTwo) );
        sp->pinnedFrames_.upStream ((thePtr->pinnedFrames_).downStream() );
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        SlotPtr slotPtr( new Slot(downStreamPtr->Name().c_str(), frameThree.relativeTo(frameTwo) ) );
        slotPtr->setReferenceTime( downStreamPtr->getReferenceTime() );
        slotPtr->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() );
        putBelow( std::find(theList_.begin(), theList_.end(), thePtr), slotPtr );
        remove( downStreamPtr );
        replaced_list.push_back( downStreamPtr );
      }
    }

    else if( (0 != upStreamPtr) && (0 == downStreamPtr ) ) {
      FramePusher fp( frameZero );
      upStreamPtr->accept( fp );
      frameOne = fp.getFrame();
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      frameThree = frameTwo;

      pinnedFrameOne = ( (thePtr->pinnedFrames_).upStream()   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->pinnedFrames_).downStream() ).patchedOnto( frameTwo );

      Vector displacement(u*frameOne.getAxis(axis));
      frameOne.translate( displacement );
      frameTwo.translate( displacement );

      (thePtr->pinnedFrames_).upStream(   pinnedFrameOne.relativeTo( frameOne ) );
      (thePtr->pinnedFrames_).downStream( pinnedFrameTwo.relativeTo( frameTwo ) );

      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        SlotPtr sp = boost::dynamic_pointer_cast<Slot>( upStreamPtr );
        sp->setInFrame( frameZero );
        sp->setOutFrame( frameOne );
        sp->pinnedFrames_.downStream ((thePtr->pinnedFrames_).upStream() );
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        SlotPtr slotPtr( new Slot(upStreamPtr->Name().c_str(), frameOne ) );  // ?????
        slotPtr->setReferenceTime( upStreamPtr->getReferenceTime() );
        slotPtr->pinnedFrames_.downStream( (thePtr->pinnedFrames_).upStream() );
        putAbove( std::find(theList_.begin(), theList_.end(), thePtr), slotPtr ); // TERRIBLE !
        remove( upStreamPtr );
        replaced_list.push_back( upStreamPtr );
      }
    }

    else if( (0 == upStreamPtr) && (0 == downStreamPtr ) ) {
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void beamline::moveRel( int axis, double const& u, ... )",
             "An impossibility has occurred. Am stopping." ) );
    }
  }

  return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::list<ElmPtr> beamline::pitch(   ElmPtr thePtr, double const& angle, double const& pct, int* errorCodePtr )
{

  std::list<ElmPtr> replaced_list;

  int dummyErr;

  if( 0 == errorCodePtr ) {
           rotateRel( Frame::xAxisIndex(), angle, thePtr, pct, &dummyErr, replaced_list, string("beamline::pitch") );
  }
  else {
            rotateRel( Frame::xAxisIndex(), angle, thePtr, pct, errorCodePtr, replaced_list, string("beamline::pitch") );
  }
  return  replaced_list;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::list<ElmPtr> beamline::yaw( ElmPtr thePtr, double const& angle, double const& pct, int* errorCodePtr )
{

  std::list<ElmPtr> replaced_list;
  int dummyErr;

  if( 0 == errorCodePtr ) {
           rotateRel( Frame::yAxisIndex(), angle, thePtr, pct, &dummyErr, replaced_list, string("beamline::pitch") );
  }
  else {
           rotateRel( Frame::yAxisIndex(), angle, thePtr, pct, errorCodePtr, replaced_list, string("beamline::pitch"));
  }

  return  replaced_list;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::list<ElmPtr> beamline::roll(   ElmPtr thePtr, double const& angle, double const& pct, int* errorCodePtr )
{

  std::list<ElmPtr> replaced_list;

  int dummyErr;
  if( 0 == errorCodePtr ) {
           rotateRel( Frame::zAxisIndex(), angle, thePtr, pct, 
                      &dummyErr, replaced_list, 
                      string("beamline::pitch") );
  }
  else {
           rotateRel( Frame::zAxisIndex(), angle, thePtr, pct, 
                      errorCodePtr, replaced_list, 
                      string("beamline::pitch") );
  }
  return replaced_list;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::rotateRel(   int axis, double const& angle
                           , ElmPtr thePtr
                           , double pct
                           , int* errorCodePtr, std::list<ElmPtr>& replaced_list
                           , string invoker )
{
  // Upon entry: axis            = rotation direction
  //                               (should be a unit vector)
  //             angle [radians] = displacement
  //             thePtr          = pointer to element to be translated
  //             pct             = percentage downstream of element to serve
  //                               as fixed point of the rotation
  //             errorCodePtr    = pointer to a valid int 
  //             recycleBinPtr   = pointer to a valid BmlPtrList
  //             invoker         = name of calling routine (used in error messages)
  //             
  // Upon exit:  *thePtr will have been rotated along the direction "axis"
  //               by adjusting its neighboring free-space 
  //               elements. *thePtr is not changed at all. Only its
  //               neighbors are altered. In fact, they may have been
  //               replaced.
  //             *recycleBinPtr: appended zero, one, or two
  //               neighboring free-space elements that have been replaced.
  //             *errorCode  = 0, nothing wrong
  //                           1, errorCodePtr, recycleBinPtr, or thePtr 
  //                              is null on entry
  //                           2, |angle| < 1 nanoradian displacement
  //                           3, *thePtr is a sector element
  // 
  //             NOTE WELL: if the returned list is not empty, it means
  //               that Slots have been created and installed in the line
  //               for which the calling program must take responsibility.
  //               This could be a serious problem. We need smart pointers!
  // 

  // #error *** WARNING ***
  // #error *** WARNING ***  beamline::_rotateRel must be reviewed and tested!!
  // #error *** WARNING ***

  int dummyError;
  int* errPtr;

  // Set the error pointer ...
  if( 0 == errorCodePtr ) { errPtr = &dummyError;  }
  else                    { errPtr = errorCodePtr; }
  *errPtr = 0;

  // Argument filter
  if( !thePtr ) { *errPtr = 1; return;}

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
    *errPtr = 2;
    return;
  }

  // Check for a valid element
  if( typeid(*thePtr) == typeid(sector) ) {
    (*pcerr) << "\n*** ERROR *** "
         << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** Called by " << invoker
         << "\n*** ERROR *** Unable to perform rotation on "
         << thePtr->Type() << "  " << thePtr->Name() << "."
         << endl;
    *errPtr = 3;
    return;
  }


  // Continue program ...

  ElmPtr upStreamPtr;
  ElmPtr downStreamPtr;

  Frame frameZero, frameOne, frameTwo, frameThree;
  Frame pinnedFrameOne, pinnedFrameTwo;

  if( find( upStreamPtr, thePtr, downStreamPtr ) ) {

    if( (upStreamPtr) && (downStreamPtr) ) {
      FramePusher fp( frameZero );
      upStreamPtr->accept( fp );
      frameOne = fp.getFrame();
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();

      pinnedFrameOne = ( (thePtr->pinnedFrames_).upStream()   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->pinnedFrames_).downStream() ).patchedOnto( frameTwo );

      // Construct a Frame in between frameOne and frameTwo
      Frame midFrame;
      if     ( 0.0 == pct ) { midFrame = frameOne; }
      else if( 1.0 == pct ) { midFrame = frameTwo; }
      else {
        if(    typeid( *thePtr.get())  == typeid(Slot) 
            || typeid( *thePtr.get() ) == typeid(beamline) ) {
          midFrame = Frame::tween( frameOne, frameTwo, pct );
        }
        else {
          ElmPtr usHalfPtr;
          ElmPtr dsHalfPtr;
          thePtr->Split( pct, usHalfPtr, dsHalfPtr );
          FramePusher fp2( frameOne );
          usHalfPtr->accept(fp2);
          midFrame = fp2.getFrame();
        }
      }

      // !!! The next lines can be modified (maybe) to do pinned-referenced movements
      // Do the rotation
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
      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        SlotPtr sp = boost::dynamic_pointer_cast<Slot>( upStreamPtr );
        sp->setInFrame( frameZero );
        sp->setOutFrame( frameOne );
        sp->pinnedFrames_.downStream( (thePtr->pinnedFrames_).upStream() );
      }
      if( 0 == strcmp("Slot", downStreamPtr->Type() ) ) {
        SlotPtr sp = boost::dynamic_pointer_cast<Slot>( downStreamPtr );
        sp->setInFrame( frameZero );
        sp->setOutFrame( frameThree.relativeTo(frameTwo) );
        sp->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() );
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        SlotPtr slotPtr( new Slot(upStreamPtr->Name().c_str(), frameOne ) );
        slotPtr->setReferenceTime( upStreamPtr->getReferenceTime() );
        slotPtr->pinnedFrames_.downStream ((thePtr->pinnedFrames_).upStream() );
        putAbove( std::find(theList_.begin(), theList_.end(), thePtr), slotPtr ); // TERRIBLE !
        remove( upStreamPtr );
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        SlotPtr slotPtr( new Slot(downStreamPtr->Name().c_str(), frameThree.relativeTo(frameTwo) ) );
        slotPtr->setReferenceTime( downStreamPtr->getReferenceTime() );
        slotPtr->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() );
        putBelow( std::find(theList_.begin(), theList_.end(), thePtr), slotPtr ); // TERRIBLE !
        remove( downStreamPtr );
      }
    }

    else if( (0 == upStreamPtr) && (0 != downStreamPtr ) ) {
      frameOne = frameZero;
      FramePusher fp( frameOne );
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();

      pinnedFrameOne = ( (thePtr->pinnedFrames_).upStream()   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->pinnedFrames_).downStream() ).patchedOnto( frameTwo );

      // Construct a Frame in between frameOne and frameTwo
      Frame midFrame;
      if     ( 0.0 == pct ) { midFrame = frameOne; }
      else if( 1.0 == pct ) { midFrame = frameTwo; }
      else {
        if(    typeid(*thePtr) == typeid(Slot) 
            || typeid(*thePtr) == typeid(beamline) ) {
          midFrame = Frame::tween( frameOne, frameTwo, pct );
        }
        else {
          ElmPtr usHalfPtr;
          ElmPtr dsHalfPtr;
          thePtr->Split( pct, usHalfPtr, dsHalfPtr );
          FramePusher fp2( frameOne );
          usHalfPtr->accept(fp2);
          midFrame = fp2.getFrame();
        }
      }

      // Do the rotation
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
      // Note: this is done inefficiently

      SlotPtr slotPtr( new Slot( frameOne ) );
      putAbove( std::find(theList_.begin(), theList_.end(), thePtr), slotPtr );
   
      if( 0 == strcmp("Slot", downStreamPtr->Type() ) ) {
        SlotPtr sp = boost::dynamic_pointer_cast<Slot>( downStreamPtr );
        sp->setInFrame( frameZero );
        sp->setOutFrame( frameThree.relativeTo(frameTwo) );
        sp->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() );
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        SlotPtr slotPtr( new Slot(downStreamPtr->Name().c_str(), frameThree.relativeTo(frameTwo) ) );
        slotPtr->setReferenceTime( downStreamPtr->getReferenceTime() );
        slotPtr->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() );
        putBelow( std::find( theList_.begin(), theList_.end(), thePtr), slotPtr );
        remove( downStreamPtr );
      }
    }

    else if( (0 != upStreamPtr) && (0 == downStreamPtr ) ) {
      FramePusher fp( frameZero );
      upStreamPtr->accept( fp );
      frameOne   = fp.getFrame();
      thePtr->accept( fp );
      frameTwo   = fp.getFrame();
      frameThree = frameTwo;

      pinnedFrameOne = ( (thePtr->pinnedFrames_).upStream()   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->pinnedFrames_).downStream() ).patchedOnto( frameTwo );

      // Construct a Frame in between frameOne and frameTwo
      Frame midFrame;
      if     ( 0.0 == pct ) { midFrame = frameOne; }
      else if( 1.0 == pct ) { midFrame = frameTwo; }
      else {
        if(    typeid(*thePtr) == typeid(Slot) 
            || typeid(*thePtr) == typeid(beamline) ) {
          midFrame = Frame::tween( frameOne, frameTwo, pct );
        }
        else {
          ElmPtr usHalfPtr;
          ElmPtr dsHalfPtr;
          thePtr->Split( pct, usHalfPtr, dsHalfPtr );
          FramePusher fp2( frameOne );
          usHalfPtr->accept(fp2);
          midFrame = fp2.getFrame();
        }
      }

      // Do the rotation
      Vector rotationAxis( midFrame.getAxis(axis) );
      Frame uFrame( frameOne.relativeTo(midFrame) );
      Frame dFrame( frameTwo.relativeTo(midFrame) );
      uFrame.rotate( angle, rotationAxis, true );
      dFrame.rotate( angle, rotationAxis, true );
      frameOne = uFrame.patchedOnto(midFrame);
      frameTwo = dFrame.patchedOnto(midFrame);

      (thePtr->pinnedFrames_).upStream( pinnedFrameOne.relativeTo( frameOne ) );
      (thePtr->pinnedFrames_).downStream( pinnedFrameTwo.relativeTo( frameTwo ) );

      // Reset upstream and downstream elements
      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        SlotPtr sp = boost::dynamic_pointer_cast<Slot>( upStreamPtr );
        sp->setInFrame( frameZero );
        sp->setOutFrame( frameOne );
        sp->pinnedFrames_.downStream( thePtr->pinnedFrames_.upStream() );
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        SlotPtr slotPtr(new Slot(upStreamPtr->Name().c_str(), frameOne ));
        slotPtr->setReferenceTime( upStreamPtr->getReferenceTime() );
        slotPtr->pinnedFrames_.downStream( (thePtr->pinnedFrames_).upStream() );
        putAbove( std::find(theList_.begin(), theList_.end(), thePtr), slotPtr );
        remove( upStreamPtr );
        //recycleBinPtr->append( upStreamPtr );
      }
      SlotPtr slotPtr( new Slot( frameThree.relativeTo(frameTwo) ) );
      putBelow( std::find(theList_.begin(), theList_.end(), thePtr), slotPtr );
    }

    else if( (!upStreamPtr) && (!downStreamPtr) ) {
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void beamline::moveRel( int axis, double const& u, ... )",
             "An impossibility has occurred. Am stopping." ) );
    }
  }

  return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool beamline::setAlignment( const alignmentData& al ) {
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

void beamline::remove( ElmPtr Element2remove){ 
  if ( theList_.empty() ) return;
  theList_.remove( Element2remove );
  --numElem_;
  return; 
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& beamline::writeTo(ostream& os) {

  double energy = Energy();

  os <<  OSTREAM_DOUBLE_PREC << energy << endl;
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


