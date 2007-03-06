/*
**
** Demo program:
** Calculates the arctangent function and 
** derivatives at a point specified on the
** command line.
** --- Leo Michelotti
** --- February 12, 1993
** 
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <mxyzptlk/Jet.h>

using namespace std;

main( int argc, char** argv ) {

 const int  dim       = 1;

 if( argc != 4 ) {
  printf( "Usage: Limit_B_Test <point of evaluation> <iterations> <max order>\n" );
  exit(0);
 }

 double  x          = atof( argv[1] );
 int     iterations = atoi( argv[2] );
 int     maxWeight  = atoi( argv[3] );

 Jet__environment::BeginEnvironment( maxWeight );
 coord m( x );
 Jet__environment::EndEnvironment();

 double answer = atan(x);
 printf ( "atan( %lf ) = %le\n", x, answer );

 Jet u, s, c;
 int d[1];
 d[0] = 0;
 u = m;

 for( int i = 0; i < iterations; i++ ){
  s = sin( u );
  c = cos( u );
  u = u - c*( s - m*c );
  printf( "%le\n", u.standardPart() );
  for( int j = 1; j < 6; j++ ) {
   d[0] = j;
   printf( "%le  ", u.derivative(d) );
  }
  printf( "\n" );
 }

}
