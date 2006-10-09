#include <Matrix.h>
#include <RandomOrthogonal.h>

using namespace std;

int main( int, char** argv )
{
  int dim = atoi( argv[1] );
  RandomOrthogonal orthoFactory( dim );

  MatrixD X( orthoFactory.build() );

  cout << X
       << X*X.transpose()
       << X.transpose()*X;

  return 0;
}
