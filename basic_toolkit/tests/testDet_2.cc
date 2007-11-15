////////////////////////////////////////////////////////////
// 
// File:          testDet_2.cc
// Author:        Leo Michelotti
// 
// Revision date: November 13, 2007  (original version)
// 
////////////////////////////////////////////////////////////
// 
// A plea: Please, please don't templatize this file.
// 
////////////////////////////////////////////////////////////
// 
// Tests Matrix<T>.determinant() member function.
// 
// Command line: testDet_2 typeflag N N N N ...
// 
// where typeflag         is either -d or -c
//                        : -d indicates real values
//                        : -c indicates complex values
// 
// If typedef = -d, then each N is a real number.
// If typedef = -c, then an "N N" pair are the real and imaginary
// parts of a complex number.
// 
// A random matrix is generated whose eigenvalues are given
// on the command line.  The determinant of the matrix is
// then calculated and compared to the produce of the eigenvalues.
// 
// --------
// EXAMPLES:
// --------
// testDet -d 1.1  2.2  3.3  4.4
// 
// The matrix:
// ( 2.72519, -0.269293, 0.668479, 0.0342852,  )
// ( -0.269293, 4.24862, 0.214807, 0.554306,  )
// ( 0.668479, 0.214807, 2.41349, -0.56962,  )
// ( 0.0342852, 0.554306, -0.56962, 1.6127,  )
// 
// Calculated determinant: 35.1384
// Correct answer        : 35.1384
// - - - - - - - - - - - - - - - - - - - - - - - - 
// testDet_2 -c 1.1 2.2 3.3 4.4
// 
// The matrix:
// ( (1.90688,3.00688), (-1.06023,-1.06023),  )
// ( (-1.06023,-1.06023), (2.49312,3.59312),  )
// 
// Calculated determinant: (-6.05,12.1)
// Correct answer        : (-6.05,12.1)
// 
////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <string>
#include <complex>

#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/RandomOrthogonal.h>

using namespace std;

struct Options
{
  bool                                 valid_;
  enum Flavor {ddd=0,ccc}              flavor_;
  std::vector<double>                  values_;
  std::vector<std::complex<double> > c_values_;

  Options( int, char** );
};


Options::Options( int argc, char** argv )
: valid_(false)
, flavor_(ddd)
{
  int i = 1;

  if( 1 < argc ) {
    std::string s;
    s.assign( argv[i++] );
    if(      std::string("-d") == s ) { flavor_ = ddd; valid_ = true; }
    else if( std::string("-c") == s ) { flavor_ = ccc; valid_ = true; }
    else {
      return;
    }
  }

  int limit = argc;
  double x, y;
  if( ddd == flavor_ ) {
    while( i < limit ) {
      values_.push_back( atof( argv[i++] ) );
    }  
  }
  else {
    while( i < limit ) {
      x = atof( argv[i++] );
      if( i < limit ) {
        y = atof( argv[i++] );
        c_values_.push_back( std::complex<double>(x,y) );
      }
      else { 
        valid_ = false; 
        return; 
      }
    }  
  }

  if( 0 == values_.size() && 0 == c_values_.size() ) {
    valid_ = false;
  }
}


template<typename T>
bool testValue( int line, T const& v, T const& a )
{
  if( 1.0e-12 < abs(v-a) ) {
    cout <<   "*** ERROR *** "
            "\n*** ERROR *** " << __FILE__ << "," << line
         << "\n*** ERROR *** Calculated value of determinant is " << v
         << "\n*** ERROR *** It should be " << a << " ."
            "\n*** ERROR *** "
         << endl;
    return true;
  }

  return false;
}


int main( int argc, char** argv )
{
  Options myOptions( argc, argv );
  if( myOptions.valid_ ) {
    cout << "Command line: ";
    for( int i = 0; i < argc; i++ ) {
      cout << argv[i] << "  ";
    }
    cout << '\n' << endl;
  }
  else {
    cout << "Usage: " << argv[0] 
         << "  -d|-c N N N N ..." 
         << endl;
    return 1;
  }

  if( Options::ddd == myOptions.flavor_ ) {
    int n = myOptions.values_.size();
    Matrix M(n,n);
    double answer = 1.0;
    int i = 0;
    for( vector<double>::iterator it =  myOptions.values_.begin();
                                  it != myOptions.values_.end();
                                ++it                               )
    {
      M(i,i)  = *it;
      answer *= *it;
      ++i;
    }

    RandomOrthogonal generator(n);
    Matrix O( generator.build() );
    M = O * M * O.transpose();

    double value = M.determinant();

    cout << "The matrix:\n" << M << endl;
    cout << "Calculated determinant: " << setprecision(14) << value  << endl;
    cout << "Correct answer        : " << setprecision(14) << answer << endl;

    if( testValue( __LINE__, value, answer ) ) {
      return 1;
    }
  }

  else {
    int n = myOptions.c_values_.size();
    MatrixC M(n,n);
    complex<double> answer( 1.0, 0.0 );
    int i = 0;
    for( vector<complex<double> >::iterator it =  myOptions.c_values_.begin();
                                            it != myOptions.c_values_.end();
                                          ++it                               )
    {
      M(i,i)  = *it;
      answer *= *it;
      ++i;
    }

    RandomOrthogonal generator(n);
    Matrix O( generator.build() );
    M = O * M * O.transpose();

    complex<double> value = M.determinant();

    cout << "The matrix:\n" << M << endl;
    cout << "Calculated determinant: " << setprecision(14) << value  << endl;
    cout << "Correct answer        : " << setprecision(14) << answer << endl;

    if( testValue( __LINE__, value, answer ) ) {
      return 1;
    }
  }

  return 0;
}
