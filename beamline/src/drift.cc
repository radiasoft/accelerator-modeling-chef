#include "beamline.inc"

// **************************************************
//   class drift 
// **************************************************

drift::drift() : bmlnElmnt() {
}

drift::drift( char* n ) : bmlnElmnt(n) {
}

drift::drift( double l ) : bmlnElmnt(l) {
}

drift::drift( char* n, double l ) : bmlnElmnt( n, l ) {
}

drift::drift( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

drift::drift( const drift& x ) 
: bmlnElmnt( (bmlnElmnt&) x ) 
{
}

drift::~drift() {
}

// ??? REMOVE void drift::image( int d, slist* s, BMLN_posInfo* cg ) {
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


// ??? REMOVE  bmlnElmnt* drift::Clone()
// ??? REMOVE  {
// ??? REMOVE   bmlnElmnt* a;
// ??? REMOVE   a = (bmlnElmnt*) new drift( *this ); 
// ??? REMOVE   return a;
// ??? REMOVE  }
