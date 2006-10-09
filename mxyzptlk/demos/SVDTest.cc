/*
**
** File: SVDTest.cc
** 
** Original.
** --- Leo Michelotti
** --- Prehistory
**
*/

#include <Matrix.h>
#include <RandomOrthogonal.h>

using namespace std;

int main( int argc, char** argv )
{
  // --------------------
  // Process command line.
  // --------------------
  if( argc != 4 ) {
    cerr << "Usage: " << argv[0]
         << "  x  y  z"
            "\nwhere the arguments are arbitrary numbers."
            "\n------------------------------------------"
            "\nThe program will construct a random 3x3"
            "\nmatrix having the arguments as its singular"
            "\nvalues and perform an SVD decomposition of it."
            "\n\nNotice that ordering is not preserved."
         << endl;
    return -1;
  }
  cout << "Command line: " << argv[0];
  for( int i = 1; i < argc; i++ ) {
    cout << "  " << argv[i];
  }
  cout << endl;

  double x = atof( argv[1] );
  double y = atof( argv[2] );
  double z = atof( argv[3] );

  // --------------------------
  // Construct the input matrix.
  // --------------------------
  RandomOrthogonal orthoFactory( 3 );
  MatrixD UU( orthoFactory.build() );
  MatrixD VV( orthoFactory.build() );

  MatrixD A(3,3), D(3,3);

  D(0,0) =  x;
  D(1,1) =  y;
  D(2,2) =  z;

  A = UU*D*VV.transpose();

  // ------------------------
  // Do the SVD decomposition.
  // ------------------------
  MatrixD U(3,3), V(3,3);
  MatrixD W(3);

  A.SVD( U, W, V );

  // ---------------
  // Display results.
  // ---------------
  cout << "\nInput  U: 	     \n" << UU
       << "\nOutput U: 	     \n" << U
       << "\nInput  V: 	     \n" << VV
       << "\nOutput V: 	     \n" << V
       << "\nSingular values:\n" << W;

  D(0,0) =  W(0);
  D(1,1) =  W(1);
  D(2,2) =  W(2);

  cout << "\nInput matrix: \n"  << A
       << "\nU*D*V^T:      \n"  << ( U*D*V.transpose() )
       << endl;
  
  cout << "\nOrthogonality tests"
       << "\nU^T*U:\n"  << ( U.transpose()*U )
       << "\nV^T*V:\n"  << ( V.transpose()*V )
       << endl;

  return 0;
}
