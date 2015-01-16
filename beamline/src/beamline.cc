/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      beamline.cc
******
******  Copyright (c) Fermi Research Alliance
******                Universities Research Association, Inc.
******                Fermilab
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contracts No. DE-AC02-76CH03000
******  and No. DE-AC02-07CH11359.
******
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******
******  Author:    Leo Michelotti
******             Email: michelotti@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
****** 	Sep 2006    Jean-Francois Ostiguy ostiguy@fnal.gov
****** 	- eliminated redundant (and dangerous) c-style hard casts
****** 	  (dlist*) this  [ where this is a beamline * ]
******
****** 	Oct 2006:  Jean-Francois Ostiguy  ostiguy@fnal.gov
****** 	- beamline: decoupled list container from public interface
****** 	            now using std::list<> instead of dlist
****** 	- introduced new iterators with stl-compatible interface
****** 	- eliminated all references to old-style BeamlineIterator,
****** 	  DeepBeamlineIterator etc ..
******
****** 	Jan-Mar 2007  ostiguy@fnal.gov
****** 	- added support for reference counted elements
****** 	- eliminated unneeded dynamic casts
******
****** 	Sep 2007      ostiguy@fnal.gov
****** 	- new iterator based interface for misalignments and rotations.
****** 	  introducing a misalignments trough an entire beamline is now
****** 	  a O(N) operation.
****** 	- refactored rotateRel(..) moveRel(..): eliminate duplicated code
****** 	- eliminated find( ..):    use stl::algorithm instead
****** 	- eliminated replace/deepReplace
******
****** 	Apr 2008     michelotti@fnal.gov
****** 	- additional argument list for beamline::InsertElementsFromList(..)
******
****** 	Nov 2009     michelotti@fnal.gov
****** 	- fixed error in void iefl(..) that was triggered by trying
****** 	  to insert elements close to (i.e. within machine presision)
****** 	  but not exactly at the upstream or downstream end of an element.
****** 	- added additional error checking and throwing exceptions
****** 	  to iefl(..) and beamline::InsertElementsFromList(..)
****** 	- thanks to Eric Stern for reporting the error, for his analysis
****** 	  of the circumstances that triggered it, and for testing the
****** 	  updated version of this file.
******
****** 	Dec 2013     michelotti@fnal.gov
****** 	- made changes suggested by Jim Amundson to reduce number
****** 	  of compiler warnings. These should not alter behavior.
******
**************************************************************************
*************************************************************************/

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
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

