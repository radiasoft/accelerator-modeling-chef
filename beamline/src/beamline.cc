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
******  Copyright (c) 1991 Universities Research Association, Inc.
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef NO_RTTI 
#include <typeinfo>
#endif
#include <string>

#include "bmlnElmnt.h"
#include "Slot.h"
#include "sector.h"
#include "BeamlineIterator.h"
#include "InsertionList.h"
#include "FramePusher.h"

#include <iomanip>

using namespace std;


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

// **************************************************
//   struct lattRing
// **************************************************

lattRing::lattRing() : BarnacleData() {
 tune.hor  = NOTKNOWN;
 tune.ver  = NOTKNOWN;
 chromaticity.hor  = NOTKNOWN;
 chromaticity.ver  = NOTKNOWN;
} 


lattRing& lattRing::operator=( const lattRing& x ){
 tune.hor = x.tune.hor;
 tune.ver = x.tune.ver;
 chromaticity.hor = x.chromaticity.hor;
 chromaticity.ver = x.chromaticity.ver;
 return *this;
}
ostream& operator<<(ostream& os, const lattRing& x) {
  os << x.tune.hor << " " << x.tune.ver << " ";
  os << " " << x.chromaticity.hor << " " << x.chromaticity.ver;
  return (os << endl);
}

// **************************************************
//   struct beamlineData
// **************************************************

beamlineData::beamlineData() : bmlnElmntData() {
 more = 1;     // ??? Is this correct???
 numElem = 0;
}

beamlineData::beamlineData( beamlineData& x ) : bmlnElmntData( (bmlnElmntData&) x ) {
 more = x.more;
 numElem = x.numElem;
}

beamlineData::~beamlineData() {
}

void beamlineData::eliminate() {
 delete this;
}

void* beamlineData::clone() {
 void* p = new beamlineData( *this );
 return p;
}

// ??? REMOVE or REWRITE:  void beamlineData::writeTo( FILE* f ) {
// ??? REMOVE or REWRITE:   int sz = strlen( name );
// ??? REMOVE or REWRITE:   if( sz == 0 ) {
// ??? REMOVE or REWRITE:    printf( "\n" );
// ??? REMOVE or REWRITE:    printf( "*** ERROR ***                                        \n" );
// ??? REMOVE or REWRITE:    printf( "*** ERROR *** void beamlineData::writeTo( FILE*  )   \n" );
// ??? REMOVE or REWRITE:    printf( "*** ERROR *** Anomoly in ident.  Quitting.           \n" );
// ??? REMOVE or REWRITE:    printf( "*** ERROR ***                                        \n" );
// ??? REMOVE or REWRITE:    printf( "\n" );
// ??? REMOVE or REWRITE:    exit(0);
// ??? REMOVE or REWRITE:   }
// ??? REMOVE or REWRITE:  
// ??? REMOVE or REWRITE:   fwrite( this, sizeof( beamlineData ), 1, f );
// ??? REMOVE or REWRITE:   fwrite( &sz, sizeof( int ), 1, f );
// ??? REMOVE or REWRITE:   fprintf( f, "%s ", name );
// ??? REMOVE or REWRITE:  }



// **************************************************
//   class beamline::arrayRep
// **************************************************

beamline::arrayRep::arrayRep( const beamline* x, bool doClone )
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

  DeepBeamlineIterator dbi(x);
  bmlnElmnt* q;
  int i = 0;

  _n = x->countHowManyDeeply();
  _cloned = doClone;

  if ( 0 < _n ) {
    _element = new bmlnElmnt* [_n];
  
    if( _cloned ) {
      while( ( 0 != (q = dbi++) ) && ( i < _n ) ) {
        _element[i] = q->Clone();   // The second check, i < _n,
        i++;                        // is simply paranoia;
      }                             // it should not be necessary.
    }
    else {
      while( ( 0 != (q = dbi++) ) && ( i < _n ) ) {
        _element[i] = q;
        i++;
      }
    }
  }
}


beamline::arrayRep::~arrayRep()
{
  if ( _cloned ) {
    for( int i = 0; i < _n; i++ ) {
      delete _element[i];
    }
  }
  delete [] _element;
}



// **************************************************
//   class beamline
// **************************************************

beamline::beamline( const char* nm ) 
: bmlnElmnt( nm ), dlist() 
{
 _mode          = unknown;
 numElem        = 0;
 nominalEnergy  = NOTKNOWN;
 depth          = 0;
 twissDone      = 0;
} 


beamline::beamline( const beamline& a ) 
: bmlnElmnt( (const bmlnElmnt&) a ), dlist( (dlist&) a ) 
{
 _mode             = a._mode;
 nominalEnergy     = a.nominalEnergy;
 numElem           = a.numElem;
 twissDone         = 0;
} 

bmlnElmnt* beamline::Clone() const {
 beamline* ret = new beamline( ident );
 // ??? REMOVE beamline* ret = new beamline;

 // ??? REMOVE ret->ident         = new char [ strlen(ident) + 1 ];
 // ??? REMOVE                      strcpy( ret->ident, ident );
 ret->length        = length;
 ret->strength      = strength;
 ret->depth         = depth;

 ret->_mode         = _mode;
   //
   // O.K.
   //
   // ret->aperture      = aperture;

 ret->pAperture      = pAperture;
 
 ret->iToField      = iToField;
 ret->nominalEnergy = nominalEnergy;
 ret->twissDone     = 0;

 dlist_iterator getNext ( *(dlist*) this );
 bmlnElmnt* p;
 bmlnElmnt* q;
 while ((  p = (bmlnElmnt*) getNext()  )) {
  q = p->Clone();
  ret->append( q );
 }
 return (bmlnElmnt*) ret;
}



const char*  beamline::Type() const 
{ 
  return "beamline"; 
}


double beamline::OrbitLength( const Particle& x )
{
 dlist_iterator getNext ( *(dlist*) this );
 bmlnElmnt* p;
 double s = 0.0;
 while ((  p = (bmlnElmnt*) getNext()  )) {
  s += p->OrbitLength( x );
 }
 return s;
}


beamline::beamline( bmlnElmnt* q ) : bmlnElmnt(), dlist() 
{
 dlist::insert( q );

 _mode          = unknown;
 numElem        = 1;
 nominalEnergy  = NOTKNOWN;
 depth          = q->depth + 1;
}


beamline::beamline( char* n, bmlnElmnt* q ) : bmlnElmnt( n ), dlist() {
 dlist::insert( q );

 _mode          = unknown;
 numElem        = 1;
 nominalEnergy  = NOTKNOWN;
 depth          = q->depth + 1;

 twissDone      = 0;
}


// Destructors

beamline::~beamline() {
 // Notice that this destructor does not destroy
 // the beamline elements.  To do that, use
 // beamline::zap().
 bmlnElmnt* p;
 while((  p = (bmlnElmnt*) get()  ));
 this->dlist::clear();  // Wipes out all the links; probably unnecessary.
}

void beamline::zap() {
 bmlnElmnt* p;
 while((  p = (bmlnElmnt*) get()  )) {
   if( 0 == strcmp( p->Type(), "beamline" ) ) {
    ((beamline*) p)->zap();
   }
   delete p;  // ??? This will produce errors if *p is on the stack.
 }
 this->dlist::clear();  // Wipes out all the links.  Probably unnecessary.
 numElem = 0;
}

void beamline::eliminate() {
 zap();
 delete this;
}


void beamline::clear() {
  this->dlist::clear();
  numElem = 0;
}


void beamline::localPropagate( Particle& x ) {
 dlist_iterator getNext ( *(dlist*) this );
 bmlnElmnt* p;
 while ((  p = (bmlnElmnt*) getNext() )) {
   p->propagate( x );
 }
} 

void beamline::localPropagate( ParticleBunch& x ) {
 dlist_iterator getNext ( *(dlist*) this );
 bmlnElmnt* p;
 while ((  p = (bmlnElmnt*) getNext() )) {
   p -> propagate( x );
 }
} 

void beamline::localPropagate( JetParticle& x ) {
 dlist_iterator getNext ( *(dlist*) this );
 bmlnElmnt* p;
 while ((  p = (bmlnElmnt*) getNext()  )) {
   p->propagate( x );
 }
} 

void beamline::setEnergy( double E ) {
 nominalEnergy = E;
}

void beamline::unTwiss() {
 dlist_iterator getNext( *(dlist*) this );
 bmlnElmnt* p;
 dataHook.eraseFirst( "Ring" );
 if( !dataHook.eraseFirst( "Twiss" ) )
  while((  p = (bmlnElmnt*) getNext()  ))
   p->dataHook.eraseFirst( "Twiss" );
 twissDone = 0;   // ??? Remove this eventually.
}


void beamline::eraseBarnacles( const char* s )
{
  DeepBeamlineIterator dbi( this );
  bmlnElmnt* q;
  while((  q = dbi++  )) {
    q->dataHook.eraseAll( s );
  }
}


