/*
**
** Demo program:
** Calculates the Jet expansion of the
** inverse of an "arbitrary" function
** specified by a set of coefficients
** on the command line.
** --- Leo Michelotti
** --- August 4, 1993
**
*/

#include <stdlib.h>
#include <stdio.h>
#include "mxyzptlk.h"

main( int argc, char** argv ) {

 if( argc == 1 ) {
  printf( "Usage: bbb <degree> <n> <c_1> <c_2> ... <c_n>\n" );
  exit(0);
 }

 int i = 0;
 int deg = atoi( argv[1] );
 Jet::BeginEnvironment( deg );
 coord x(0.0);
 Jet::EndEnvironment();

 int n = atoi( argv[2] );
 double* c = new double [n];
 for( i = 0; i < n; i++ )
  c[i] = atof( argv[i+3] );

 if( c[0] == 0.0 ) {
  printf( "\nERROR: The coefficient of x vanishes.\n" );
  exit(0);
 }

 Jet u, v;
 Jet f, g;
 int d = 1;

 f = c[n-1];
 if( n > 1 )
  for( i = n-2; i >= 0; i-- )
   f = f*x + c[i];
 f *= x;
 g = f.D(&d);

 u = x / c[0];
 v = ( f(&u) - x ) / g(&u);
 while( v != 0.0 ) {
  u -= v;
  v = ( f(&u) - x ) / g(&u);
 }

 printf( "\nThe answers: __________________________ \n" );
 f.peekAt();
 u.peekAt();
 f(&u).peekAt();
 u(&f).peekAt();
}
