#include "thinpoles.h"

// **************************************************
//   class thin12pole 
// **************************************************

thin12pole::thin12pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin12pole::thin12pole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin12pole::thin12pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin12pole::thin12pole( const thin12pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin12pole::~thin12pole() {
}


const char* thin12pole::Type() const 
{ 
  return "thin12pole"; 
}


// **************************************************
//   class thin14pole 
// **************************************************

thin14pole::thin14pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin14pole::thin14pole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin14pole::thin14pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin14pole::thin14pole( const thin14pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin14pole::~thin14pole() {
}


const char* thin14pole::Type() const 
{ 
  return "thin14pole"; 
}


// **************************************************
//   class thin16pole 
// **************************************************

thin16pole::thin16pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin16pole::thin16pole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin16pole::thin16pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin16pole::thin16pole( const thin16pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin16pole::~thin16pole() {
}

const char* thin16pole::Type() const 
{ 
  return "thin16pole"; 
}


// **************************************************
//   class thin18pole 
// **************************************************

thin18pole::thin18pole ( double s ) : bmlnElmnt( 0.0, s ) {
}

thin18pole::thin18pole ( char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
}


thin18pole::thin18pole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thin18pole::thin18pole( const thin18pole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thin18pole::~thin18pole() {
}


const char* thin18pole::Type() const 
{ 
  return "thin18pole"; 
}


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


