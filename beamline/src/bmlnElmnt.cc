/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
******                                    
******  File:      bmlnElmnt.cc
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


#include <typeinfo>
#include <string>
// REMOVE: #include <string.h>

#include <iomanip>

#ifdef __SUNPRO_CC
  #ifndef __STL_NEED_EXPLICIT
    #define __STL_NEED_EXPLICIT
  #endif
#endif

#include "bmlnElmnt.h"
#include "Particle.h"
#include "ParticleBunch.h"
#include "Aperture.h"
#include "BmlVisitor.h"


#ifdef OBJECT_DEBUG
int bmlnElmnt::objectCount = 0;
#endif


// Error flags for _tag manipulation functions.
const short bmlnElmnt::BF_OK         = 0;
const short bmlnElmnt::BF_NULL_ARG   = 1;
const short bmlnElmnt::BF_BAD_START  = 2;

using namespace std;

// **************************************************
//   yes and no Criteria
// **************************************************

beamline::Aye beamline::yes;
beamline::Nay beamline::no;

// **************************************************
//   struct BMLN_posInfo
// **************************************************

BMLN_posInfo::BMLN_posInfo() {
 inPoint.set   ( 0., 0., 0. );
 inAxes[0].set ( 1., 0., 0. );
 inAxes[1].set ( 0., 1., 0. );
 inAxes[2].set ( 0., 0., 1. );

 outPoint.set   ( 0., 0., 0. );
 outAxes[0].set ( 1., 0., 0. );
 outAxes[1].set ( 0., 1., 0. );
 outAxes[2].set ( 0., 0., 1. );
}


BMLN_posInfo::BMLN_posInfo( const BMLN_posInfo& x ) {
 inPoint  = x.inPoint;
 outPoint = x.outPoint;
 int j; // O.K.
 for( j = 0; j < 3; j++ ) inAxes[j]  = x.inAxes[j];
 for( j = 0; j < 3; j++ ) outAxes[j] = x.outAxes[j];
}


BMLN_posInfo& BMLN_posInfo::operator=( const BMLN_posInfo& x ) {
 inPoint  = x.inPoint;
 outPoint = x.outPoint;
 int j; // O.K.
 for( j = 0; j < 3; j++ ) inAxes[j]  = x.inAxes[j];
 for( j = 0; j < 3; j++ ) outAxes[j] = x.outAxes[j];
 return *this;
}
ostream& operator<<(ostream& os, BMLN_posInfo& x) {

  os << "\n inPoint = " << x.inPoint(0) 
                << ", " << x.inPoint(1) 
                << ", " << x.inPoint(2) << endl;
  int i;
  for( i = 0; i < 3; i++ ) {
    os << "\n inAxes["<< i <<"] = " 
       << x.inAxes[i](0) << ", " 
       << x.inAxes[i](1) << ", " 
       << x.inAxes[i](2) << endl;
  }
  os << "\n outPoint = " 
     << x.outPoint(0) << ", " 
     << x.outPoint(1) << ", " 
     << x.outPoint(2) << endl;
  for( i = 0; i < 3; i++ ) {
    os << "\n outAxes["<< i <<"] = " 
       << x.outAxes[i](0) << ", " 
       << x.outAxes[i](1) << ", " 
       << x.outAxes[i](2) << endl;
  }
 return os << "\n" << endl;
}

// **************************************************
//   struct bmlnElmntData
// **************************************************

bmlnElmntData::bmlnElmntData() {
 strcpy( type, "" );
 address  = 0;
 name     = 0;  // ??? START HERE, LEO ... Is this a problem?
 depth    = 0;
 length   = 0.0;
 strength = 0.0;
 pAperture = 0;
 
 iToField = 1.0;                // 1 KA = 1 Tesla default
 shuntCurrent = 0.0;

 // ...... Initialize geometry .................................
 geometry.inPoint.set   ( 0., 0., 0. );
 geometry.inAxes[0].set ( 1., 0., 0. );
 geometry.inAxes[1].set ( 0., 1., 0. );
 geometry.inAxes[2].set ( 0., 0., 1. );

 geometry.outPoint.set   ( 0., 0., 0. );
 geometry.outAxes[0].set ( 1., 0., 0. );
 geometry.outAxes[1].set ( 0., 1., 0. );
 geometry.outAxes[2].set ( 0., 0., 1. );

 more     = 0;
}

