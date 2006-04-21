/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.2
******                                    
******  File:      bmlnElmnt.cc
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
**************************************************************************
*************************************************************************/


#include <typeinfo>
#include <string>

#include <iomanip>
#include <iosetup.h>
#include <bmlnElmnt.h>
#include <Particle.h>
#include <ParticleBunch.h>
#include <Aperture.h>
#include <BmlVisitor.h>


#ifdef OBJECT_DEBUG
int bmlnElmnt::objectCount = 0;
#endif


// Error flags for _tag manipulation functions.
const short bmlnElmnt::BF_OK         = 0;
const short bmlnElmnt::BF_NULL_ARG   = 1;
const short bmlnElmnt::BF_BAD_START  = 2;

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

// **************************************************
//   yes and no Criteria
// **************************************************

beamline::Aye beamline::yes;
beamline::Nay beamline::no;


// **************************************************
//   class bmlnElmnt::AsinFunctor
// **************************************************

const int bmlnElmnt::AsinFunctor::_size = 13;
const double bmlnElmnt::AsinFunctor::_coeff [] = {
  1.0, 
  0.16666666666667,   0.075, 
  0.044642857142857,  0.030381944444444, 
  0.022372159090909,  0.017352764423077, 
  0.01396484375,      0.01155180089614, 
  0.0097616095291941, 0.0083903358096168, 
  0.0073125258735988, 0.0064472103118896
};


bmlnElmnt::AsinFunctor::AsinFunctor( bool m, int d )
: _n(d), _exactMode(m)
{
}


bmlnElmnt::AsinFunctor::AsinFunctor( const bmlnElmnt::AsinFunctor& x )
: _n(x._n), _exactMode(x._exactMode)
{
}


int bmlnElmnt::AsinFunctor::setNumTerms( int d )
{
  int ret = _n;
  _n = d;
  if( _n < 1 ) { _n = 1; }
  if( _size < _n ) { _n = _size; }
  return ret;
}


int bmlnElmnt::AsinFunctor::getNumTerms()
{
  return _n;
}


bool bmlnElmnt::AsinFunctor::isExact()
{
  return _exactMode;
}


bool bmlnElmnt::AsinFunctor::makeExact()
{
  bool ret = _exactMode;
  _exactMode = true;
  return ret;
}


bool bmlnElmnt::AsinFunctor::makeApproximate()
{
  bool ret = _exactMode;
  _exactMode = false;
  return ret;
}


double bmlnElmnt::AsinFunctor::operator()( double x ) const 
{
  double ret;
  if( _exactMode ) { ret = asin(x); }
  else {
    double u = x*x;
    int i = _n-1;
    double multiplier = _coeff[i--];
    while( i >= 0 ) {
      multiplier *= u;
      multiplier += _coeff[i--];
    }
    ret = x*multiplier;
  }
  return ret;
}


Jet bmlnElmnt::AsinFunctor::operator()( const Jet& x ) const
{
  if( _exactMode ) { return asin(x); }
  else {
    Jet__environment_ptr envPtr = x.Env();
    Jet u(envPtr); 
    Jet multiplier(envPtr);
    u = x*x;
    int i = _n-1;
    multiplier = _coeff[i--];
    while( i >= 0 ) {
      multiplier = u*multiplier;
      multiplier = multiplier + _coeff[i--];
    }
    return (x*multiplier);
  }
}


// **************************************************
//   struct bmlnElmntData
// **************************************************

bmlnElmntData::bmlnElmntData() {
 strcpy( type, "" );
 address  = 0;
 name     = 0;  // ??? START HERE, LEO ... Is this a problem?
 length   = 0.0;
 strength = 0.0;
 pAperture = 0;
 
 iToField = 1.0;                // 1 KA = 1 Tesla default
 shuntCurrent = 0.0;

 more     = 0;
}

