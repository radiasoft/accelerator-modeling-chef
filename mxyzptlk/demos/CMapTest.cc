#include "JetC.h"
#include "MappingC.h"

using namespace std;

main( int, char** argv ) {
 std::complex<double> z( atof( argv[1] ), atof( argv[2] ) );

 Jet__environment::BeginEnvironment( 6 );
 coordC x( z );
 Jet__environment::EndEnvironment();

 MappingC f;
 JetC     u;

 u = std::complex<double>( 3.0,1.0) + exp( x*(1.0 + 3.0*x) );

 f.SetComponent( 0, u );

 u               .printCoeffs();
 f.Inverse()     .printCoeffs();
 f( f.Inverse() ).printCoeffs();

 cout << "\n\n"
      <<  f( f.Inverse() );
}
