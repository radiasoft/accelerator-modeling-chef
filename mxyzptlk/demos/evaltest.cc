//************************************
//************************************
//************************************
//
//
//      Test program.
//

#include <iostream>
#include <Jet.h>

using namespace std;

int main() 
{
 double r[3], s[3];
 
 Jet__environment::BeginEnvironment( 7 );
 coord x(0.5), y(0.4), z(0.0);
 Jet__environment::EndEnvironment();

 Jet u, v;
 
 u = exp( x );
 v = exp( x + y + z );

 r[0] = 1.0;   s[0] = 0.33;
 r[1] = 0.0;   s[1] = 0.33;
 r[2] = 0.0;   s[2] = 1.0 - s[0] - s[1];
 
 for( int w = 1; w <= 7; w++ ) {
   cout << w << "  " 
        << ((u.filter( 0, w ))( r )) << "  "
        << ((v.filter( 0, w ))( s ))
        << endl;
 }

 return 0;
}
