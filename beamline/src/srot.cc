#include "srot.h"

// **************************************************
//   class srot
// **************************************************

srot::srot() : bmlnElmnt() {
}

srot::srot( double s) : bmlnElmnt(0.0,s) {
}

srot::srot( char* n ) : bmlnElmnt(n) {
}

srot::srot( char* n, double s) : bmlnElmnt( n, 0.0,s) {
}

srot::srot( const srot& x )
: bmlnElmnt( (bmlnElmnt&) x )
{
}

srot::~srot() {
}


const char* srot::Type() const 
{ 
  return "srot"; 
}