bmlnElmntData::bmlnElmntData( bmlnElmntData& x ) {
 strcpy( type, x.type );
 address   = x.address;
 name      = new char [ strlen(x.name) + 1 ];
             strcpy( name, x.name );
 depth     = x.depth;
 length    = x.length;
 strength  = x.strength;
 geometry  = x.geometry;   // ??? Memory leak!!!  Bit copy
 more      = x.more;
 pAperture = 0;
 if(x.pAperture != 0) 
   pAperture = x.pAperture->Clone();
 
 iToField  = x.iToField;
 shuntCurrent = 0.0;
 align     = x.align;
}

bmlnElmntData::~bmlnElmntData( ) {
 delete [] name;
}

void bmlnElmntData::eliminate() {
 delete this;
}

void* bmlnElmntData::clone() {
 void* p = new bmlnElmntData( *this );
 return p;
}


// **************************************************
//   class bmlnElmnt
// **************************************************

bmlnElmnt::bmlnElmnt( const char* n, PropFunc* pf ) {
 if( n ) {
  ident = new char [ strlen(n) + 1 ];
  strcpy( ident, n );
 }
 else {
  ident = new char [8];
  strcpy( ident, "NONAME" );
 }

 _ctRef = 0.0;

 length       = 0.0;
 strength     = 0.0;
 depth        = 0;
 pAperture = 0;
 
 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;

 for( int i = 0; i < BF_MAXCHAR; i++ ) {
   _tag[i] = '\0';
 }

 // ...... Initialize geometry .................................
 geometry.inPoint.set   ( 0., 0., 0. );
 geometry.inAxes[0].set ( 1., 0., 0. );
 geometry.inAxes[1].set ( 0., 1., 0. );
 geometry.inAxes[2].set ( 0., 0., 1. );

 geometry.outPoint.set   ( 0., 0., 0. );
 geometry.outAxes[0].set ( 1., 0., 0. );
 geometry.outAxes[1].set ( 0., 1., 0. );
 geometry.outAxes[2].set ( 0., 0., 1. );
 #ifdef OBJECT_DEBUG
 objectCount++;
 #endif
}

bmlnElmnt::bmlnElmnt( double l /* length */, PropFunc* pf ) {
 ident        = new char [8];
                 strcpy( ident, "NONAME" );
 length       = l;
 strength     = 0.0;
 depth        = 0;

 pAperture = 0;
 
 _ctRef = 0.0;

 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;

 for( int i = 0; i < BF_MAXCHAR; i++ ) {
   _tag[i] = '\0';
 }

 // ...... Initialize geometry .................................
 geometry.inPoint.set   ( 0., 0., 0. );
 geometry.inAxes[0].set ( 1., 0., 0. );
 geometry.inAxes[1].set ( 0., 1., 0. );
 geometry.inAxes[2].set ( 0., 0., 1. );

 geometry.outPoint.set   ( 0., 0., 0. );
 geometry.outAxes[0].set ( 1., 0., 0. );
 geometry.outAxes[1].set ( 0., 1., 0. );
 geometry.outAxes[2].set ( 0., 0., 1. );
 #ifdef OBJECT_DEBUG
 objectCount++;
 #endif
}

bmlnElmnt::bmlnElmnt( double l /* length */, 
                      double s /* strength */, 
                      PropFunc* pf ) {
 ident        = new char [8];
                 strcpy( ident, "NONAME" );
 length       = l;
 strength     = s;
 depth        = 0;

 _ctRef = 0.0;

 pAperture    = 0;
 
 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;

 for( int i = 0; i < BF_MAXCHAR; i++ ) {
   _tag[i] = '\0';
 }

 // ...... Initialize geometry .................................
 geometry.inPoint.set   ( 0., 0., 0. );
 geometry.inAxes[0].set ( 1., 0., 0. );
 geometry.inAxes[1].set ( 0., 1., 0. );
 geometry.inAxes[2].set ( 0., 0., 1. );

 geometry.outPoint.set   ( 0., 0., 0. );
 geometry.outAxes[0].set ( 1., 0., 0. );
 geometry.outAxes[1].set ( 0., 1., 0. );
 geometry.outAxes[2].set ( 0., 0., 1. );
 #ifdef OBJECT_DEBUG
 objectCount++;
 #endif
}

