/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
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
#include <beamline/InsertionList.h>
#include <beamline/FramePusher.h>
#include <beamline/beamline.h>
#include <beamline/BmlPtrList.h>
#include <beamline/BmlVisitor.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>

#include <iomanip>
#include <algorithm>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


// **************************************************
//   struct lattFunc
// **************************************************

lattFunc::lattFunc() : BarnacleData() {
 arcLength  = 0.0;
 // map        = LieOperator( "ident" );    ??? Produces error: requires
 // mapInv     = map;                       ??? call to Jet::setup first.

 dispersion .hor  = NOTKNOWN;
 dispersion .ver  = NOTKNOWN;
 dPrime .hor  = NOTKNOWN;
 dPrime .ver  = NOTKNOWN;
 beta .hor  = NOTKNOWN;
 beta .ver  = NOTKNOWN;
 alpha.hor  = NOTKNOWN;
 alpha.ver  = NOTKNOWN;
 psi  .hor  = NOTKNOWN;
 psi  .ver  = NOTKNOWN;
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

lattFunc& lattFunc::operator=( const lattFunc& x ){
 arcLength = x.arcLength;
 dispersion.hor = x.dispersion.hor;
 dispersion.ver = x.dispersion.ver;
 dPrime.hor = x.dPrime.hor;
 dPrime.ver = x.dPrime.ver;
 beta.hor = x.beta.hor;
 beta.ver = x.beta.ver;
 alpha.hor = x.alpha.hor;
 alpha.ver = x.alpha.ver;
 psi.hor = x.psi.hor;
 psi.ver = x.psi.ver;

 return *this;
}

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

// **************************************************
//   struct lattRing
// **************************************************

lattRing::lattRing() : BarnacleData() {
 tune.hor  = NOTKNOWN;
 tune.ver  = NOTKNOWN;
 chromaticity.hor  = NOTKNOWN;
 chromaticity.ver  = NOTKNOWN;
} 


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


lattRing& lattRing::operator=( const lattRing& x ){
 tune.hor = x.tune.hor;
 tune.ver = x.tune.ver;
 chromaticity.hor = x.chromaticity.hor;
 chromaticity.ver = x.chromaticity.ver;
 return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<(ostream& os, const lattRing& x) {
  os << x.tune.hor << " " << x.tune.ver << " ";
  os << " " << x.chromaticity.hor << " " << x.chromaticity.ver;
  return (os << endl);
}


// **************************************************
//   class beamline::arrayRep
// **************************************************

beamline::arrayRep::arrayRep(beamline const* x, bool doClone )
: _element(0)
{
 
  if( 0 == x ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "beamline::arrayRep::arrayRep( const beamline* x, bool doClone )", 
           "Constructor invoked with null pointer." ) );
  }

#ifndef NO_RTTI 
  if( typeid(*x) != typeid(beamline) ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "beamline::arrayRep::arrayRep( const beamline* x, bool doClone )", 
           "Constructor invoked with pointer to something other "
           "than a beamline." ) );
  }
