#include <iomanip.h>
#include "beamline.inc"


// **************************************************
//   struct thinSeptumData
// **************************************************

thinSeptumData::thinSeptumData() : bmlnElmntData() 
{
 strengthPos = 0.0;
 strengthNeg = 0.0;
 xWire = 0.0;
}

thinSeptumData::thinSeptumData( thinSeptumData& x ) : bmlnElmntData( (bmlnElmntData&) x ) 
{
 strengthPos = x.strengthPos;
 strengthNeg = x.strengthNeg;
 xWire = x.xWire;
}

thinSeptumData::~thinSeptumData() { 
}

void thinSeptumData::eliminate() {
 delete this;
}

void* thinSeptumData::clone() {
 void* p = new thinSeptumData( *this );
 return p;
}


// **************************************************
//   class thinSeptum
// **************************************************

thinSeptum::thinSeptum() : bmlnElmnt() {
  cerr << "\n*** ERROR *** Cannot define thinSeptum without parameters.\n" 
       << endl;
  exit(1);
}

thinSeptum::thinSeptum( char* n )
: bmlnElmnt( n ) {
  strengthPos = 0.0;
  strengthNeg = 0.0;
  xWire = 0.0;
}

thinSeptum::thinSeptum( char* n, double sP, double sN, double xw)
: bmlnElmnt( n ) {
  strengthPos = sP;
  strengthNeg = sN;
  xWire = xw;
}

thinSeptum::thinSeptum( double sP, double sN, double xw)
: bmlnElmnt( ) {
  strengthPos = sP;
  strengthNeg = sN;
  xWire = xw;
}

thinSeptum::thinSeptum( thinSeptumData& x) : bmlnElmnt( (bmlnElmntData&) x ) 
{
 strengthPos = x.strengthPos;
 strengthNeg = x.strengthNeg;
 xWire = x.xWire;
}


thinSeptum::thinSeptum( const thinSeptum& x ) : bmlnElmnt( (bmlnElmnt&) x )
{
  strengthPos = x.strengthPos;
  strengthNeg = x.strengthNeg;
  xWire = x.xWire;
}


thinSeptum::~thinSeptum() {
}


void thinSeptum::setStrengths( const double sPos, const double sNeg ) {
 strengthPos = sPos;
 strengthNeg = sNeg;
}

void thinSeptum::setWire( const double x) {
 xWire = x;
}

thinSeptumData* thinSeptum::image() {
  thinSeptumData* p = new thinSeptumData;
  bmlnElmnt::image( (bmlnElmntData*) p );
  
  p->more = 1;
  p->strengthPos = strengthPos;
  p->strengthNeg = strengthNeg;
  p->xWire = xWire;

  return p;
}

void thinSeptum::image( int d, slist* s, BMLN_posInfo* cg ) {
 int       j;
 thinSeptumData* p = new thinSeptumData;
 bmlnElmnt::image( (bmlnElmntData*) p );

 p->address   = this;
 p->depth     = d;
 p->more      = 1;
 p->strengthPos = strengthPos;
 p->strengthNeg = strengthNeg;
 p->xWire     = xWire;

 geomToEnd( *cg );
 cg->outPoint = geometry.outPoint;
 for( j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];

 p->geometry.inPoint  = geometry.inPoint;
 p->geometry.outPoint = geometry.outPoint;
 for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
 for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];

 s->append( p );
}


void thinSeptum::eliminate() {
 delete this;
}

istream& thinSeptum::readFrom(istream& is) 
{
  is >> strengthPos >> strengthNeg >> xWire;
  return is;
}

ostream& thinSeptum::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC << strengthPos << " " << strengthNeg << " " << xWire << "\n";
  return os;
}