lattRing beamline::whatIsRing() {
  lattRing errRet;
  lattRing* ringPtr = (lattRing*)dataHook.find( "Ring" );
  if(ringPtr != 0)
    return *ringPtr;
    cout << endl;
    cout << "*** WARNING ***                            \n"
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
 dlist_iterator getNext( *(dlist*) this );
 bmlnElmnt* p;

 while((  p = (bmlnElmnt*) getNext()  )) 
  if( n == count++ ) 
   return (*(lattFunc*) p->dataHook.find( "Twiss" ));
 
 cout << endl;
 cout << "*** WARNING ***                               \n"
      << "*** WARNING *** beamline::whatIsLattice       \n"
      << "*** WARNING *** Entry was not found.          \n"
      << "*** WARNING *** Meaningless value being       \n"
      << "*** WARNING *** returned.                     \n"
      << "*** WARNING ***                               \n" << endl;
 return errRet;
}


lattFunc beamline::whatIsLattice( char* n ) {
  // Function written by Mike Martens.
  // May 24, 1996
  lattFunc errRet;
  
  dlist_iterator getNext( *(dlist*) this );
  bmlnElmnt* p;
  
  while((  p = (bmlnElmnt*) getNext()  )) 
    if( !strcmp(p->Name(),n) ) 
      return (*(lattFunc*) p->dataHook.find( "Twiss" ));
        return errRet;  
}

    // ++++++++++++ Begin: Insert and append functions ++++++++++++++++++

void beamline::insert( bmlnElmnt& q ) {
 dlist::insert( &q );
 if( q.depth + 1 > depth ) depth = q.depth + 1;

 if( twissDone ) unTwiss();

 length += q.length;

 if( strcmp( q.Type(), "beamline" ) == 0 )  
      numElem += ((beamline&) q).numElem;
 else numElem++;
} 


void beamline::insert( bmlnElmnt* q ) {
 dlist::insert( q );
 if( q->depth + 1 > depth ) depth = q->depth + 1;

 if( twissDone ) unTwiss();

 length += q -> length;

 if( strcmp( q->Type(), "beamline" ) == 0 )  
      numElem += ((beamline*) q)->numElem;
 else numElem++;
}  


void beamline::InsertElementsFromList( double& s, 
                                       InsertionList& inList,
                                       slist& removedElements 
                                     )
{
 dlist_iterator getNext( *(dlist*) this );
 bmlnElmnt* p_be   = (bmlnElmnt*) getNext();
 bmlnElmnt* p_be_a = 0;
 bmlnElmnt* p_be_b = 0;
 InsertionListElement* p_ile = inList(0);  // top element; not removed
 Proton prtn;
 bool   firstWarning = true;

 prtn = inList.GetParticle();

 if( !p_be ) {
  cerr << "\n"
          "*** WARNING ***                                     \n"
          "*** WARNING *** beamline::InsertElementsFromList( s_0, list ) \n"
          "*** WARNING *** The beamline is empty!              \n"
          "*** WARNING *** Nothing will be done.               \n"
          "*** WARNING ***                                     \n"
       << endl;
  return;
 }

 if( !p_ile ) {
  cerr << "\n"
          "*** WARNING ***                                     \n"
          "*** WARNING *** beamline::InsertElementsFromList( s_0, list ) \n"
          "*** WARNING *** The list is empty!                  \n"
          "*** WARNING *** Nothing will be done.               \n"
          "*** WARNING ***                                     \n"
       << endl;
  return;
 }


 while( p_be && p_ile ) {
  if( strcasecmp( p_be->Type(), "beamline" ) == 0 ) {
    ( (beamline*) p_be )->InsertElementsFromList( s, inList, removedElements );
    p_ile = inList(0);   // this may have changed
    p_be = (bmlnElmnt*) getNext();
  }

  else if ( s + p_be->OrbitLength( prtn ) <= p_ile->s ) {
    s += p_be->OrbitLength( prtn );
    p_be = (bmlnElmnt*) getNext();
  }

  else if ( s == p_ile->s ) {
    putAbove( *(p_be), *(p_ile->q) );
    inList.Get();      // removes top element
    p_ile = inList(0); // accesses new top element
  }

  // else if ( p_be->p_bml ) {
  //   p_be->p_bml->InsertElementsFromList( s, inList, removedElements );
  //   p_ile = inList(0);   // this may have changed
  //   p_be = (bmlnElmnt*) getNext();
  // }

  else if (  0 == strcmp( p_be->Type(), "combinedFunction" )  ) {
    p_be->p_bml->InsertElementsFromList( s, inList, removedElements );
    p_ile = inList(0);   // this may have changed
    p_be = (bmlnElmnt*) getNext();

    if( firstWarning ) {
      cerr << "\n*** WARNING:                                   *** "
              "\n*** WARNING: Insertion into a combinedFunction *** "
              "\n*** WARNING: element will hide what is being   *** "
              "\n*** WARNING: inserted.                         *** "
              "\n*** WARNING:                                   *** "
           << endl;
      firstWarning = false;
    }
  }

  else if ( ( s < p_ile->s ) && ( p_ile->s < s + p_be->OrbitLength( prtn ) ) ) {
    p_be->Split( ( p_ile->s - s )/p_be->OrbitLength( prtn ), 
                 &p_be_a, 
                 &p_be_b 
               );
    putAbove( *(p_be), *(p_be_a)   );
    putAbove( *(p_be), *(p_ile->q) );
    putAbove( *(p_be), *(p_be_b)   );

    getNext.GoBack();
    s += ( p_be_a->OrbitLength( prtn ) + p_ile->q->OrbitLength( prtn ) );

    if( ((void*) p_be) == this->lastInfoPtr() ) {
      dlist::remove( (void*) p_be );
      getNext.Terminate();
      // This is required because of the way 
      // dlist_iterator::operator() is written.
    }
    else {
      dlist::remove( (void*) p_be );
    }
    removedElements.append( (void*) p_be );

    p_be = p_be_b;
    inList.Get();      // removes top element
    p_ile = inList(0); // accesses new top element
  }

  else {
    cerr << "\n"
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

    cerr << "Here are the tests:\n";
    cerr << "else if ( s + p_be->OrbitLength( prtn ) <= p_ile->s )\n"
         << "else if ( " << setprecision(10) << ( s + p_be->OrbitLength( prtn ) )
         << " <= "       << setprecision(10) << ( p_ile->s )
         << " )\n";
    cerr << "else if ( s == p_ile->s )\n"
         << "else if ( " << setprecision(10) << s 
         << " == "       << setprecision(10) << p_ile->s 
         << " )\n";
    cerr << "else if ( p_be->p_bml )\n"
         << "else if ( " << p_be->p_bml << " )\n";
    cerr << "else if ( ( s < p_ile->s ) && ( p_ile->s < s + p_be->OrbitLength( prtn ) ) )\n"
         << "else if ( ( " << setprecision(10) << s 
         << " < "          << setprecision(10) << p_ile->s 
         << " ) && ( "     << setprecision(10) << p_ile->s 
         << " < "          << setprecision(10) << ( s + p_be->OrbitLength( prtn ) )
         << " ) )\n";

    s += p_be->OrbitLength( prtn );
    p_be = (bmlnElmnt*) getNext();
  }
 }

}

beamline& beamline::operator^( bmlnElmnt& x ) {
 append( x );
 return *this;
}

beamline& beamline::operator^( beamline& x ) {
 append( x );
 return *this;
}

beamline& operator^( bmlnElmnt& x, bmlnElmnt& y ) {
 beamline* result = new beamline;
 result->append( x );
 result->append( y );
 return *result;
}

beamline& operator-( beamline& x ) {
 const static char* rev = "REVERSE_";
 char* theName = new char[ strlen(rev) + strlen(x.Name()) + 1 ];
 theName[0] = '\0';
 strcat( theName, rev );
 strcat( theName, x.Name() );
 beamline* result = new beamline( theName );
 dlist_reverseIterator getNext( (dlist&) x );
 bmlnElmnt* p;
 while((  p = (bmlnElmnt*) getNext()  )) {
  if( strcasecmp( p->Type(), "beamline" ) == 0 ) 
                            result->append( - *(beamline*) p );
  else                      result->append( p );
 }
 delete [] theName;
 return *result;
}

beamline& beamline::operator+( bmlnElmnt& x ) {
 beamline* result = new beamline( *this );
 result->append( x );
 return *result;
}

beamline& beamline::operator+( beamline& x ) {
 beamline* result = new beamline( *this );
 result->append( x );
 return *result;
}

beamline& beamline::operator-( beamline& x ) {
 beamline* result = new beamline( *this );
 dlist_reverseIterator getNext( (dlist&) x );
 bmlnElmnt* p;
 while((  p = (bmlnElmnt*) getNext()  )) {
  if( strcasecmp( p->Type(), "beamline" ) == 0 ) 
                            result->append( - *(beamline*) p );
  else                      result->append( p );
 }
 return *result;
}

beamline& operator*( int c, beamline& x ) {
 beamline* result = new beamline;
 int i;
 if ( c == 0 ) return *result;
 if ( c < 0 )  c = - c;
 for ( i = 0; i < c; i++ ) result->append( x );
 return *result;
}

