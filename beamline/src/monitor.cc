#include "beamline.inc"

// **************************************************
//   struct monitorData
// **************************************************

monitorData::monitorData() : bmlnElmntData() {
 more = 1;
 onOff = 0;
 driftFraction = 0.5;
}

monitorData::monitorData( monitorData& x  ) : bmlnElmntData( (bmlnElmntData&) x ) {
 more = x.more;
 onOff = x.onOff;
 driftFraction = x.driftFraction;
}

monitorData::~monitorData() {
}


void monitorData::eliminate() {
 delete this;
}

void* monitorData::clone() {
 void* p = new monitorData( *this );
 return p;
}

// ??? REMOVE void monitorData::writeTo( FILE* f ) {
// ??? REMOVE  int sz = strlen( name );
// ??? REMOVE  if( sz == 0 ) {
// ??? REMOVE   printf( "\n" );
// ??? REMOVE   printf( "*** ERROR ***                                        \n" );
// ??? REMOVE   printf( "*** ERROR *** void monitorData::writeTo( FILE* )  \n" );
// ??? REMOVE   printf( "*** ERROR *** Anomoly in ident.  Quitting.           \n" );
// ??? REMOVE   printf( "*** ERROR ***                                        \n" );
// ??? REMOVE   printf( "\n" );
// ??? REMOVE   exit(0);
// ??? REMOVE  }
// ??? REMOVE 
// ??? REMOVE  fwrite( this, sizeof( monitorData ), 1, f );
// ??? REMOVE  fwrite( &sz, sizeof( int ), 1, f );
// ??? REMOVE  fprintf( f, "%s ", name );
// ??? REMOVE }
// ??? REMOVE 

// **************************************************
//   class monitor 
// **************************************************

monitor::monitor() : bmlnElmnt() {
 outputFile = stdout;
 onOff      = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( char* n ) : bmlnElmnt(n) {
 outputFile = stdout;
 onOff      = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( char* n, double l ) : bmlnElmnt(n, l) {
 outputFile = stdout;
 onOff      = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( FILE* of ) : bmlnElmnt() {
 outputFile = of;
 onOff      = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( char* n, FILE* of ) : bmlnElmnt(n) {
 outputFile = of;
 onOff      = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( bmlnElmntData& x ) : bmlnElmnt( x ) {
 onOff = 0;
 driftFraction = 0.5;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = 0.0;
}

monitor::monitor( const monitor& x )
: bmlnElmnt( (bmlnElmnt&) x )
{
 onOff = x.onOff;
 driftFraction = x.driftFraction;
 rgr   = new double [ BMLN_dynDim ];
 for( int i = 0; i < BMLN_dynDim; i++ ) rgr[i] = x.rgr[i];
}

monitor::~monitor() {
 delete [] rgr;
}

void monitor::on() {
 onOff = 1;
}

void monitor::off() {
 onOff = 0;
}

void monitor::setOutputFile( FILE* of ) {
 outputFile = of;
}


char* monitor::Type() const 
{ 
  return "monitor"; 
}


/* ??? REMOVE */ void monitor::image( int d, slist* s, BMLN_posInfo* cg ) {
/* ??? REMOVE */  bmlnElmntData* p = bmlnElmnt::image();
/* ??? REMOVE */ 
/* ??? REMOVE */  strcpy( p->type, Type() );
/* ??? REMOVE */  p->address   = this;
/* ??? REMOVE */  p->depth     = d;
/* ??? REMOVE */ 
/* ??? REMOVE */  geomToEnd( *cg );
/* ??? REMOVE */  cg->outPoint = geometry.outPoint;
                  int j; // O.K.

/* ??? REMOVE */  for( j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];
/* ??? REMOVE */ 
/* ??? REMOVE */      p->geometry.inPoint  = geometry.inPoint;
/* ??? REMOVE */      p->geometry.outPoint = geometry.outPoint;
/* ??? REMOVE */      for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
/* ??? REMOVE */      for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];
/* ??? REMOVE */ 
/* ??? REMOVE */  s->append( p );
/* ??? REMOVE */ }
/* ??? REMOVE */ 
double monitor::operator[]( int n ) {
 if( n < 0 || n > 5 ) return 0.0;
 return rgr[n];
}

ostream& monitor::writeTo(ostream &os) {
  if ( Length() != 0 ) 
    os << driftFraction << endl;
  return os;
}

istream& monitor::readFrom(istream &is) {
  if ( Length() != 0 ) {
    is >> driftFraction ;
    if ( driftFraction < 0 || driftFraction > 1 ) {
      cerr << "monitor::readFrom(istream&): Read a drift fraction of " << driftFraction << "; substituting 0.5" << endl;
      driftFraction = 0.5;
    }
  } else {
    driftFraction = 0.5; // Basically irrelevant if the monitor has no length!
  }
  return is;
}
