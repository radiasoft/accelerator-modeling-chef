#include "mxyzptlk.h"
#include <iomanip.h>

main() {
static int index[] = { 3, 2 };
double   answer[2];

Jet::BeginEnvironment( 7 );
coord x( 0.0 );
coord y( 0.0 );
Jet::EndEnvironment();

Jet u, v, w, z;
// Mapping a( 2, 0, x.Env() ), c( 2, 0, x.Env() );
Mapping a( 2, 0, x.Env() );

u = x*y*y + exp( x + y );  v = cos( y*x*x ) / ( x + 2.0 );  
w = sin(u) * cos(v);       z = exp( u*u*u ) / ( u*v );

a.SetComponent( 0, u ); a.SetComponent( 1, v );

Jet::BeginEnvironment( 7 );
coord xx( u.standardPart() );
coord yy( v.standardPart() );
Jet::EndEnvironment();

Mapping b( 2, 0, xx.Env() );

u = sin(xx) * cos(yy);  v = exp( xx*xx*xx ) / ( xx*yy );

b.SetComponent( 0, u ); b.SetComponent( 1, v );

// c = b(a);
// c.derivative( index, answer );

b(a).derivative( index, answer );

cout.setf( ios::fixed,ios::floatfield);
cout.setf( ios::right,ios::adjustfield);
cout << "                Using: expansion     composition\n"
     << "Component zero:  " 
     << setw(15) << answer[0] 
     << " " 
     << setw(15) << w.derivative( index ) 
     << "\n"
     << "Component one :  " 
     << setw(15) << answer[1] 
     << " " 
     << setw(15) << z.derivative( index )
     << endl;
}
