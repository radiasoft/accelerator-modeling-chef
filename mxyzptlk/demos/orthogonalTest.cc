#include <stdlib.h>
#include "Matrix.h"

main( int, char** argv )
{
  int dim = atoi( argv[1] );
  MatrixD X( "o", dim );
  cout << X
       << X*X.transpose()
       << X.transpose()*X;
}
