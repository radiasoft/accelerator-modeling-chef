#include "beamline.inc"

// **************************************************
//   class thinMultipole 
// **************************************************

thinMultipole::thinMultipole ( double s ) : bmlnElmnt( 0.0, s ) {
 static char firstCall = 1;

 if ( firstCall ) {
   printf( "\n*** SORRY:                                         " );
   printf( "\n*** SORRY: class thinMultipole is not implemented. " );
   printf( "\n*** SORRY: A marker will be substituted            " );
   printf( "\n*** SORRY: for each instance.   Length and tilt    " );
   printf( "\n*** SORRY: information are being ignored.          " );
   printf( "\n*** SORRY:                                       \n" );
   firstCall = 0;
 }

}

thinMultipole::thinMultipole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
 static char firstCall = 1;

 if ( firstCall ) {
   printf( "\n*** SORRY:                                         " );
   printf( "\n*** SORRY: class thinMultipole is not implemented. " );
   printf( "\n*** SORRY: A marker will be substituted            " );
   printf( "\n*** SORRY: for each instance.   Length and tilt    " );
   printf( "\n*** SORRY: information are being ignored.          " );
   printf( "\n*** SORRY:                                       \n" );
   firstCall = 0;
 }

}


thinMultipole::thinMultipole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinMultipole::thinMultipole( const thinMultipole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinMultipole::~thinMultipole() {
}



const char* thinMultipole::Type() const 
{ 
  return "thinMultipole"; 
}


// ??? REMOVE void thinMultipole::image( int d, slist* s, BMLN_posInfo* cg ) {
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

