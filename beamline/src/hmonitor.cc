#include "beamline.inc"

// **************************************************
//   class hmonitor 
// **************************************************

hmonitor::hmonitor() : monitor() {
}

hmonitor::hmonitor( char* n ) : monitor( n ){
}

hmonitor::hmonitor( FILE* of ) : monitor( of ) {
}

hmonitor::hmonitor( char* n, FILE* of ) : monitor( n, of ) {
}

hmonitor::hmonitor( bmlnElmntData& x ) : monitor( x ) {
}

hmonitor::hmonitor( const hmonitor& x ) 
: monitor( (monitor&) x ) 
{
}

hmonitor::~hmonitor() {
}

double hmonitor::operator[]( int n ) {
 if( n == 0 ) return rgr[0];
 if( n == 1 ) return rgr[3];
 return 0.0;
}


char* hmonitor::Type() const 
{ 
  return "hmonitor"; 
}
