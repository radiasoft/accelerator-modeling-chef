/*
**
** Demo program:
** 
** Uses the exponential map method to 
** integrate a non-Hamiltonian, 
** nonlinear vector
** field that spirals inward.
** 
** --- Leo Michelotti
** --- August 10, 1993
**
*/

#include <mxyzptlk/LieOperator.h>

using namespace std;

main( int argc, char** argv ) 
{
 if( argc != 3 ) {
  cout << "\n"
       << "Usage: " << argv[0] << "  d  t            \n"
       << "where  d  = highest degree derivative     \n"
       << "       t  = upper limit of integration.   \n"
       << endl;
  exit(0);
 }

 int    deg   = atoi( argv[1] );
 double limit = atof( argv[2] );

 Jet__environment::BeginEnvironment( deg );
 coord x( 0.0 );
 coord y( 0.0 );
 param t( limit );
 Jet__environment::EndEnvironment();

 LieOperator v;
 Jet f, g;
 
 f = 1.0; 
 v.SetComponent( 0, f );
 v.SetComponent( 1, 2.0*x );

 // f = 1.0 - exp( - x*x - y*y );
 // v.SetComponent( 0,  f*y         );
 // v.SetComponent( 1, -f*( x + y ) );

 // v.SetComponent( 0, -x - y );
 // v.SetComponent( 1,  x - y );

 cout << "\n-*-*-*-  x  coefficients  -*-*-*-*-*" << endl;
 ( f = v.expMap( t, x ) ).printCoeffs();
 cout << "\n-*-*-*-  y  coefficients  -*-*-*-*-*" << endl;
 ( g = v.expMap( t, y ) ).printCoeffs();

 // Enter a point and evaluate the map.
 static double s[] = { 0.0, 0.0, 0.0 };
 double tlo, thi, delta;

 cout << "Enter x and y:  ";
 cin >> s[0] >> s[1];
 cout << "Enter range and increment  ";
 cin >> tlo >> thi >> delta;

 s[2] = limit;

 cout.setf( ios::fixed,ios::floatfield);
 cout << "( " << s[0] << ", " << s[1] << " ) maps to ( "
      << f(s) << ", " << g(s) << " )\n"
      << endl;

 for( limit = tlo; limit < thi + delta / 2.0 ; limit += delta ) {
  s[2] = limit;
  cout << limit << ": ( " << f(s) << ", " << g(s) << " )"
       << endl;
 }
}
