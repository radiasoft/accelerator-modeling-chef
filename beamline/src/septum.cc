#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "septum.h"


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

thinSeptum::thinSeptum( const thinSeptum& x ) : bmlnElmnt( (bmlnElmnt&) x )
{
  strengthPos = x.strengthPos;
  strengthNeg = x.strengthNeg;
  xWire = x.xWire;
}


thinSeptum::~thinSeptum() {
}


void thinSeptum::setStrengths( double sPos, double sNeg ) {
 strengthPos = sPos;
 strengthNeg = sNeg;
}

void thinSeptum::setWire( double x) {
 xWire = x;
}


void thinSeptum::eliminate() {
 delete this;
}


const char* thinSeptum::Type() const 
{ 
  return "thinSeptum"; 
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
