#include "drift.h"

// **************************************************
//   class drift 
// **************************************************

drift::drift() : bmlnElmnt() {
}

drift::drift( char* n ) : bmlnElmnt(n) {
}

drift::drift( double l ) : bmlnElmnt(l) {
}

drift::drift( char* n, double l ) : bmlnElmnt( n, l ) {
}

drift::drift( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

drift::drift( const drift& x ) 
: bmlnElmnt( (bmlnElmnt&) x ) 
{
}

drift::~drift() {
}


const char* drift::Type() const 
{ 
  return "drift"; 
}
