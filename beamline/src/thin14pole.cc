#include "beamline.inc"

// **************************************************
//   class thin14pole 
// **************************************************

thin14pole::thin14pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin14pole::thin14pole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin14pole::thin14pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin14pole::thin14pole( const thin14pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin14pole::~thin14pole() {
}


char* thin14pole::Type() const 
{ 
  return "thin14pole"; 
}
