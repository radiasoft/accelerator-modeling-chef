#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

#include "mxyzptlk.h"


main() {

 double u1, u2, v1, v2;
 double w, y, z, answer;

 u1 = 0.32;
 u2 = 0.5;
 v1 = -3.1;
 v2 = 1.5;
 
 Jet::BeginEnvironment( 5 );
 coord x1( u1 );
 coord x2( u2 );
 coord p1( v1 );
 coord p2( v2 );
 Jet::Parameters();
 Jet::EndEnvironment();

 Jet a, b, c;
 Jet pb;
 
 a = (x1*x1) * (x2*x2*x2) * p1 * (p2*p2*p2*p2);
 b = sin( x1 * (p2*p2) * (x2*x2*x2) );
 pb = a^b;
 cout << "Computed by Jet: " << pb.standardPart() << "\n";
 
 w = (u1*u1) * (u2*u2*u2) * v1 * (v2*v2*v2*v2);
 y =      u1 * (v2*v2) * (u2*u2*u2)  ;
 z = cos( y );
 answer = w*y*z*( 6.0/(u2*v2) - 1.0/(u1*v1) - 12.0/(u2*v2) );
 cout << "Exact answer   : " << answer << "\n";
 
 cout << "And also       : " 
      << ( ( (x1*x1) * (x2*x2*x2) * p1 * (p2*p2*p2*p2) ) ^
           ( sin( x1 * (p2*p2) * (x2*x2*x2) ) ) 
         ).standardPart()
      << "\n\n";
 
 // -- Test of the Jacobi identity
 c = exp( p1*x1 + p2*x2 );
 pb = (a^(b^c)) + (b^(c^a)) + (c^(a^b));
 cout << "Jacobi identity" << endl;
 pb.printCoeffs();

}
