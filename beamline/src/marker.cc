#include "beamline.inc"

// **************************************************
//   class marker 
// **************************************************

marker::marker() : bmlnElmnt() {
}

marker::marker( char* n ) : bmlnElmnt(n) {
}

marker::marker( const marker& x ) : bmlnElmnt( (bmlnElmnt&) x ) {
}

marker::marker( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

marker::~marker() {
}



char* marker::Type() const 
{ 
  return "marker"; 
}


// ??? REMOVE void marker::image( int d, slist* s, BMLN_posInfo* cg ) {
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
