/*
**
** Demo program:
** 
** Uses the exponential map method to 
** integrate x' = x.
** 
** --- Leo Michelotti
** --- August 10, 1993
**
*/

#include "mxyzptlk.h"

main( int argc, char** argv ) {

 if( argc != 3 ) {
  cout << "\n"
       << "Usage: " << argv[0] << " d  t             \n"
       << "where  d  = degree of highest derivative  \n"
       << "       t  = upper limit of integration.   \n"
       << endl;
  exit(0);
 }

 int    deg   = atoi( argv[1]  );
 double limit = atof( argv[2]  );

 Jet::BeginEnvironment( deg );
 coord x( 0.0 );
 Jet::Parameters();
 coord t( limit );
 Jet::EndEnvironment();

 LieOperator v;
 v.SetComponent( 0, x );

 cout << "\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
      << "Upper limit is double variable."
      << endl;
 v.expMap( limit, x ) .printCoeffs();
 cout << "\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-\n"
      << "Upper limit is Jet variable."
      << endl;
 v.expMap( t, x ) .printCoeffs();
 
}
