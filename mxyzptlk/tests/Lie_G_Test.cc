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

#include <mxyzptlk/JetC.h>
#include <mxyzptlk/CLieOperator.h>

using namespace std;

int main( int argc, char** argv ) 
{
 if( argc != 3 ) {
  cout << "Usage: " << argv[0] << "   d  t"
          "\nwhere  d  = degree of highest derivative"
          "\n       t  = upper limit of integration."
       << endl;
  return -1;
 }

 int    deg   = atoi( argv[1]  );
 double limit = atof( argv[2]  );

 JetC__environment::BeginEnvironment( deg );
 coordC x( 0.0 );
 paramC t( limit );
 JetC__environment::EndEnvironment();

 CLieOperator v;
 v.SetComponent( 0, x*x );

 cout << "\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-" << endl;
 cout << "Upper limit is double variable." << endl;
 v.expMap( limit, x ) .printCoeffs();
 cout << "\n-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-" << endl;
 cout << "Upper limit is Jet variable." << endl;
 v.expMap( t, x ) .printCoeffs();
 
 return 0;
}