bmlnElmnt::bmlnElmnt( const char*  n /* name */, 
                      double l       /* length */, 
                      PropFunc* pf ) {
 if( n ) {
  ident = new char [ strlen(n) + 1 ];
  strcpy( ident, n );
 }
 else {
  ident = new char [8];
  strcpy( ident, "NONAME" );
 }
 length       = l;
 strength     = 0.0;
 depth        = 0;

 _ctRef = 0.0;

 pAperture = 0;
 
 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;

 for( int i = 0; i < BF_MAXCHAR; i++ ) {
   _tag[i] = '\0';
 }

 // ...... Initialize geometry .................................
 geometry.inPoint.set   ( 0., 0., 0. );
 geometry.inAxes[0].set ( 1., 0., 0. );
 geometry.inAxes[1].set ( 0., 1., 0. );
 geometry.inAxes[2].set ( 0., 0., 1. );

 geometry.outPoint.set   ( 0., 0., 0. );
 geometry.outAxes[0].set ( 1., 0., 0. );
 geometry.outAxes[1].set ( 0., 1., 0. );
 geometry.outAxes[2].set ( 0., 0., 1. );
 #ifdef OBJECT_DEBUG
 objectCount++;
 #endif
}

bmlnElmnt::bmlnElmnt( const char*  n /* name */, 
                      double l       /* length */, 
                      double s       /* strength */, 
                      PropFunc* pf ) {
 if( n ) {
  ident = new char [ strlen(n) + 1 ];
  strcpy( ident, n );
 }
 else {
  ident = new char [8];
  strcpy( ident, "NONAME" );
 }
 length       = l;
 strength     = s;
 depth        = 0;

 _ctRef = 0.0;

 pAperture = 0;
 
 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;

 for( int i = 0; i < BF_MAXCHAR; i++ ) {
   _tag[i] = '\0';
 }

 // ...... Initialize geometry .................................
 geometry.inPoint.set   ( 0., 0., 0. );
 geometry.inAxes[0].set ( 1., 0., 0. );
 geometry.inAxes[1].set ( 0., 1., 0. );
 geometry.inAxes[2].set ( 0., 0., 1. );

 geometry.outPoint.set   ( 0., 0., 0. );
 geometry.outAxes[0].set ( 1., 0., 0. );
 geometry.outAxes[1].set ( 0., 1., 0. );
 geometry.outAxes[2].set ( 0., 0., 1. );
 #ifdef OBJECT_DEBUG
 objectCount++;
 #endif
}