#endif

  int i = 0;

  _n = x->countHowManyDeeply();

  _cloned = doClone;

  if ( _n > 0 ) {

    _element = new bmlnElmnt* [_n];
    bmlnElmnt** elm = &_element[0];
  
    if( _cloned ) {
      for ( beamline::deep_iterator it = x->deep_begin();  it != x->deep_end(); ++it, ++elm ) {
         *elm = (*it)->Clone();   
      }                             
    }
    else {
     for (beamline::deep_iterator it = x->deep_begin(); it != x->deep_end() ; ++it, ++elm ) {
         *elm = (*it);
     }
    }
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


beamline::arrayRep::~arrayRep()
{
  if ( _cloned ) {
    for( int i = 0; i < _n; i++ ) {
      delete _element[i];
    }
  }
  delete [] _element;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************
//   class beamline
// **************************************************

beamline::beamline( const char* nm ) 
: bmlnElmnt( nm ), _theList() 
{
 _mode          = unknown;
 numElem        = 0;
 nominalEnergy  = NOTKNOWN;
 twissDone      = 0;
} 


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::beamline( beamline const& a ) 
  : bmlnElmnt(a), 
    _mode(a._mode),  
    nominalEnergy(a.nominalEnergy), 
    numElem(a.numElem), 
    twissDone(0), 
   _theList() 
{

 for ( beamline::const_iterator it = a.begin(); it != a.end(); ++it) {
   append( (*it)->Clone() );
 }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline* beamline::Clone() const {

 return new beamline(*this);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


const char*  beamline::Type() const 
{ 
  return "beamline"; 
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

// Destructors

beamline::~beamline() {

 // Notice that this destructor does not destroy
 // the beamline elements.  To do that, use
 // beamline::zap().

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void beamline::zap() {

 for ( std::list<bmlnElmnt*>::iterator it=_theList.begin(); it!= _theList.end(); ++it)  { 

   if ( !(*it) ) continue; // skip null elements ... 
 
   if ( typeid( **it) == typeid(beamline) ) {
    dynamic_cast<beamline*>(*it)->zap();
   }
   delete (*it);  
 }

 _theList.clear();  
  numElem = 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void beamline::clear() {
  _theList.clear();
  numElem = 0;
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
 nominalEnergy = E;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::unTwiss() {

 dataHook.eraseFirst( "Ring" );
 if( !dataHook.eraseFirst( "Twiss" ) )

 for (beamline::iterator it = begin(); it != end();  ++it ) { 
   (*it)->dataHook.eraseFirst( "Twiss" );
   twissDone = 0;   // ??? Remove this eventually.
 }
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
  lattRing* ringPtr = (lattRing*)dataHook.find( "Ring" );
  if(ringPtr != 0)
    return *ringPtr;
    (*pcout) << endl;
    (*pcout) << "*** WARNING ***                            \n"
         << "*** WARNING *** beamline::whatIsRing       \n"
         << "*** WARNING *** Entry was not found.       \n"
         << "*** WARNING *** Meaningless value being    \n"
         << "*** WARNING *** returned.                  \n"
         << "*** WARNING ***                            \n" << endl;
  return errRet;
}

lattFunc beamline::whatIsLattice( int n ) {
 lattFunc errRet;

 if ( ( n < 0 ) || ( numElem <= n ) ){
    ostringstream uic;
    uic  << "Argument n = " << n 
         << " lies outside [0," 
         << (numElem-1) << "].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "lattFunc beamline::whatIsLattice( int n ) {", 
           uic.str().c_str() ) );
 }

int count = 0;
for (beamline::iterator it = begin(); it != end(); ++it ) { 
  if( n == count++ ) 
   return (*(lattFunc*) (*it)->dataHook.find( "Twiss" ));
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

lattFunc beamline::whatIsLattice( char* n ) {
  // Function written by Mike Martens.
  // May 24, 1996
  lattFunc errRet;
  
  for (beamline::iterator it = begin(); it != end(); ++it ) { 

    if( !strcmp((*it)->Name(),n) ) 
      return (*(lattFunc*) (*it)->dataHook.find( "Twiss" ));
  } 

    return errRet;  
}

    // ++++++++++++ Begin: Insert and append functions ++++++++++++++++++


void beamline::insert( bmlnElmnt* q ) {

  if ( !q) throw GenericException( __FILE__, __LINE__,  "beamline::insert( bmlnElmnt* q )", "Error: Attempt to insert a null bmlnElmnt*.");  

 _theList.push_front(q);
 
 if( twissDone ) unTwiss();

 length += q -> length;

 if ( beamline* bml = dynamic_cast<beamline*>(q) )  
      numElem += bml->numElem;
 else numElem++;
}  

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::append( bmlnElmnt* q ) {

  if ( !q) throw GenericException( __FILE__, __LINE__, "beamline::append( bmlnElmnt* q )", "Error: Attempt to append a null bmlnElmnt*.");  

 _theList.push_back( q );

 if( twissDone ) unTwiss();

 length += q->length;
 if(   beamline* bml = dynamic_cast<beamline*>(q) )  
      numElem += bml->numElem;
 else numElem++;
} 


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::InsertElementsFromList( double& s, InsertionList& inList, slist& removedElements )
{

 std::list<bmlnElmnt*>::iterator bml_iter = _theList.begin();

 bmlnElmnt* p_be   = ( bml_iter == _theList.end() ) ? 0 : *bml_iter;

 bmlnElmnt* p_be_a = 0;
 bmlnElmnt* p_be_b = 0;

 InsertionListElement* p_ile = inList(0);  // top element; not removed

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

 if( !p_ile ) {
  (*pcerr) << "\n"
          "*** WARNING ***                                     \n"
          "*** WARNING *** beamline::InsertElementsFromList( s_0, list ) \n"
          "*** WARNING *** The list is empty!                  \n"
          "*** WARNING *** Nothing will be done.               \n"
          "*** WARNING ***                                     \n"
       << endl;
  return;
 }


 Particle const* prtnPtr = inList.clonedParticlePtr();

 while( p_be && p_ile ) {

  if( typeid(*p_be) == typeid(beamline) ) {

    static_cast<beamline*>(p_be)->InsertElementsFromList( s, inList, removedElements );

    p_ile = inList(0);   // this may have changed
    ++bml_iter; 
    p_be = (bml_iter == _theList.end()) ? 0 : *bml_iter; 
  }

  else if ( s + p_be->OrbitLength( *prtnPtr ) <= p_ile->s ) {
    s += p_be->OrbitLength( *prtnPtr );
    ++bml_iter; 
    p_be = (bml_iter == _theList.end()) ? 0 : *bml_iter;     
  }

  else if ( s == p_ile->s ) {

    putAbove( bml_iter, p_ile->q );
    inList.Get();      // removes top element
    p_ile = inList(0); // accesses new top element

  }

  else if (  typeid(*p_be) == typeid(combinedFunction)  ) {

    p_be->p_bml->InsertElementsFromList( s, inList, removedElements );
    p_ile = inList(0);   // this may have changed
    ++bml_iter; 
    p_be = (bml_iter == _theList.end()) ? 0 : *bml_iter;     

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

  else if ( ( s < p_ile->s ) && ( p_ile->s < s + p_be->OrbitLength( *prtnPtr ) ) )  {

    //std::cout << " about to split element ... " << std::endl;
    //std::cout << " element to be split: Name = " << p_be->Name() << " Type = " << p_be ->Type() << std::endl;

    p_be->Split( ( p_ile->s - s )/p_be->OrbitLength( *prtnPtr ), &p_be_a, &p_be_b );

    //std::cout << " about to delete element ... " << std::endl;
    //std::cout << " element to be deleted: Name = " << p_be->Name() << " Type = " << p_be ->Type() << std::endl;
    
    putAbove( bml_iter, p_be_a   );
    putAbove( bml_iter, p_ile->q );
    putAbove( bml_iter, p_be_b   );

    s += ( p_be_a->OrbitLength( *prtnPtr ) + p_ile->q->OrbitLength( *prtnPtr ) );

  
    removedElements.append( (void*) p_be );

    p_be = p_be_b;
    inList.Get();      // removes top element
    p_ile = inList(0); // accesses new top element

    bml_iter = _theList.erase( bml_iter ); // bml_iter now points to element downstream of p_be_b !
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
    (*pcerr) << "else if ( s + p_be->OrbitLength( *prtnPtr ) <= p_ile->s )\n"
         << "else if ( " << setprecision(10) << ( s + p_be->OrbitLength( *prtnPtr ) )
         << " <= "       << setprecision(10) << ( p_ile->s )
         << " )\n";
    (*pcerr) << "else if ( s == p_ile->s )\n"
         << "else if ( " << setprecision(10) << s 
         << " == "       << setprecision(10) << p_ile->s 
         << " )\n";
    (*pcerr) << "else if ( p_be->p_bml )\n"
         << "else if ( " << p_be->p_bml << " )\n";
    (*pcerr) << "else if ( ( s < p_ile->s ) && ( p_ile->s < s + p_be->OrbitLength( *prtnPtr ) ) )\n"
         << "else if ( ( " << setprecision(10) << s 
         << " < "          << setprecision(10) << p_ile->s 
         << " ) && ( "     << setprecision(10) << p_ile->s 
         << " < "          << setprecision(10) << ( s + p_be->OrbitLength( *prtnPtr ) )
         << " ) )\n";


    s += p_be->OrbitLength( *prtnPtr );

    ++bml_iter;
    p_be =  ( bml_iter == _theList.end() ) ? 0: *bml_iter;

  }
 }

 delete prtnPtr;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int beamline::replace( bmlnElmnt const* a, bmlnElmnt const* b ) 
{

 
  std::list<bmlnElmnt*>::iterator it =  std::find( _theList.begin(), _theList.end(), a );   
 
  if ( it == _theList.end() ) return 1; // not found 

  (*it) = const_cast<bmlnElmnt*>(b);

  return  0;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int beamline::deepReplace( const bmlnElmnt* a, const bmlnElmnt* b ) 
{

  if( 0 == a || 0 == b ) { return 2; }


  for (std::list<bmlnElmnt*>::iterator it = _theList.begin(); it != _theList.end(); ++it ) {

    if( (*it) == a ) {

      (*it) = const_cast<bmlnElmnt*>(b); 
      return 0; 
    }

    else if( typeid(**it) == typeid(beamline) ) {

      if( dynamic_cast<beamline*>(*it)->deepReplace( a, b ) == 0  ) { return 0; }
    }

  }
  return 1;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline* beamline::reverse() const {

 std::string theName = std::string("REVERSE_") + std::string( Name() );
 beamline* result = new beamline( theName.c_str() );

  for ( beamline::reverse_iterator rit = rbegin(); rit != rend(); ++rit) {
  if( typeid(**rit) == typeid(beamline) ) 
    result->append(  dynamic_cast<beamline*>(*rit)->reverse() );
  else                      
    result->append( (*rit)->Clone() ); 
 }

 return result;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if  0
beamline& beamline::operator^( bmlnElmnt& x ) {
 append( x );
 return this;
}
#endif

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#if 0

beamline& operator^( bmlnElmnt& x, bmlnElmnt& y ) {
 beamline* result = new beamline;
 result->append( x );
 result->append( y );
 return *result;
}
#endif

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline& operator-( beamline const& x ) {

  // NOTE: THIS IS A MEMORY LEAK ! FIX ME !
  // the reverse beamline is not in the list of beamlines that get deleted 

 std::string theName = std::string("REVERSE_") + std::string( x.Name() );
 beamline* result = new beamline( theName.c_str() );

  for ( beamline::reverse_iterator rit = x.rbegin(); rit != x.rend(); ++rit) {
  if( typeid(**rit) == typeid(beamline) ) 
     result->append( &(- *static_cast<beamline*>(*rit)) );
  else                      
    result->append( (*rit)->Clone() ); // all elements are cloned 
 }

 return *result;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
beamline& beamline::operator+( bmlnElmnt& x ) {
 beamline* result = new beamline( *this );
 result->append( x );
 return *result;
}
#endif

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
beamline& beamline::operator+( beamline& x ) {
 beamline* result = new beamline( *this );
 result->append( x );
 return *result;
}

#endif

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
beamline& operator*( int c, beamline& x ) {
 beamline* result = new beamline;
 int i;
 if ( c == 0 ) return *result;
 if ( c < 0 )  c = - c;
 for ( i = 0; i < c; i++ ) result->append( x );
 return *result;
}
#endif


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if  0
beamline& operator*( beamline& x, int c ) {
 beamline* result = new beamline;
 int i;
 if ( c == 0 ) return *result;
 if ( c < 0 )  c = - c;
 for ( i = 0; i < c; i++ ) result->append( x );
 return *result;
}

#endif
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if  0
beamline& operator*( int c, bmlnElmnt& x ) {
 beamline* result = new beamline;
 int i;
 if ( c == 0 ) return *result;
 if ( c < 0 )  c = - c;
 for ( i = 0; i < c; i++ ) result->append( x );
 return *result;
}
#endif

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
beamline& operator*( bmlnElmnt& x, int c ) {
 beamline* result = new beamline;
 int i;
 if ( c == 0 ) return *result;
 if ( c < 0 )  c = - c;
 for ( i = 0; i < c; i++ ) result->append( x );
 return *result;
}
#endif


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::Split( double const&, bmlnElmnt**, bmlnElmnt** ) const
{
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "void beamline::Split( double const&, bmlnElmnt**, bmlnElmnt** )", 
         "This method should not be invoked by a beamline object." ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::putAbove( std::list<bmlnElmnt*>::iterator const& iter, bmlnElmnt*  y ) 
{
 // Insert y above (before; upstream of) iter in the beamline
 // upon return, iter points Insert y above (before; upstream of) iter in the beamline

 unTwiss();

 _theList.insert( iter, y );

 length += y->length;
 numElem++;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::putBelow( std::list<bmlnElmnt*>::iterator const& iter, bmlnElmnt* y ) 
{
 // Insert y below (after; downstream of) x in the beamline

 unTwiss();

 std::list<bmlnElmnt*>::iterator tmp_iter = iter; 

 if ( iter == _theList.end() ) {
   (*pcout) << "*** WARNING **** : attempt to insert an element downstream of a beamline's end." << std::endl;
   (*pcout) << "                   beamline name :" << Name()  << std::endl;
   return;
 }

 ++tmp_iter;

 _theList.insert( tmp_iter, y );
 
 length += y->length;
 
 ++numElem;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


    // ++++++++++++ End:   Insert and append functions ++++++++++++++++++


beamline* beamline::flatten() const {

 // Produces a flattened version of itself.
 // WARNING: the elements are not cloned.
 //   Thus the flattened line contains the
 //   same objects as its original.


 beamline* r = new beamline;

 for (beamline::deep_iterator it = deep_begin(); it != deep_end(); ++it ) {  
     r->append( (*it) );
 }

 r->setEnergy( this->Energy() );
 r->rename( this->Name() );
 r->setLineMode( _mode );

 return r;
} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int beamline::startAt( bmlnElmnt const* x, int n ) {

  int count = 0;

  std::list<bmlnElmnt*> newList;
  std::list<bmlnElmnt*>::iterator pos;

  for ( std::list<bmlnElmnt*>::iterator it =_theList.begin();  it != _theList.end(); ++it) { 
       if ( *it == x )   ++count;            
       if ( count == n ) {  
          pos = it;
          break;
       }
  }
       
  if  ( pos == _theList.end()) return 1; // new starting element not found  

  for ( std::list<bmlnElmnt*>::iterator it = pos;  it != _theList.end(); ++it) { 
    newList.push_back(*it);
  }

  for ( std::list<bmlnElmnt*>::iterator it = _theList.begin();  it != pos; ++it) { 
    newList.push_back(*it);
  }

  _theList = newList;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::startAt( char const* s, int n ) {

  int count = 0;

  std::list<bmlnElmnt*> newList;
  std::list<bmlnElmnt*>::iterator pos;

  for ( std::list<bmlnElmnt*>::iterator it =_theList.begin();  it != _theList.end(); ++it) { 
       if ( std::string( (*it)->Name() ) == std::string( s ) )   ++count;            
       if ( count == n ) {  
          pos = it;
          break;
       }
  }
       
  if  ( pos == _theList.end()) return 1; // new starting element not found  

  for ( std::list<bmlnElmnt*>::iterator it = pos;  it != _theList.end(); ++it) { 
    newList.push_back(*it);
  }

  for ( std::list<bmlnElmnt*>::iterator it = _theList.begin();  it != pos; ++it) { 
    newList.push_back(*it);
  }

  _theList = newList;

  return 1;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector* beamline::MakeSector ( bmlnElmnt const&  be_1, const bmlnElmnt& be_2, int deg, JetParticle& jp ) {

 // This assumes that the argument jp has been initialized as
 // desired by the calling program.  This routine does NOT
 // initialize the state of jp.

 bool       firstFound  = false;
 bool       secondFound = false;
 Particle*  p_prt;
 double     s           = 0.0;

 p_prt = new Particle( jp );

 for (beamline::deep_iterator it = deep_begin(); it != deep_end(); ++it) {

  if( (*it) == &be_2 ) {
   (*pcout) << "*** WARNING ***                                      \n" 
        << "*** WARNING *** beamline::MakeSector                 \n" 
        << "*** WARNING *** Second element found first.          \n" 
        << "*** WARNING *** Returning zero.                      \n" 
        << "*** WARNING ***                                      \n" 
        << endl;
   delete p_prt;
   return 0;
  }

  else if( (*it) == &be_1 ) { 
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
  delete p_prt;
  return 0;
 }
 
 for (beamline::deep_iterator it = deep_begin(); it != deep_end(); ++it) { // Notice: we do not propagate through

  if( (*it) == &be_2 ) {                                                    // be_1 and be_2
    secondFound = true;
    break;
  }
  else {
    (*it)->propagate( jp );
    s += (*it)->OrbitLength( *p_prt );
  }
 }

 if( !secondFound ) {
  (*pcout) << "*** WARNING ***                                      \n" 
       << "*** WARNING *** beamline::MakeSector                 \n" 
       << "*** WARNING *** Unable to find second element.       \n" 
       << "*** WARNING *** Returning zero.                      \n" 
       << "*** WARNING ***                                      \n" 
       << endl;
  delete p_prt;
  return 0;
 }
 
 delete p_prt;

 return new sector( jp.State().filter( 0, deg ), s );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sector* beamline::MakeSectorFromStart ( const bmlnElmnt& be_1, int deg, JetParticle& jp ) {

 // This assumes that the argument jp has been initialized as
 // desired by the calling program.  This routine does NOT
 // initialize the state of jp.

 bool       firstFound  = false;
 Particle*  p_prt;
 double     s           = 0.0;
 
 p_prt = new Particle(jp);

 // Check first element against the argument ------------

 beamline::iterator it = begin();

 bmlnElmnt* p_be = *it;

 if( !p_be ) {
  (*pcout) << "*** WARNING ***                                      \n" 
       << "*** WARNING *** beamline::MakeSectorFromStart        \n" 
       << "*** WARNING *** The beamline was empty!!             \n" 
       << "*** WARNING *** Returning zero.                      \n" 
       << "*** WARNING ***                                      \n" 
       << endl;
   delete p_prt;
   return 0;
 }

 if( p_be == &be_1 ) {
   delete p_prt;
   return 0;
 }

 // Propagate the JetParticle through first element -----------
 p_be->propagate( jp );
 s += p_be->OrbitLength( *p_prt );


 // Find element that matches argument ------------------
 for ( beamline::iterator it = begin(); it != end(); ++it) {
  
   p_be = *it; 
 
  if( p_be == &be_1 ) {     // Notice: we do not propagate through be_1
    firstFound = true;
    break;
  }
  else {
    p_be->propagate( jp );
    s += p_be->OrbitLength( *p_prt );
  }
 }

 if( !firstFound ) {
  (*pcout) << "*** WARNING ***                                      \n" 
       << "*** WARNING *** beamline::MakeSectorFromStart        \n" 
       << "*** WARNING *** Unable to find element.              \n" 
       << "*** WARNING *** Returning zero.                      \n" 
       << "*** WARNING ***                                      \n" 
       << endl;
  delete p_prt;
  return 0;
 }
 
 // Construct the map and return the sector -----------------------------------

 delete p_prt;
 return new sector( jp.State().filter( 0, deg ), s );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


sector* beamline::MakeSectorToEnd ( const bmlnElmnt& be_1, int deg, JetParticle& jp ) {

 // This assumes that the argument jp has been initialized as
 // desired by the calling program.  This routine does NOT
 // initialize the state of jp.

 bmlnElmnt* p_be;
 char       firstFound  = 0;
 Particle*  p_prt;
 double     s           = 0.0;

 p_prt = new Particle(jp);

 // Find the element that matches argument ---------------------------------

 beamline::iterator it = begin(); 

 for (  ; it != end(); ++it) {
  if( (*it) == &be_1 ) { 
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
  delete p_prt;
  return 0;
 }
 
 // Check that it is not the last element --------------------------------
 p_be = *( ++it );

 if( p_be ) {
    delete p_prt;
    return 0;
 }
 else {
    p_be->propagate( jp );
    s += p_be->OrbitLength( *p_prt );
 }

 // Construct the map and return sector ------------------------------------

 for ( ; it != end(); ++it) {
   p_be = (*it);
   p_be->propagate( jp );
   s += p_be->OrbitLength( *p_prt );
 }

 delete p_prt;
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

 sector* s       = new sector ( ident, zd, length );
 // ??? REMOVE sector* s       = new sector ( zd, length );
 // ??? REMOVE strcpy( s->ident, ident );
 s->length       = length;
 s->strength     = strength;
 s->align        = align;
   //
   // O.K.
   //
   // s->aperture     = aperture;
 
 s->pAperture     = pAperture;

 return s;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::sectorize( int degree, JetParticle& pd ) {

 sector* s = makeSector( degree, pd );

 _theList.clear();
 unTwiss();
 numElem      = 0;
 length       = 0.0;
 strength     = 0.0;
 align        = 0;

 append( s );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline beamline::sectorize( bmlnElmnt* x, bmlnElmnt* y, int degree, JetParticle& pd, const char* sectorName ) {

  beamline a = remove( x, y ); // remove should peharps somehow return the iterator position for x

  sector* s = a.makeSector( degree, pd );
  s->rename( sectorName );

  std::list<bmlnElmnt*>::iterator xpos = std::find( _theList.begin(), _theList.end(), y ); 
   
  putAbove( xpos, s );

  return a;
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::peekAt( double& s, const Particle& prt ) const
{

  bmlnElmnt* p;

  (*pcout) << "\nBegin beamline::peekat() -- Address of beamline: "
       << ident << " = " << (int) this 
       << endl;

 for (beamline::iterator it = begin(); it != end(); ++it )  {
    if( typeid(**it) == typeid(beamline) ) 
      static_cast<beamline*>(*it)->peekAt( s, prt );
    else (*it)->peekAt( s, prt );
  }

  (*pcout) << "End beamline::peekat() -- Address of beamline: "
       << ident << " = " << (int) this 
       << endl;

  // REMOVE: if( newProton ) delete p_prt;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::countHowMany( CRITFUNC query, slist* listPtr ) const {


 bmlnElmnt* p = 0;
 int ret      = 0;

 if( query == 0 ) {

   for (beamline::iterator it = begin(); it != end(); ++it, ++ret ) ;

   if( ret != numElem ) {
     (*pcerr) << "\n*** WARNING ***                                     \n"
               "*** WARNING *** beamline::countHowMany              \n"
               "*** WARNING *** Inconsistency in the count:         \n"
               "*** WARNING *** "
          << ret
          << " != "
          << numElem
          << "\n*** WARNING ***                                     \n"
          << endl;
   }
 }

 else {

   for (beamline::iterator it = begin(); it != end(); ++it) {
     p = (*it);
     if( query(p) ) { 
       ++ret; 
       if( listPtr ) {
         listPtr->append(p);
       }
     }
   }
 }

 return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::countHowManyDeeply( CRITFUNC query, slist* listPtr ) const {

 int ret      = 0;
 bmlnElmnt* p = 0;

   for (beamline::iterator it = begin(); it != end(); ++it) {
     p = (*it);
     if( typeid(*p) == typeid(beamline) ) {
       ret += static_cast<beamline*>(p)->countHowManyDeeply( query, listPtr );
     }
     else {
       if( ( query == 0 ) || ( query(p) ) ) {
         ++ret;
         if( listPtr ) {
           listPtr->append(p);
         }
       }
     }
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
  
  bmlnElmnt* p_be = 0;

  for (beamline::iterator it = begin(); it != end(); ++it) {
    p_be = (*it);
    ret = 0;
    if(  typeid(*p_be) == typeid(beamline) ) {
      int subDepth = (dynamic_cast<const beamline*>(p_be))->depth();
      if( maxSubDepth < subDepth ) { maxSubDepth = subDepth; }
    }
  }

  if( 0 == ret ) { ret = maxSubDepth + 1; }
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::contains( bmlnElmnt const* x ) const
{

  int ret = 0;
  for (beamline::deep_iterator it = deep_begin(); it != deep_end(); ++it) {
    if( (*it) == x ) { ++ret; }
  }
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline beamline::remove( bmlnElmnt *x, bmlnElmnt * y ) {

 // remove the elements in the interval  ] x, y [  and return them in a new beamline

 unTwiss();

 beamline a;

 if (x == y ) return a; // empty

 std::list<bmlnElmnt*>::iterator pos1 = std::find(_theList.begin(), _theList.end(), x); 
 std::list<bmlnElmnt*>::iterator pos2 = std::find(pos1,             _theList.end(), y);  

 if ( (pos1 == _theList.end()) || (pos2 == _theList.end() ) ) 
   return a; // one of the element not found. return an empty line  

 ++pos1; --pos2; // exclude the boundary elements

 ++pos2;

 for ( std::list<bmlnElmnt*>::iterator it = pos1; it != pos2;  ++it ) { 
   a.append(*it);
 }

 --pos2; 

 _theList.erase( pos1, pos2 ); 

 numElem = numElem - a.numElem; 

 return a;


}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool beamline::find( bmlnElmnt*& u, bmlnElmnt*& v, bmlnElmnt*& w ) const
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

  int elementCount = this->countHowManyDeeply();
 
  if( 0 == elementCount ) { 
    return false; 
  }

  if( 1 == elementCount ) { 
    if( v == this->firstElement() ) {
      u = w = 0;
      return true;
    }
    return false;
  }
 
  if( 2 == elementCount ) { 
    if( v == this->firstElement() ) {
      u = 0;
      w = this->lastElement();
      return true;
    }
    if( v == this->lastElement() ) {
      u = this->firstElement();
      w = 0;
      return true;
    }
    return false;
  }

  // Setup iterator


   beamline::deep_iterator dbi = deep_begin();
   bmlnElmnt* q = 0;
     
  // Check for possibility that *v is the first element.

  if( v == this->firstElement() ) {
    if( beamline::ring == this->getLineMode() ) {
        u = this->lastElement();
      } else {
        u = 0;
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
      if( v == this->lastElement() ) {
        if( beamline::ring == this->getLineMode() ) {
          w = this->firstElement();
        }
        else {
          w = 0;
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

BmlPtrList beamline::moveRelX(   bmlnElmnt* thePtr
                               , double const& u
                               , int* errorCodePtr )
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

  BmlPtrList recycleBin;
  int dummyErr;
  if( 0 == errorCodePtr ) {
    this->_moveRel( Frame::xAxisIndex(), u, thePtr, &dummyErr, &recycleBin, 
                    string("beamline::moveRelX") );
  }
  else {
    this->_moveRel( Frame::xAxisIndex(), u, thePtr, errorCodePtr, &recycleBin, 
                    string("beamline::moveRelX") );
  }
  return recycleBin;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtrList beamline::moveRelY(   bmlnElmnt* thePtr
                               , double const& u
                               , int* errorCodePtr )
{
  BmlPtrList recycleBin;
  int dummyErr;
  if( 0 == errorCodePtr ) {
    this->_moveRel( Frame::yAxisIndex(), u, thePtr, &dummyErr, &recycleBin, 
                    string("beamline::moveRelX") );
  }
  else {
    this->_moveRel( Frame::yAxisIndex(), u, thePtr, errorCodePtr, &recycleBin, 
                    string("beamline::moveRelX") );
  }
  return recycleBin;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtrList beamline::moveRelZ(   bmlnElmnt* thePtr
                               , double const& u
                               , int* errorCodePtr )
{
  BmlPtrList recycleBin;
  int dummyErr;
  if( 0 == errorCodePtr ) {
    this->_moveRel( Frame::zAxisIndex(), u, thePtr, &dummyErr, &recycleBin, 
                    string("beamline::moveRelX") );
  }
  else {
    this->_moveRel( Frame::zAxisIndex(), u, thePtr, errorCodePtr, &recycleBin, 
                    string("beamline::moveRelX") );
  }
  return recycleBin;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::_moveRel(   int axis, double const& u
                         , bmlnElmnt* thePtr
                         , int* errorCodePtr, BmlPtrList* recycleBinPtr
                         , string invoker )
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
  // #error *** WARNING ***  beamline::_moveRel must be reviewed and tested!!
  // #error *** WARNING ***

  int dummyError;
  int* errPtr;

  // Set the error pointer ...
  if( 0 == errorCodePtr ) { errPtr = &dummyError;  }
  else                    { errPtr = errorCodePtr; }
  *errPtr = 0;

  // Argument filter
  if( 0 == thePtr || 0 == recycleBinPtr ) { 
    *errPtr = 1; 
    return;
  }
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
  bmlnElmnt* upStreamPtr   = 0;
  bmlnElmnt* downStreamPtr = 0;

  Frame frameZero, frameOne, frameTwo, frameThree;
  Frame pinnedFrameOne, pinnedFrameTwo;
  // Note: frameZero never changes; it remains the identity.

  if( this->find( upStreamPtr, thePtr, downStreamPtr ) )
  // ??? if( this->find( upStreamPtr, ((const bmlnElmnt*&)thePtr), downStreamPtr ) )
  {
    if( (0 != upStreamPtr) && (0 != downStreamPtr ) ) {
      FramePusher fp( frameZero );
      upStreamPtr->accept( fp );
      frameOne = fp.getFrame();
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();

      pinnedFrameOne = ( (thePtr->_pinnedFrames)._upStream   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->_pinnedFrames)._downStream ).patchedOnto( frameTwo );

      // !!! The next lines can be modified (maybe) to do pinned-referenced movements
      Vector displacement(u*frameOne.getAxis(axis));
      frameOne.translate( displacement );
      frameTwo.translate( displacement );

      (thePtr->_pinnedFrames)._upStream   = pinnedFrameOne.relativeTo( frameOne );
      (thePtr->_pinnedFrames)._downStream = pinnedFrameTwo.relativeTo( frameTwo );

      // Reset upstream and downstream elements
      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(upStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(upStreamPtr)->setOutFrame( frameOne );
        dynamic_cast<Slot*>(upStreamPtr)->_pinnedFrames._downStream = (thePtr->_pinnedFrames)._upStream;
        dynamic_cast<Slot*>(upStreamPtr)->_pinnedFrames._altered = true;
        // ??? This is not quite right.
      }
      if( 0 == strcmp("Slot", downStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(downStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(downStreamPtr)->setOutFrame( frameThree.relativeTo(frameTwo) );
        dynamic_cast<Slot*>(downStreamPtr)->_pinnedFrames._upStream = (thePtr->_pinnedFrames)._downStream;
        dynamic_cast<Slot*>(downStreamPtr)->_pinnedFrames._altered = true;
        // ??? This is not quite right.
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(upStreamPtr->Name(), frameOne );
        slotPtr->setReferenceTime( upStreamPtr->getReferenceTime() );
        slotPtr->_pinnedFrames._downStream = (thePtr->_pinnedFrames)._upStream;
        slotPtr->_pinnedFrames._altered = true;
        putAbove( std::find(_theList.begin(), _theList.end(), thePtr),  slotPtr ); // !!!! TERRIBLE !
        remove( upStreamPtr );
        recycleBinPtr->append( upStreamPtr );
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) );
        slotPtr->setReferenceTime( downStreamPtr->getReferenceTime() );
        slotPtr->_pinnedFrames._upStream = (thePtr->_pinnedFrames)._downStream;
        slotPtr->_pinnedFrames._altered = true;
        putBelow( std::find(_theList.begin(), _theList.end(), thePtr), slotPtr ); // !!!! TERRIBLE  !
        this->remove( downStreamPtr );
        recycleBinPtr->append( downStreamPtr );
      }
    }

    else if( (0 == upStreamPtr) && (0 != downStreamPtr ) ) {
      frameOne = frameZero;
      FramePusher fp( frameOne );
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();

      pinnedFrameOne = ( (thePtr->_pinnedFrames)._upStream   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->_pinnedFrames)._downStream ).patchedOnto( frameTwo );

      Vector displacement(u*frameTwo.getAxis(axis));
      frameOne.translate( displacement );
      frameTwo.translate( displacement );

      (thePtr->_pinnedFrames)._upStream   = pinnedFrameOne.relativeTo( frameOne );
      (thePtr->_pinnedFrames)._downStream = pinnedFrameTwo.relativeTo( frameTwo );

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
        dynamic_cast<Slot*>(downStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(downStreamPtr)->setOutFrame( frameThree.relativeTo(frameTwo) );
        dynamic_cast<Slot*>(downStreamPtr)->_pinnedFrames._upStream = (thePtr->_pinnedFrames)._downStream;
        dynamic_cast<Slot*>(downStreamPtr)->_pinnedFrames._altered = true;
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) );
        slotPtr->setReferenceTime( downStreamPtr->getReferenceTime() );
        slotPtr->_pinnedFrames._upStream = (thePtr->_pinnedFrames)._downStream;
        slotPtr->_pinnedFrames._altered = true;
        this->putBelow( std::find(_theList.begin(), _theList.end(), thePtr), slotPtr );
        this->remove( downStreamPtr );
        recycleBinPtr->append( downStreamPtr );
      }
    }

    else if( (0 != upStreamPtr) && (0 == downStreamPtr ) ) {
      FramePusher fp( frameZero );
      upStreamPtr->accept( fp );
      frameOne = fp.getFrame();
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      frameThree = frameTwo;

      pinnedFrameOne = ( (thePtr->_pinnedFrames)._upStream   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->_pinnedFrames)._downStream ).patchedOnto( frameTwo );

      Vector displacement(u*frameOne.getAxis(axis));
      frameOne.translate( displacement );
      frameTwo.translate( displacement );

      (thePtr->_pinnedFrames)._upStream   = pinnedFrameOne.relativeTo( frameOne );
      (thePtr->_pinnedFrames)._downStream = pinnedFrameTwo.relativeTo( frameTwo );

      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(upStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(upStreamPtr)->setOutFrame( frameOne );
        dynamic_cast<Slot*>(upStreamPtr)->_pinnedFrames._downStream = (thePtr->_pinnedFrames)._upStream;
        dynamic_cast<Slot*>(upStreamPtr)->_pinnedFrames._altered = true;
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(upStreamPtr->Name(), frameOne );  // ?????
        slotPtr->setReferenceTime( upStreamPtr->getReferenceTime() );
        slotPtr->_pinnedFrames._downStream = (thePtr->_pinnedFrames)._upStream;
        slotPtr->_pinnedFrames._altered = true;
        putAbove( std::find(_theList.begin(), _theList.end(), thePtr), slotPtr ); // TERRIBLE !
        remove( upStreamPtr );
        recycleBinPtr->append( upStreamPtr );
      }
    }

    else if( (0 == upStreamPtr) && (0 == downStreamPtr ) ) {
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void beamline::_moveRel( int axis, double const& u, ... )",
             "An impossibility has occurred. Am stopping." ) );
    }
  }

  thePtr->_pinnedFrames._altered = true;
  return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtrList beamline::pitch(   bmlnElmnt* thePtr, double const& angle, double const& pct, int* errorCodePtr )
{
  BmlPtrList recycleBin;
  int dummyErr;
  if( 0 == errorCodePtr ) {
    this->_rotateRel( Frame::xAxisIndex(), angle, thePtr, pct, 
                      &dummyErr, &recycleBin, 
                      string("beamline::pitch") );
  }
  else {
    this->_rotateRel( Frame::xAxisIndex(), angle, thePtr, pct, 
                      errorCodePtr, &recycleBin, 
                      string("beamline::pitch") );
  }
  return recycleBin;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtrList beamline::yaw(   bmlnElmnt* thePtr, double const& angle, double const& pct, int* errorCodePtr )
{
  BmlPtrList recycleBin;
  int dummyErr;
  if( 0 == errorCodePtr ) {
    this->_rotateRel( Frame::yAxisIndex(), angle, thePtr, pct, &dummyErr, &recycleBin, string("beamline::pitch") );
  }
  else {
    this->_rotateRel( Frame::yAxisIndex(), angle, thePtr, pct, errorCodePtr, &recycleBin, string("beamline::pitch"));
  }
  return recycleBin;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtrList beamline::roll(   bmlnElmnt* thePtr
                           , double const& angle
                           , double const& pct
                           , int* errorCodePtr )
{
  BmlPtrList recycleBin;
  int dummyErr;
  if( 0 == errorCodePtr ) {
    this->_rotateRel( Frame::zAxisIndex(), angle, thePtr, pct, 
                      &dummyErr, &recycleBin, 
                      string("beamline::pitch") );
  }
  else {
    this->_rotateRel( Frame::zAxisIndex(), angle, thePtr, pct, 
                      errorCodePtr, &recycleBin, 
                      string("beamline::pitch") );
  }
  return recycleBin;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::_rotateRel(   int axis, double const& angle
                           , bmlnElmnt* thePtr
                           , double pct
                           , int* errorCodePtr, BmlPtrList* recycleBinPtr
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
  if( 0 == thePtr || 0 == recycleBinPtr ) { 
    *errPtr = 1; 
    return;
  }
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
  bmlnElmnt* upStreamPtr   = 0;
  bmlnElmnt* downStreamPtr = 0;

  Frame frameZero, frameOne, frameTwo, frameThree;
  Frame pinnedFrameOne, pinnedFrameTwo;

  if( this->find( upStreamPtr, thePtr, downStreamPtr ) ) {

    if( (0 != upStreamPtr) && (0 != downStreamPtr ) ) {
      FramePusher fp( frameZero );
      upStreamPtr->accept( fp );
      frameOne = fp.getFrame();
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();

      pinnedFrameOne = ( (thePtr->_pinnedFrames)._upStream   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->_pinnedFrames)._downStream ).patchedOnto( frameTwo );

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
          bmlnElmnt* usHalfPtr;
          bmlnElmnt* dsHalfPtr;
          thePtr->Split( pct, &usHalfPtr, &dsHalfPtr );
          FramePusher fp2( frameOne );
          usHalfPtr->accept(fp2);
          midFrame = fp2.getFrame();
          delete usHalfPtr;
          delete dsHalfPtr;
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

      (thePtr->_pinnedFrames)._upStream   = pinnedFrameOne.relativeTo( frameOne );
      (thePtr->_pinnedFrames)._downStream = pinnedFrameTwo.relativeTo( frameTwo );

      // Reset upstream and downstream elements
      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(upStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(upStreamPtr)->setOutFrame( frameOne );
        dynamic_cast<Slot*>(upStreamPtr)->_pinnedFrames._downStream = (thePtr->_pinnedFrames)._upStream;
        dynamic_cast<Slot*>(upStreamPtr)->_pinnedFrames._altered = true;
      }
      if( 0 == strcmp("Slot", downStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(downStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(downStreamPtr)->setOutFrame( frameThree.relativeTo(frameTwo) );
        dynamic_cast<Slot*>(downStreamPtr)->_pinnedFrames._upStream = (thePtr->_pinnedFrames)._downStream;
        dynamic_cast<Slot*>(downStreamPtr)->_pinnedFrames._altered = true;
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(upStreamPtr->Name(), frameOne );
        slotPtr->setReferenceTime( upStreamPtr->getReferenceTime() );
        slotPtr->_pinnedFrames._downStream = (thePtr->_pinnedFrames)._upStream;
        slotPtr->_pinnedFrames._altered = true;
        putAbove( std::find(_theList.begin(), _theList.end(), thePtr), slotPtr ); // TERRIBLE !
        remove( upStreamPtr );
        recycleBinPtr->append( upStreamPtr );
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) );
        slotPtr->setReferenceTime( downStreamPtr->getReferenceTime() );
        slotPtr->_pinnedFrames._upStream = (thePtr->_pinnedFrames)._downStream;
        slotPtr->_pinnedFrames._altered = true;
        putBelow( std::find(_theList.begin(), _theList.end(), thePtr), slotPtr ); // TERRIBLE !
        remove( downStreamPtr );
        recycleBinPtr->append( downStreamPtr );
      }
    }

    else if( (0 == upStreamPtr) && (0 != downStreamPtr ) ) {
      frameOne = frameZero;
      FramePusher fp( frameOne );
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();

      pinnedFrameOne = ( (thePtr->_pinnedFrames)._upStream   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->_pinnedFrames)._downStream ).patchedOnto( frameTwo );

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
          bmlnElmnt* usHalfPtr;
          bmlnElmnt* dsHalfPtr;
          thePtr->Split( pct, &usHalfPtr, &dsHalfPtr );
          FramePusher fp2( frameOne );
          usHalfPtr->accept(fp2);
          midFrame = fp2.getFrame();
          delete usHalfPtr;
          delete dsHalfPtr;
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

      (thePtr->_pinnedFrames)._upStream   = pinnedFrameOne.relativeTo( frameOne );
      (thePtr->_pinnedFrames)._downStream = pinnedFrameTwo.relativeTo( frameTwo );

      // Reset upstream and downstream elements
      // Note: this is done inefficiently
      // DANGEROUS!!  Creates free object
      Slot* slotPtr = new Slot( frameOne );
      putAbove( std::find(_theList.begin(), _theList.end(), thePtr), slotPtr );
      if( 0 == strcmp("Slot", downStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(downStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(downStreamPtr)->setOutFrame( frameThree.relativeTo(frameTwo) );
        dynamic_cast<Slot*>(downStreamPtr)->_pinnedFrames._upStream = (thePtr->_pinnedFrames)._downStream;
        dynamic_cast<Slot*>(downStreamPtr)->_pinnedFrames._altered = true;
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) );
        slotPtr->setReferenceTime( downStreamPtr->getReferenceTime() );
        slotPtr->_pinnedFrames._upStream = (thePtr->_pinnedFrames)._downStream;
        slotPtr->_pinnedFrames._altered = true;
        putBelow( std::find( _theList.begin(), _theList.end(), thePtr), slotPtr );
        remove( downStreamPtr );
        recycleBinPtr->append( downStreamPtr );
      }
    }

    else if( (0 != upStreamPtr) && (0 == downStreamPtr ) ) {
      FramePusher fp( frameZero );
      upStreamPtr->accept( fp );
      frameOne = fp.getFrame();
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      frameThree = frameTwo;

      pinnedFrameOne = ( (thePtr->_pinnedFrames)._upStream   ).patchedOnto( frameOne );
      pinnedFrameTwo = ( (thePtr->_pinnedFrames)._downStream ).patchedOnto( frameTwo );

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
          bmlnElmnt* usHalfPtr;
          bmlnElmnt* dsHalfPtr;
          thePtr->Split( pct, &usHalfPtr, &dsHalfPtr );
          FramePusher fp2( frameOne );
          usHalfPtr->accept(fp2);
          midFrame = fp2.getFrame();
          delete usHalfPtr;
          delete dsHalfPtr;
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

      (thePtr->_pinnedFrames)._upStream   = pinnedFrameOne.relativeTo( frameOne );
      (thePtr->_pinnedFrames)._downStream = pinnedFrameTwo.relativeTo( frameTwo );

      // Reset upstream and downstream elements
      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(upStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(upStreamPtr)->setOutFrame( frameOne );
        dynamic_cast<Slot*>(upStreamPtr)->_pinnedFrames._downStream = (thePtr->_pinnedFrames)._upStream;
        dynamic_cast<Slot*>(upStreamPtr)->_pinnedFrames._altered = true;
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(upStreamPtr->Name(), frameOne );
        slotPtr->setReferenceTime( upStreamPtr->getReferenceTime() );
        slotPtr->_pinnedFrames._downStream = (thePtr->_pinnedFrames)._upStream;
        slotPtr->_pinnedFrames._altered = true;
        putAbove( std::find(_theList.begin(), _theList.end(), thePtr), slotPtr );
        remove( upStreamPtr );
        recycleBinPtr->append( upStreamPtr );
      }
      // DANGEROUS!!  Creates free object
      Slot* slotPtr = new Slot( frameThree.relativeTo(frameTwo) );
      putBelow( std::find(_theList.begin(), _theList.end(), thePtr), slotPtr );
    }

    else if( (0 == upStreamPtr) && (0 == downStreamPtr ) ) {
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void beamline::_moveRel( int axis, double const& u, ... )",
             "An impossibility has occurred. Am stopping." ) );
    }
  }

  thePtr->_pinnedFrames._altered = true;
  return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool beamline::setAlignment( const alignmentData& al ) {
  // Propogate alignment data of entire  beamline to each individual element


  bmlnElmnt* p = 0;
  for (beamline::iterator it = begin() ; it != end(); ++it) {
    p = (*it);
    if( !(p->setAlignment(al)) ) {
      (*pcerr) << "\n*** ERROR *** "
           << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
           << "\n*** ERROR *** bool beamline::setAlignment( const alignmentData& al )"
              "\n*** ERROR *** Unable to perform operation on "
           << p->Type() << "  " << p->Name()
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

void beamline::remove( bmlnElmnt* Element2remove){ 
  if (numElem == 0 ) return;
  _theList.remove( Element2remove );
  --numElem;
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

  bmlnElmnt* element = 0;

  for (beamline::iterator it = begin() ; it != end(); ++it) {

    element = (*it);

    if( (typeid(*element) == typeid(sbend) ) ||
        (typeid(*element) == typeid(rbend) ) ) {    

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

  bmlnElmnt* element = 0;

  for (beamline::iterator it = begin() ; it != end(); ++it) {
    element = (*it);
    if( (typeid(*element) == typeid(sbend) )||
        (typeid(*element) == typeid(rbend) )  ) {
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

bool beamline::isFlat() 
{
  bmlnElmnt* q;

  for (beamline::iterator it = begin() ; it != end(); ++it) {
    if( typeid(**it) == typeid(beamline) ) return false; 
  }
  return true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool beamline::Criterion::operator()( const bmlnElmnt* x )
{
  return false;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool beamline::Criterion::operator()( const bmlnElmnt& x )
{
  return operator()( &x );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::Action::operator()( bmlnElmnt* x )
{
  return -1;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int beamline::Action::operator()( bmlnElmnt& x )
{
  return operator()( &x );
}


