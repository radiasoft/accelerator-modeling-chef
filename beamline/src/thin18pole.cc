#include "beamline.inc"

// **************************************************
//   class thin18pole 
// **************************************************

thin18pole::thin18pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin18pole::thin18pole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin18pole::thin18pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin18pole::thin18pole( const thin18pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin18pole::~thin18pole() {
}


const char* thin18pole::Type() const 
{ 
  return "thin18pole"; 
}
