#include "beamline.inc"

// **************************************************
//   class thinQuad
// **************************************************

thinQuad::thinQuad( double s ) : bmlnElmnt() {
 strength = s;      // B'L in Tesla
}

thinQuad::thinQuad( char* n, double s ) : bmlnElmnt(n) {
 strength = s;
}

thinQuad::thinQuad( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinQuad::thinQuad( const thinQuad& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinQuad::~thinQuad() {
}

// ??? REMOVE void thinQuad::image( int d, slist* s, BMLN_posInfo* cg ) {
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
void thinQuad::eliminate() {
 delete this;
}


const char* thinQuad::Type() const { 
  return "thinQuad"; 
}
