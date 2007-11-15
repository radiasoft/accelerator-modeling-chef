////////////////////////////////////////////////////////////
// 
// File:          testDet_1.cc
// Author:        Leo Michelotti
// 
// Revision date: November 13, 2007  (original version)
// 
////////////////////////////////////////////////////////////
// 
// Tests Matrix<T>.determinant() member function
// for a set of singular matrices.
// 
// Command line: testDet_1 
// 
////////////////////////////////////////////////////////////

#include <iostream>
#include <basic_toolkit/Matrix.h>

using namespace std;

bool testValue( int line, double v )
{
  if( 1.0e-12 < abs(v) ) {
    cout <<   "*** ERROR *** "
            "\n*** ERROR *** " << __FILE__ << "," << line
         << "\n*** ERROR *** Calculated value of determinant is " << v
         << "\n*** ERROR *** It should be zero."
            "\n*** ERROR *** "
         << endl;
    return true;
  }

  return false;
}

int main()
{
  double value;

  // ----------------------------------------------
  // First test
  // ----------------------------------------------
  Matrix w(3,3);
  w(0,0) = 1.0;
  w(0,1) = 2.0;
  w(0,2) = 3.0;
  w(1,0) = 2.0;
  w(1,1) = 4.0;
  w(1,2) = 6.0;
  w(2,0) = 11.0;
  w(2,1) = 12.0; 
  w(2,2) = 13.0;

  #if 0
  cout << "DGN: " << __FILE__ << "," << __LINE__ 
       << ": " << w.determinant() 
       << endl;
  cout << "DGN: " << __FILE__ << "," << __LINE__ 
       << ": " << w.eigenValues()
       << endl;
  #endif

  if( testValue( __LINE__, w.determinant() ) ) {
    return 1;
  }
 
  // ----------------------------------------------
  // Second test
  // ----------------------------------------------
  Matrix u(4,4);
  if( testValue( __LINE__, u.determinant() ) ) {
    return 2;
  }

  return 0;
}