bmlnElmnt::bmlnElmnt( const bmlnElmnt& a ) {
 ident         = new char [ strlen(a.ident) + 1 ];
                 strcpy( ident, a.ident );
 length        = a.length;
 strength      = a.strength;
 depth         = a.depth;

 _ctRef = a._ctRef;

 pAperture = 0;
 if(a.pAperture != 0) 
   pAperture = a.pAperture->Clone();
 
 iToField      = a.iToField;
 shuntCurrent  = a.getShunt();

 Propagator = a.Propagator;

 memcpy( (void*) _tag, (const void*) a._tag, BF_MAXCHAR*sizeof(char) );

 if(a.align != 0) {
   alignmentData data = a.align->getAlignment();
   if((data.xOffset != 0.0) || (data.yOffset != 0.0) || (data.tilt != 0.0))
     align         = new alignment(*a.align);
 } 
 else align = 0;

 if( a.p_bml ) 
 {
   p_bml = (beamline*) a.p_bml->Clone();
   if( a.p_bml_e ) {
     int count = 0;
     dlist_iterator getNext( *(dlist*) a.p_bml );
     bmlnElmnt* q;
     bool notFound = true;
     while((  q = (bmlnElmnt*) getNext()  )) {
       count++;
       if( q == a.p_bml_e ) {
         getNext.Reset( *(dlist*) p_bml );
         for( int i = 0; i < count; i++ ) {
           q = (bmlnElmnt*) getNext();
         }
         p_bml_e = q;
         notFound = false;
         break;
       }
     }
     if( notFound ) {
       p_bml_e = a.p_bml_e->Clone();
       cerr << "*** WARNING *** \n"
            << "*** WARNING *** bmlnElmnt::bmlnElmnt( const bmlnElmnt& )\n"
            << "*** WARNING *** The element pointed to by p_bml_e does not exist\n"
            << "*** WARNING *** within the beamline pointed to by p_bml .\n"
            << "*** WARNING *** "
            << endl;
     }
   }
   else {
     p_bml_e = 0;
   }
 }
 else 
 {
   p_bml = 0;
   if( a.p_bml_e ) {
     p_bml_e = a.p_bml_e->Clone();
   }
   else p_bml_e = 0;
 }


 geometry.inPoint  = a.geometry.inPoint;
 geometry.outPoint = a.geometry.outPoint;
 for( int i = 0; i < 3; i++ ) {
    geometry.inAxes[i] = a.geometry.inAxes[i];
   geometry.outAxes[i] = a.geometry.outAxes[i];
 }
 #ifdef OBJECT_DEBUG
 objectCount++;
 #endif

}

bmlnElmnt::bmlnElmnt( bmlnElmntData& x ) {
 ident         = new char [ strlen(x.name) + 1 ];
                 strcpy( ident, x.name );
 length        = x.length;
 strength      = x.strength;
 depth         = x.depth;  // ??? This is meaningless.

 _ctRef = 0.0;  // not kosher, but I should eliminate bmlnElmntData anyway.

 pAperture = 0;
 if(x.pAperture != 0) 
   pAperture = x.pAperture->Clone();
 
 iToField      = x.iToField;
 shuntCurrent  = x.shuntCurrent;
 align         = 0;
 if( (x.align.xOffset != 0.0) || 
     (x.align.yOffset != 0.0) || 
     (x.align.tilt != 0.0) )
 align         = new alignment(x.align);

 cerr << "*** WARNING ***                                    \n"
         "*** WARNING *** bmlnElmnt::bmlnElmnt( bmlnElmntData& ) \n"
         "*** WARNING *** p_bml and p_bml_e are not going    \n"
         "*** WARNING *** to be copied.  Sorry.              \n"
         "*** WARNING ***                                    \n"
      << endl;
 p_bml = 0;
 p_bml_e = 0;

 geometry.inPoint  = x.geometry.inPoint;
 geometry.outPoint = x.geometry.outPoint;
 for( int i = 0; i < 3; i++ ) {
    geometry.inAxes[i] = x.geometry.inAxes[i];
   geometry.outAxes[i] = x.geometry.outAxes[i];
 }
 #ifdef OBJECT_DEBUG
 objectCount++;
 #endif
}

bmlnElmnt::~bmlnElmnt() {
 delete []     ident;
 if( align )   delete align;
 if(pAperture) delete pAperture;
 if( p_bml )   {
   p_bml->eliminate();
   p_bml = 0;
 }
 if( p_bml_e ) {
   delete p_bml_e;
   p_bml_e = 0;
 }
 dataHook.eraseAll();
 #ifdef OBJECT_DEBUG
 objectCount--;
 #endif
}

void bmlnElmnt::set( const bmlnElmntData& data ) {
 length       = data.length;                            // J. Holt
 strength     = data.strength;                          // J. Holt
   //
   // O.K.
   //
   // aperture     = data.aperture;

// pAperture    = data.pAperture;
 pAperture = 0;
 if(data.pAperture != 0) 
   pAperture = data.pAperture->Clone();
 
 iToField     = data.iToField;
 shuntCurrent = data.shuntCurrent;
 if(align != 0) {
   delete align;
   align = 0;
 }
 if((data.align.xOffset != 0.0) || (data.align.yOffset != 0.0) || 
    (data.align.tilt != 0.0))
   align        = new alignment(data.align);

}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Begin: basic propagaor functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void bmlnElmnt::propagate( Particle& x ) 
{
  if( align == 0 ) {
    localPropagate  ( x );
  }
  else {
    enterLocalFrame ( x );
    localPropagate  ( x );
    leaveLocalFrame ( x );
  }
}


