#include "beamline.inc"

// **************************************************
//   class thin16pole 
// **************************************************

thin16pole::thin16pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin16pole::thin16pole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin16pole::thin16pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin16pole::thin16pole( const thin16pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin16pole::~thin16pole() {
}
