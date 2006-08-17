#include <IntArray.h>
#include <VectorD.h>
#include "mxyzptlk.h"
#include <iomanip>

using namespace std;

main() {
int index[] = { 3, 2 };
double   answer[2];

Jet__environment::BeginEnvironment( 7 );
coord x( 0.0 );
coord y( 0.0 );
Jet__environment::EndEnvironment();

Jet u, v, w, z;
Mapping a( 2, 0 );

u = x*y*y + exp( x + y );  v = cos( y*x*x ) / ( x + 2.0 );  
w = sin(u) * cos(v);       z = exp( u*u*u ) / ( u*v );

a.SetComponent( 0, u ); a.SetComponent( 1, v );

Jet__environment::BeginEnvironment( 1 );
coord xx( u.standardPart() );
coord yy( v.standardPart() );
Jet__environment::EndEnvironment();

Mapping b( 2, 0 );

u = sin(xx) * cos(yy);  v = exp( xx*xx*xx ) / ( xx*yy );

b.SetComponent( 0, u ); b.SetComponent( 1, v );

#if 0
Mapping c( 2, 0 );
c = b(a);
c.derivative( index, answer );
#endif

#if 1
b(a).derivative( index, answer );
#endif

cout.setf( ios::fixed,ios::floatfield);
cout.setf( ios::right,ios::adjustfield);
cout << "                Using: expansion     composition\n"
     << "Component zero:  " 
     << setw(15) << w.derivative( index ) 
     << " " 
     << setw(15) << answer[0] 
     << "\n"
     << "Component one :  " 
     << setw(15) << z.derivative( index )
     << " " 
     << setw(15) << answer[1] 
     << endl;
}