bmlnElmntData::bmlnElmntData( bmlnElmntData& x ) {
 strcpy( type, x.type );
 address   = x.address;
 name      = new char [ strlen(x.name) + 1 ];
             strcpy( name, x.name );
 length    = x.length;
 strength  = x.strength;
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


// ***********************************
//   class bmlnElmnt::PinnedFrameSet
// ***********************************


bmlnElmnt::PinnedFrameSet::PinnedFrameSet()
:   _altered(false)
  , _upStream()
  , _downStream()
{
}


void bmlnElmnt::PinnedFrameSet::reset()
{
  _upStream.reset();
  _downStream.reset();
  _altered = false;
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
 pAperture = 0;
 
 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;
 // WARNING: I'm assuming that any derived class will 
 //          initialize its propagator functor within
 //          its constructor.

 #ifdef OBJECT_DEBUG
 objectCount++;
 #endif
}

bmlnElmnt::bmlnElmnt( double l /* length */, PropFunc* pf ) {
 ident        = new char [8];
                 strcpy( ident, "NONAME" );
 length       = l;
 strength     = 0.0;

 pAperture = 0;
 
 _ctRef = 0.0;

 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;
 // WARNING: I'm assuming that any derived class will 
 //          initialize its propagator functor within
 //          its constructor.

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

 _ctRef = 0.0;

 pAperture    = 0;
 
 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;
 // WARNING: I'm assuming that any derived class will 
 //          initialize its propagator functor within
 //          its constructor.

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

 _ctRef = 0.0;

 pAperture = 0;
 
 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;
 // WARNING: I'm assuming that any derived class will 
 //          initialize its propagator functor within
 //          its constructor.

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

 _ctRef = 0.0;

 pAperture = 0;
 
 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;
 // WARNING: I'm assuming that any derived class will 
 //          initialize its propagator functor within
 //          its constructor.

 #ifdef OBJECT_DEBUG
 objectCount++;
 #endif
}

bmlnElmnt::bmlnElmnt( const bmlnElmnt& a ) 
: _tag(a._tag)
{
 ident         = new char [ strlen(a.ident) + 1 ];
                 strcpy( ident, a.ident );
 length        = a.length;
 strength      = a.strength;

 _ctRef = a._ctRef;

 pAperture = 0;
 if(a.pAperture != 0) 
   pAperture = a.pAperture->Clone();
 
 iToField      = a.iToField;
 shuntCurrent  = a.getShunt();

 Propagator = a.Propagator;
 // WARNING: I'm assuming that any derived class will 
 //          initialize its propagator functor within
 //          its own copy constructor.

 _attributes = a._attributes; 
 
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
       (*pcerr) << "*** WARNING *** \n"
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


 #ifdef OBJECT_DEBUG
 objectCount++;
 #endif

}

bmlnElmnt::bmlnElmnt( bmlnElmntData& x ) {
 ident         = new char [ strlen(x.name) + 1 ];
                 strcpy( ident, x.name );
 length        = x.length;
 strength      = x.strength;

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

 (*pcerr) << "*** WARNING ***                                    \n"
         "*** WARNING *** bmlnElmnt::bmlnElmnt( bmlnElmntData& ) \n"
         "*** WARNING *** p_bml and p_bml_e are not going    \n"
         "*** WARNING *** to be copied.  Sorry.              \n"
         "*** WARNING ***                                    \n"
      << endl;
 p_bml = 0;
 p_bml_e = 0;

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
 // virtual method releasePropFunc() must
 //   be invoked within the derived class's destructor
 //   if it is necessary.
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
  Particle* p = 0;
  if( align == 0 ) {
    localPropagate  ( x );
  }
  else {
    ParticleBunch::Iterator get( x );
    while((  p = (Particle*) get.next()  )) { enterLocalFrame( *p ); }
    localPropagate  ( x );
    get.reset();
    while((  p = (Particle*) get.next()  )) { leaveLocalFrame( *p ); }
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// End: basic propagator functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Begin: tagging routines
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

short  bmlnElmnt::writeTag( const char* s )
{
  if( s == 0 ) { return BF_NULL_ARG; }
  _tag.assign(s);
  return BF_OK;
}


short  bmlnElmnt::writeTag( char c, short index )
{
  _tag.replace( index, 1, 1, c );
  return BF_OK;
}


short bmlnElmnt::writeTag( const char* s, short index )
{
  if( s == 0 ) { return BF_NULL_ARG; }
  _tag.replace( index, strlen(s), s );
  return BF_OK;
}


short  bmlnElmnt::writeTag( const std::string& s )
{
  _tag.assign(s);
  return BF_OK;
}


short  bmlnElmnt::writeTag( const std::string& s, short index )
{
  _tag.assign( s, index, s.size() );
  return BF_OK;
}


std::string bmlnElmnt::readTag() const
{
  return _tag;
}


std::string bmlnElmnt::readTag( short start, short num ) const
{
  if( 0 == _tag.size() ) { 
    return _tag; 
  }
  if( start < 0 || start + 1 > _tag.size() ) {
    start = 0;
  }
  if( start + num > _tag.size() ) {
    num = _tag.size() - start;
  }
  return _tag.substr( start, num );
}


short  bmlnElmnt::readTag( char* s ) const
{
  return this->readTag( s, 0, _tag.size() );
}


short bmlnElmnt::readTag( char* s, short start, short num ) const
{
  if( s == 0 ) {
    return BF_NULL_ARG;
  }
  if( start < 0 || start + 1 > _tag.size() ) {
    return BF_BAD_START;
  }
  if( start + num > _tag.size() ) {
    num = _tag.size() - start;
  }

  _tag.copy( s, num, start );
  return BF_OK;
}


char bmlnElmnt::readTag( short pos ) const
{
  if( (pos < 0) || (pos + 1 > _tag.size()) ) {
    pos = 0;
    (*pcerr) << "\n*** WARNING ***                              "
                "\n*** WARNING *** File: " 
             << __FILE__ << "  Line: " << __LINE__
             << "\n*** WARNING *** bmlnElmnt::readTag            "
                "\n*** WARNING *** Character position out of    "
                "\n*** WARNING *** bounds. Will return first    "
                "\n*** WARNING *** character of tag.            "
                "\n*** WARNING ***                              "
             << endl;
  }
  return (_tag.c_str())[pos];
}


short bmlnElmnt::getTagSize() const
{
  return _tag.size();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// End: tagging routines
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


void bmlnElmnt::setLength( double x ) {
  if( length < 0.0 ) {
    (*pcerr) << "*** WARNING ***                       \n"
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

  if( 0 != Propagator ) { this->setupPropFunc(); }
}


void bmlnElmnt::setStrength( double s ) {
  strength = s - getShunt()*IToField(); 
  if( 0 != Propagator ) { this->setupPropFunc(); }
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


void bmlnElmnt::releasePropFunc()
{
  ostringstream uic;
  uic  << "Base class method called by "
       << this->Type()
       << " object.";
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__
         , "void bmlnElmnt::releasePropFunc()"
         , uic.str().c_str() ) );
}


void bmlnElmnt::setupPropFunc()
{
  ostringstream uic;
  uic  << "Base class method called by "
       << this->Type()
       << " object.";
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__
         , "void bmlnElmnt::setupPropFunc()"
         , uic.str().c_str() ) );
}


void bmlnElmnt::eliminate() {
 delete this;
}


bmlnElmnt::PropFunc* bmlnElmnt::setPropFunction ( const PropFunc* a ) 
{ 
  bmlnElmnt::PropFunc* ret = this->Propagator;

  if( 0 == a ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "bmlnElmnt::setPropFunction(const PropFunc* a)",
           "Argument is null." ) );
  }
  else {
    this->releasePropFunc();  // virtual
    this->Propagator = const_cast<PropFunc*>(a);
    this->setupPropFunc();    // virtual
  }

  return ret;
}


