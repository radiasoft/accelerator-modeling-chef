/*
**
** Demo program:
** Calculates the Jet expansion of the
** inverse of an "arbitrary" function
** specified by a set of coefficients
** on the command line.
** --- Leo Michelotti
** --- August 4, 1993
**
*/

#include <mxyzptlk/Jet.h>

using namespace std;

int main( int argc, char** argv ) 
{
 if( argc == 1 ) {
  cout << "Usage: " << argv[0] << "  <degree> <n> <c_1> <c_2> ... <c_n>" << endl;
  exit(0);
 }

 int i = 0;
 int deg = atoi( argv[1] );
 Jet__environment::BeginEnvironment( deg );
 coord x(0.0);
 Jet__environment::EndEnvironment();

 int n = atoi( argv[2] );
 double* c = new double [n];
 for( i = 0; i < n; i++ )
 { c[i] = atof( argv[i+3] ); }

 if( c[0] == 0.0 ) {
  cout << "\nERROR: The coefficient of x vanishes." << endl;
  return -1;
 }

 Jet u, v;
 Jet f, g;
 IntArray d(1);
 d[0] = 1;

 f = c[n-1];
 if( n > 1 )
  for( i = n-2; i >= 0; i-- )
   f = f*x + c[i];
 f *= x;
 g = f.D(d);

 std::vector<Jet> w(1);

 u = x / c[0];
 w[0] = u;
 v = ( f(w) - x ) / g(w);
 while( v != 0.0 ) {
  u -= v;
  w[0] = u;
  v = ( f(w) - x ) / g(w);
 }

 cout << "\nThe answers: __________________________ " << endl;
 f.peekAt();
 u.peekAt();
 w[0] = u;
 f(w).peekAt();
 w[0] = f;
 u(w).peekAt();

 return 0;
}
