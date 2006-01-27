/*
**
** Test of MappingC::Inverse() 
** This does not work well because of retaining
** negligible terms (of order 10**-16).  I do
** not yet understand why these terms are being
** kept.
** 
** First test with non-nilpotent Jets
** Complex version
**
** --- Leo Michelotti
** --- February 9, 1995
**
*/

#include "MappingC.h"

using namespace std;

main( int, char** ) {
 
 JetC__environment::BeginEnvironment( 4 );
 coordC x( std::complex<double>( -1.2,  0.9 ) ), 
        y( std::complex<double>(  3.5,  1.7 ) ), 
        z( std::complex<double>(  2.1, -0.3 ) );
 JetC__environment::EndEnvironment();

 MappingC  w;
 MappingC  u;

 w.SetComponent( 0, std::complex<double>(  3.0,  1.0 ) + x + 3.0*y + x*y - y*z   );
 w.SetComponent( 1, std::complex<double>( -1.0,  0.2 ) + y - x + z + x*z + y*y   );
 w.SetComponent( 2, std::complex<double>(  2.0, -0.9 ) + z + 2.0*x + y*z - x*y*z );

 u = w.Inverse();

 cout << "\n====== w.printCoeffs(); =================\n" << endl; 
 w.printCoeffs();

 cout << "\n====== u.printCoeffs(); =================\n" << endl; 
 u.printCoeffs();

 cout << "\n====== w(u).printCoeffs(); =================\n" << endl; 
 w(u).printCoeffs();

 cout << "\n====== u(w).printCoeffs(); =================\n" << endl; 
 u(w).printCoeffs();
}
