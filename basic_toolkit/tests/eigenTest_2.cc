////////////////////////////////////////////////////////////
// 
// File:          eigenTest_2.cc
// Author:        Leo Michelotti
// 
// Revision date:    December 12, 2013  (original version)
// 
////////////////////////////////////////////////////////////
// 
// Tests member functions Matrix<T>.eigenValues() 
// and Matrix<T>.eigenVectors().
// 
// Usage
// -----
// Command line: testEigen
//               testEigen N
//               testEigen N S
//               testEigen -h
// 
// where: (integer) N is the dimension  (default 3)
//        (integer) S is a seed for random number generation
//        -h        flag to print help message
// 
////////////////////////////////////////////////////////////


#include <basic_toolkit/Matrix.h>


int main( int argc, char** argv )
{
  // Default arguments
  // -----------------
  int N    = 3;
  int seed = 123;


  // Command line processing
  // -----------------------
  cout << "Command line: ";
  for( int i = 0; i < argc; ++i ) { cout << argv[i] << "  "; }
  cout << "\n--------------------------------\n" << endl;

  if( 3 == argc ) {
    N = atoi( argv[1] ); 
    if( N < 2 ) { N = 2; }
    seed = atoi( argv[2] );
    if( seed  < 1    ) { seed = 1;     }
    if( 12345 < seed ) { seed = 12345; }
    cout << "DGN: seed = " << seed << endl;
  }
  else if( 2 == argc ) {
    if( std::string(argv[1]) == std::string("-h") ) {
      cout <<   "Usage: " << argv[0]
           << "\n     : " << argv[0] << " N"
           << "\n     : " << argv[0] << " N S"
           << "\n     : " << argv[0] << " -h\n"
           << "\nwhere: (integer) N is the dimension  (default 3)"
              "\n       (integer) S is a seed for random number generation"
              "\n       -h        prints this message"
           << endl;
      return -1;
    }
    else {
      N = atoi( argv[1] ); 
      if( N < 2 ) { N = 2; }
    }
  }


  // Construct a random square matrix
  // with elements between -1 and 1.
  // --------------------------------
  cout << "DGN: srand( " << seed << ");   // Initialize a seed" << endl;
  srand48( seed );   // Initialize a seed

  Matrix linearModel(N,N);
  for( int i = 0; i < N; i++ ) {
    for( int j = 0; j < N; j++ ) {
      linearModel[i][j] = -1.0 + 2.0*drand48();
    }
  }
  cout << "Your matrix is:\n" << linearModel << endl;

  cout << "\nx =\n" << linearModel << endl;


  // Eigenvalues
  // -----------
  MatrixC u = linearModel.eigenValues();
  cout << "eigenvalues u =\n" << u << endl;

  // Eigenvectors
  // ------------
  MatrixC v = linearModel.eigenVectors();
  cout << "eigenvectors v =\n" << v << endl;


  // The test
  // --------
  MatrixC E(N,N);
  for( int i = 0; i < N; ++i ) {
    E[i][i] = u[0][i];
  }

  cout << "linearModel*v = \n" << linearModel*v << endl;
  cout << "v*E = \n" << v*E << endl;

  MatrixC diff( linearModel*v - v*E );
  cout << "Difference = \n" << diff << endl;

  cout << "====================================\n" << endl;


  // Return value of test
  // --------------------
  for( int i = 0; i < diff.rows(); ++i ) 
  {
    for( int j = 0; j < diff.cols(); ++j ) 
    {
      if(    ( 1.0e-13 < std::abs(diff[i][j].real()) )
          || ( 1.0e-13 < std::abs(diff[i][j].imag()) ) ) 
      {
        cout << "Returning -1" << endl;
        return -1;
      }
    }
  }

  cout << "Returning 0" << endl;
  return 0;
}
