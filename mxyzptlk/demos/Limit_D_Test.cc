/*
**
** Demo program:
** Calculates the Jet expansion of arcsin 
** using Newton's method.
** --- Leo Michelotti
** --- February 12, 1993
** 
*/

#include <stdlib.h>
#include <stdio.h>
#include "mxyzptlk.h"

main( int argc, char** argv ) {

 if( argc != 3 ) {
  printf( "Usage: Limit_D_Test <degree> <iterations>\n" );
  exit(0);
 }

 int deg = atoi( argv[1] );
 int n   = atoi( argv[2] );

 Jet::BeginEnvironment( deg );
 coord x(0.0);
 Jet::EndEnvironment();

 Jet u;
 u = x;
 for( int i = 0; i < n; i++ ) 
  u -= ( sin(u) - x ) / cos(u);
 u.printCoeffs();
}