void bmlnElmnt::propagate( JetParticle& x )
{
  if( align == 0 ) {
    localPropagate  ( x );
  }
  else {
    enterLocalFrame ( x );
    localPropagate  ( x );
    leaveLocalFrame ( x );
  }
}


void bmlnElmnt::propagate( ParticleBunch& x )
{
  static Particle* p;
  if( align == 0 ) {
    localPropagate  ( x );
  }
  else {
    ParticleBunch::Iterator get( x );
    while((  p = (Particle*) get.next()  )) enterLocalFrame( *p );
    localPropagate  ( x );
    get.reset();
    while((  p = (Particle*) get.next()  )) leaveLocalFrame( *p );
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// End: basic propagator functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Begin: tagging routines
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

short bmlnElmnt::writeTag ( char c, short index )
{
  if( index < 0 || index >= BF_MAXCHAR ) {
    return BF_BAD_START;
  }
  _tag[index] = c;
  return BF_OK;
}


short bmlnElmnt::writeTag ( const char* s, short start, short num )
{
  if( s == 0 ) {
    return BF_NULL_ARG;
  }
  if( start < 0 || start + 1 > BF_MAXCHAR ) {
    return BF_BAD_START;
  }

  if( start + num > BF_MAXCHAR ) {
    num = BF_MAXCHAR - start;
  }
  memcpy( (void*) (_tag + start), (const void*) s, num*sizeof(char) );
  return BF_OK;
}


short bmlnElmnt::writeTag ( const char* s )
{
  static int num;

  if( s == 0 ) {
    return BF_NULL_ARG;
  }

  num = strlen(s);
  if( num > BF_MAXCHAR ) {
    num = BF_MAXCHAR;
  }
  memcpy( (void*) _tag, (const void*) s, num );
  return BF_OK;
}


short  bmlnElmnt::writeTag ( const std::string& s, short start, short  num )
{
  if( start < 0 || start + 1 > BF_MAXCHAR ) {
    return BF_BAD_START;
  }
  if( start + num > BF_MAXCHAR ) {
    num = BF_MAXCHAR - start;
  }
  s.copy( _tag + start, num );
  return BF_OK;
}


short  bmlnElmnt::writeTag ( const std::string& s )
{
  static int num;

  num = s.length();
  if( num > BF_MAXCHAR ) {
    num = BF_MAXCHAR;
  }
  s.copy( _tag, num );
  return BF_OK;
}


short bmlnElmnt::readTag  ( char* s, short start, short num )
{
  if( s == 0 ) {
    return BF_NULL_ARG;
  }
  if( start < 0 || start + 1 > BF_MAXCHAR ) {
    return BF_BAD_START;
  }
  if( start + num > BF_MAXCHAR ) {
    num = BF_MAXCHAR - start;
  }
  memcpy( (void*) s, (const void*) (_tag + start), num*sizeof(char) );
  return BF_OK;
}


short bmlnElmnt::readTag( char* s )
{
  return this->readTag( s, 0, BF_MAXCHAR );
}


char bmlnElmnt::readTag( short pos )
{
  if( (pos < 0) || (pos + 1 > BF_MAXCHAR) ) {
    pos = 0;
    cerr << "*** WARNING ***                              \n"
         << "*** WARNING *** bmlnElmnt::readTag           \n"
         << "*** WARNING *** Character position out of    \n"
         << "*** WARNING *** bounds. Will return first    \n"
         << "*** WARNING *** character of tag.            \n"
         << "*** WARNING ***                              \n"
         << endl;
  }
  return _tag[pos];
}


std::string bmlnElmnt::readTag( short start, short  num )
{
  std::string ret;
  if( start < 0 || start + 1 > BF_MAXCHAR ) {
    start = 0;
  }
  if( start + num > BF_MAXCHAR ) {
    num = BF_MAXCHAR - start;
  }
  ret = ret.append( _tag + start, num );
  return ret;
}


std::string bmlnElmnt::readTag()
{
  return this->readTag( 0, BF_MAXCHAR );
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// End: tagging routines
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


void bmlnElmnt::setLength( double x ) {
  if( length < 0.0 ) {
    cerr << "*** WARNING ***                       \n"
            "*** WARNING *** bmlnElmnt::setLength  \n"
            "*** WARNING *** Lengths must be positive.  \n"
            "*** WARNING *** You have entered "
         << x 
         << "    \n"
            "*** WARNING *** I will use its absolute\n"
            "*** WARNING *** value.                \n"
            "*** WARNING ***                       \n"
         << endl;
    length = -x;
  }
  else {
    length = x;
  }
}

void bmlnElmnt::setStrength( double s ) {
  strength = s - getShunt()*IToField(); 
}

void bmlnElmnt::setStrength( double, int ) {
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "void bmlnElmnt::setStrength( double, int ) {", 
         "This virtual function should never have been "
         "called in its base class version." ) );
}

void bmlnElmnt::setCurrent( double I ) {
  setStrength((I-getShunt()) * IToField());
}

void bmlnElmnt::geomToEnd( BMLN_posInfo& g ) {
 // Default operation is to act like a drift.
 for( int i = 0; i < 3; i++ ) {
   geometry .inAxes[i] = g.outAxes[i];
   geometry.outAxes[i] = g.outAxes[i];
 }
 geometry.inPoint = g.outPoint;
 geometry.outPoint = geometry.inPoint + length*geometry.outAxes[2];
}

void bmlnElmnt::geomToStart( BMLN_posInfo& g ) {
 // Default operation is to act like a drift.
 for( int i = 0; i < 3; i++ ) {
   geometry .inAxes[i] = g.inAxes[i];
   geometry.outAxes[i] = g.inAxes[i];
 }
 geometry.outPoint = g.inPoint;
 geometry.inPoint = geometry.outPoint - length*geometry.outAxes[2];
}

void bmlnElmnt::eliminate() {
 delete this;
}

bmlnElmnt& bmlnElmnt::operator=( const bmlnElmnt& a ) {
 strcpy( ident, a.ident );
 length        = a.length;
 strength      = a.strength;
 depth         = a.depth;
   //
   // O.K.
   //
   // aperture      = a.aperture;

// pAperture     = a.pAperture;
 pAperture = 0;
 if(a.pAperture != 0) 
   pAperture = a.pAperture->Clone();
 
 iToField      = a.iToField;
 shuntCurrent  = a.getShunt();
 if(align != 0) {
   delete align;
   align = 0;
 }
 if(a.align != 0) {
   alignmentData data = a.align->getAlignment();
   if((data.xOffset != 0.0) || (data.yOffset != 0.0) || (data.tilt != 0.0))
     align         =  new alignment(data);
 }
 geometry.inPoint  = a.geometry.inPoint;
 geometry.outPoint = a.geometry.outPoint;
 for( int i = 0; i < 3; i++ ) {
    geometry.inAxes[i] = a.geometry.inAxes[i];
   geometry.outAxes[i] = a.geometry.outAxes[i];
 }
 return *this;
}


bmlnElmnt::PropFunc* bmlnElmnt::setPropFunction ( const PropFunc* a ) 
{ 
  bmlnElmnt::PropFunc* ret = this->Propagator;
  this->Propagator = (PropFunc*) a;  
  return ret;
}

bmlnElmnt::PropFunc* bmlnElmnt::setPropFunction ( const PropFunc& a ) 
{ 
  bmlnElmnt::PropFunc* ret = this->Propagator;
  this->Propagator = (PropFunc*) &a; 
  return ret;
}


void bmlnElmnt::peekAt( double& s, Particle* p_prt ) {
 cout << setw(12) << s;
 s += OrbitLength( *p_prt );
 cout << setw(12) << s           
                  << " : " 
      << setw(10) << (int) this  
      << setw(15) << ident       
      << setw(15) << Type()      
      << setw(12) << length      
      << setw(12) << strength    
      << setw(12) << shuntCurrent
      << endl;
}


bool bmlnElmnt::equivTo( const bmlnElmnt& x ) const 
{
  static double maxLength;
  static double maxStrength;

  if( typeid(*this) !=  typeid(x) ) {
    return false;
  }

  if( length   < x.length   ) { maxLength   = x.length;   }
  else                        { maxLength   =   length;   }
  if( strength < x.strength ) { maxStrength = x.strength; }
  else                        { maxStrength =   strength; }

  return ( ( fabs( length   - x.length   ) < 1.0e-6 * maxLength   )  &&
           ( fabs( strength - x.strength ) < 1.0e-6 * maxStrength )     );
}


bool bmlnElmnt::equivTo( const bmlnElmnt* x ) const 
{
  return this->equivTo( *x );
}


void bmlnElmnt::setAlignment(const alignmentData& a) {
  if(align != 0) {
    delete align;
    align = 0;
  }
  if((a.xOffset != 0.0) || (a.yOffset != 0.0) || (a.tilt != 0.0))
    align = new alignment(a);
}

alignmentData bmlnElmnt::Alignment() const {
  alignmentData x;
  if(align != 0) {
    x = align->getAlignment();
  }
  return x;
}

void bmlnElmnt::enterLocalFrame( Particle& p ) const
{
  static double inState[ BMLN_dynDim ];
  static double temp;
  static double cs, sn;

  cs = align->cos_roll();
  sn = align->sin_roll();

  p.getState( inState );

  inState[0] -= align->x_offset();
  inState[1] -= align->y_offset();

  if( align->roll() != 0.0) {
    temp       = inState[0] * cs + inState[1] * sn;
    inState[1] = inState[1] * cs - inState[0] * sn;
    inState[0] = temp;

    temp       = inState[3] * cs + inState[4] * sn;
    inState[4] = inState[4] * cs - inState[3] * sn;
    inState[3] = temp;
  }
 
  p.setState( inState) ;
}

void bmlnElmnt::enterLocalFrame( JetParticle& p ) const
{
  JetVector inState ( p.State() );
  Jet       temp    ( ((Jet__environment*) inState.Env()) );
  static    double  cs, sn;

  cs = align->cos_roll();
  sn = align->sin_roll();

  inState(0) -= align->x_offset();
  inState(1) -= align->y_offset();

  if( align->roll() != 0.0) {
    temp       = inState(0) * cs + inState(1) * sn;
    inState(1) = inState(1) * cs - inState(0) * sn;
    inState(0) = temp;

    temp       = inState(3) * cs + inState(4) * sn;
    inState(4) = inState(4) * cs - inState(3) * sn;
    inState(3) = temp;
  }

  p.setState( inState );
}

void bmlnElmnt::leaveLocalFrame( Particle& p ) const
{
  static double outState[ BMLN_dynDim ];
  static double temp;
  static double cs, sn;

  cs = align->cos_roll();
  sn = align->sin_roll();

  p.getState( outState );

  if( align->roll() != 0.0) {
    temp        = outState[0] * cs - outState[1] * sn;
    outState[1] = outState[1] * cs + outState[0] * sn;
    outState[0] = temp;

    temp        = outState[3] * cs - outState[4] * sn;
    outState[4] = outState[4] * cs + outState[3] * sn;
    outState[3] = temp;
  }

  outState[0] += align->x_offset();
  outState[1] += align->y_offset();

  p.setState( outState );
}

void bmlnElmnt::leaveLocalFrame( JetParticle& p ) const
{
  JetVector outState ( p.State() );
  Jet       temp     ( ((Jet__environment*) outState.Env()) );
  static    double   cs, sn;

  cs = align->cos_roll();
  sn = align->sin_roll();

  if( align->roll() != 0.0) {
    temp        = outState(0) * cs - outState(1) * sn;
    outState(1) = outState(1) * cs + outState(0) * sn;
    outState(0) = temp;

    temp        = outState(3) * cs - outState(4) * sn;
    outState(4) = outState(4) * cs + outState(3) * sn;
    outState(3) = temp;
  }

  outState(0) += align->x_offset();
  outState(1) += align->y_offset();

  p.setState( outState );
}

void bmlnElmnt::setAperture( Aperture* pAperture_in ) {
    //
    // aperture = x;
    //
  pAperture = pAperture_in;
}


double bmlnElmnt::setReferenceTime( const Particle& prtn )
{
  static bool firstTime = true;
  if( firstTime ) {
    cerr << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** double bmlnElmnt::setReferenceTime( const Particle& )"
            "\n*** WARNING *** Invoked on base class. Nothing will happen."
            "\n*** WARNING *** This warning is issued only once."
            "\n*** WARNING *** "
         << endl;
    firstTime = false;
  }
}


double bmlnElmnt::setReferenceTime( double x )
{
  double oldValue = _ctRef;
  _ctRef = x;
  if( fabs(_ctRef) < 1.0e-12 ) { _ctRef = 0.0; }
  return oldValue;
}


void bmlnElmnt::Rename( const char* n ) {
  delete [] ident;
  if( n ) {
    ident = new char [ strlen(n) + 1 ];
    strcpy( ident, n );
  }
  else {
    ident = new char [8];
    strcpy( ident, "NONAME" );
  }
}

Aperture* bmlnElmnt::getAperture() {
  Aperture* app = 0;
  if(pAperture !=0)
    app = pAperture->Clone();
  return app;
}

void bmlnElmnt::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  if( p_bml || p_bml_e ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void bmlnElmnt::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )", 
           "Cannot use base ::Split function when the element is composite." ) );
  }

  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "Requested percentage = " << pc << "; not within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void bmlnElmnt::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }

  *a = Clone();
  *b = Clone();

  delete [] (*a)->ident;
  (*a)->ident = new char [ strlen(ident) + 6 ];
  strcpy( (*a)->ident, ident );
  strcat( (*a)->ident, "_1" );

  delete [] (*b)->ident;
  (*b)->ident = new char [ strlen(ident) + 6 ];
  strcpy( (*b)->ident, ident );
  strcat( (*b)->ident, "_2" );
  
  (*a)->strength = pc*strength;
  (*b)->strength = ( 1.0 - pc )*strength;
  (*a)->length = pc*length;
  (*b)->length = ( 1.0 - pc )*length;
}

