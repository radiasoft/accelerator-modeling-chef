////////////////////////////////////////////////////////////
// 
// File:          eigenTest.cc
// Author:        Leo Michelotti
// 
// Revision date:    December 11, 2013  (i.e. 13/12/11)
// Original version: November 13, 2007  (called TestEigen.cc)
// 
////////////////////////////////////////////////////////////
// 
// Tests member functions Matrix<T>.eigenValues() 
// and Matrix<T>.eigenVectors().
// 
// Command line: testEigen
// 
////////////////////////////////////////////////////////////


#include <basic_toolkit/Matrix.h>


// Global
int errorFlag = 1;


bool testing( int dimension, double* elements )
{
  // Set up the matrix
  // -----------------
  MatrixD x(dimension,dimension);
  int k = 0;
  for( int i = 0; i < x.rows(); ++i ) {
    for( int j = 0; j < x.cols(); ++j ) {
      x[i][j] = elements[k++];
    }    
  }

  cout << "Test dimension = " << dimension << endl;

  cout << "\nx =\n" << x << endl;

  // Eigenvalues
  // -----------
  MatrixC u = x.eigenValues();
  cout << "eigenvalues u =\n" << u << endl;

  // Eigenvectors
  // ------------
  MatrixC v = x.eigenVectors();
  cout << "eigenvectors v =\n" << v << endl;

  // The test
  // --------
  MatrixC E(dimension,dimension);
  for( int i = 0; i < dimension; ++i ) {
    E[i][i] = u[0][i];
  }

  cout << "x*v = \n" << x*v << endl;
  cout << "v*E = \n" << v*E << endl;

  MatrixC diff( x*v - v*E );
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
        return false;
      }
    }
  }

  return true;
}


int main()
{
  int ret = 0;

  { // 2x2
  double aData [] = { 12, 5,
                       4, 2  };
  if( !testing( 2, aData ) ) { ret -= errorFlag; errorFlag *= 2; }
  }


  { // 3x3
  double aData [] = { 12, 5, 7,
                       4, 2, 3,
                       1, 8, 9 };
  if( !testing( 3, aData ) ) { ret -= errorFlag; errorFlag *= 2; };
  }


  { // 4x4
  double aData [] = { 1, 2, 3, 4,
                      2, 3, 4, 1,
                      3, 4, 1, 2,
                      4, 1, 2, 3 };
  if( !testing( 4, aData ) ) { ret -= errorFlag; errorFlag *= 2; };
  }


  { // 2x2 rotation matrix
  double cs = 1./sqrt(2.);
  double sn = sqrt( 1. - cs*cs );
  double aData [] = {  cs, sn,
                      -sn, cs  };
  if( !testing( 2, aData ) ) { ret -= errorFlag; errorFlag *= 2; };
  }

  cout << "Return value = " << ret << endl;

  return ret;
}
