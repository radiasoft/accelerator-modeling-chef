/*
**
** Demo program:
** Calculates the Jet expansion of the 
** inverse of an "arbitrary" function
** - in this case, x + x^2 + x^3 - 
** using Newton's method.
** --- Leo Michelotti
** --- February 12, 1993
** 
*/

#include <mxyzptlk/Jet.h>

using namespace std;

main( int argc, char** argv ) {

 if( argc != 3 ) {
  printf( "Usage: aaa <degree> <iterations>\n" );
  exit(0);
 }

 int deg = atoi( argv[1] );
 int n   = atoi( argv[2] );
 
 Jet__environment::BeginEnvironment( deg );
 coord x(0.0);
 Jet__environment::EndEnvironment();

 Jet u, v;
 Jet f, g;
 int d = 1;

 f = x + x*x + x*x*x;
 g = f.D(&d);
 
 int i = 0;
 u = x;
 v = ( f(&u) - x ) / g(&u);
 while( ( i++ < n ) &&  ( v != 0.0 ) ) {
  u -= v;
  v = ( f(&u) - x ) / g(&u);
 }

 u.printCoeffs();
 f(&u).printCoeffs();
 u(&f).printCoeffs();
}
