#include "beamline.inc"

// **************************************************
//   class thinDecapole 
// **************************************************

thinDecapole::thinDecapole ( double s ) : bmlnElmnt( 0.0, s ) {
 // The strength is to be interpreted as
 // B''l/Brho  in  meters^-2
}

thinDecapole::thinDecapole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
 // The strength is to be interpreted as
 // B''l/Brho  in  meters^-2
}


thinDecapole::thinDecapole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinDecapole::thinDecapole( const thinDecapole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinDecapole::~thinDecapole() {
}



const char* thinDecapole::Type() const 
{ 
  return "thinDecapole"; 
}


// ??? REMOVE void thinDecapole::image( int d, slist* s, BMLN_posInfo* cg ) {
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
