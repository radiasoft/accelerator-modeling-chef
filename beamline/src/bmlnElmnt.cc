#ifdef __VISUAL_CPP__
#include <iomanip>
using std::setprecision;
#else
#include <iomanip.h>
#endif
#include <beamline.inc>

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


BMLN_posInfo::BMLN_posInfo( BMLN_posInfo& x ) {
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
 
 iToField = 1.0;		// 1 KA = 1 Tesla default
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

// ??? REMOVE void bmlnElmntData::writeTo( FILE* f ) {
// ??? REMOVE  int sz = strlen( name );
// ??? REMOVE  if( sz == 0 ) {
// ??? REMOVE   printf( "\n" );
// ??? REMOVE   printf( "*** ERROR ***                                        \n" );
// ??? REMOVE   printf( "*** ERROR *** void bmlnElmntData::writeTo( FILE* )   \n" );
// ??? REMOVE   printf( "*** ERROR *** Anomoly in ident.  Quitting.           \n" );
// ??? REMOVE   printf( "*** ERROR ***                                        \n" );
// ??? REMOVE   printf( "\n" );
// ??? REMOVE   exit(0);
// ??? REMOVE  }
// ??? REMOVE 
// ??? REMOVE  fwrite( this, sizeof( bmlnElmntData ), 1, f );
// ??? REMOVE  fwrite( &sz, sizeof( int ), 1, f );
// ??? REMOVE  fprintf( f, "%s ", name );
// ??? REMOVE }

// **************************************************
//   class bmlnElmnt
// **************************************************

bmlnElmnt::bmlnElmnt( const char* n, PROPFUNC pf, JETPROPFUNC jpf ) {
 if( n ) {
  ident = new char [ strlen(n) + 1 ];
  strcpy( ident, n );
 }
 else {
  ident = new char [8];
  strcpy( ident, "NONAME" );
 }
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
 JetPropagator = jpf;

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

bmlnElmnt::bmlnElmnt( const double l /* length */, 
                      PROPFUNC pf, JETPROPFUNC jpf ) {
 ident        = new char [8];
                 strcpy( ident, "NONAME" );
 length       = l;
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
 JetPropagator = jpf;

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

bmlnElmnt::bmlnElmnt( const double l /* length */, 
                      const double s /* strength */, 
                      PROPFUNC pf, JETPROPFUNC jpf ) {
 ident        = new char [8];
                 strcpy( ident, "NONAME" );
 length       = l;
 strength     = s;
 depth        = 0;

 pAperture    = 0;
 
 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;
 JetPropagator = jpf;

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
                      const double l /* length */, 
                      PROPFUNC pf, JETPROPFUNC jpf ) {
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

 pAperture = 0;
 
 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;
 JetPropagator = jpf;

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
                      const double l /* length */, 
                      const double s /* strength */, 
                      PROPFUNC pf, JETPROPFUNC jpf ) {
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

 pAperture = 0;
 
 iToField     = 1.0;
 shuntCurrent = 0.0;

 // Pointers
 align         = 0;
 p_bml         = 0;
 p_bml_e       = 0;
 Propagator    = pf;
 JetPropagator = jpf;

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

bmlnElmnt::bmlnElmnt( bmlnElmnt& a ) {
 ident         = new char [ strlen(a.ident) + 1 ];
                 strcpy( ident, a.ident );
 length        = a.length;
 strength      = a.strength;
 depth         = a.depth;

 pAperture = 0;
 if(a.pAperture != 0) 
   pAperture = a.pAperture->Clone();
 
 iToField      = a.iToField;
 shuntCurrent  = a.getShunt();

 Propagator = a.Propagator;
 JetPropagator = a.JetPropagator;

 if(a.align != 0) {
   alignmentData data = a.align->getAlignment();
   if((data.xOffset != 0.0) || (data.yOffset != 0.0) || (data.tilt != 0.0))
     align         = new alignment(*a.align);
 } 
 else align = 0;

 if( a.p_bml )   p_bml = (beamline*) a.p_bml->Clone();
 else            p_bml = 0;

 dlist_iterator* getNext;
 if( a.p_bml_e ) {
   if( !p_bml ) {
     cerr << "\n"
             "*** ERROR ***                                       \n"
             "*** ERROR *** bmlnElmnt::bmlnElmnt( bmlnElmnt& )    \n"
             "*** ERROR *** p_bml_e is defined but p_bml is not!  \n"
             "*** ERROR ***                                       \n"
          << endl;
     exit(1);
   }

   getNext = new dlist_iterator( *(dlist*) p_bml );

   if( strcasecmp( ((bmlnElmnt*) (getNext->operator()()))->Type(), "drift" ) ) {
     cerr << "\n"
             "*** ERROR ***                                       \n"
             "*** ERROR *** bmlnElmnt::bmlnElmnt( bmlnElmnt& )    \n"
             "*** ERROR *** First element of p_bml not a drift.   \n"
             "*** ERROR ***                                       \n"
          << endl;
     exit(1);
   }

   p_bml_e = (bmlnElmnt*) (getNext->operator()());

   if( strcasecmp( ((bmlnElmnt*) (getNext->operator()()))->Type(), "drift" ) ) {
     cerr << "\n"
             "*** ERROR ***                                       \n"
             "*** ERROR *** bmlnElmnt::bmlnElmnt( bmlnElmnt& )    \n"
             "*** ERROR *** Second element of p_bml not a drift.  \n"
             "*** ERROR ***                                       \n"
          << endl;
     exit(1);
   }

   if( getNext->operator()() ) {
     cerr << "\n"
             "*** ERROR ***                                       \n"
             "*** ERROR *** bmlnElmnt::bmlnElmnt( bmlnElmnt& )    \n"
             "*** ERROR *** Too many elements in p_bml            \n"
             "*** ERROR ***                                       \n"
          << endl;
     exit(1);
   }

   delete getNext;
 }

 else          p_bml_e = 0;

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
 if( p_bml )   p_bml->eliminate();
 if( p_bml_e ) delete p_bml_e;
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

void bmlnElmnt::setStrength( const double s ) {
  strength = s - getShunt()*IToField(); 
}

void bmlnElmnt::setStrength( const double, const int ) {
 cerr << "*** ERROR *** "
      << "*** ERROR *** void bmlnElmnt::setStrength( const double s, const int index )\n"
      << "*** ERROR *** This virtual function should never \n"
      << "*** ERROR *** have been called in its base class version.\n"
      << endl;
 exit(1);
}

void bmlnElmnt::setCurrent( const double I ) {
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



void bmlnElmnt::image( bmlnElmntData* p ) {
 strcpy( p->type, Type() );
 p->address   = this;
 p->name      = new char [ strlen(ident) + 1 ];
                strcpy( p->name, ident );
 p->depth     = depth;
 p->length    = length;
 p->strength  = strength;
 p->more      = 0;
   //
   // O.K.
   //
   // p->aperture  = aperture;
 
// p->pAperture  = pAperture;
 p->pAperture = 0;
 if(pAperture != 0) 
   p->pAperture = pAperture->Clone();
 
 p->iToField  = iToField;
 p->shuntCurrent = shuntCurrent;
 if(align != 0)
   p->align   = align->getAlignment();
 else {
   p->align.xOffset = 0.0;
   p->align.yOffset = 0.0;
   p->align.tilt = 0.0;
 }
 p->geometry.inPoint  = geometry.inPoint;
 p->geometry.outPoint = geometry.outPoint;
 for( int j = 0; j < 3; j++ ) {
   p->geometry.inAxes[j]  = geometry.inAxes[j];
   p->geometry.outAxes[j] = geometry.outAxes[j];
 }
}


void bmlnElmnt::peekAt( double& s, Particle* p_prt ) {
 cout << s << "   ";
 s += OrbitLength( *p_prt );
 cout << s           << " :  (" 
      << (int) this  << ")  "
      << ident       << "  " 
      << Type()      << "  " 
      << length      << "  " 
      << strength    << "  "
      << shuntCurrent
      << endl;
}


bmlnElmntData* bmlnElmnt::image() {
 bmlnElmntData* p = new bmlnElmntData;
 strcpy( p->type, Type() );

 p->address   = this;
 p->name      = new char [ strlen(ident) + 1 ];
                   strcpy( p->name, ident );
 p->depth     = depth;
 p->length    = length;
 p->strength  = strength;
 p->more      = 0;
   //
   // O.K.
   //
   // p->aperture  = aperture;

// p->pAperture  = pAperture;
 p->pAperture = 0;
 if(pAperture != 0) 
   p->pAperture = pAperture->Clone();
 
 p->iToField  = iToField;
 p->shuntCurrent = shuntCurrent;

 if(align != 0)
   p->align     = align->getAlignment();
 else {
   p->align.xOffset = 0.0;
   p->align.yOffset = 0.0;
   p->align.tilt = 0.0;
 }

 p->geometry.inPoint  = geometry.inPoint;
 p->geometry.outPoint = geometry.outPoint;
 for( int j = 0; j < 3; j++ ) {
   p->geometry.inAxes[j]  = geometry.inAxes[j];
   p->geometry.outAxes[j] = geometry.outAxes[j];
 }

 return p;
}



void bmlnElmnt::image( int d, slist* s, BMLN_posInfo* cg ) {
 bmlnElmntData* p = bmlnElmnt::image();

 strcpy( p->type, Type() );
 p->address   = this;
 p->depth     = d;

 geomToEnd( *cg );
 cg->outPoint = geometry.outPoint;
 int j;
 for( j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];

     p->geometry.inPoint  = geometry.inPoint;
     p->geometry.outPoint = geometry.outPoint;
     for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
     for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];

 s->append( p );
}


void bmlnElmnt::setAlignment(const alignmentData& a) {
  if(align != 0) {
    delete align;
    align = 0;
  }
  if((a.xOffset != 0.0) || (a.yOffset != 0.0) || (a.tilt != 0.0))
    align = new alignment(a);
}

alignmentData bmlnElmnt::Alignment() {
  alignmentData x;
  if(align != 0) {
    x = align->getAlignment();
  }
  return x;
}

void bmlnElmnt::setAperture( Aperture* pAperture_in ) {
    //
    // aperture = x;
    //
  pAperture = pAperture_in;
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
    cerr << "\n"
            "*** ERROR ***                                    \n"
            "*** ERROR *** bmlnElmnt::Split                   \n"
            "*** ERROR *** Cannot use base ::Split function   \n"
            "*** ERROR *** when the element is composite.     \n"
            "*** ERROR ***                                    \n"
         << endl;
    exit (1);
  }

  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    cerr << "\n"
            "*** ERROR ***                                    \n"
            "*** ERROR *** bmlnElmnt::Split                   \n"
            "*** ERROR *** pc = " << pc << 
                               "and is out of bounds.         \n"
            "*** ERROR ***                                    \n"
         << endl;
    exit (1);
  }

  *a = Clone();
  *b = Clone();

  delete [] (*a)->ident;
  (*a)->ident = new char [ strlen(ident) + 6 ];
  strcpy( (*a)->ident, ident );
  strcat( (*a)->ident, "_CL_A" );

  delete [] (*b)->ident;
  (*b)->ident = new char [ strlen(ident) + 6 ];
  strcpy( (*b)->ident, ident );
  strcat( (*b)->ident, "_CL_B" );
  
  (*a)->strength = pc*strength;
  (*b)->strength = ( 1.0 - pc )*strength;
  (*a)->length = pc*length;
  (*b)->length = ( 1.0 - pc )*length;
}

void bmlnElmnt::setShunt(double a) 
{		// Set the value of the shunt, creating it if necessary
  strength += ( shuntCurrent - a ) * IToField();
  shuntCurrent = a;
}

