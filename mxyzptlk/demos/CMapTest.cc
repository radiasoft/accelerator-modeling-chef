#include "JetC.h"
#include "MappingC.h"

main( int, char** argv ) {
 Complex z( atof( argv[1] ), atof( argv[2] ) );

 JetC::BeginEnvironment( 6 );
 coordC x( z );
 JetC::EndEnvironment();

 MappingC f;
 JetC     u;

 u = Complex( 3.0,1.0) + exp( x*(1.0 + 3.0*x) );

 f.SetComponent( 0, u );

 u               .printCoeffs();
 f.Inverse()     .printCoeffs();
 f( f.Inverse() ).printCoeffs();

 cout << "\n\n"
      <<  f( f.Inverse() );
}