namespace {

template<bool USELENGTH>
void iefl(  beamline* invoker
          , Particle const& particle
          , double& s
          , std::list<std::pair<ElmPtr,double> >& inList )
{
  static const ElmPtr null;
  double const fuzzTolerance = 1.0e-9;

  beamline::iterator bml_iter = invoker->begin();

  ElmPtr p_be = ( bml_iter == invoker->end() ) ? null : *bml_iter;

  ElmPtr p_be_a;
  ElmPtr p_be_b;

  //
  // First sanity check: if the invoking beamline is empty, we're finished.
  //
  if( !p_be ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** beamline::InsertElementsFromList(...)"
                "\n*** WARNING *** The beamline is empty! "
                "\n*** WARNING *** Nothing will be done. "
                "\n*** WARNING *** "
             << endl;
    return;
  }

  //
  // Second sanity check: if the insertion list is empty, we're finished.
  //
  if( inList.empty() ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** beamline::InsertElementsFromList(...)"
                "\n*** WARNING *** The list is empty! "
                "\n*** WARNING *** Nothing will be done. "
                "\n*** WARNING *** "
         << endl;
    return;
  }


  //
  // Loop through the insertion list and the beamline
  // inserting elements as we go.
  //
  Particle lparticle(particle);  // May just be dummy particle, but that's okay.
  double   localLength      = 0;
  bool     firstWarning     = true;
  double   last_insertion_s = 0;

  std::pair<ElmPtr,double>  p_ile = inList.front();  // top element; not removed

  while( p_be && (p_ile.first) )
  {
    //
    // Final sanity check: azimuth sequence must be non-negative and non-decreasing.
    // This check could fail to capture an error under certain circumstances
    // with nested beamlines. If so, the error will be captured later but with
    // the wrong diagnostic message.
    //
    if( p_ile.second < last_insertion_s ) {
      ostringstream uic;
      uic << "\n*** ERROR *** "
             "\n*** ERROR *** Insertion list azimuth in wrong sequence."
             "\n*** ERROR *** Test failed: " << p_ile.second << " < " << last_insertion_s
          << "\n*** ERROR *** The element to be inserted was: "
          << p_ile.first->Type() << " " << p_ile.first->Name()
          << "\n*** ERROR *** ";
      throw( GenericException( __FILE__, __LINE__,
             "void iefl(...)",
             uic.str().c_str() ) );
    }
    else {
      last_insertion_s = p_ile.second;
    }


    if(USELENGTH) {
      localLength = p_be->Length();
    }
    else {
      localLength = p_be->OrbitLength( lparticle );
    }


    //
    // If there is a nested beamline, enter into it.
    //
    if( typeid(*p_be) == typeid(beamline) )  {
      if(USELENGTH) {
        boost::static_pointer_cast<beamline>(p_be)->InsertElementsFromList(s, inList );
      }
      else {
        boost::static_pointer_cast<beamline>(p_be)->InsertElementsFromList( lparticle, s, inList );
      }

      p_ile = inList.front();   // this may have changed!
      ++bml_iter;
      p_be = (bml_iter == invoker->end()) ? null : *bml_iter;
    }

    //
    // combinedFunction elements that are not used as a base class
    // for inheritance are treated in a default manner.
    //
    else if (  typeid(*p_be) == typeid(combinedFunction)  ) {
      if(USELENGTH) {
        bmlnElmnt::core_access::get_BmlPtr(*p_be)->InsertElementsFromList( s, inList );
      }
      else {
        bmlnElmnt::core_access::get_BmlPtr(*p_be)->InsertElementsFromList( lparticle, s, inList );
      }
      p_ile = inList.front();     // this may have changed

      ++bml_iter;
      p_be = (bml_iter == invoker->end()) ? null : *bml_iter;

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

    //
    // If the requested azimuth has not yet been reached, move on.
    //
    else if ( s + localLength <= p_ile.second )  {
      s += localLength;
      ++bml_iter;
      p_be = (bml_iter == invoker->end()) ? null : *bml_iter;
    }

    //
    // If the requested azimuth has been reached "exactly,"
    // insert element upstream of current element.
    //
    else if ( std::abs(s - p_ile.second) < fuzzTolerance ) {
      invoker->putAbove( bml_iter, p_ile.first );

      // NOTE: behavior of beamline::putAbove means that
      // bml_iter still points to "current element"
      // -- i.e. downstream of inserted element.

      // NOTE ALSO: s is not adjusted because it tracks azimuth
      // with reference to the original beamline.

      inList.pop_front();             // removes top element
      if (inList.empty() ) break;
      p_ile = inList.front();         // accesses new top element
    }

    //
    // If the requested azimuth is within the current element,
    // split it and insert the element between the two pieces.
    //
    else if ( ( s < p_ile.second ) && ( p_ile.second < s + localLength ) )  {
      p_be->Split( ( p_ile.second - s )/localLength, p_be_a,  p_be_b );

      invoker->putAbove( bml_iter, p_be_a      );
      invoker->putAbove( bml_iter, p_ile.first );
      invoker->putAbove( bml_iter, p_be_b      );

      // SEE COMMENT ABOVE
      if(USELENGTH) {
        s += p_be_a->Length();
      }
      else {
        s += p_be_a->OrbitLength( lparticle );
      }
      // REMOVE: if(USELENGTH) {
      // REMOVE:   s += ( p_be_a->Length() + p_ile.first->Length() );
      // REMOVE: }
      // REMOVE: else {
      // REMOVE:   s += ( p_be_a->OrbitLength( lparticle ) + p_ile.first->OrbitLength( lparticle ) );
      // REMOVE: }

      p_be = p_be_b;

      bml_iter = invoker->erase( bml_iter );    // bml_iter now points to element downstream of p_be_b !

      inList.pop_front();                       // removes top element
      if (inList.empty() ) break;
      p_ile = inList.front();                   // accesses new top element

      --bml_iter;                               // now points to p_be_b
    }

    //
    // The current azimuth has gone past the requested azimuth.
    // This should never happen!
    //
    else if( p_ile.second < s ) {
      ostringstream uic;
      uic << "\n*** ERROR *** "
             "\n*** ERROR *** While trying to insert "
          << p_ile.first->Type() << "  " << p_ile.first->Name()
          << " into beamline " << invoker->Name()
          << "\n*** ERROR *** we passed the requested azimuth, "
          << p_ile.second << " meters."
             "\n*** ERROR *** Currently, s = " << s << " meters."
             "\n*** ERROR *** "
             "\n*** ERROR *** This should not have happened!"
             "\n*** ERROR *** Please alert michelotti@fnal.gov"
             "\n*** ERROR *** ";
      throw( GenericException( __FILE__, __LINE__,
             "void iefl(...)",
             uic.str().c_str() ) );
    }

    //
    // And this DEFINITELY should never happen!!
    //
    else {
      ostringstream uic;
      uic << "\n*** ERROR *** "
             "\n*** ERROR *** While trying to insert "
          << p_ile.first->Type() << "  " << p_ile.first->Name()
          << " into beamline " << invoker->Name()
          << "\n*** ERROR *** the impossible occurred:"
          << "\n*** ERROR *** no azimuth test was triggered."
          << "\n*** ERROR *** Requested azimuth was "
          << p_ile.second << " meters."
             "\n*** ERROR *** Currently, s = " << s << " meters."
             "\n*** ERROR *** "
             "\n*** ERROR *** This DEFINITELY should not have happened!!"
             "\n*** ERROR *** Please alert michelotti@fnal.gov"
          << "\n*** ERROR *** ";
      throw( GenericException( __FILE__, __LINE__,
             "void iefl(...)",
             uic.str().c_str() ) );
    }
  }

  //
  // Final insertions, in case we fell off the end of the beamline,
  // which will happen if the final elements to be inserted come
  // at the end.
  //
  if( !(inList.empty()) && (p_ile.first) ) {  // Double test may be redundant.
                                              // Who cares?  Paranoia is useful.
    while( std::abs(s - p_ile.second) < fuzzTolerance ) {
      invoker->append( p_ile.first ); // element is appended at end.
      inList.pop_front();             // removes top element from insertion list;
                                      // inList need not be empty here!
      if (inList.empty() ) break;
      // REMOVE: if(USELENGTH) {
      // REMOVE:   s += p_ile.first->Length();
      // REMOVE: }
      // REMOVE: else {
      // REMOVE:   s += p_ile.first->OrbitLength( lparticle );
      // REMOVE: }
      p_ile = inList.front();         // accesses new top element
    }
  }

  // NOTE: inList need not be empty here either.
}


//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void iefl<false>( beamline*, Particle const&, double&, std::list<std::pair<ElmPtr,double> >& );
template void iefl<true>( beamline*, Particle const&, double&, std::list<std::pair<ElmPtr,double> >& );

#endif

} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#define PREC setprecision(4)

ostream& operator<<(ostream& os, LattFunc const& x) {
  os << PREC << x.alpha.hor << " " << x.beta.hor << " " << x.psi.hor;
  os << " " << x.alpha.ver << " " << x.beta.ver << " " << x.psi.ver;
  os << " " << x.dispersion.hor << " " << x.dPrime.hor;
  os << " " << x.dispersion.ver << " " << x.dPrime.ver;
  return (os << endl);
}

istream& operator>>(istream& is, LattFunc& x) {
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

ostream& operator<<(ostream& os, LattRing const& x) {
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
 nominalEnergy_  = NOTKNOWN;
 twissDone_      = false;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::beamline( beamline const& a )
  : bmlnElmnt(a),
    mode_(a.mode_),
    nominalEnergy_(a.nominalEnergy_),
    twissDone_(false),
    theList_( a.theList_)
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::~beamline()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline* beamline::Clone() const 
{
 beamline*  bml  = new beamline("");

 bml->bmlnElmnt::operator=(*this); // copy bmlnElmnt state

 bml->mode_          = mode_;
 bml->nominalEnergy_ = nominalEnergy_;
 bml->twissDone_     = false;

 // Reset length to zero before appending elements.

 bml->length_ = 0.0;

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

ElmPtr& beamline::firstElement()
{
    return   theList_.front();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElmPtr const& beamline::firstElement() const
{
    return   theList_.front();
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

double beamline::OrbitLength( Particle const& x )
{
 double s = 0.0;

 for ( beamline::iterator it = begin(); it != end(); ++it) {
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

void beamline::setEnergy( double const& E )
{
 nominalEnergy_ = E;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::unTwiss()
{
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

LattFunc beamline::whatIsLattice( int n )
{

 LattFunc errRet;

 int numElem = howMany();

 if ( ( n < 0 ) || ( numElem <= n ) ){
    ostringstream uic;
    uic  << "Argument n = " << n
         << " lies outside [0,"
         << (numElem-1) << "].";
    throw( GenericException( __FILE__, __LINE__,
           "LattFunc beamline::whatIsLattice( int n ) {",
           uic.str().c_str() ) );
 }

 int count = 0;
 for (beamline::iterator it = begin(); it != end(); ++it ) {
   if( n == count++ )
   return any_cast<LattFunc>( (*it)->dataHook.find( "Twiss")->info ) ;
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

LattFunc beamline::whatIsLattice( std::string n )
{
  LattFunc errRet;

  for (beamline::iterator it = begin(); it != end(); ++it ) {
    if( (*it)->Name() == n  )
      return any_cast<LattFunc>( (*it)->dataHook.find( "Twiss" )->info );
  }

  return errRet;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::insert( ElmPtr const& q )
{
  if ( !q) throw GenericException( __FILE__, __LINE__,  "beamline::insert( bmlnElmnt* q )", "Error: Attempt to insert a null bmlnElmnt*.");

  theList_.push_front(q);
  if( twissDone_ ) unTwiss();
  length_ += q -> length_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::insert( bmlnElmnt const& elm )
{
  insert( ElmPtr( elm.Clone() ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::append( ElmPtr const& q )
{
  if ( !q ) throw GenericException( __FILE__, __LINE__, "beamline::append( bmlnElmnt* q )", "Error: Attempt to append a null bmlnElmnt*.");

  theList_.push_back( q );
  if( twissDone_ ) unTwiss();
  length_ += q->length_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::append( bmlnElmnt const& elm )
{
  append( ElmPtr( elm.Clone() ) );
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

void beamline::InsertElementsFromList( Particle const& particle, double& s, std::list<std::pair<ElmPtr,double> >& inList )
{
  try {
    ::iefl<false>( this, particle, s, inList );
  }
  catch (GenericException& ge){
    throw ge;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::InsertElementsFromList( double& s, std::list<std::pair<ElmPtr,double> >& inList )
{
  Proton placeholder(200);  // unused, placeholder, dummy proton
  try {
    ::iefl<true>( this, placeholder, s, inList );
  }
  catch (GenericException& ge){
    throw ge;
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
  throw( GenericException( __FILE__, __LINE__,
         "void beamline::Split( double const&, bmlnElmnt&, bmlnElmnt& )",
         "This method should not be invoked by a beamline object." ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::putAbove( beamline::iterator it, ElmPtr const&  y )
{
 // Insert y above (before; upstream of) iter in the beamline
 // UPON RETURN: iter points to the same element, i.e. the position downstream of the
 // inserted element.

 unTwiss();

 theList_.insert( it, y );

 length_ += y->length_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::putBelow( beamline::iterator  iter, ElmPtr const& y )
{

 //------------------------------------------------------------------
 // Insert y below (after; downstream of) x in the beamline.
 // UPON RETURN: iter points to the newly inserted element's position.
 //-------------------------------------------------------------------

 unTwiss();

 if ( iter == end() ) {
   (*pcout) << "*** WARNING **** : attempt to insert an element downstream of a beamline's end." << std::endl;
   (*pcout) << "                   beamline name :" << Name()  << std::endl;
   return iter;
 }

 ++iter;

 theList_.insert( iter, y );

 length_ += y->length_;

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

  for ( beamline::iterator it = begin();  it != end(); ++it) {
       if ( *it == x )   ++count;
       if ( count == n ) {
          pos = it;
          break;
       }
  }

  if  ( pos == end()) return 1; // new starting element not found

  for ( std::list<ElmPtr>::iterator it = pos;  it != theList_.end(); ++it) {
    newList.push_back(*it);
  }

  for ( std::list<ElmPtr>::iterator it = theList_.begin();  it != pos; ++it) {
    newList.push_back(*it);
  }

  theList_ = newList;
  return 0;
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

  unTwiss();

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
       << ident_ << " = " << this
       << endl;

 for (beamline::const_iterator it = begin(); it != end(); ++it )  {

    if( typeid( **it ) == typeid(beamline) )
      boost::static_pointer_cast<beamline>(*it)->peekAt( s, prt );
    else (*it)->peekAt( s, prt );
  }

  (*pcout) << "End beamline::peekat() -- Address of beamline: "
       << ident_ << " = " << this
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

 unTwiss();
 return iterator( this, theList_.erase( pos1, pos2) );

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

  beamline::iterator it  = ipos;

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
  else if( (sp = boost::dynamic_pointer_cast<Slot>(upStreamPtr)) ) {
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
   else if( (sp = boost::dynamic_pointer_cast<Slot>(downStreamPtr)) ) {
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

beamline::iterator beamline::yaw2(   beamline::iterator pos, double const& angle, double const& pct )
{
  return rotateRel2( Frame::yAxisIndex(), angle, pos, pct, string("beamline::yaw"));
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

   if(  (sp = boost::dynamic_pointer_cast<Slot>( upStreamPtr )) ) {
        sp->setInFrame( frameZero );
        sp->setOutFrame( frameOne );
        sp->pinnedFrames_.downStream( (thePtr->pinnedFrames_).upStream() );
   }
   if( (sp = boost::dynamic_pointer_cast<Slot>( downStreamPtr )) ) {
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

beamline::iterator beamline::rotateRel2(   int axis, double const& angle, iterator ipos, double pct, string invoker )
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
  ElmPtr sliceBeginPtr = *ipos;
  ElmPtr sliceEndPtr = *ipos;
  ElmPtr downStreamPtr, upStreamPtr;

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

  beamline::iterator it = ipos;
  beamline::iterator tmp_it  = ipos;

  // finding downstream element
  int downCount = 0;
  double length = thePtr->Length();
  if (tmp_it == end()) {
      downStreamPtr = ElmPtr();
  } else {
      ++tmp_it;
      while ((*tmp_it)->Type() != std::string("drift") && tmp_it != end()) {
          if ((*tmp_it)->Type() == thePtr->Type()) {
              sliceEndPtr = *tmp_it;
              length += (*tmp_it)->Length();
          }
          if ((*tmp_it)->Type() == std::string("Slot")) break;
          ++tmp_it;
          downCount += 1;
      }
      downStreamPtr = (tmp_it != end()) ? *tmp_it : ElmPtr();
  }
  // finding upstream element
  tmp_it = it;
  int upCount = 0;
  if (tmp_it == begin()) {
      upStreamPtr = ElmPtr();
  } else {
      --tmp_it;
      while ((*tmp_it)->Type() != std::string("drift") && tmp_it != begin()) {
          if ((*tmp_it)->Type() == (*it)->Type()) sliceBeginPtr = *tmp_it;
          if ((*tmp_it)->Type() == std::string("Slot")) break;
          --tmp_it;
          upCount += 1;
      }
      upStreamPtr = (tmp_it == begin()) ? ElmPtr() : *(tmp_it);
      length += upStreamPtr->Length();
  }

  if ((typeid(*upStreamPtr) == typeid(Slot))
          || (typeid(*downStreamPtr) == typeid(Slot))) {
      ((*it)->pinnedFrames_).downStream((sliceBeginPtr->pinnedFrames_).downStream());
      ((*it)->pinnedFrames_).upStream((sliceBeginPtr->pinnedFrames_).upStream());
      return it;
  }

  Frame const frameZero;
  Frame frameOne, frameTwo, frameThree;
  Frame pinnedFrameOne, pinnedFrameTwo;

  FramePusher fp( frameZero );

  frameOne   =  upStreamPtr ? upStreamPtr->accept(fp),  fp.getFrame() : fp.getFrame();
  frameTwo   =  ( thePtr->accept( fp ),  fp.getFrame() );

  Vector newOrigin(3);
  newOrigin[2] = length;
  frameTwo.setOrigin(newOrigin);
  frameThree =  downStreamPtr ? downStreamPtr->accept(fp), fp.getFrame() : fp.getFrame();
  newOrigin[2] += downStreamPtr->Length();
  frameThree.setOrigin(newOrigin);

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
  } //else { // TODO
  //    ElmPtr usHalfPtr, dsHalfPtr;
  //    thePtr->Split( pct, usHalfPtr, dsHalfPtr );
  //    FramePusher fp2( frameOne );
  //    usHalfPtr->accept(fp2);
  //    midFrame = fp2.getFrame();
  //}

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

   SlotPtr sp;
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
        for (int i = 0; i <= upCount; ++i) --it;
        tmp_it = erase( it );
        putAbove( tmp_it, slotPtr );
        it = ipos;
   }
   if( typeid(*downStreamPtr) == typeid(drift) ) {
       SlotPtr slotPtr( new Slot(downStreamPtr->Name().c_str(), frameThree.relativeTo(frameTwo) ) );
       slotPtr->setReferenceTime( downStreamPtr->getReferenceTime() );
       slotPtr->pinnedFrames_.upStream( (thePtr->pinnedFrames_).downStream() );
       for (int i = 0; i <= downCount; ++i) ++it;
       tmp_it = erase( it );
       putAbove( tmp_it, slotPtr );
       it = ipos;
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

void beamline::remove( ElmPtr Element2remove){
  if ( theList_.empty() ) return;
  theList_.remove( Element2remove );
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

      throw( GenericException( __FILE__, __LINE__,
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
      throw( GenericException( __FILE__, __LINE__,
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

double beamline::getReferenceTime()                    const
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
  (*pcerr) << "*** WARNING ****: Continuing, nonetheless... " << std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::setLength( double const& )
{
  ostringstream methodIdent;
  methodIdent << "void " << Type() << "::setLength( double const& )";

  (*pcerr) <<   "*** ERROR ****: "
              "\n*** ERROR ****: "  << __FILE__ << "," << __LINE__
           << "\n*** ERROR ****: void " << Type() << "::setLength( double const& )"
              "\n*** ERROR ****: Resetting the length of "
           << Type() << " is not allowed in this version."
              "\n*** ERROR ****: "
           << std::endl;

  ostringstream uic;
  uic << "Resetting the length of " << Type() << " is not allowed in this version.";
  throw( GenericException( __FILE__, __LINE__,
           methodIdent.str().c_str(),
           uic.str().c_str() ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::iterator beamline::erase(beamline::iterator it)
{
  std::list<ElmPtr>::iterator lit = it; // implicit conversion

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
