#include "decapole.h"

// **************************************************
//   class thinDecapole 
// **************************************************

thinDecapole::thinDecapole ( double s ) : bmlnElmnt( 0.0, s ) {
 // The strength is to be interpreted as
 // B''l/Brho  in  meters^-2
}

thinDecapole::thinDecapole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
 // The strength is to be interpreted as
 // B''l/Brho  in  meters^-2
}


thinDecapole::thinDecapole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinDecapole::thinDecapole( const thinDecapole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinDecapole::~thinDecapole() {
}



const char* thinDecapole::Type() const 
{ 
  return "thinDecapole"; 
}


