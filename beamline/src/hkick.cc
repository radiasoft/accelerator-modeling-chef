#include "beamline.inc"

// **************************************************
//   class hkick
// **************************************************

hkick::hkick() : bmlnElmnt() {
}

hkick::hkick( double k ) : bmlnElmnt() {
 strength = k;
}

hkick::hkick( char* n ) : bmlnElmnt(n) {
}

hkick::hkick( char* n, double k ) 
: bmlnElmnt( n ) 
{
 strength = k;      // Unfortunately, this is a
                    // special case. Normally a
}                   // single "double" argument
                    // indicates length.

hkick::hkick( const hkick& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

hkick::hkick( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

hkick::~hkick() {
}


// ??? REMOVE void hkick::image( int d, slist* s, BMLN_posInfo* cg ) {
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
