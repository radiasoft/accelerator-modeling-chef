/*
**
** Demo program:
** 
** Illustrates manipulating Jets returned
** from a function.
** 
** --- Leo Michelotti
** --- August 19, 1993
**
*/

#include "mxyzptlk.h"

Jet g( const Jet& x, int n ) {
  Jet z = 0.0;
  Jet term;
  term = x;
  for( int k = 1; k <= n; k++ ) {
    z += term / ( (double) k );
    term *= x;
  }
  return z;
}

main() {
 Jet::BeginEnvironment( 6 );
 coord x(0.0), y(0.0), z(0.0);
 Jet::EndEnvironment();

 Jet a; 
 a = x*y + y*z + z*x;
 a.printCoeffs();
 ( g( a, 3 )*g( sin(a), 5 ) ).printCoeffs();

}