beamline& operator*( beamline& x, int c ) {
 beamline* result = new beamline;
 int i;
 if ( c == 0 ) return *result;
 if ( c < 0 )  c = - c;
 for ( i = 0; i < c; i++ ) result->append( x );
 return *result;
}

beamline& operator*( int c, bmlnElmnt& x ) {
 beamline* result = new beamline;
 int i;
 if ( c == 0 ) return *result;
 if ( c < 0 )  c = - c;
 for ( i = 0; i < c; i++ ) result->append( x );
 return *result;
}

beamline& operator*( bmlnElmnt& x, int c ) {
 beamline* result = new beamline;
 int i;
 if ( c == 0 ) return *result;
 if ( c < 0 )  c = - c;
 for ( i = 0; i < c; i++ ) result->append( x );
 return *result;
}


void beamline::append( bmlnElmnt& q ) {
 dlist::append( &q );
 if( q.depth + 1 > depth ) depth = q.depth + 1;

 if( twissDone ) unTwiss();

 length += q.length;
 if( strcmp( q.Type(), "beamline" ) == 0 )  
      numElem += ((beamline&) q).numElem;
 else numElem++;
} 


void beamline::append( bmlnElmnt* q ) {
 dlist::append( q );
 if( q->depth + 1 > depth ) depth = q->depth + 1;

 if( twissDone ) unTwiss();

 length += q->length;
 if( strcmp( q->Type(), "beamline" ) == 0 )  
      numElem += ((beamline*) q)->numElem;
 else numElem++;
} 


void beamline::Split( double, bmlnElmnt**, bmlnElmnt** ) const
{
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "void beamline::Split( double, bmlnElmnt**, bmlnElmnt** )", 
         "This method should not be invoked by a beamline object." ) );
}


char beamline::putAbove( const bmlnElmnt& x, const bmlnElmnt& y ) 
{
 // Insert y above (before; upstream of) x in the beamline

 unTwiss();

 if ( !dlist::putAbove( &x, &y ) ) {
   cerr << "\n*** ERROR ***                                             \n"
             "*** ERROR *** beamline::putAbove                          \n"
             "*** ERROR *** dlist::putAbove did not work.               \n"
             "*** ERROR ***                                             \n"
        << endl;
   return 1;
 }

 length += y.length;
 if( y.depth + 1 > depth ) depth = y.depth + 1;
 numElem++;

 return 0;
}


char beamline::putBelow( const bmlnElmnt& x, const bmlnElmnt& y ) 
{
 // Insert y below (after; downstream of) x in the beamline

 unTwiss();

 if ( !dlist::putBelow( &x, &y ) ) {
   cerr << "\n*** ERROR ***                                             \n"
             "*** ERROR *** beamline::putBelow                          \n"
             "*** ERROR *** dlist::putAbove did not work.               \n"
             "*** ERROR ***                                             \n"
        << endl;
   return 1;
 }

 length += y.length;
 if( y.depth + 1 > depth ) depth = y.depth + 1;
 numElem++;

 return 0;
}


    // ++++++++++++ End:   Insert and append functions ++++++++++++++++++


// REMOVE: beamline& beamline::operator=( const beamline& a ) {
// REMOVE:  dlist::operator=( (dlist&) a );
// REMOVE:  bmlnElmnt::operator=( (bmlnElmnt&) a );
// REMOVE:  nominalEnergy     = a.nominalEnergy;
// REMOVE:  numElem           = a.numElem;
// REMOVE:  twissDone         = 0;
// REMOVE:  return *this;
// REMOVE: } 

beamline* beamline::flatten() const {
 dlist_iterator  getNext( *(dlist*) this );
 dlist_iterator* getNew;
 beamline*       r;
 beamline*       s;
 bmlnElmnt*      p;
 bmlnElmnt*      q;

 r = new beamline;

 while ((  p = (bmlnElmnt*) getNext()  ))  {
   if( strcasecmp( p->Type(), "beamline" ) == 0 )
   { 
     s = ( (beamline*) p ) -> flatten();
     getNew = new dlist_iterator( *(dlist*) s );
     while ((  q = (bmlnElmnt*) (*getNew)()  )) {
       r->append( q );
     }
     delete getNew;
     delete s;
   }
   else
   { // Only beamlines are composite objects
     r->append( p );
   }
 }

 r->setEnergy( this->Energy() );
 r->Rename( this->Name() );
 r->setLineMode( _mode );
 return r;
} 


int beamline::startAt( const bmlnElmnt* x, int n )
{
  return this->dlist::startAt( (void*) x, n );
}


int beamline::startAt( const char* s, int n )
{
  dlist_traversor getNext( *(dlist*) this );
  dlink* q;
  int count(0);

  while((  q = getNext()  )) 
  {
    if( 0 == strcmp( ((bmlnElmnt*)(q->info()))->Name(), s ) ) 
    {
      if( (++count) == n ) {
        this->dlist::riskStartAt( q );
        return 0;
      }
    }
  }

  return 1;
}



