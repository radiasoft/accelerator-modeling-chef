#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "beamline.inc"

// **************************************************
//   struct thinLambData
// **************************************************

thinLambData::thinLambData() : bmlnElmntData() 
{
 xSeptum = 0.0;
 ExtBeamline = 0;
 for (int i =0; i < 6 ; i++ ) RefState[i] = 0.0;
}

thinLambData::thinLambData( thinLambData& x ) : bmlnElmntData( (bmlnElmntData&) x ) 
{
 xSeptum = x.xSeptum;
 ExtBeamline = x.ExtBeamline;
 for (int i =0; i < 6 ; i++ ) RefState[i] = x.RefState[i];
}

thinLambData::~thinLambData() { 
}

void thinLambData::eliminate() {
 delete this;
}

void* thinLambData::clone() {
 void* p = new thinLambData( *this );
 return p;
}


// **************************************************
//   class thinLamb
// **************************************************

thinLamb::thinLamb() : bmlnElmnt() {
  xSeptum = 0.0;
  ExtBeamline = 0;
  for (int i =0; i < 6 ; i++ ) RefState[i] = 0.0;
}

thinLamb::thinLamb( char* n)
: bmlnElmnt( n ) {
  xSeptum = 0.0;
  ExtBeamline = 0;
  for (int i =0; i < 6 ; i++ ) RefState[i] = 0.0;
}

thinLamb::thinLamb( char* n, double x, beamline* b, double* s)
: bmlnElmnt( n ) {
  xSeptum = x;
  ExtBeamline = b;
  for (int i =0; i < 6 ; i++ ) RefState[i] = s[i];
}

thinLamb::thinLamb( double x, beamline* b, double* s)
: bmlnElmnt( ) {
  xSeptum = x;
  ExtBeamline = b;
  for (int i =0; i < 6 ; i++ ) RefState[i] = s[i];
}

thinLamb::thinLamb( thinLambData& x) : bmlnElmnt( (bmlnElmntData&) x ) 
{
  xSeptum = x.xSeptum;
  ExtBeamline = x.ExtBeamline;
  for (int i =0; i < 6 ; i++ ) RefState[i] = x.RefState[i];
}

thinLamb::thinLamb( const thinLamb& x ) : bmlnElmnt( (bmlnElmnt&) x )
{
  xSeptum = x.xSeptum;
  ExtBeamline = x.ExtBeamline;
  for (int i =0; i < 6 ; i++ ) RefState[i] = x.RefState[i];
}

thinLamb::~thinLamb() {
}

void thinLamb::setSeptum( double x) {
 xSeptum = x;
}

void thinLamb::setBeamline( beamline* b) {
 ExtBeamline = b;
}

void thinLamb::setRefState( const double* x) {
  for (int i =0; i < 6 ; i++ ) RefState[i] = x[i];
}

void thinLamb::getRefState( double* x) {
  for (int i =0; i < 6 ; i++ ) x[i] = RefState[i];
}

thinLambData* thinLamb::image() {
  thinLambData* p = new thinLambData;
  bmlnElmnt::image( (bmlnElmntData*) p );
  
  p->more = 1;
  p->xSeptum = xSeptum;
  p->ExtBeamline = ExtBeamline;
  for (int i =0; i < 6 ; i++ ) p->RefState[i] = RefState[i];
  return p;
}

void thinLamb::image( int d, slist* s, BMLN_posInfo* cg ) {
 int       j;
 thinLambData* p = new thinLambData;
 bmlnElmnt::image( (bmlnElmntData*) p );

 p->address   = this;
 p->depth     = d;
 p->more      = 1;
 p->xSeptum = xSeptum;
 p->ExtBeamline = ExtBeamline;
 for (int i =0; i < 6 ; i++ ) p->RefState[i] = RefState[i];

 geomToEnd( *cg );
 cg->outPoint = geometry.outPoint;
 for( j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];

 p->geometry.inPoint  = geometry.inPoint;
 p->geometry.outPoint = geometry.outPoint;
 for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
 for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];

 s->append( p );
}


void thinLamb::eliminate() {
 delete this;
}

ostream& thinLamb::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC << xSeptum;
  for ( int i = 0; i < 6; i++) {
    os  << " " << RefState[i];
  }
  os << "\n";
  return os;
}

istream& thinLamb::readFrom(istream& is) 
{
  is >> xSeptum;
  for ( int i = 0; i < 6; i++) {
    is >> RefState[i];
  }
  return is;
}
