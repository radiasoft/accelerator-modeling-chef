#include "mxyzptlk.h"

//************************************
//************************************
//************************************
//
//
//      Test program.
//

main() {
 double r[3], s[3];
 
 Jet::BeginEnvironment( 7 );
 coord x(0.5), y(0.4), z(0.0);
 Jet::EndEnvironment();

 Jet u, v;
 
 u = exp( x );
 v = exp( x + y + z );

 r[0] = 1.0;   s[0] = 0.33;
 r[1] = 0.0;   s[1] = 0.33;
 r[2] = 0.0;   s[2] = 1.0 - s[0] - s[1];
 
 for( int w = 1; w <= 7; w++ ) {
   printf( "%d: %lf   %lf  \n",
           w,
           (u.filter( 0, w ))( r ),
           (v.filter( 0, w ))( s )
         );
   }
}
