#include <mxyzptlk/Jet.h>

using namespace std;

//***************************************************************
//***************************************************************
//***************************************************************
// 
//
//      Test program.
//

main() {

  Jet__environment::BeginEnvironment( 7 );
  coord x( 0.5 );
  coord y( 0.3 );
  coord z( 0.1 );
  Jet__environment::EndEnvironment();
  
  Jet h, u, v;
  
  double r_values[]     = { 1.0,  0.0,  0.0 };
  double s_values[]     = { 0.33, 0.33, 0.33 };
  int    index_values[] = { 2, 1, 3 };
  
  Vector       r( 3, r_values    );
  Vector       s( 3, s_values    );
  IntArray index( 3, index_values);
  
  // First test ....
  h = 1.0 / ( cos( exp( x*x*y - 3.0*x*z*z ) ) );
  cout << "First test\nThe derivative is: " 
       << h.derivative( index ) 
       << " = " 
       << (1.0 / ( cos( exp( x*x*y - 3.0*x*z*z ) ) )).derivative( index ) 
       << "\n"
       << endl;
  
  // Second test ....
  u = exp( x );
  v = exp( x + y + z );
  
  cout << "Second test\nThe correct result is: " 
       << exp( 1.0 ) << "\n"
       << endl;

  s[2] = 1.0 - s[0] - s[1];
  for( int w = 1; w <= 7; w++ ) {
    cout << w                       << "  "
         << (u.filter( 0, w ))( r ) << "  "
         << (v.filter( 0, w ))( s ) << "  "
         << endl;
  }
}