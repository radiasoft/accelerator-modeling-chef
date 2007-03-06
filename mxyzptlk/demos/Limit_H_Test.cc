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
 int d = 1;

 f = c[n-1];
 if( n > 1 )
  for( i = n-2; i >= 0; i-- )
   f = f*x + c[i];
 f *= x;
 g = f.D(&d);

 u = x / c[0];
 v = ( f(&u) - x ) / g(&u);
 while( v != 0.0 ) {
  u -= v;
  v = ( f(&u) - x ) / g(&u);
 }

 cout << "\nThe answers: __________________________ " << endl;
 f.peekAt();
 u.peekAt();
 f(&u).peekAt();
 u(&f).peekAt();

 return 0;
}
