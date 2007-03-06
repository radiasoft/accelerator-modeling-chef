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

#include <stdlib.h>
#include <stdio.h>
#include <mxyzptlk/Jet.h>

using namespace std;

main( int argc, char** argv ) {

 if( argc != 3 ) {
  printf( "Usage: Limit_E_Test <degree> <iterations>\n" );
  exit(0);
 }

 int deg = atoi( argv[1] );
 int n   = atoi( argv[2] );

 Jet__environment::BeginEnvironment( deg );
 coord x(0.0);
 Jet__environment::EndEnvironment();

 Jet u;
 Jet f, g;
 int d = 1;

 f = x + x*x + x*x*x;
 g = f.D(&d);
 
 u = x;
 for( int i = 0; i < n; i++ ) {
  u -= ( f(&u) - x ) / g(&u);
  u.peekAt();
 }

 f(&u).peekAt();
}
