#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/IntArray.h>
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


// --------------------------------------------------------------
// "traditional" environment + coordinate declarations 
// --------------------------------------------------------------
#if 0

  Jet__environment::BeginEnvironment( 7 );
  coord x( 0.5 );
  coord y( 0.3 );
  coord z( 0.1 );
  Jet__environment::EndEnvironment();
  
#endif
// --------------------------------------------------------------
// "newstyle" environment + coordinate declarations 
// --------------------------------------------------------------

#if 1
  double const ref[] = { 0.5, 0.3, 0.1 }; 
  Vector  refpt( &ref[0], &ref[0]+3 );
 
  EnvPtr<double> env = Jet__environment::makeJetEnvironment( 7, 3, 3, refpt);   

  Jet x = Jet::makeCoordinate( env, 0 );
  Jet y = Jet::makeCoordinate( env, 1 );
  Jet z = Jet::makeCoordinate( env, 2 );

#endif

//---------------------------------------------------------------

  double r_values[]     = { 1.0,  0.0,  0.0 };
  double s_values[]     = { 0.33, 0.33, 0.33 };
  int    index_values[] = { 2, 1, 3 };
  
  Vector       r( &r_values[0], &r_values[0]+3   );
  Vector       s( &s_values[0], &s_values[0]+3   );
  IntArray index( &index_values[0], &index_values[0]+3 );
  
  // First test ....
  Jet h = 1.0 / ( cos( exp( x*x*y - 3.0*x*z*z ) ) );
  cout << "First test\nThe derivative is: " 
       << h.derivative( index ) 
       << " = " 
       << (1.0 / ( cos( exp( x*x*y - 3.0*x*z*z ) ) )).derivative( index ) 
       << "\n"
       << endl;
  
  // Second test ....
  Jet u = exp( x );
  Jet v = exp( x + y + z );
  
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
