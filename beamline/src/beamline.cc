/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.2
******                                    
******  File:      beamline.cc
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
#include "sector.h"
#include "BeamlineIterator.h"
#include "InsertionList.h"

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
 numElem        = 0;
 nominalEnergy  = NOTKNOWN;
 depth          = 0;
 twissDone      = 0;
} 


beamline::beamline( const beamline& a ) 
: bmlnElmnt( (const bmlnElmnt&) a ), dlist( (dlist&) a ) 
{
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


beamline::beamline( bmlnElmnt* q ) : bmlnElmnt(), dlist() {
 dlist::insert( q );

 numElem        = 1;
 nominalEnergy  = NOTKNOWN;
 depth          = q->depth + 1;

 // ...... Initialize geometry .................................
 printf( "\n" );
 printf( "*** WARNING ***  beamline::beamline( bmlnElmnt* ) ....        \n" );
 printf( "*** WARNING ***  Geometry may be set up incorrectly if the    \n" );
 printf( "*** WARNING ***  first element is not a drift or marker.      \n" );
 printf( "*** WARNING ***                                               \n" );
 printf( "\n" );

 geometry.outPoint(0) = 0.0;   // Works only for non-bending ???
 geometry.outPoint(1) = 0.0;   // objects.                   ???
 geometry.outPoint(2) = q->length;

 geometry.outAxes[0](0) = 1.0;
 geometry.outAxes[0](1) = 0.0;
 geometry.outAxes[0](2) = 0.0;

 geometry.outAxes[1](0) = 0.0;
 geometry.outAxes[1](1) = 1.0;
 geometry.outAxes[1](2) = 0.0;

 geometry.outAxes[2](0) = 0.0;
 geometry.outAxes[2](1) = 0.0;
 geometry.outAxes[2](2) = 1.0;
}


beamline::beamline( char* n, bmlnElmnt* q ) : bmlnElmnt( n ), dlist() {
 dlist::insert( q );

 numElem        = 1;
 nominalEnergy  = NOTKNOWN;
 depth          = q->depth + 1;

 twissDone      = 0;

 // ...... Initialize geometry .................................
 printf( "\n" );
 printf( "*** WARNING ***  beamline::beamline( bmlnElmnt* ) ....        " );
 printf( "*** WARNING ***  Geometry may be set up incorrectly if the    " );
 printf( "*** WARNING ***  first element is not a drift or marker.      " );
 printf( "*** WARNING ***                                               " );
 printf( "\n" );

 geometry.outPoint(0) = 0.0;   // Works only for non-bending ???
 geometry.outPoint(1) = 0.0;   // objects.                   ???
 geometry.outPoint(2) = q->length;

 geometry.outAxes[0](0) = 1.0;
 geometry.outAxes[0](1) = 0.0;
 geometry.outAxes[0](2) = 0.0;

 geometry.outAxes[1](0) = 0.0;
 geometry.outAxes[1](1) = 1.0;
 geometry.outAxes[1](2) = 0.0;

 geometry.outAxes[2](0) = 0.0;
 geometry.outAxes[2](1) = 0.0;
 geometry.outAxes[2](2) = 1.0;
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


void beamline::geomToEnd( BMLN_posInfo& ) {
 static char firstCall = 1;
 if( firstCall ) {
   firstCall = 0;
   // ??? REMOVE: printf( "\n*** WARNING *** \n" );
   // ??? REMOVE: printf(   "*** WARNING *** Inserting beamlines into beamlines provides\n" );
   // ??? REMOVE: printf(   "*** WARNING *** unreliable geometry information.\n" );
   // ??? REMOVE: printf(   "*** WARNING *** This will be fixed.\n" );
   // ??? REMOVE: printf(   "*** WARNING *** \n" );
 }
} 

void beamline::geomToStart( BMLN_posInfo& ) {
 static char firstCall = 1;
 if( firstCall ) {
   firstCall = 0;
   // ??? REMOVE: printf( "\n*** WARNING *** \n" );
   // ??? REMOVE: printf(   "*** WARNING *** Inserting beamlines into beamlines provides\n" );
   // ??? REMOVE: printf(   "*** WARNING *** unreliable geometry information.\n" );
   // ??? REMOVE: printf(   "*** WARNING *** This will be fixed.\n" );
   // ??? REMOVE: printf(   "*** WARNING *** \n" );
 }
} 



void beamline::resetGeometry() {
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  bmlnElmnt* q;
//  bmlnElmnt* r;
  
  if((  p = (bmlnElmnt*) getNext()  ))
    while((  q = (bmlnElmnt*) getNext()  )) {
    if((   p->geometry.outPoint   != q->geometry.inPoint   ) ||
        ( p->geometry.outAxes[0] != q->geometry.inAxes[0] ) ||
        ( p->geometry.outAxes[1] != q->geometry.inAxes[1] ) ||
        ( p->geometry.outAxes[2] != q->geometry.inAxes[2] )    )
    {
     q->geomToEnd( p->geometry );
    }
    p = q;
   }
  else{
    printf( "\n*** WARNING *** \n" );
    printf(   "*** WARNING *** beamline::resetGeometry         \n" );
    printf(   "*** WARNING *** Called with empty beamline.     \n" );
    printf(   "*** WARNING *** \n" );
  }
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
  cout << "*** WARNING ***                               \n"
    << "*** WARNING *** beamline::whatIsRing       \n"
      << "*** WARNING *** Entry was not found.          \n"
        << "*** WARNING *** Meaningless value being       \n"
          << "*** WARNING *** returned.                     \n"
            << "*** WARNING ***                               \n" << endl;
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

 // ...... Adjust geometry
 int j;
 q.geomToStart( geometry );
 geometry.inPoint = q.geometry.inPoint;
 for( j = 0; j < 3; j++ ) geometry.inAxes[j] = q.geometry.inAxes[j];
} 


void beamline::insert( bmlnElmnt* q ) {
 dlist::insert( q );
 if( q->depth + 1 > depth ) depth = q->depth + 1;

 if( twissDone ) unTwiss();

 length += q -> length;

 if( strcmp( q->Type(), "beamline" ) == 0 )  
      numElem += ((beamline*) q)->numElem;
 else numElem++;

 // ...... Adjust geometry
 int j;
 q->geomToStart( geometry );
 geometry.inPoint = q->geometry.inPoint;
 for( j = 0; j < 3; j++ ) geometry.inAxes[j] = q->geometry.inAxes[j];
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

    if( ((ent) p_be) == this->lastInfoPtr() ) {
      dlist::remove( (ent) p_be );      // ??? Screws up geometry, of course.
      getNext.Terminate();
      // This is required because of the way 
      // dlist_iterator::operator() is written.
    }
    else {
      dlist::remove( (ent) p_be );
    }
    removedElements.append( (ent) p_be );

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

 // ...... Adjust geometry
 int j;
 q.geomToEnd( geometry );
 geometry.outPoint = q.geometry.outPoint;
 for( j = 0; j < 3; j++ ) geometry.outAxes[j] = q.geometry.outAxes[j];
} 


void beamline::append( bmlnElmnt* q ) {
 dlist::append( q );
 if( q->depth + 1 > depth ) depth = q->depth + 1;

 if( twissDone ) unTwiss();

 length += q->length;
 if( strcmp( q->Type(), "beamline" ) == 0 )  
      numElem += ((beamline*) q)->numElem;
 else numElem++;

 // ...... Adjust geometry
 int j;
 q->geomToEnd( geometry );
 geometry.outPoint = q->geometry.outPoint;
 for( j = 0; j < 3; j++ ) geometry.outAxes[j] = q->geometry.outAxes[j];
} 


void beamline::Split( double, bmlnElmnt**, bmlnElmnt** )
{
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "void beamline::Split( double, bmlnElmnt**, bmlnElmnt** )", 
         "This method should not be invoked by a beamline object." ) );
}


