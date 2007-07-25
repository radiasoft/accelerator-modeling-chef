/*
**
** Demo program:
** Calculates the Jet expansion of the 
** inverse of an "arbitrary" function
** - in this case, x + x^2 + x^3 - 
** using Newton's method with a constant
** denominator.
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
  printf( "Usage: Limit_F_Test <degree> <iterations>\n" );
  exit(0);
 }

 int deg = atoi( argv[1] );
 int n   = atoi( argv[2] );

 Jet__environment::BeginEnvironment( deg );
 coord x(0.0);
 Jet__environment::EndEnvironment();

 Jet u;
 Jet f;
 double g;
 IntArray d(1);
 d[0] = 1;

 f = x + x*x + x*x*x;
 g = f.derivative(d);
 
 u = x;
 std::vector<Jet> w(1);
 w[0] = u;
 for( int i = 0; i < n; i++ ) {
  u -= ( f(w) - x ) / g;
  w[0] = u;
 }

 u.printCoeffs();
 f(w).printCoeffs();
}
