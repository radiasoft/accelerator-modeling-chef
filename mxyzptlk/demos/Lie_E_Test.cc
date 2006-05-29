/*
**
** Demo program:
** 
** Uses the exponential map method to 
** integrate equations of motion of a 
** sextupolar type vector field.
** 
** As part of demo, we introduce the
** class field to make writing the
** vector field easier. 
** 
** --- Leo Michelotti
** --- August 10, 1993
**
*/

#include <stdlib.h>
#include "mxyzptlk.h"

using namespace std;

class field {
 private:
  Jet v[3];
 public:
  field( coord&, coord&, coord& );
  Jet operator[]( int );
};

field::field( coord& x, coord& y, coord& z ) {
 v[0] = - 2.0*x*y;
 v[1] =   x*x - y*y;
 v[2] =   0.0;
}

Jet field::operator[]( int i ) {
 return v[i];
}

main( int argc, char** argv ) {

 if( argc != 4 ) {
  printf( "\n" );
  printf( "Usage: %s  d  m  t                        \n", argv[0] );
  printf( "where  d  = highest order derivative      \n" );
  printf( "       m  = mass                          \n" );
  printf( "       t  = upper limit of integration.   \n" );
  printf( "\n" );
  exit(0);
 }

 int    deg = atoi( argv[1] );
 double msq = atof( argv[2] );
 double t   = atof( argv[3] );

 msq = msq*msq;

 Jet__environment::BeginEnvironment( deg );
 coord  x( 0.0 ),  y( 0.0 ),  z( 0.0 );
 coord px( 0.0 ), py( 0.0 ), pz( 0.0 );
 Jet__environment::EndEnvironment();

 Jet E = sqrt( px*px + py*py + pz*pz + msq );
 field B( x, y, z );

 LieOperator v;
 v.SetComponent( 0,  px                    / E );
 v.SetComponent( 1,  py                    / E );
 v.SetComponent( 2,  pz                    / E );
 v.SetComponent( 3,  ( py*B[2] - pz*B[1] ) / E );
 v.SetComponent( 4,  ( pz*B[0] - px*B[2] ) / E );
 v.SetComponent( 5,  ( px*B[1] - py*B[0] ) / E );

 printf( "\n-*-*-*-  x  coefficients  -*-*-*-*-*\n" );
 v.expMap( t, x ).printCoeffs();
 printf( "\n-*-*-*-  y  coefficients  -*-*-*-*-*\n" );
 v.expMap( t, y ).printCoeffs();
 printf( "\n-*-*-*-  z  coefficients  -*-*-*-*-*\n" );
 v.expMap( t, z ).printCoeffs();
}
