#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/RandomOrthogonal.h>

using namespace std;

int main( int argc, char** argv )
{
  if( 2 != argc ) {
    cerr << "Usage: " << argv[0]
         << "  <dimension>"
         << endl;
    return -1;
  }

  int dim = atoi( argv[1] );
  RandomOrthogonal orthoFactory( dim );

  MatrixD X( orthoFactory.build() );

  cout << X
       << X*X.transpose()
       << X.transpose()*X;

  return 0;
}