sector* beamline::MakeSector ( const bmlnElmnt& be_1, const bmlnElmnt& be_2, 
                               int deg, JetParticle& jp ) {

 // This assumes that the argument jp has been initialized as
 // desired by the calling program.  This routine does NOT
 // initialize the state of jp.

 // REMOVE: dlist_iterator getNext( *(dlist*) this );
 DeepBeamlineIterator dbi( this );
 bmlnElmnt* p_be;
 char       firstFound  = 0;
 char       secondFound = 0;
 Particle*  p_prt;
 double     s           = 0.0;

 p_prt = jp.ConvertToParticle();

 // REMOVE: while ( p_be = (bmlnElmnt*) getNext() ) {
 while ((  p_be = dbi++  )) {

  if( p_be == &be_2 ) {
   cout << "*** WARNING ***                                      \n" 
        << "*** WARNING *** beamline::MakeSector                 \n" 
        << "*** WARNING *** Second element found first.          \n" 
        << "*** WARNING *** Returning zero.                      \n" 
        << "*** WARNING ***                                      \n" 
        << endl;
   delete p_prt;
   return 0;
  }

  else if( p_be == &be_1 ) { 
    firstFound = 1;
    break;
  }
 }

 if( !firstFound ) {
  cout << "*** WARNING ***                                      \n" 
       << "*** WARNING *** beamline::MakeSector                 \n" 
       << "*** WARNING *** Unable to find first element.        \n" 
       << "*** WARNING *** Returning zero.                      \n" 
       << "*** WARNING ***                                      \n" 
       << endl;
  delete p_prt;
  return 0;
 }
 
 // REMOVE: while ( p_be = (bmlnElmnt*) getNext() ) {
 while ((  p_be = dbi++  )) {  // Notice: we do not propagate through
  if( p_be == &be_2 ) {        // be_1 and be_2
    secondFound = 1;
    break;
  }
  else {
    p_be->propagate( jp );
    s += p_be->OrbitLength( *p_prt );
  }
 }

 if( !secondFound ) {
  cout << "*** WARNING ***                                      \n" 
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


sector* beamline::MakeSectorFromStart ( const bmlnElmnt& be_1, int deg, JetParticle& jp ) {

 // This assumes that the argument jp has been initialized as
 // desired by the calling program.  This routine does NOT
 // initialize the state of jp.

 dlist_iterator getNext( *(dlist*) this );
 bmlnElmnt* p_be;
 char       firstFound  = 0;
 Particle*  p_prt;
 double     s           = 0.0;

 p_prt = jp.ConvertToParticle();

 // Check first element against the argument ------------
 p_be = (bmlnElmnt*) getNext();

 if( !p_be ) {
  cout << "*** WARNING ***                                      \n" 
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

 // Propagate jetProton through first element -----------
 p_be->propagate( jp );
 s += p_be->OrbitLength( *p_prt );


 // Find element that matches argument ------------------
 while ( p_be = (bmlnElmnt*) getNext() ) {
  if( p_be == &be_1 ) {     // Notice: we do not propagate through be_1
    firstFound = 1;
    break;
  }
  else {
    p_be->propagate( jp );
    s += p_be->OrbitLength( *p_prt );
  }
 }

 if( !firstFound ) {
  cout << "*** WARNING ***                                      \n" 
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


sector* beamline::MakeSectorToEnd ( const bmlnElmnt& be_1, int deg, JetParticle& jp ) {

 // This assumes that the argument jp has been initialized as
 // desired by the calling program.  This routine does NOT
 // initialize the state of jp.

 dlist_iterator getNext( *(dlist*) this );
 bmlnElmnt* p_be;
 char       firstFound  = 0;
 Particle*  p_prt;
 double     s           = 0.0;

 p_prt = jp.ConvertToParticle();

 // Find the element that matches argument ---------------------------------
 while ( p_be = (bmlnElmnt*) getNext() ) {
  if( p_be == &be_1 ) { 
    firstFound = 1;
    break;
  }
 }

 if( !firstFound ) {
  cout << "*** WARNING ***                                      \n" 
       << "*** WARNING *** beamline::MakeSectorToEnd            \n" 
       << "*** WARNING *** Unable to find first element.        \n" 
       << "*** WARNING *** Returning zero.                      \n" 
       << "*** WARNING ***                                      \n" 
       << endl;
  delete p_prt;
  return 0;
 }
 
 // Check that it is not the last element --------------------------------
 p_be = (bmlnElmnt*) getNext();
 if( !p_be ) {
    delete p_prt;
    return 0;
 }
 else {
    p_be->propagate( jp );
    s += p_be->OrbitLength( *p_prt );
 }

 // Construct the map and return sector ------------------------------------
 while ( p_be = (bmlnElmnt*) getNext() ) {  // Notice: we do not propagate through be_1
    p_be->propagate( jp );
    s += p_be->OrbitLength( *p_prt );
 }

 delete p_prt;
 return new sector( jp.State().filter( 0, deg ), s );
}


sector* beamline::makeSector( int degree, JetParticle& pd ) {
 double zero[ BMLN_dynDim ];
 Jet* zd;
 int i;

 for( i = 0; i < BMLN_dynDim; i++ ) zero[i] = 0.0;

 zd = new Jet [BMLN_dynDim];
 pd.setState ( zero );
 propagate   ( pd );
 pd.getState ( zd );

 for( i = 0; i < BMLN_dynDim; i++ )
   zd[i] = zd[i].filter( 0, degree );

 sector* s       = new sector ( ident, zd, length );
 // ??? REMOVE sector* s       = new sector ( zd, length );
 // ??? REMOVE strcpy( s->ident, ident );
 s->length       = length;
 s->strength     = strength;
 s->depth        = 0;
 s->align        = align;
   //
   // O.K.
   //
   // s->aperture     = aperture;
 
 s->pAperture     = pAperture;

 delete [] zd;
 return s;
}


void beamline::sectorize( int degree, JetParticle& pd ) {
 sector* s;
 s = makeSector( degree, pd );
 this->dlist::clear();
 unTwiss();
 numElem      = 0;
 length       = 0.0;
 depth        = 0;
 strength     = 0.0;
 align        = 0;
 append( *s );
}


beamline beamline::sectorize( bmlnElmnt& x, bmlnElmnt& y, int degree,
                              JetParticle& pd, const char* sectorName ) {
 beamline a;
 sector* s;
 if( &x == &y ) return a;
 a = remove( x, y );
 s = a.makeSector( degree, pd );
 s->Rename( sectorName );
 putAbove( y, *s );
 return a;
}



void beamline::peekAt( double& s, Particle* p_prt ) {
 char newProton = 0;
 if( !p_prt ) {
    p_prt = new Proton( nominalEnergy );
    newProton = 1;
 }

 dlist_iterator getNext( *(dlist*) this );
 bmlnElmnt* p;

 cout << "\nBegin beamline::peekat() -- Address of beamline: "
      << ident << " = " << (int) this 
      << endl;

 while ((  p = (bmlnElmnt*) getNext()  ))  {
   if( strcasecmp( p->Type(), "beamline" ) == 0 ) 
     ( (beamline*) p)->peekAt( s, p_prt );
   else p->peekAt( s, p_prt );
 }

 cout << "End beamline::peekat() -- Address of beamline: "
      << ident << " = " << (int) this 
      << endl;

 if( newProton ) delete p_prt;
}


int beamline::countHowMany( CRITFUNC query, slist* listPtr ) const {
 int ret;
 dlist_iterator getNext ( *(dlist*) this );
 bmlnElmnt* p;

 ret = 0;

 if( query == 0 ) {
   while ((  p = (bmlnElmnt*) getNext()  )) { 
     ret++; 
   }
   if( ret != numElem ) {
     cerr << "\n*** WARNING ***                                     \n"
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
   while ((  p = (bmlnElmnt*) getNext()  )) {
     if( query(p) ) { 
       ret++; 
       if( listPtr ) {
         listPtr->append(p);
       }
     }
   }
 }

 return ret;
}

int beamline::countHowManyDeeply( CRITFUNC query, slist* listPtr ) const {
 int ret;
 dlist_iterator getNext ( *(dlist*) this );
 bmlnElmnt* p;

 ret = 0;
 while ((  p = (bmlnElmnt*) getNext()  )) {
   if( 0 == strcmp( p->Type(), "beamline" ) ) {
     ret += ((beamline*) p)->countHowManyDeeply( query, listPtr );
   }
   else {
     if( ( query == 0 ) || ( query(p) ) ) {
       ret++;
       if( listPtr ) {
         listPtr->append(p);
       }
     }
   }
 }

 return ret;
}

int beamline::howDeep() {
 return depth;
}


int beamline::contains( const bmlnElmnt* x ) const
{
  DeepBeamlineIterator dbi( this );
  int ret = 0;
  bmlnElmnt* p_be;
  while((  p_be = dbi++  )) {
    if( p_be == x ) { ret++; }
  }
  return ret;
}


beamline beamline::remove( const bmlnElmnt& x, const bmlnElmnt& y ) {
 beamline a;
 dlist dl;
 int i;
 bmlnElmnt* p;

 unTwiss();
 dl = dlist::remove( ((void*) &x), ((void*) &y) );

 dlist_iterator getNext( dl );
 if((  p = (bmlnElmnt*) getNext()  )) {
   a.append( *p );
 }
 while ((  p = (bmlnElmnt*) getNext()  ))  a.append( *p );

 a.nominalEnergy = nominalEnergy;
 numElem = numElem - a.numElem; // ??? What about depth?
 return a;
}


bool beamline::find( bmlnElmnt*& u, const bmlnElmnt*& v, bmlnElmnt*& w ) const
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
  DeepBeamlineIterator dbi(this);
  bmlnElmnt* q;
    
  // Check for possibility that *v is the first element.
  if( v == this->firstElement() ) {
    if( beamline::ring == this->getLineMode() ) {
      u = this->lastElement();
    }
    else {
      u = 0;
    }
    w = dbi++;
    return true;
  }

  // Continuing ...
  q = dbi++;
  u = q;
  q = dbi++;

  while( q ) {
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
        w = dbi++;
      }
      return true;
    }

    u = q;
    q = dbi++;
  }

  // Target element was never found.
  return false;
}


BmlPtrList beamline::moveRelX(   const bmlnElmnt* thePtr
                               , double u
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


BmlPtrList beamline::moveRelY(   const bmlnElmnt* thePtr
                               , double u
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


BmlPtrList beamline::moveRelZ(   const bmlnElmnt* thePtr
                               , double u
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


void beamline::_moveRel(   int axis, double u
                         , const bmlnElmnt* thePtr
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
  //                           3, |u| < 1 micron displacement
  // 
  //             NOTE WELL: if the returned list is not empty, it means
  //               that Slots have been created and installed in the line
  //               for which the calling program must take responsibility.
  //               This could be a serious problem. We need smart pointers!
  // 

  *errorCodePtr = 0;

  // Argument filter
  if( 0 == thePtr || 0 == errorCodePtr || 0 == recycleBinPtr ) { 
    *errorCodePtr = 1; 
    return;
  }
  if( axis < 0 || 2 < axis ) {
    *errorCodePtr = 2;
    return;
  }

  // Will not displace anything less than 1 micron.
  if( std::abs(u) < 1.0e-6 ) { 
    cerr << "\n*** ERROR *** "
         << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** Called by " << invoker
         << "\n*** ERROR *** Unable to perform operation on "
         << thePtr->Type() << "  " << thePtr->Name() << "."
         << "\n*** ERROR *** Requested displacement, " 
         << (1.e6*u) 
         << ", microns is too small."
            "\n*** ERROR *** Must be at least 1 micron."
            "\n*** ERROR *** "
         << endl;
    *errorCodePtr = 3;
    return;
  }


  // Continue program ...
  bmlnElmnt* upStreamPtr   = 0;
  bmlnElmnt* downStreamPtr = 0;

  FramePusher fp;
  Frame frameZero, frameOne, frameTwo, frameThree;

  if( this->find( upStreamPtr, thePtr, downStreamPtr ) ) {

    if( (0 != upStreamPtr) && (0 != downStreamPtr ) ) {
      upStreamPtr->accept( fp );
      frameOne = fp.getFrame();
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();

      Vector displacement(u*frameOne.getAxis(axis));
      frameOne.translate( displacement );
      frameTwo.translate( displacement );

      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(upStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(upStreamPtr)->setOutFrame( frameOne );
      }
      if( 0 == strcmp("Slot", downStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(downStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(downStreamPtr)->setOutFrame( frameThree.relativeTo(frameTwo) );
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(upStreamPtr->Name(), frameOne );
        this->putAbove( *thePtr, *slotPtr );
        this->remove( *upStreamPtr );
        recycleBinPtr->append( upStreamPtr );
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) );
        this->putBelow( *thePtr, *slotPtr );
        this->remove( downStreamPtr );
        recycleBinPtr->append( downStreamPtr );
      }
    }

    else if( (0 == upStreamPtr) && (0 != downStreamPtr ) ) {
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();

      Vector displacement(u*frameTwo.getAxis(axis));
      frameTwo.translate( displacement );

      cerr << "\n*** WARNING *** "
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
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) );
        this->putBelow( *thePtr, *slotPtr );
        this->remove( downStreamPtr );
        recycleBinPtr->append( downStreamPtr );
      }
    }

    else if( (0 != upStreamPtr) && (0 == downStreamPtr ) ) {
      upStreamPtr->accept( fp );
      frameOne = fp.getFrame();
      thePtr->accept( fp );
      frameTwo = fp.getFrame();

      Vector displacement(u*frameOne.getAxis(axis));
      frameOne.translate( displacement );

      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(upStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(upStreamPtr)->setOutFrame( frameOne );
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(upStreamPtr->Name(), frameOne );
        this->putAbove( *thePtr, *slotPtr );
        this->remove( upStreamPtr );
        recycleBinPtr->append( upStreamPtr );
      }
    }

    else if( (0 == upStreamPtr) && (0 == downStreamPtr ) ) {
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void beamline::_moveRel( int axis, double u, ... )",
             "An impossibility has occurred. Am stopping." ) );
    }
  }

  return;
}


BmlPtrList beamline::pitch(   const bmlnElmnt* thePtr
                            , double angle
                            , double pct
                            , int* errorCodePtr )
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


BmlPtrList beamline::yaw(   const bmlnElmnt* thePtr
                          , double angle
                          , double pct
                          , int* errorCodePtr )
{
  BmlPtrList recycleBin;
  int dummyErr;
  if( 0 == errorCodePtr ) {
    this->_rotateRel( Frame::yAxisIndex(), angle, thePtr, pct, 
                      &dummyErr, &recycleBin, 
                      string("beamline::pitch") );
  }
  else {
    this->_rotateRel( Frame::yAxisIndex(), angle, thePtr, pct, 
                      errorCodePtr, &recycleBin, 
                      string("beamline::pitch") );
  }
  return recycleBin;
}


BmlPtrList beamline::roll(   const bmlnElmnt* thePtr
                           , double angle
                           , double pct
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


void beamline::_rotateRel(   int axis, double angle
                           , const bmlnElmnt* thePtr
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
  //                           2, |angle| < 1 microradian displacement
  //                           3, *thePtr is a sector element
  // 
  //             NOTE WELL: if the returned list is not empty, it means
  //               that Slots have been created and installed in the line
  //               for which the calling program must take responsibility.
  //               This could be a serious problem. We need smart pointers!
  // 

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

  // Will not rotate anything less than 1 microradian.
  if( std::abs(angle) < 1.0e-6 ) { 
    cerr << "\n*** ERROR *** "
         << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** Called by " << invoker
         << "\n*** ERROR *** Unable to perform operation on "
         << thePtr->Type() << "  " << thePtr->Name() << "."
         << "\n*** ERROR *** Requested rotation angle, " 
         << (1.e6*angle) 
         << ", microradians is too small."
            "\n*** ERROR *** Must be at least 1 microradian."
            "\n*** ERROR *** "
         << endl;
    *errPtr = 2;
    return;
  }

  // Check for a valid element
  if( typeid(*thePtr) == typeid(sector) ) {
    cerr << "\n*** ERROR *** "
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

  FramePusher fp;
  Frame frameZero, frameOne, frameTwo, frameThree;

  if( this->find( upStreamPtr, thePtr, downStreamPtr ) ) {

    if( (0 != upStreamPtr) && (0 != downStreamPtr ) ) {
      upStreamPtr->accept( fp );
      frameOne = fp.getFrame();
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();


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

      // Reset upstream and downstream elements
      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(upStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(upStreamPtr)->setOutFrame( frameOne );
      }
      if( 0 == strcmp("Slot", downStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(downStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(downStreamPtr)->setOutFrame( frameThree.relativeTo(frameTwo) );
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(upStreamPtr->Name(), frameOne );
        this->putAbove( *thePtr, *slotPtr );
        this->remove( *upStreamPtr );
        recycleBinPtr->append( upStreamPtr );
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) );
        this->putBelow( *thePtr, *slotPtr );
        this->remove( downStreamPtr );
        recycleBinPtr->append( downStreamPtr );
      }
    }

    else if( (0 == upStreamPtr) && (0 != downStreamPtr ) ) {
      frameOne = frameZero;
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      downStreamPtr->accept( fp );
      frameThree = fp.getFrame();


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


      // Reset upstream and downstream elements
      // Note: this is done inefficiently
      // DANGEROUS!!  Creates free object
      Slot* slotPtr = new Slot(frameOne );
      this->putAbove( *thePtr, *slotPtr );
      if( 0 == strcmp("Slot", downStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(downStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(downStreamPtr)->setOutFrame( frameThree.relativeTo(frameTwo) );
      }
      if( 0 == strcmp("drift", downStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) );
        this->putBelow( *thePtr, *slotPtr );
        this->remove( downStreamPtr );
        recycleBinPtr->append( downStreamPtr );
      }
    }

    else if( (0 != upStreamPtr) && (0 == downStreamPtr ) ) {
      upStreamPtr->accept( fp );
      frameOne = fp.getFrame();
      thePtr->accept( fp );
      frameTwo = fp.getFrame();
      frameThree = frameTwo;


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


      // Reset upstream and downstream elements
      // Note: this is done inefficiently
      if( 0 == strcmp("Slot", upStreamPtr->Type() ) ) {
        dynamic_cast<Slot*>(upStreamPtr)->setInFrame( frameZero );
        dynamic_cast<Slot*>(upStreamPtr)->setOutFrame( frameOne );
      }
      if( 0 == strcmp("drift", upStreamPtr->Type() ) ) {
        // DANGEROUS!!  Creates free object
        Slot* slotPtr = new Slot(upStreamPtr->Name(), frameOne );
        this->putAbove( *thePtr, *slotPtr );
        this->remove( *upStreamPtr );
        recycleBinPtr->append( upStreamPtr );
      }
      // DANGEROUS!!  Creates free object
      Slot* slotPtr = new Slot( frameThree.relativeTo(frameTwo) );
      this->putBelow( *thePtr, *slotPtr );
    }

    else if( (0 == upStreamPtr) && (0 == downStreamPtr ) ) {
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void beamline::_moveRel( int axis, double u, ... )",
             "An impossibility has occurred. Am stopping." ) );
    }
  }

  return;
}


bool beamline::setAlignment( const alignmentData& al ) {
  // Propogate alignment data of entire  beamline to each individual element
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  while ((  p = (bmlnElmnt*) getNext()  )) {
    if( !(p->setAlignment(al)) ) {
      cerr << "\n*** ERROR *** "
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

char beamline::remove( void * Element2remove){
  if(numElem > 0) numElem--;
  return !dlist::remove( Element2remove );
}

char beamline::remove( const bmlnElmnt& x ){
  if(numElem > 0) numElem--;
  return !dlist::remove( (void*) &x );
}

char beamline::remove( const bmlnElmnt* x ) {
  if(numElem > 0) numElem--;
  return !dlist::remove( (void*) x );
}

ostream& beamline::writeTo(ostream& os) {
  dlist_iterator getNext(*this);
  bmlnElmnt* element;
  double energy = Energy();

  os <<  OSTREAM_DOUBLE_PREC << energy << endl;
  while((element = (bmlnElmnt*)getNext()) != 0) {
    os << *element ;
  }
  os << "beamline_END " << Name() << " 0 0 0 0 0\n";
  return os;
}

// REMOVE: istream& beamline::readFrom(istream& is)
// REMOVE: {
// REMOVE:   // Written by Elliott McCrory
// REMOVE:   bmlnElmnt *e = NULL;
// REMOVE:   is >> nominalEnergy;
// REMOVE:   // cerr << "Beamline " << Name() << " has energy of " << nominalEnergy << "\n";
// REMOVE:   // Now, continue reading is until we see the end of this beamline
// REMOVE:   length = 0;
// REMOVE:   do {
// REMOVE:     e = read_istream(is);        // (Recursion)
// REMOVE:     // read_istream will return NULL when end of file or end of beamline is reached.
// REMOVE:     if ( e ) 
// REMOVE:       append(e);
// REMOVE:   } while ( e );
// REMOVE: 
// REMOVE:   return is;
// REMOVE: }

// **************************************************
//   Frame functions
// **************************************************

void beamline::enterLocalFrame( Particle& p ) const
{
  // Check for bends
  dlist_iterator getNext( *(dlist*) this );
  static bmlnElmnt* element;

  while((  element = (bmlnElmnt*) getNext() )) 
  {
    if( strcasecmp( element->Type(), "sbend" ) == 0 ||
        strcasecmp( element->Type(), "rbend" ) == 0     
      ) {
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void beamline::enterLocalFrame( Particle& p ) const", 
             "Not implemented for beamlines containing bends." ) );
    }
  }
  
  bmlnElmnt::enterLocalFrame( p );
}

void beamline::enterLocalFrame( JetParticle& p ) const
{
  // Check for bends
  dlist_iterator getNext( *(dlist*) this );
  static bmlnElmnt* element;

  while((  element = (bmlnElmnt*) getNext() )) 
  {
    if( strcasecmp( element->Type(), "sbend" ) == 0 ||
        strcasecmp( element->Type(), "rbend" ) == 0     
      ) {
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void beamline::enterLocalFrame( JetParticle& p ) const", 
             "Not implemented for beamlines containing bends." ) );
    }
  }
  
  bmlnElmnt::enterLocalFrame( p );
}


void beamline::leaveLocalFrame( Particle& p ) const
{
  bmlnElmnt::leaveLocalFrame( p );
}

void beamline::leaveLocalFrame( JetParticle& p ) const
{
  bmlnElmnt::leaveLocalFrame( p );
}




bool beamline::Criterion::operator()( const bmlnElmnt* x )
{
  return false;
}


bool beamline::Criterion::operator()( const bmlnElmnt& x )
{
  return operator()( &x );
}


int beamline::Action::operator()( bmlnElmnt* x )
{
  return -1;
}


int beamline::Action::operator()( bmlnElmnt& x )
{
  return operator()( &x );
}


// OBSOLETE: REMOVE: bool beamline::moveRelZ( const bmlnElmnt* thePtr, double u /*[m]*/)
// OBSOLETE: REMOVE: {
// OBSOLETE: REMOVE:   // Upon entry: thePtr      = pointer to element to be translated
// OBSOLETE: REMOVE:   //                           longitudinally
// OBSOLETE: REMOVE:   //             u      [m]  = displacement
// OBSOLETE: REMOVE:   // 
// OBSOLETE: REMOVE:   // Upon exit:  *thePtr will have been displaced in the direction
// OBSOLETE: REMOVE:   //               of its local z coordinate by adjusting its neighboring
// OBSOLETE: REMOVE:   //               free-space elements. 
// OBSOLETE: REMOVE:   // 
// OBSOLETE: REMOVE:   //             Value returned: true,  operation successful
// OBSOLETE: REMOVE:   //                             false, operation unsuccessful
// OBSOLETE: REMOVE:   // 
// OBSOLETE: REMOVE:   // Comments:   This routine alters attributes of the neighboring 
// OBSOLETE: REMOVE:   //               free-space elements but does not change their identities.
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   bmlnElmnt* upStreamPtr = 0;
// OBSOLETE: REMOVE:   bmlnElmnt* downStreamPtr = 0;
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   bool ret = this->find( upStreamPtr, thePtr, downStreamPtr ); 
// OBSOLETE: REMOVE:   if( ret ) {
// OBSOLETE: REMOVE:     // Test neighbors for free space.
// OBSOLETE: REMOVE:     if( 0 == upStreamPtr || 0 == downStreamPtr ) {
// OBSOLETE: REMOVE:       cerr << "\n*** WARNING *** "
// OBSOLETE: REMOVE:            << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:            << "\n*** WARNING *** bool beamline::moveRelZ( const bmlnElmnt* thePtr, double u /*[m]*/)
// OBSOLETE: REMOVE:               "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:            << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:            << "\n*** ERROR *** Not bounded by neighboring elements.
// OBSOLETE: REMOVE:               "\n*** WARNING *** "
// OBSOLETE: REMOVE:            << endl;
// OBSOLETE: REMOVE:       return false;
// OBSOLETE: REMOVE:     }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:     bool upStreamDrift   = ( 0 == strcmp("drift",   upStreamPtr->Type()) );
// OBSOLETE: REMOVE:     bool downStreamDrift = ( 0 == strcmp("drift", downStreamPtr->Type()) );
// OBSOLETE: REMOVE:     bool upStreamSlot    = ( 0 == strcmp("Slot",    upStreamPtr->Type()) );
// OBSOLETE: REMOVE:     bool downStreamSlot  = ( 0 == strcmp("Slot",  downStreamPtr->Type()) );
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:     if( upStreamSlot ) {
// OBSOLETE: REMOVE:       if( !(upStreamPtr->isSimple()) ) {
// OBSOLETE: REMOVE:         cerr << "\n*** WARNING *** "
// OBSOLETE: REMOVE:              << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:              << "\n*** WARNING *** bool beamline::moveRelZ( const bmlnElmnt* thePtr, double u /*[m]*/)
// OBSOLETE: REMOVE:                 "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:              << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:              << "\n*** ERROR *** Upstream neighbor is not free space.
// OBSOLETE: REMOVE:                 "\n*** WARNING *** "
// OBSOLETE: REMOVE:              << endl;
// OBSOLETE: REMOVE:         return false;
// OBSOLETE: REMOVE:       }
// OBSOLETE: REMOVE:     if( downStreamSlot ) {
// OBSOLETE: REMOVE:       if( !(downStreamPtr->isSimple()) ) {
// OBSOLETE: REMOVE:         cerr << "\n*** WARNING *** "
// OBSOLETE: REMOVE:              << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:              << "\n*** WARNING *** bool beamline::moveRelZ( const bmlnElmnt* thePtr, double u /*[m]*/)
// OBSOLETE: REMOVE:                 "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:              << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:              << "\n*** ERROR *** Downstream neighbor is not free space.
// OBSOLETE: REMOVE:                 "\n*** WARNING *** "
// OBSOLETE: REMOVE:              << endl;
// OBSOLETE: REMOVE:         return false;
// OBSOLETE: REMOVE:       }
// OBSOLETE: REMOVE:     }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:     // ??? Too many "if" statements. Do we need another base class?
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:     if( upStreamDrift && downStreamDrift ) {
// OBSOLETE: REMOVE:       ret = beamline::moveRelZ( dynamic_cast<drift*>(upStreamPtr),
// OBSOLETE: REMOVE:                                 thePtr,
// OBSOLETE: REMOVE:                                 dynamic_cast<drift*>(downStreamPtr), 
// OBSOLETE: REMOVE:                                 u );
// OBSOLETE: REMOVE:     }
// OBSOLETE: REMOVE:     else if( upStreamDrift && downStreamSlot ) {
// OBSOLETE: REMOVE:       ret = beamline::moveRelZ( dynamic_cast<drift*>(upStreamPtr),
// OBSOLETE: REMOVE:                                 thePtr,
// OBSOLETE: REMOVE:                                 dynamic_cast<Slot*>(downStreamPtr), 
// OBSOLETE: REMOVE:                                 u );
// OBSOLETE: REMOVE:     }
// OBSOLETE: REMOVE:     else if( upStreamSlot && downStreamDrift ) {
// OBSOLETE: REMOVE:       ret = beamline::moveRelZ( dynamic_cast<Slot*>(upStreamPtr),
// OBSOLETE: REMOVE:                                 thePtr,
// OBSOLETE: REMOVE:                                 dynamic_cast<drift*>(downStreamPtr), 
// OBSOLETE: REMOVE:                                 u );
// OBSOLETE: REMOVE:     }
// OBSOLETE: REMOVE:     else if( upStreamSlot && downStreamSlot ) {
// OBSOLETE: REMOVE:       ret = beamline::moveRelZ( dynamic_cast<Slot*>(upStreamPtr),
// OBSOLETE: REMOVE:                                 thePtr,
// OBSOLETE: REMOVE:                                 dynamic_cast<Slot*>(downStreamPtr), 
// OBSOLETE: REMOVE:                                 u );
// OBSOLETE: REMOVE:     }
// OBSOLETE: REMOVE:     else {
// OBSOLETE: REMOVE:       cerr << "\n*** WARNING *** "
// OBSOLETE: REMOVE:            << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:            << "\n*** WARNING *** bool beamline::moveRelZ( const bmlnElmnt* thePtr, double u /*[m]*/)
// OBSOLETE: REMOVE:               "\n*** WARNING *** Unable to perform operation on "
// OBSOLETE: REMOVE:            << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:            << "\n*** WARNING *** Does not possess two free-space neighbors."
// OBSOLETE: REMOVE:               "\n*** WARNING *** "
// OBSOLETE: REMOVE:            << endl;
// OBSOLETE: REMOVE:       ret = false;
// OBSOLETE: REMOVE:     }
// OBSOLETE: REMOVE:   }
// OBSOLETE: REMOVE:   
// OBSOLETE: REMOVE:   return ret;
// OBSOLETE: REMOVE: }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE: bool beamline::moveRelZ( drift* usPtr, const bmlnElmnt* thePtr, drift* dsPtr, double u )
// OBSOLETE: REMOVE: {
// OBSOLETE: REMOVE:   // Will not displace anything less than 1 micron.
// OBSOLETE: REMOVE:   if( std::abs(u) < 1.0e-6 ) { 
// OBSOLETE: REMOVE:     cerr << "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:          << "\n*** ERROR *** bool beamline::moveRelZ( drift* usPtr, bmlnElmnt* thePtr, drift* dsPtr, double u )"
// OBSOLETE: REMOVE:             "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:          << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:          << "\n*** ERROR *** Displacement of " << (1.e6*u) << " microns is too small."
// OBSOLETE: REMOVE:             "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << endl;
// OBSOLETE: REMOVE:     return false; 
// OBSOLETE: REMOVE:   }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   double usLength = usPtr->Length() + u;
// OBSOLETE: REMOVE:   double dsLength = dsPtr->Length() - u;
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Check for error
// OBSOLETE: REMOVE:   if( (usLength < 1.0e-7) || (dsLength < 1.0e-7) ) {
// OBSOLETE: REMOVE:     cerr << "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:          << "\n*** ERROR *** bool beamline::moveRelZ( drift* usPtr, bmlnElmnt* thePtr, drift* dsPtr, double u )"
// OBSOLETE: REMOVE:             "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:          << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:          << "\n*** ERROR *** Longitudinal displacement too large.
// OBSOLETE: REMOVE:             "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << endl;
// OBSOLETE: REMOVE:     return false;
// OBSOLETE: REMOVE:   }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Perform the movement
// OBSOLETE: REMOVE:   usPtr->setLength( usLength );
// OBSOLETE: REMOVE:   dsPtr->setLength( dsLength );
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   return true;
// OBSOLETE: REMOVE: }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE: bool beamline::moveRelZ( Slot* usPtr, const bmlnElmnt* thePtr, drift* dsPtr, double u )
// OBSOLETE: REMOVE: {
// OBSOLETE: REMOVE:   // Assumes orthonormal frames and in Frame is the identity.
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Will not displace anything less than 1 micron.
// OBSOLETE: REMOVE:   if( std::abs(u) < 1.0e-6 ) { 
// OBSOLETE: REMOVE:     cerr << "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:          << "\n*** ERROR *** bool beamline::moveRelZ( drift* usPtr, bmlnElmnt* thePtr, drift* dsPtr, double u )"
// OBSOLETE: REMOVE:             "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:          << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:          << "\n*** ERROR *** Displacement of " << (1.e6*u) << " microns is too small."
// OBSOLETE: REMOVE:             "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << endl;
// OBSOLETE: REMOVE:     return false; 
// OBSOLETE: REMOVE:   }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   Frame usOutFrame( usPtr->getOutFrame() );
// OBSOLETE: REMOVE:   usOutFrame.translate( u*usOutFrame.getZAxis() );
// OBSOLETE: REMOVE:   double usZ = (usOutFrame.getOrigin())(usOutFrame.zAxisIndex());
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   double dsLength = dsPtr->Length() - u;
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Check for error
// OBSOLETE: REMOVE:   if( (usZ < 1.0e-7) || (dsLength < 1.0e-7) ) {
// OBSOLETE: REMOVE:     cerr << "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:          << "\n*** ERROR *** bool beamline::moveRelZ( Slot* usPtr, bmlnElmnt* thePtr, drift* dsPtr, double u )"
// OBSOLETE: REMOVE:             "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:          << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:          << "\n*** ERROR *** Longitudinal displacement too large.
// OBSOLETE: REMOVE:             "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << endl;
// OBSOLETE: REMOVE:     return false;
// OBSOLETE: REMOVE:   }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Perform the movement
// OBSOLETE: REMOVE:   usPtr->setOutFrame( usOutFrame );
// OBSOLETE: REMOVE:   dsPtr->setLength( dsLength );
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   return true;
// OBSOLETE: REMOVE: }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE: bool beamline::moveRelZ( drift* usPtr, const bmlnElmnt* thePtr, Slot* dsPtr, double u )
// OBSOLETE: REMOVE: {
// OBSOLETE: REMOVE:   // Assumes orthonormal frames and in Frame is the identity.
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Will not displace anything less than 1 micron.
// OBSOLETE: REMOVE:   if( std::abs(u) < 1.0e-6 ) { 
// OBSOLETE: REMOVE:     cerr << "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:          << "\n*** ERROR *** bool beamline::moveRelZ( drift* usPtr, bmlnElmnt* thePtr, Slot* dsPtr, double u )"
// OBSOLETE: REMOVE:             "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:          << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:          << "\n*** ERROR *** Displacement of " << (1.e6*u) << " microns is too small."
// OBSOLETE: REMOVE:             "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << endl;
// OBSOLETE: REMOVE:     return false; 
// OBSOLETE: REMOVE:   }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   double usLength = usPtr->Length() + u;
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   Frame dsInFrame( dsPtr->getInFrame() );
// OBSOLETE: REMOVE:   dsInFrame.translate( (-u)*usOutFrame.getZAxis() );
// OBSOLETE: REMOVE:   Frame dsOutFrame( (dsPtr->getOutFrame()).relativeTo(dsInFrame) );
// OBSOLETE: REMOVE:   double dsZ = (dsOutFrame.getOrigin())(dsOutFrame.zAxisIndex());
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Check for error
// OBSOLETE: REMOVE:   if( (usLength < 1.0e-7) || (dsZ < 1.0e-7) ) {
// OBSOLETE: REMOVE:     cerr << "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:          << "\n*** ERROR *** bool beamline::moveRelZ( drift* usPtr, bmlnElmnt* thePtr, Slot* dsPtr, double u )"
// OBSOLETE: REMOVE:             "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:          << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:          << "\n*** ERROR *** Longitudinal displacement too large.
// OBSOLETE: REMOVE:             "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << endl;
// OBSOLETE: REMOVE:     return false;
// OBSOLETE: REMOVE:   }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Perform the movement
// OBSOLETE: REMOVE:   usPtr->setLength( usLength );
// OBSOLETE: REMOVE:   dsPtr->setOutFrame( dsOutFrame );
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   return true;
// OBSOLETE: REMOVE: }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE: bool beamline::moveRelZ( Slot* usPtr, const bmlnElmnt* thePtr, Slot* dsPtr, double u )
// OBSOLETE: REMOVE: {
// OBSOLETE: REMOVE:   // Assumes orthonormal frames and in Frame is the identity.
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Will not displace anything less than 1 micron.
// OBSOLETE: REMOVE:   if( std::abs(u) < 1.0e-6 ) { 
// OBSOLETE: REMOVE:     cerr << "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:          << "\n*** ERROR *** bool beamline::moveRelZ( drift* usPtr, bmlnElmnt* thePtr, drift* dsPtr, double u )"
// OBSOLETE: REMOVE:             "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:          << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:          << "\n*** ERROR *** Displacement of " << (1.e6*u) << " microns is too small."
// OBSOLETE: REMOVE:             "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << endl;
// OBSOLETE: REMOVE:     return false; 
// OBSOLETE: REMOVE:   }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   Frame usOutFrame( usPtr->getOutFrame() );
// OBSOLETE: REMOVE:   usOutFrame.translate( u*usOutFrame.getZAxis() );
// OBSOLETE: REMOVE:   double usZ = (usOutFrame.getOrigin())(usOutFrame.zAxisIndex());
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   Frame dsInFrame( dsPtr->getInFrame() );
// OBSOLETE: REMOVE:   dsInFrame.translate( (-u)*usOutFrame.getZAxis() );
// OBSOLETE: REMOVE:   Frame dsOutFrame( (dsPtr->getOutFrame()).relativeTo(dsInFrame) );
// OBSOLETE: REMOVE:   double dsZ = (dsOutFrame.getOrigin())(dsOutFrame.zAxisIndex());
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Check for error
// OBSOLETE: REMOVE:   if( (usZ < 1.0e-7) || (dsZ < 1.0e-7) ) {
// OBSOLETE: REMOVE:     cerr << "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:          << "\n*** ERROR *** bool beamline::moveRelZ( Slot* usPtr, bmlnElmnt* thePtr, Slot* dsPtr, double u )"
// OBSOLETE: REMOVE:             "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:          << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:          << "\n*** ERROR *** Longitudinal displacement too large.
// OBSOLETE: REMOVE:             "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << endl;
// OBSOLETE: REMOVE:     return false;
// OBSOLETE: REMOVE:   }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Perform the movement
// OBSOLETE: REMOVE:   usPtr->setOutFrame( usOutFrame );
// OBSOLETE: REMOVE:   dsPtr->setOutFrame( dsOutFrame );
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   return true;
// OBSOLETE: REMOVE: }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE: BmlPtrList beamline::moveRelX(   const bmlnElmnt* const thePtr, 
// OBSOLETE: REMOVE:                                , double u /*[m]*/ )
// OBSOLETE: REMOVE: {
// OBSOLETE: REMOVE:   // Upon entry: thePtr      = pointer to element to be translated
// OBSOLETE: REMOVE:   //             u      [m]  = displacement
// OBSOLETE: REMOVE:   // 
// OBSOLETE: REMOVE:   // Upon exit:  *thePtr will have been displaced in the direction
// OBSOLETE: REMOVE:   //               of its local x coordinate by adjusting its neighboring
// OBSOLETE: REMOVE:   //               free-space elements. in fact, *thePtr is not changed
// OBSOLETE: REMOVE:   //               at all. Only its neighbors are altered. In fact, they
// OBSOLETE: REMOVE:   //               may have been replaced.
// OBSOLETE: REMOVE:   // 
// OBSOLETE: REMOVE:   //             Value returned: a list of the (zero, one, or two)
// OBSOLETE: REMOVE:   //               neighboring free-space elements that have been replaced.
// OBSOLETE: REMOVE:   // 
// OBSOLETE: REMOVE:   //             NOTE WELL: if the returned list is not empty, it means
// OBSOLETE: REMOVE:   //               that Slots have been created and installed in the line
// OBSOLETE: REMOVE:   //               for which the calling program must take responsibility.
// OBSOLETE: REMOVE:   //               This could be a serious problem. We need smart pointers!
// OBSOLETE: REMOVE:   // 
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   BmlPtrList recycleBin;
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   // Will not displace anything less than 1 micron.
// OBSOLETE: REMOVE:   if( std::abs(u) < 1.0e-6 ) { 
// OBSOLETE: REMOVE:     cerr << "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:          << "\n*** ERROR *** BmlPtrList beamline::moveRelX( const bmlnElmnt* const thePtr, double u /*[m]*/)"
// OBSOLETE: REMOVE:             "\n*** ERROR *** Unable to perform operation on "
// OBSOLETE: REMOVE:          << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:          << "\n*** ERROR *** Displacement of " << (1.e6*u) << " microns is too small."
// OBSOLETE: REMOVE:             "\n*** ERROR *** "
// OBSOLETE: REMOVE:          << endl;
// OBSOLETE: REMOVE:     return recycleBin; 
// OBSOLETE: REMOVE:   }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   bmlnElmnt* upStreamPtr   = 0;
// OBSOLETE: REMOVE:   bmlnElmnt* downStreamPtr = 0;
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   FramePusher fp;
// OBSOLETE: REMOVE:   Frame frameZero, frameOne, frameTwo, frameThree;
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   if( this->find( upStreamPtr, thePtr, downStreamPtr ) ) {
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:     if( (0 != upStreamPtr) && (0 != downStreamPtr ) {
// OBSOLETE: REMOVE:       upStreamPtr->accept( fp );
// OBSOLETE: REMOVE:       frameOne = fp.getFrame();
// OBSOLETE: REMOVE:       thePtr->accept( fp );
// OBSOLETE: REMOVE:       frameTwo = fp.getFrame();
// OBSOLETE: REMOVE:       downStreamPtr->accept( fp );
// OBSOLETE: REMOVE:       frameThree = fp.getFrame();
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:       Vector displacement(u*frameOne.getxAxis());
// OBSOLETE: REMOVE:       frameOne.translate( displacement );
// OBSOLETE: REMOVE:       frameTwo.translate( displacement );
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:       // THIS IS DONE INCREDIBLY INEFFICIENTLY
// OBSOLETE: REMOVE:       if( 0 == strcmp("Slot", upStreamPtr->Type() ) {
// OBSOLETE: REMOVE:         upStreamPtr->setInFrame( frameZero );
// OBSOLETE: REMOVE:         upStreamPtr->setOutFrame( frameOne );
// OBSOLETE: REMOVE:       }
// OBSOLETE: REMOVE:       if( 0 == strcmp("Slot", downStreamPtr->Type() ) {
// OBSOLETE: REMOVE:         downStreamPtr->setInFrame( frameZero );
// OBSOLETE: REMOVE:         downStreamPtr->setOutFrame( frameThree.relativeTo(frameTwo) );
// OBSOLETE: REMOVE:       }
// OBSOLETE: REMOVE:       if( 0 == strcmp("drift", upStreamPtr->Type() ) {
// OBSOLETE: REMOVE:         // DANGEROUS!!
// OBSOLETE: REMOVE:         Slot* slotPtr = new Slot(upStreamPtr->Name(), frameOne );
// OBSOLETE: REMOVE:         this->putAbove( thePtr, slotPtr );
// OBSOLETE: REMOVE:         this remove( upStreamPtr );
// OBSOLETE: REMOVE:         recycleBin.append( upStreamPtr );
// OBSOLETE: REMOVE:       }
// OBSOLETE: REMOVE:       if( 0 == strcmp("drift", downStreamPtr->Type() ) {
// OBSOLETE: REMOVE:         // DANGEROUS!!
// OBSOLETE: REMOVE:         Slot* slotPtr = new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) );
// OBSOLETE: REMOVE:         this->putBelow( thePtr, slotPtr );
// OBSOLETE: REMOVE:         this remove( downStreamPtr );
// OBSOLETE: REMOVE:         recycleBin.append( downStreamPtr );
// OBSOLETE: REMOVE:       }
// OBSOLETE: REMOVE:     }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:     else if( (0 == upStreamPtr) && (0 != downStreamPtr ) {
// OBSOLETE: REMOVE:       thePtr->accept( fp );
// OBSOLETE: REMOVE:       frameTwo = fp.getFrame();
// OBSOLETE: REMOVE:       downStreamPtr->accept( fp );
// OBSOLETE: REMOVE:       frameThree = fp.getFrame();
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:       Vector displacement(u*frameTwo.getxAxis());
// OBSOLETE: REMOVE:       frameTwo.translate( displacement );
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:       cerr << "\n*** WARNING *** "
// OBSOLETE: REMOVE:            << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
// OBSOLETE: REMOVE:            << "\n*** WARNING *** BmlPtrList beamline::moveRelX( const bmlnElmnt* const thePtr, double u /*[m]*/)"
// OBSOLETE: REMOVE:               "\n*** WARNING *** Will displace using downstream end of "
// OBSOLETE: REMOVE:            << thePtr->Type() << "  " << thePtr->Name() << "."
// OBSOLETE: REMOVE:            << "\n*** WARNING *** There is no upstream neighbor."
// OBSOLETE: REMOVE:               "\n*** WARNING *** "
// OBSOLETE: REMOVE:            << endl;
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:       // THIS IS DONE INCREDIBLY INEFFICIENTLY
// OBSOLETE: REMOVE:       if( 0 == strcmp("Slot", downStreamPtr->Type() ) {
// OBSOLETE: REMOVE:         downStreamPtr->setInFrame( frameZero );
// OBSOLETE: REMOVE:         downStreamPtr->setOutFrame( frameThree.relativeTo(frameTwo) );
// OBSOLETE: REMOVE:       }
// OBSOLETE: REMOVE:       if( 0 == strcmp("drift", downStreamPtr->Type() ) {
// OBSOLETE: REMOVE:         // DANGEROUS!!
// OBSOLETE: REMOVE:         Slot* slotPtr = new Slot(downStreamPtr->Name(), frameThree.relativeTo(frameTwo) );
// OBSOLETE: REMOVE:         this->putBelow( thePtr, slotPtr );
// OBSOLETE: REMOVE:         this remove( downStreamPtr );
// OBSOLETE: REMOVE:         recycleBin.append( downStreamPtr );
// OBSOLETE: REMOVE:       }
// OBSOLETE: REMOVE:     }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:     else if( (0 != upStreamPtr) && (0 == downStreamPtr ) {
// OBSOLETE: REMOVE:       upStreamPtr->accept( fp );
// OBSOLETE: REMOVE:       frameOne = fp.getFrame();
// OBSOLETE: REMOVE:       thePtr->accept( fp );
// OBSOLETE: REMOVE:       frameTwo = fp.getFrame();
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:       Vector displacement(u*frameOne.getxAxis());
// OBSOLETE: REMOVE:       frameOne.translate( displacement );
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:       // THIS IS DONE INCREDIBLY INEFFICIENTLY
// OBSOLETE: REMOVE:       if( 0 == strcmp("Slot", upStreamPtr->Type() ) {
// OBSOLETE: REMOVE: // OBSOLETE: REMOVE:         upStreamPtr->setInFrame( frameZero );
// OBSOLETE: REMOVE:         upStreamPtr->setOutFrame( frameOne );
// OBSOLETE: REMOVE:       }
// OBSOLETE: REMOVE:       if( 0 == strcmp("drift", upStreamPtr->Type() ) {
// OBSOLETE: REMOVE:         // DANGEROUS!!
// OBSOLETE: REMOVE:         Slot* slotPtr = new Slot(upStreamPtr->Name(), frameOne );
// OBSOLETE: REMOVE:         this->putAbove( thePtr, slotPtr );
// OBSOLETE: REMOVE:         this remove( upStreamPtr );
// OBSOLETE: REMOVE:         recycleBin.append( upStreamPtr );
// OBSOLETE: REMOVE:       }
// OBSOLETE: REMOVE:     }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:     else if( (0 == upStreamPtr) && (0 == downStreamPtr ) {
// OBSOLETE: REMOVE:       ostringstream uic;
// OBSOLETE: REMOVE:       uic  << "An impossibility has occurred. Am stopping.";
// OBSOLETE: REMOVE:       throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
// OBSOLETE: REMOVE:              "BmlPtrList beamline::moveRelX"
// OBSOLETE: REMOVE:              uic.str().c_str() ) );
// OBSOLETE: REMOVE:     }
// OBSOLETE: REMOVE:   }
// OBSOLETE: REMOVE: 
// OBSOLETE: REMOVE:   return recycleBin;
// OBSOLETE: REMOVE: }
