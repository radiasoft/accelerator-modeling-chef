#include <math.h>
#include "Matrix.h"

main( int, char** argv )
{
  double x = atof( argv[1] );
  double y = atof( argv[2] );
  double z = atof( argv[3] );

  MatrixD A( 3, 3 ), U( "o", 3 ), D( 3, 3 ), V( "o", 3 );

  D( 0, 0 ) =  x;
  D( 1, 1 ) =  y;
  D( 2, 2 ) =  z;

  A = U*D*V.transpose();

  MatrixD UU( 3, 3 ), VV( 3, 3 );
  MatrixD W(3);

  A.SVD( UU, W, VV );

  cout << "\nA:\n"  << A;
  cout << "\nU:\n"  << U;
  cout << "\nUU:\n" << UU;
  cout << "\nV:\n"  << V;
  cout << "\nVV:\n" << VV;
  cout << "\nW:\n"  << W;
  cout << endl;
}

