/*
**
** Test of LieOperator inversion algorithm.
** 
** --- Leo Michelotti
** --- September 15, 1993
**
*/

#include <stdlib.h>
#include <stdio.h>
#include "mxyzptlk.h"

main( int argc, char** argv ) {
 
 if( argc != 2 ) {
  printf( "\n" );
  printf( "Usage: %s  limit                          \n", argv[0] );
  printf( "\n" );
  exit(0);
 }

 int limit = atoi( argv[1] );

 Jet::BeginEnvironment( 4 );
 coord x( 0.0 ), y( 0.0 ), z( 0.0 );
 Jet::EndEnvironment();

 Mapping u, v, w;
 Mapping id( "ident" );


 w.SetComponent( 0, x + 3.0*y + x*y - y*z   );
 w.SetComponent( 1, y - x + z + x*z + y*y   );
 w.SetComponent( 2, z + 2.0*x + y*z - x*y*z );

 MatrixD M( 3, 3 );
 M = w.Jacobian();
 M = M.inverse();

 int i = 0;
 u = M*id;
 v = ( w(u) - id );
 while ( ( i++ < limit ) && ( v != 0.0 ) ) {
  u = u - M*v;
  v = ( w(u) - id );
 }

 printf( "\n====== u.printCoeffs(); =================\n" );
 u.printCoeffs();

 printf( "\n====== w.printCoeffs(); =================\n" );
 w.printCoeffs();

 printf( "\n====== w(u).printCoeffs(); =================\n" );
 w(u).printCoeffs();

 printf( "\n====== u(w).printCoeffs(); =================\n" );
 u(w).printCoeffs();

 if( i >= limit ) printf( "++++ Limit exceeded! ++++\n" );
}
