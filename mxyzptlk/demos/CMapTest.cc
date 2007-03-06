#include <mxyzptlk/JetC.h>
#include <mxyzptlk/MappingC.h>

using namespace std;

int main( int argc, char** argv ) 
{
 if( 3 != argc ) {
   cout << "Usage: " << argv[0] << "  x  y " << endl;
   return -1;
 }

 std::complex<double> z( atof( argv[1] ), atof( argv[2] ) );

 JetC__environment::BeginEnvironment( 6 );
 coordC x( z );
 JetC__environment::EndEnvironment();

 MappingC f;
 JetC     u;

 u = std::complex<double>( 3.0,1.0) + exp( x*(1.0 + 3.0*x) );

 f.SetComponent( 0, u );

 u               .printCoeffs();
 f.Inverse()     .printCoeffs();
 f( f.Inverse() ).printCoeffs();

 cout << "\n\n"
      <<  f( f.Inverse() );

 return 0;
}