void bmlnElmnt::setShunt(double a) 
{               // Set the value of the shunt, creating it if necessary
  strength += ( shuntCurrent - a ) * IToField();
  shuntCurrent = a;
}

bmlnElmnt* bmlnElmnt::clone() const
{
  return this->Clone();  // should call virtual function
}

bmlnElmnt* bmlnElmnt::Clone(const char* name) {
  // Clone this bmlnElmnt, changing the name to my argument.
  bmlnElmnt *temp = Clone();
  temp->Rename(name);
  return temp;
}

bmlnElmnt* bmlnElmnt::clone(const char* name) {
  return this->Clone(name);
}

ostream& operator<<(ostream& os, bmlnElmnt& b)
{
  if ( &b ) {
    os << OSTREAM_DOUBLE_PREC 
       << b.Type() 
       << " " 
       << b.Name() 
       << " " 
       << b.Length() 
       << " " 
       << b.Strength() 
       << " " ;
    os << (*b.align) << "\n";
    b.writeTo(os); // Polymorphically call the appropriate writeTo().
  }
  return os;
}


double bmlnElmnt::Length() const
{
  return length;
}


bmlnElmnt::Discriminator::Discriminator()
{
  // This does nothing, but its presence seems
  // to be needed by constructors of derived classes.
  // Go figure!
}


bmlnElmnt::Discriminator::Discriminator( const Discriminator& )
{
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "bmlnElmnt::Discriminator::Discriminator( const Discriminator& )", 
         "Copy constructor invoked for a Discriminator object." ) );
}


// Exceptions

bmlnElmnt::GenericException::GenericException( string fileName, int lineNumber, 
                                              const char* fcn, 
                                              const char* msg )
{
  ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    cerr << errorString;
    cerr << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

const char* bmlnElmnt::GenericException::what() const throw()
{
  return errorString.c_str();
}




