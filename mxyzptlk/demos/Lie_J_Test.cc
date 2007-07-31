/*
**
** Demo program:
** 
** --- Leo Michelotti
** --- February 3, 1994
**
*/

#include <mxyzptlk/LieOperator.h>

using namespace std;

int main( int argc, char** argv ) 
{
  if( argc != 3 ) {
    cout <<   "Usage: " << argv[0] << "  d  t"
            "\nwhere  d  = highest order derivative"
            "\n       t  = upper limit of integration"
         << endl;
    return -1;
  }

  int    deg = atoi( argv[1] );
  double   t = atof( argv[2] );

  JetC__environment::BeginEnvironment( deg );
    coordC   x(std::complex<double>(0,0))
           , y(std::complex<double>(0,0))
           , z(std::complex<double>(0,0));
  JetC__environment::EndEnvironment();

  CLieOperator u, v;
  JetC r1, r2;

  u.SetComponent( 0, x*x );
  u.SetComponent( 1, y*y );
  u.SetComponent( 2, z*z );

  v.SetComponent( 0, y*z );
  v.SetComponent( 1, z*x );
  v.SetComponent( 2, x*y );

  cout << "\n-*-*-*-  x  coefficients  -*-*-*-*-*" << endl;
  r1 = u.expMap( t, v.expMap( t, x ) );
  r2 = v.expMap( t, u.expMap( t, x ) );
  (r1 - r2).printCoeffs();

  cout << "\n-*-*-*-  y  coefficients  -*-*-*-*-*" << endl;
  r1 = u.expMap( t, v.expMap( t, y ) );
  r2 = v.expMap( t, u.expMap( t, y ) );
  (r1 - r2).printCoeffs();

  cout << "\n-*-*-*-  z  coefficients  -*-*-*-*-*" << endl;
  r1 = u.expMap( t, v.expMap( t, z ) );
  r2 = v.expMap( t, u.expMap( t, z ) );
  (r1 - r2).printCoeffs();

  return 0;
}
