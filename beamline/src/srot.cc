#include "beamline.inc"

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

srot::srot( bmlnElmntData& x ) : bmlnElmnt( x ) {
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


// ??? REMOVE void srot::image( int d, slist* s, BMLN_posInfo* cg ) {
// ??? REMOVE  bmlnElmntData* p = bmlnElmnt::image();
// ??? REMOVE 
// ??? REMOVE  strcpy( p->type, Type() );
// ??? REMOVE  p->address   = this;
// ??? REMOVE  p->depth     = d;
// ??? REMOVE 
// ??? REMOVE  geomToEnd( *cg );
// ??? REMOVE  cg->outPoint = geometry.outPoint;
// ??? REMOVE  for( int j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];
// ??? REMOVE 
// ??? REMOVE      p->geometry.inPoint  = geometry.inPoint;
// ??? REMOVE      p->geometry.outPoint = geometry.outPoint;
// ??? REMOVE      for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
// ??? REMOVE      for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];
// ??? REMOVE 
// ??? REMOVE  s->append( p );
// ??? REMOVE }
// ??? REMOVE 