char beamline::putAbove( bmlnElmnt& x, bmlnElmnt& y ) 
{
 // Insert y above (before; upstream of) x in the beamline

 static char firstCall = 1;
 if( firstCall ) {
   firstCall = 0;
   cerr << "\n*** WARNING ***                                             \n"
             "*** WARNING *** beamline::putAbove                          \n"
             "*** WARNING *** Geometry information will be corrupted.     \n"
             "*** WARNING ***                                             \n"
        << endl;
 }

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


char beamline::putBelow( bmlnElmnt& x, bmlnElmnt& y ) 
{
 // Insert y below (after; downstream of) x in the beamline

 static char firstCall = 1;
 if( firstCall ) {
   firstCall = 0;
   cerr << "\n*** WARNING ***                                             \n"
             "*** WARNING *** beamline::putBelow                          \n"
             "*** WARNING *** Geometry information will be corrupted.     \n"
             "*** WARNING ***                                             \n"
        << endl;
 }

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


beamline& beamline::operator=( const beamline& a ) {
 dlist::operator=( (dlist&) a );
 bmlnElmnt::operator=( (bmlnElmnt&) a );
 nominalEnergy     = a.nominalEnergy;
 numElem           = a.numElem;
 twissDone         = 0;
 return *this;
} 

beamline* beamline::flatten() const {
 dlist_iterator  getNext( *(dlist*) this );
 dlist_iterator* getNew;
 beamline*       r;
 beamline*       s;
 bmlnElmnt*      p;
 bmlnElmnt*      q;

 r = new beamline;
 r->setEnergy(nominalEnergy);
 while ((  p = (bmlnElmnt*) getNext()  ))  {
   if( strcasecmp( p->Type(), "beamline" ) == 0 )
   { 
     s = ( (beamline*) p ) -> flatten();
     getNew = new dlist_iterator( *(dlist*) s );
     while ((  q = (bmlnElmnt*) (*getNew)()  ))
       r->append( q );
     delete getNew;
     delete s; // ??? s->eliminate() ???
   }
   else
   { // Only beamlines are composite objects
     r->append( p );
   }
 }

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

 s->geometry.inPoint  = geometry.inPoint;
 s->geometry.outPoint = geometry.outPoint;
 for( i = 0; i < 3; i++ ) {
    s->geometry.inAxes[i] = geometry.inAxes[i];
   s->geometry.outAxes[i] = geometry.outAxes[i];
 }

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


beamline beamline::remove( bmlnElmnt& x, bmlnElmnt& y ) {
 beamline a;
 dlist dl;
 int i;
 bmlnElmnt* p;

 unTwiss();
 dl = dlist::remove( &x, &y );

 dlist_iterator getNext( dl );
 if((  p = (bmlnElmnt*) getNext()  )) {
   a.geometry.inPoint = p->geometry.inPoint;
   a.geometry.outPoint = a.geometry.inPoint;
   for( i = 0; i < 3; i++ ) a.geometry.inAxes[i] = p->geometry.inAxes[i];
   for( i = 0; i < 3; i++ ) a.geometry.outAxes[i] = a.geometry.inAxes[i];
   a.append( *p );
 }
 while ((  p = (bmlnElmnt*) getNext()  ))  a.append( *p );

 a.nominalEnergy = nominalEnergy;
 numElem = numElem - a.numElem; // ??? What about depth?
 return a;
}

void beamline::setAlignment( const alignmentData& al ) {
  // Propogate alignment data of entire  beamline to each individual element
  dlist_iterator getNext ( *(dlist*) this );
  bmlnElmnt* p;
  while ((  p = (bmlnElmnt*) getNext()  )) {
    p->setAlignment(al);
  }
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

// ??? REMOVE: istream& beamline::readFrom(istream& is)
// ??? REMOVE: {
// ??? REMOVE:   // Written by Elliott McCrory
// ??? REMOVE:   bmlnElmnt *e = NULL;
// ??? REMOVE:   is >> nominalEnergy;
// ??? REMOVE:   // cerr << "Beamline " << Name() << " has energy of " << nominalEnergy << "\n";
// ??? REMOVE:   // Now, continue reading is until we see the end of this beamline
// ??? REMOVE:   length = 0;
// ??? REMOVE:   do {
// ??? REMOVE:     e = read_istream(is);	// (Recursion)
// ??? REMOVE:     // read_istream will return NULL when end of file or end of beamline is reached.
// ??? REMOVE:     if ( e ) 
// ??? REMOVE:       append(e);
// ??? REMOVE:   } while ( e );
// ??? REMOVE: 
// ??? REMOVE:   return is;
// ??? REMOVE: }

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
