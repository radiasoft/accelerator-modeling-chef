/*
**
** Demo program:
** 
** Uses the exponential map method to 
** integrate z' = z.
** 
** --- Leo Michelotti
** --- January 26, 1994
**
*/

#include <stdlib.h>
#include "JetC.h"
#include "CLieOperator.h"

using namespace std;

main( int argc, char** argv ) {

 if( argc != 3 ) {
  printf( "\n" );
  printf( "Usage: %s  d  t                           \n", argv[0] );
  printf( "where  d  = degree of highest derivative  \n" );
  printf( "       t  = upper limit of integration.   \n" );
  printf( "\n" );
  exit(0);
 }

 int    deg   = atoi( argv[1]  );
 double limit = atof( argv[2]  );

 Jet__environment::BeginEnvironment( deg );
 coordC x( 0.0 );
 paramC t( limit );
 Jet__environment::EndEnvironment();

 CLieOperator v;
 v.SetComponent( 0, x*x );

 printf( "\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n" );
 printf( "Upper limit is double variable.\n" );
 v.expMap( limit, x ) .printCoeffs();
 printf( "\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n" );
 printf( "Upper limit is Jet variable.\n" );
 v.expMap( t, x ) .printCoeffs();
 
}