bmlnElmnt::PropFunc* bmlnElmnt::setPropFunction ( const PropFunc& a ) 
{ 
  return this->setPropFunction( &a );
}


void bmlnElmnt::peekAt( double& s, const Particle& prt ) const
{ 
 (*pcout) << setw(12) << s;

 s += const_cast<bmlnElmnt*>(this)->OrbitLength( prt );  // Kludge!!

 (*pcout) << setw(12) << s           
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


bool bmlnElmnt::hasParallelFaces() const
{
  return true;
}


bool bmlnElmnt::hasStandardFaces() const
{
  return true;
}


bool bmlnElmnt::isSimple() const
{
  return ( (0 == p_bml) && (0 == p_bml_e)  );
}


bool bmlnElmnt::isMagnet() const
{
  return false;
}


// --- Local, small alignment routines.
// --- Changes alignment struct.
// --- Should not disturb neighboring elements.

bool bmlnElmnt::alignRelX( double u )
{
  bool ret = true;
  if( this->hasParallelFaces() ) {
    if( 0 == align ) {
      align = new alignment(u, 0.0, 0.0);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.xOffset += u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }
  return ret;
}


bool bmlnElmnt::alignRelY( double u )
{
  bool ret = true;
  if( this->hasParallelFaces() ) {
    if( 0 == align ) {
      align = new alignment(0.0, u, 0.0);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.yOffset += u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }
  return ret;
}


bool bmlnElmnt::alignAbsX( double u )
{
  bool ret = true;
  if( this->hasParallelFaces() ) {
    if( 0 == align ) {
      align = new alignment(u, 0.0, 0.0);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.xOffset = u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }
  return ret;
}


bool bmlnElmnt::alignAbsY( double u )
{
  bool ret = true;
  if( this->hasParallelFaces() ) {
    if( 0 == align ) {
      align = new alignment(0.0, u, 0.0);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.yOffset = u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }
  return ret;
}


bool bmlnElmnt::alignRelXmm( double u )
{
  return (this->alignRelX( 0.001*u ));
}


bool bmlnElmnt::alignRelYmm( double u )
{
  return (this->alignRelY( 0.001*u ));
}


bool bmlnElmnt::alignAbsXmm( double u )
{
  return (this->alignAbsX( 0.001*u ));
}


bool bmlnElmnt::alignAbsYmm( double u )
{
  return (this->alignAbsY( 0.001*u ));
}


bool bmlnElmnt::alignRelRoll( double u )
{
  bool ret = true;
  if( this->hasParallelFaces() && this->hasStandardFaces() ) {
    if( 0 == align ) {
      align = new alignment(0.0, 0.0, u);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.tilt += u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }
  return ret;
}


bool bmlnElmnt::alignAbsRoll( double u )
{
  bool ret = true;
  if( this->hasParallelFaces() && this->hasStandardFaces() ) {
    if( 0 == align ) {
      align = new alignment(0.0, 0.0, u);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.tilt = u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }
  return ret;
}


bool bmlnElmnt::alignRelRollmrad( double u )
{
  return (this->alignRelRoll( 0.001*u ));
}


bool bmlnElmnt::alignAbsRollmrad( double u )
{
  return (this->alignAbsRoll( 0.001*u ));
}


void bmlnElmnt::realign()
{
  // #error *** WARNING ***
  // #error *** WARNING ***  bmlnElmnt::realign is not finished.
  // #error *** WARNING ***

  if(0 != align ) 
  { delete align; align = 0; }

  #if 1
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File " << __FILE__ << ", Line " << __LINE__
             << "\n*** WARNING *** void bmlnElmnt::realign()"
                "\n*** WARNING *** --------------------------------------"
                "\n*** WARNING *** This routine is undergoing renovation "
                "\n*** WARNING *** and currently will only reset the "
                "\n*** WARNING *** attribute \"align.\""
                "\n*** WARNING *** "
                "\n*** WARNING *** Temporary workaround: clone your original "
                "\n*** WARNING *** beamline, before shifting elements around, "
                "\n*** WARNING *** and reuse it as needed."
                "\n*** WARNING *** "
             << endl; 
    firstTime = false;
  }
  #endif

  # if 0
  THE ISSUES:
    - if Slots have been introduced above and below the element, 
    they must be returned to drifts, if possible
    - if they were originally Slots, then they have to be reset too.

  _pinnedFrameSet.reset();
  #endif
}


void bmlnElmnt::markPins()
{
  // #error *** WARNING ***
  // #error *** WARNING ***  bmlnElmnt::markPins is not written!!!
  // #error *** WARNING ***

  #if 1
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File " << __FILE__ << ", Line " << __LINE__
             << "\n*** WARNING *** void bmlnElmnt::markPins()"
                "\n*** WARNING *** ------------------------------"
                "\n*** WARNING *** This routine is not written."
                "\n*** WARNING *** Nothing will happen."
                "\n*** WARNING *** "
             << endl; 
    firstTime = false;
  }
  #endif
}


void bmlnElmnt::loadPinnedCoordinates( const Particle& prtcl, Vector& ret, double pct )
{
  // #error *** WARNING ***
  // #error *** WARNING ***  bmlnElmnt::loadPinnedCoordinates is not tested!!
  // #error *** WARNING ***

  static Vector p( Particle::PSD / 2 );
  static Vector v( Particle::PSD / 2 );
  //                         |
  //                         +--------- Not foolproof.

  static int x  = Particle::_x();
  static int y  = Particle::_y();
  static int z  = Particle::_cdt();
  //                         |
  //                         +--------- Not quite what I want.
  static int xp = Particle::_xp();
  static int yp = Particle::_yp();
  static int zp = Particle::_dpop();
  //                         |
  //                         +--------- Not quite what I want.


  if( _pinnedFrames._altered ) 
  {
    if( pct < 0.000001 ) { pct = 0; }
    if( 0.999999 < pct ) { pct = 1; }

    Frame  ref;
    if( Particle::PSD == ret.Dim() ) {
      // Load position and momentum vectors
      p(x) = prtcl.get_x();   
      p(y) = prtcl.get_y();   
      p(z) = 0;

      v(x) = prtcl.get_npx(); 
      v(y) = prtcl.get_npy(); 
      v(z) = prtcl.get_npz();

      // Downstream end (default)
      if( (1.0 - pct) < 0.001 ) {
        _pinnedFrames._downStream.convertInPlace( p, v );
      }

      // Upstream end
      else if( pct < 0.001 ) {
        _pinnedFrames._upStream.convertInPlace( p, v );
      }

      // Somewhere in the middle
      else {
        Frame ref( Frame::tween( _pinnedFrames._upStream, _pinnedFrames._downStream, pct, false ) );
        ref.convertInPlace( p, v );
      }

      // Transfer the answer
      ret(x)  = p(x);
      ret(y)  = p(y);
      ret(z)  = prtcl.get_cdt();
      ret(xp) = v(x);
      ret(yp) = v(y);
      ret(zp) = prtcl.get_ndp();
    }  

    else {
      (*pcerr) << "\n*** WARNING *** "
               << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
               << "\n*** WARNING *** void bmlnElmnt::loadPinnedCoordinates(...)"
               << "\n*** WARNING *** Dimension of vector argument is "
               << (ret.Dim())
               << ';'
               << "\n*** WARNING *** current implementation only allows "
               << BMLN_dynDim
               << '.'
               << "\n*** WARNING *** This function will do nothing."
               << endl;
    }
  }

  else // bmlnElmnt Frames have not been altered
  {
    ret(x)  = prtcl.get_x();
    ret(y)  = prtcl.get_y();
    ret(z)  = prtcl.get_cdt();
    ret(xp) = prtcl.get_npx();
    ret(yp) = prtcl.get_npy();
    ret(zp) = prtcl.get_ndp();
  }
}


bool bmlnElmnt::setAlignment(const alignmentData& a) 
{
  bool ret = true;
  alignment* nuAlignPtr = new alignment(a);

  this->realign();

  if( nuAlignPtr->isNull() ) {
    delete nuAlignPtr;
    nuAlignPtr = 0;
  }
  else if( this->hasParallelFaces() ) {
    if( 0.0 == a.tilt ) {
      if( (a.xOffset != 0.0) || (a.yOffset != 0.0) ) {
        this->align = nuAlignPtr;
      }
    }
    else if( this->hasStandardFaces() ) {
      this->align = nuAlignPtr;
    }
    else { 
      ret = false;
    }
  }
  else {
    ret = false;
  }

  if( !ret ) {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::setAlignment(const alignmentData& a)"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing has been done."
            "\n*** WARNING *** "
         << endl;

    if( 0 != nuAlignPtr ) { delete nuAlignPtr; nuAlignPtr = 0; }
    ret = false;
  }

  // _pinnedFrames._altered is not changed

  return ret;
}


alignmentData bmlnElmnt::Alignment() const {
  alignmentData x;
  if(0 != align) {
    x = align->getAlignment();
  }
  return x;
}


// --- End of local, small alignment routines.

void bmlnElmnt::enterLocalFrame( Particle& p ) const
{
  double inState[ BMLN_dynDim ];
  double temp;
  double cs, sn;

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
 
  p.setState( inState ) ;
}


void bmlnElmnt::enterLocalFrame( JetParticle& p ) const
{
  JetVector inState ( p.State() );
  Jet       temp    ( inState.Env() );
  double    cs, sn;

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
  Jet       temp     ( outState.Env() );
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
    (*pcerr) << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** double bmlnElmnt::setReferenceTime( const Particle& )"
            "\n*** WARNING *** Invoked on base class. Nothing will happen."
            "\n*** WARNING *** This warning is issued only once."
            "\n*** WARNING *** "
         << endl;
    firstTime = false;
  }
}


double bmlnElmnt::getReferenceTime() const
{
  return _ctRef;
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

void bmlnElmnt::Split( double pc, bmlnElmnt** a, bmlnElmnt** b ) const
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
  // REMOVE: strength += ( shuntCurrent - a ) * IToField();
  this->setStrength( strength + ( shuntCurrent - a ) * IToField() );
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
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}


const char* 
bmlnElmnt::GenericException::what() const throw()
{
  return errorString.c_str();
}


boost::any& 
bmlnElmnt::operator[](const std::string& s) {

  return _attributes[s.c_str()]; 

}

bool  
bmlnElmnt::attributeExists( const std::string& s ) {

  return (_attributes.end() !=  _attributes.find( s.c_str() ) );


}

void  
bmlnElmnt::attributeClear( const std::string& s ) {

  _attributes.erase( s.c_str() ); 

}


void  
bmlnElmnt::attributeClear() {

  _attributes.clear();

}

