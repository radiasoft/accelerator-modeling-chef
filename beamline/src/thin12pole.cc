#include "beamline.inc"

// **************************************************
//   class thin12pole 
// **************************************************

thin12pole::thin12pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin12pole::thin12pole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin12pole::thin12pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin12pole::thin12pole( const thin12pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin12pole::~thin12pole() {
}


char* thin12pole::Type() const 
{ 
  return "thin12pole"; 
}
