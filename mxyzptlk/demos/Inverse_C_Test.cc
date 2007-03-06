/*
**
** Test of LieOperator::Inverse() 
** This does not work well because of retaining
** negligible terms (of order 10**-16).  I do
** not yet understand why these terms are being
** kept.
** 
** --- Leo Michelotti
** --- September 15, 1993
**
*/

#include <mxyzptlk/Mapping.h>

main( int, char** ) 
{
 Jet__environment::BeginEnvironment( 4 );
 coord x( 0.0 ), y( 0.0 ), z( 0.0 );
 Jet__environment::EndEnvironment();

 Mapping w, u;

 w.SetComponent( 0, x + 3.0*y + x*y - y*z   );
 w.SetComponent( 1, y - x + z + x*z + y*y   );
 w.SetComponent( 2, z + 2.0*x + y*z - x*y*z );

 u = w.Inverse();

 cout << "\n====== w.printCoeffs(); =================\n";
 w.printCoeffs();

 cout << "\n====== u.printCoeffs(); =================\n";
 u.printCoeffs();

 cout << "\n====== w(u).printCoeffs(); =================\n";
 w(u).printCoeffs();

 cout << "\n====== u(w).printCoeffs(); =================\n";
 u(w).printCoeffs();
}
