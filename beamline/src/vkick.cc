#include "beamline.inc"

// **************************************************
//   class vkick 
// **************************************************

vkick::vkick() : bmlnElmnt() {
}

vkick::vkick( double k ) : bmlnElmnt() {
                      // Unfortunately, this is a
 strength = k;        // special case. Normally a
                      // single "double" argument
}                     // indicates length.

vkick::vkick( char* n ) : bmlnElmnt(n) {
}

vkick::vkick( char* n, double k ) : bmlnElmnt(n) {
                      // Unfortunately, this is a
 strength = k;        // special case. Normally a
                      // single "double" argument
}                     // indicates length.

vkick::vkick( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

vkick::vkick( const vkick& x )
: bmlnElmnt( (bmlnElmnt&) x )
{
}

vkick::~vkick() {
}

// ??? REMOVE void vkick::image( int d, slist* s, BMLN_posInfo* cg ) {
// ??? REMOVE  bmlnElmntData* p = bmlnElmnt::image();
// ??? REMOVE 
// ??? REMOVE  strcpy( p->type, "vkick" );
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

