#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "lambertson.h"

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


void thinLamb::eliminate() {
 delete this;
}


const char* thinLamb::Type() const 
{ 
  return "thinLamb"; 
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
