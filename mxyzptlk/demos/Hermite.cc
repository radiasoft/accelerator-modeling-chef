/*
** Demo program:
** 
** Prints coefficients of the polynomials
** exp( - x*x ) * d^n / dx^n exp( x*x )
** 
** --- Original  : August  31, 1995
** --- Modified  : February 6, 2007
** --- Programmer: Leo Michelotti
*/

#include <iostream>
#include <mxyzptlk/Jet.h>

using namespace std;

int main( int argc, char** argv ) 
{
  if( argc != 2 ) {
    cout << "\nUsage: " << argv[0] << "  n"
         << endl;
    return 1;
  }
  int n = atoi( argv[1] );

  Jet__environment::BeginEnvironment(2*n);
  coord x( 0.0 );
  Jet__environment::EndEnvironment();

  Jet f, g;
  f = exp( - x*x );
  g = f;

  int k = 0;
  cout << "Results for k = " << k << endl;
  ( g / f ).printCoeffs();

  IntArray d(1);
  d[0] = 1;
  for( k = 1; k <= n; k++ ) {
    g = - g.D( d );
    cout << "Results for k = " << k << endl;
    ( g / f ).printCoeffs();
  }

  return 0;
}
