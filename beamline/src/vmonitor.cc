#include "beamline.inc"

// **************************************************
//   class vmonitor 
// **************************************************

vmonitor::vmonitor() : monitor() {
}

vmonitor::vmonitor( char* n ) : monitor( n ){
}

vmonitor::vmonitor( char* n, double l ) : monitor( n,l ){
}

vmonitor::vmonitor( FILE* of ) : monitor( of ) {
}

vmonitor::vmonitor( char* n, FILE* of ) : monitor( n, of ) {
}

vmonitor::vmonitor( bmlnElmntData& x ) : monitor( x ) {
}

vmonitor::vmonitor( const vmonitor& x ) 
: monitor( (monitor&) x ) 
{
}

vmonitor::~vmonitor() {
}

double vmonitor::operator[]( int n ) {
 if( n == 0 ) return rgr[1];
 if( n == 1 ) return rgr[4];
 return 0.0;
}


char* vmonitor::Type() const 
{ 
  return "vmonitor"; 
}
