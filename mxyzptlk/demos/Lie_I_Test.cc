/*
**
** Demo program:
** 
** --- Leo Michelotti
** --- February 3, 1994
**
*/

#include <stdlib.h>
#include "LieOperator.h"

using namespace std;

main( int argc, char** argv ) {

 if( argc != 3 ) {
  printf( "\n" );
  printf( "Usage: %s  d  t                           \n", argv[0] );
  printf( "where  d  = highest order derivative      \n" );
  printf( "       t  = upper limit of integration.   \n" );
  printf( "\n" );
  exit(0);
 }

 int    deg = atoi( argv[1] );
 double   t = atof( argv[2] );

 Jet__environment::BeginEnvironment( deg );
 coord  x( 0.0 ),  y( 0.0 ),  z( 0.0 );
 Jet__environment::EndEnvironment();

 LieOperator u, v;
 Jet r1, r2;

 u.SetComponent( 0,  x*x );
 u.SetComponent( 1,  y*y );
 u.SetComponent( 2,  z*z );

 v.SetComponent( 0,  y*z );
 v.SetComponent( 1,  z*x );
 v.SetComponent( 2,  x*y );

 printf( "\n-*-*-*-  x  coefficients  -*-*-*-*-*\n" );
 r1 = u.expMap( t, v.expMap( t, x ) );
 r2 = v.expMap( t, u.expMap( t, x ) );
 (r1 - r2).printCoeffs();

 printf( "\n-*-*-*-  y  coefficients  -*-*-*-*-*\n" );
 r1 = u.expMap( t, v.expMap( t, y ) );
 r2 = v.expMap( t, u.expMap( t, y ) );
 (r1 - r2).printCoeffs();

 printf( "\n-*-*-*-  z  coefficients  -*-*-*-*-*\n" );
 r1 = u.expMap( t, v.expMap( t, z ) );
 r2 = v.expMap( t, u.expMap( t, z ) );
 (r1 - r2).printCoeffs();

}
