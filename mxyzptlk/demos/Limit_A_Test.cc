#include <math.h>
#include <stdio.h>

main( int argc, char** argv ) {

 double x = atof( argv[1] );
 double answer = atan(x);
 printf ( "atan( %lf ) = %le\n", x, answer );

 double u;
 if( x < 1.5 ) u = x;
 else          u = 1.5;
 double s, c;
 printf( "%le  %le  %le\n", u, tan(u), x );
 while( fabs( tan(u) - x ) >= 1.0e-10 ) {
  s = sin( u );
  c = cos( u );
  u -= c*( s - x*c );
  printf( "%le  %le  %le\n", u, tan(u), x );
 }

}
