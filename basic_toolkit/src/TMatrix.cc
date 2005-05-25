/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BASIC TOOLKIT:  Low level utility C++ classes.
******
******  File:      TMatrix.cc
******  Version:   1.0
******
******  Copyright (c) 2004 Universities Research Association, Inc.
******                All Rights Reserved
******
******  Author:    Leo Michelotti
******
******             Fermilab
******             P.O.Box 500
******             Mail Stop 220
******             Batavia, IL   60510
******
******             Phone: (630) 840 4956
******             Email: michelotti@fnal.gov
******
******  Usage, modification, and redistribution are subject to terms
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include <sstream>
#include <MathConstants.h>


#include "TMatrix.h"
//extern "C" {strstr(char*,char*);} // this may need to be commented out
                                  // on some systems.

extern "C" { void rg_(int*, int*, double*, double*,double*, int*, double*,
                 int*, double*, int*); }

extern "C" { void cg_(int*, int*, double*, double*,double*,double*, int*,
                      double*,double*,double*, double*, double*,int*); }


using namespace std;
using FNAL::Complex;

const double tiny = double(1e-20);
const double limit = double(1e-14);
const int PHASESPACE = 6;

#define M_SMALL  1.0e-30
#define WID setw(8)
#define PREC setprecision(8)


template<typename T>
TMatrix<T>::TMatrix() : _stacked(false)
{
  _ml = new TML<T>;
}

template<typename T>
TMatrix<T>::TMatrix(int rows) : _stacked(false)
{
  _ml = new TML<T>(rows,1,T());
}

template<typename T>
TMatrix<T>::TMatrix(int rows, int columns) : _stacked(false)
{
  _ml = new TML<T>(rows,columns,T());
}

template<typename T>
TMatrix<T>::TMatrix(int rows, int columns, T initval) : _stacked(false)
{
  _ml = new TML<T>(rows,columns,initval);
}

template<typename T>
TMatrix<T>::TMatrix(int rows, int columns, T* initval) : _stacked(false)
{
  _ml = new TML<T>(rows,columns,initval);
}

template<typename T>
TMatrix<T>::TMatrix(const char* flag, int dimension) : _stacked(false)
{
  int    i;
  double tmp_float = dimension;

  _ml = new TML<T>(dimension,dimension,T());

  if( ((double) (dimension/2) != tmp_float/2.0) &&
      (flag[0] == 'J') ) {
    throw( GenericException( dimension, dimension
                     , "TMatrix<T>::TMatrix<T>(const char* flag, int dimension)"
                     , "Dimension must be even for J matrix") );
  }

  if (flag[0]  == 'I') {
    for (i = 0; i< dimension; i++) {
      _ml->_m[i][i] = T(1); //// consider using boost::value_initialized lib ! 
    }
  } else if (flag[0] == 'J') {
    for (i = dimension/2; i< dimension; i++) {
      _ml->_m[i-dimension/2][i] = T(1); //// consider using boost::value_initialized lib !
      _ml->_m[i][i-dimension/2] = T(-1);//// consider using boost::value_initialized lib !
    }
  } else {
      throw( GenericException( dimension, dimension
                     , "TMatrix<T>::TMatrix<T>(const char* flag, int dimension)"
                     , strcat("Unknown flag: ", flag) ) );
  }
}

template<typename T>
TMatrix<T>::TMatrix(const TMatrix& X) 
{
  _stacked = X._stacked;
  _ml = X._ml;
  (_ml->_rc)++;
}

template<typename T>
TMatrix<T>::~TMatrix<T>() 
{
  if( --(_ml->_rc) == 0) { delete _ml; }
}

template<typename T>
TMatrix<T>& TMatrix<T>::operator=(const TMatrix& x) 
{
  if(this != &x) {
    if( --(_ml->_rc) == 0) delete _ml;
#ifndef FORBIDSTACKED
    if(x._stacked) {
      _ml = x._ml;
      (_ml->_rc)++;
    } else {
#endif
      _ml = new TML<T>;
      *_ml = *(x._ml);
#ifndef FORBIDSTACKED
    }
#endif
  }
 _stacked = false;   // ??? This line is probably unnecessary.
 return *this;
}

template<typename T>
TMatrix<T>& TMatrix<T>::DeepCopy(const TMatrix<T>& x) 
{
 if(this != &x) {
   if( --(_ml->_rc) == 0) delete _ml;
     _ml = new TML<T>;
     *_ml = *(x._ml);
 }
 _stacked = false;   // ??? This line is probably unnecessary.
 return *this;
}

template<typename T>
char operator==( const TMatrix<T>& x, const TMatrix<T>& y ) 
{
 return *(x._ml) == *(y._ml);
}

template<typename T>
char operator==( const TMatrix<T>& x, const T& y ) 
{
 return *(x._ml) == y;
}

template<typename T>
char operator==( const T& y, const TMatrix<T>& x )
{
 return *(x._ml) == y;
}

template<typename T>
char operator!=( const TMatrix<T>& x, const TMatrix<T>& y ) 
{
 return !( x == y );
}

template<typename T>
char operator!=( const TMatrix<T>& x, const T& y ) 
{
 return !( x == y );
}

template<typename T>
char operator!=( const T& x, const TMatrix<T>& y ) 
{
 return !( x == y );
}

template<typename T>
void TMatrix<T>::operator+=( const T& x ) 
{
 _ml->operator+=( x );
}

template<typename T>
void TMatrix<T>::operator-=( const T& x ) 
{
 _ml->operator-=( x );
}

template<typename T>
TMatrix<T> TMatrix<T>::Square() const 
{
  int d;

  if( rows() == cols() ) return *this;

  if( rows() < cols() ) d = rows();
  else                  d = cols();

  int     i, j;
  TMatrix<T> z( d, d, T() );
  TML<T>*    zPtr = z._ml;

  for( i = 0; i < d; i++ )
    for( j = 0; j < d; j++ )
      zPtr->_m[i][j] = _ml->_m[i][j];

  return z;
}

template<typename T>
TMatrix<T> TMatrix<T>::transpose() const 
{
  TMatrix<T> z(cols(),rows(),T());
  TML<T>* zPtr = z._ml;

  for (int row = 0; row < rows(); row++)  {
    for(int col = 0; col < cols(); col++)
      zPtr->_m[col][row] = _ml->_m[row][col];
  }
  z._stacked = true;
  return z;
}

template<typename T>
T TMatrix<T>::trace() 
{
  T temp = T();
  if(rows()  != cols()) {
    throw( NotSquare( rows(), cols(), "T TMatrix<T>::trace()" )  );
  }
  else { for (int i = 0; i < rows(); i++)  { temp += _ml->_m[i][i]; } }
  return temp;
}

template<typename T>
bool TMatrix<T>::isOrthogonal() const
{
  if( rows() != cols() ) { return false; }
  int n = rows();
  int i, j;
  TMatrix<T> W( n, n );
  W = (*this) * (this->transpose());
  for( i = 0; i < n; i++ ) {
    for( j = 0; j < n; j++ ) {
      if( i != j ) { if( 1.0e-12 < std::abs( W(i,j) ) )       { return false; } }
      else         { if( 1.0e-12 < std::abs( W(i,j) - 1.0 ) ) { return false; } }
    }
  }
  W = (this->transpose())*(*this);
  for( i = 0; i < n; i++ ) {
    for( j = 0; j < n; j++ ) {
      if( i != j ) { if( 1.0e-12 < std::abs( W(i,j) ) )       { return false; } }
      else         { if( 1.0e-12 < std::abs( W(i,j) - 1.0 ) ) { return false; } }
    }
  }
  return true;
}

template<typename T>
T TMatrix<T>::determinant() 
{
  T det = T();
  if(rows() != cols()) {
    throw( NotSquare( rows(), cols(), "T TMatrix<T>::determinant()" )  );
  }
  else {
    int* indx = new int[cols()]; // create the "index vector
                                 // see pp 38. in Numerical Recipes
    int d;
    // perform the decomposition once:

    TMatrix<T> decomp = _lu_decompose(indx,d);
    det = d;
    for(int i=0; i < cols() ; i++)
      det *= decomp._ml->_m[i][i];
    delete [] indx;
  }
  return det;
}

template<typename T>
TMatrix<T> TMatrix<T>::inverse() const 
{
  if(rows()  != cols()) {
    throw( NotSquare( rows(), cols(), "TMatrix<T> TMatrix<T>::inverse()" )  );
  }
  else {
    TMatrix<T> Y("I",rows()); // create an identity TMatrix<T>
    int* indx = new int[cols()];  // create the "index vector"
    TMatrix<T> B(cols());     // see Press & Flannery
    int d;
    // perform the decomposition once:
    TMatrix<T> decomp = _lu_decompose(indx,d);
    for(int col = 0; col < cols(); col++){
      B._copy_column(Y,col,0);
      decomp._lu_back_subst(indx,B);
      Y._copy_column(B,0,col);
    }
    delete []indx;
    return Y;
  }
}


// operators

template<typename T>
T& TMatrix<T>::operator()(int i, int j) 
{
  if((i >= rows()) || (j >= cols()) ||
     (i < 0      ) || (j < 0      )    )
  { throw( IndexRange( i, j, rows()-1, cols()-1,
                       "T& TMatrix<T>::operator()(int i, int j)" ) );
  }
  else { return _ml->_m[i][j]; }
}

template<typename T>
T  TMatrix<T>::operator()(int i, int j) const 
{
  if((i >= rows()) || (j >= cols()) ||
     (i < 0      ) || (j < 0      )    )
  { throw( IndexRange( i, j, rows()-1, cols()-1,
                       "T TMatrix<T>::operator()(int i, int j) const" ) );
  }
  else { return _ml->_m[i][j]; }
}

template<typename T>
T TMatrix<T>::getElement(int i, int j) const 
{
  return this->operator()(i,j);
}

template<typename T>
T& TMatrix<T>::operator()(int i) 
{
  // This body must stay in synch with
  // T TMatrix<T>::operator()(int i) const
  if( rows() == 1 ) {
    if( i >= 0 && i < cols() ) {
      return _ml->_m[0][i];
    }
    else {
      throw( IndexRange( 0, i, 0, cols()-1,
                       "T& TMatrix<T>::operator()(int i)" ) );
    }
  }
  else if( cols() == 1 ) {
    if( i >= 0 && i < rows() ) {
      return _ml->_m[i][0];
    }
    else {
      throw( IndexRange( i, 0, rows()-1, 0,
                       "T& TMatrix<T>::operator()(int i)" ) );
    }
  }
  else {
    throw( NotVector( i, rows(), cols(),
                      "T& TMatrix<T>::operator()(int i)" ) );
  }
}

template<typename T>
T TMatrix<T>::operator()(int i) const 
{
  // This body must stay in synch with
  // T& TMatrix<T>::operator()(int i)
  if( rows() == 1 ) {
    if( i >= 0 && i < cols() ) {
      return _ml->_m[0][i];
    }
    else {
      throw( IndexRange( 0, i, 0, cols()-1,
                       "T& TMatrix<T>::operator()(int i)" ) );
    }
  }
  else if( cols() == 1 ) {
    if( i >= 0 && i < rows() ) {
      return _ml->_m[i][0];
    }
    else {
      throw( IndexRange( i, 0, rows()-1, 0,
                       "T& TMatrix<T>::operator()(int i)" ) );
    }
  }
  else {
    throw( NotVector( i, rows(), cols(),
                      "T& TMatrix<T>::operator()(int i)" ) );
  }
}

template<typename T>
ostream& operator<<(ostream& os, const TMatrix<T>& x)
{
  int i,j;

  for(i=0; i< x.rows(); i++) {
    os << "( ";
    for(j=0; j< x.cols(); j++) {
      os  << x._ml->_m[i][j] << ", ";
    }
    os << " )\n";
  }
  return os << endl;
}


template<typename T>
TMatrix<T> operator+(const TMatrix<T>& x, const TMatrix<T>& y) 
{
  TMatrix<T> z(x.rows(),x.cols(),T());
  TML<T>* xPtr = x._ml;
  TML<T>* yPtr = y._ml;
  TML<T>* zPtr = z._ml;
  if( (x.rows() != y.rows()) || (x.cols() != y.cols()) ) {
    throw( typename TMatrix<T>::Incompatible( x.rows(), x.cols(), y.rows(), y.cols(),
           "TMatrix<T> operator+(const TMatrix<T>& x, const TMatrix<T>& y)" ) );
  }
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->_m[i][j] = xPtr->_m[i][j] + yPtr->_m[i][j];
      if(std::abs(zPtr->_m[i][j]) < M_SMALL*std::abs(yPtr->_m[i][j]))
        zPtr->_m[i][j] = T();
    }
  }
  z._stacked = true;
  return z;
}

template<typename T>
TMatrix<T> operator+(const TMatrix<T>& x, const T& y) 
{
 TMatrix<T> z;
 z.DeepCopy( x );
 z += y;
 z._stacked = true;
 return z;
}

template<typename T>
TMatrix<T> operator+( const T& y, const TMatrix<T>& x ) 
{
 TMatrix<T> z;
 z.DeepCopy( x );
 z += y;
 z._stacked = true;
 return z;
}

template<typename T>
TMatrix<T> operator-(const TMatrix<T>& x) 
{
  TMatrix<T> z(x.rows(),x.cols(),T());
  TML<T>* xPtr = x._ml;
  TML<T>* zPtr = z._ml;
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->_m[i][j] = - ( xPtr->_m[i][j] );
    }
  }
  z._stacked = true;
  return z;
}

template<typename T>
TMatrix<T> operator-(const TMatrix<T>& x, const TMatrix<T>& y) 
{
  TMatrix<T> z(x.rows(),x.cols(),T());
  TML<T>* xPtr = x._ml;
  TML<T>* yPtr = y._ml;
  TML<T>* zPtr = z._ml;
  if(( x.rows() != y.rows()) || (x.cols() != y.cols())) {
    throw( typename TMatrix<T>::Incompatible( x.rows(), x.cols(), y.rows(), y.cols(),
           "TMatrix<T> operator-(const TMatrix<T>& x, const TMatrix<T>& y)" ) );
  }
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->_m[i][j] = xPtr->_m[i][j] - yPtr->_m[i][j];
      if(std::abs(zPtr->_m[i][j]) < M_SMALL*std::abs(yPtr->_m[i][j]))
        zPtr->_m[i][j] = T();
    }
  }
  z._stacked = true;
  return z;
}

template<typename T>
TMatrix<T> operator-(const TMatrix<T>& x, const T& y) 
{
 TMatrix<T> z;
 z.DeepCopy( x );
 z -= y;
 z._stacked = true;
 return z;
}

template<typename T>
TMatrix<T> operator-( const T& y, const TMatrix<T>& x ) 
{
 TMatrix<T> z;
 z.DeepCopy( x );
 z -= y;
 z._stacked = true;
 return z;
}

template<typename T>
TMatrix<T> operator*( const TMatrix<T>& x, const T y) 
{
  TMatrix<T> z(x.rows(),x.cols(),T());
  TML<T>* xPtr = x._ml;
  TML<T>* zPtr = z._ml;

  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->_m[i][j] = xPtr->_m[i][j] * y;
    }
  }
 z._stacked = true;
 return z;
}

template<typename T>
TMatrix<T> operator*( const T y, const TMatrix<T>& x) 
{
  TMatrix<T> z(x.rows(),x.cols(),T());
  TML<T>* xPtr = x._ml;
  TML<T>* zPtr = z._ml;

  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->_m[i][j] = xPtr->_m[i][j] * y;
    }
  }
 z._stacked = true;
 return z;
}

template<typename T>
TMatrix<T> operator*(const TMatrix<T>& x, const TMatrix<T>& y)  
{
  TMatrix<T> z(x.rows(),y.cols(),T());
  TML<T>* xPtr = x._ml;
  TML<T>* yPtr = y._ml;
  TML<T>* zPtr = z._ml;
  if( x.cols() != y.rows()) {
    throw( typename TMatrix<T>::Incompatible( x.rows(), x.cols(), y.rows(), y.cols(),
           "TMatrix<T> operator*(const TMatrix<T>& x, const TMatrix<T>& y)" ) );
  }
  T tmp;
  for(int row = 0; row < x.rows(); row++) {
    for(int col = 0; col < y.cols(); col++){
      T sum = T();
      for(int i = 0; i < x.cols(); i++) {
        tmp = xPtr->_m[row][i] * yPtr->_m[i][col];
        sum += tmp;
        if(std::abs(sum) < M_SMALL*std::abs(tmp))
          sum = T();
      }
      zPtr->_m[row][col] = sum;
    }
  }
  z._stacked = true;
  return z;
}

template<typename T>
TMatrix<T> operator/( const TMatrix<T>& x, const T y) 
{
  TMatrix<T> z(x.rows(),x.cols(), T());
  TML<T>* xPtr = x._ml;
  TML<T>* zPtr = z._ml;
  if(y == T())
    cerr << "TMatrix<T>:operator/ divide by zero" << endl;
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->_m[i][j] = xPtr->_m[i][j] / y;
    }
  }
 z._stacked = true;
 return z;
}

template<typename T>
TMatrix<T> operator/( const T y, TMatrix<T>& x) 
{
  return (y*x.inverse());
}

template<typename T>
TMatrix<T> operator/( TMatrix<T>& y, TMatrix<T>& x) 
{
  return (y*x.inverse());
}

// *******************************************************
// The private support functions for determinant & inverse:
// *******************************************************

// copy the from_col of mm to the to_col of "this"

template<typename T>
void TMatrix<T>::_copy_column(TMatrix<T>& mm, int from_col, int to_col)
{
  if(rows()  != mm.rows()) {
    throw( Incompatible( rows(), cols(), mm.rows(), mm.cols(),
       "void TMatrix<T>::_copy_column(TMatrix<T>& mm, int from_col, int to_col)" )  );
  }
  else {
    for(int row=0; row < rows(); row++)
      _ml->_m[row][to_col] = mm._ml->_m[row][from_col];
  }
}

template<typename T>
void TMatrix<T>::_switch_columns(int col1, int col2) 
{
  TMatrix<T> temp(rows());

  int row; // O.K.

  for(row = 0; row < rows(); row++)
    // temporarily store col 1:
    temp._ml->_m[row][0] = _ml->_m[row][col1];
  for(row = 0; row < rows(); row++)
    _ml->_m[row][col1] = _ml->_m[row][col2]; // move col2 to col1
  for(row = 0; row < rows(); row++)
    _ml->_m[row][col2] = temp._ml->_m[row][0]; // move temp to col2
}

template<typename T>
void TMatrix<T>::_switch_rows(int row1, int row2) 
{
  TMatrix<T> temp(cols());
  int col; // O.K.

  for(col = 0; col < cols(); col++)
    // temporarily store row 1:
    temp._ml->_m[col][0] = _ml->_m[row1][col];
  for(col = 0; col < cols(); col++)
    _ml->_m[row1][col] = _ml->_m[row2][col]; // move row2 to row1
  for(col = 0; col < cols(); col++)
    _ml->_m[row2][col] = temp._ml->_m[col][0]; // move temp to row2
}

// scale a matrix (used in L-U decomposition)

template<typename T>
TMatrix<T> TMatrix<T>::_scale()  
{
  T temp;
  if (rows() <= 0 || cols() <= 0) {
    throw( GenericException( rows(), cols()
                   , "TMatrix<T> TMatrix<T>::_scale()"
                   , "Bad TMatrix<T> for _scale()" ) );
  }
  if (rows() != cols()) {
    throw( NotSquare( rows(), cols(), "TMatrix<T> TMatrix<T>::_scale()" )  );
  }
  TMatrix<T> scale_vector(cols());

  for (int row = 0; row < rows(); row++){
    double maximum = 0.0;
    for(int col = 0; col < cols(); col++) {
      temp = _ml->_m[row][col];
      if (std::abs(temp) > maximum)
        maximum = std::abs(temp);
    }
    if(maximum == 0.0 ) {
      cerr << "\n*** ERROR *** Matrix = \n" << *this << endl;
      throw( GenericException( rows(), cols()
                     , "TMatrix<T> TMatrix<T>::_scale()"
                     , "Singular TMatrix<T> in _scale()" ) );
    }
    // save scaling
    scale_vector._ml->_m[row][0] = T(1)/maximum;
  }
  return scale_vector;
}

template<typename T>
TMatrix<T> TMatrix<T>::_lu_decompose(int* indx, int& d ) const
{
  // Returns the L-U decomposition of a matrix. indx is an ouput
  // vector which records the row permutation affected by the
  // partial pivoting, d is output as +-1 depending on whether the
  // number of row interchanges was even or odd, respectively.
  // This routine is used in combination with _lu_back_subst to
  // solve linear equations or invert a matrix.

  if(rows()  != cols()) {
    throw( NotSquare( rows(), cols(),
      "TMatrix<T> TMatrix<T>::_lu_decompose(int* indx, int& d ) const " )  );
  }
  d = 1; // parity check
  int j,i,k,col_max; // counters
  T dum; // from the book - I don't know signj
  T sum;
  T maximum;
  TMatrix<T> lu_decomp;
  T tmp;

  // make a direct copy of the original matrix:
  lu_decomp = *this;
  TMatrix<T> scale_vector = lu_decomp._scale(); // scale the matrix

  // The loop over columns of Crout's method:
  for(j = 0; j < rows(); j++) {
    if (j > 0) {
      // eqn 2.3.12 except for j=i:
      for (i = 0; i <= j-1; i++) {
        sum = lu_decomp._ml->_m[i][j];
        if(i > 0) {
          for(k = 0; k <= i-1; k++) {
            tmp = lu_decomp._ml->_m[i][k]*lu_decomp._ml->_m[k][j];
            sum -= tmp;
            if(std::abs(sum) < M_SMALL*std::abs(tmp))
              sum = T();
          }
          lu_decomp._ml->_m[i][j] = sum;
        }
      }
    }

// Initialize the search for the largest pivot element:
    maximum = T();
    // i=j of eq 2.3.12 & i=j+I..N of 2.3.13:
    for(i=j; i <= cols()-1; i++) {
      sum = lu_decomp._ml->_m[i][j];

      if(j > 0) {
        for(k=0; k <= j-1; k++) {
          tmp =  lu_decomp._ml->_m[i][k] * lu_decomp._ml->_m[k][j];
          sum -= tmp;
          if(std::abs(sum) < M_SMALL*std::abs(tmp))
            sum = T();
        }
        lu_decomp._ml->_m[i][j] = sum;
      }
      // figure of merit for pivot:
      dum = scale_vector._ml->_m[i][0] * std::abs(sum);
      if (dum >= maximum){
        // is it better than the best so far ?
        col_max = i;
        maximum = std::abs(dum);
      }
    }
    // Do we need to interchange rows?

    if(j != col_max)  {
      lu_decomp._switch_rows(col_max,j); // Yes, do so ...
      d *= -1;  // ... and change the parity of d
      // also interchange the scale factor:
      dum = scale_vector._ml->_m[col_max][0];
      scale_vector._ml->_m[col_max][0] = scale_vector._ml->_m[j][0];
      scale_vector._ml->_m[j][0] = dum;
    }

    indx[j] = col_max;
    // Now, finally, divide by the pivot element:
    if(j != rows() -1){
      // If the pivot element is .zero the matrix is
      // singular (at least to the precision of the
      // algorithm). For some applications on singular
      // matrices, it is desirable to substitute tiny for 0
      if(lu_decomp._ml->_m[j][j] == T())
        lu_decomp._ml->_m[j][j] = T(tiny); /// this may not work for all types
                                            
      dum = T(1)/lu_decomp._ml->_m[j][j];
      for(i=j+1; i <= cols()-1; i++)
        lu_decomp._ml->_m[i][j] *= dum;
    }

  }
  if(lu_decomp._ml->_m[rows()-1][cols()-1] == T())
    lu_decomp._ml->_m[rows()-1][cols()-1] = T(tiny);

  return lu_decomp;
}


template<typename T>
void TMatrix<T>::_lu_back_subst(int* indx, TMatrix<T>& b)  
{
// Solves the set of N linear equations A*X = B. Here "this"
// is the LU-decomposition of the matrix A, determined by the
// routine _lu_decompose(). Indx is input as the permutation
// vector returned  by _lu_decompose(). B is input as the
// right-hand side vector B, and returns with the solution
// vector X. This routine takes into  account the possibility
// that B will begin with many zero elements, so it is efficient
// for use in matrix inversion. See pp 36-37 in
// Press & Flannery.

  if(rows()  != cols()) {
    throw( NotSquare( rows(), cols(),
      "void TMatrix<T>::_lu_back_subst(int* indx, TMatrix<T>& b)" )  );
  }
  if(rows()  != b.rows()) {
    throw( Incompatible( rows(), cols(), b.rows(), b.cols(),
      "void TMatrix<T>::_lu_back_subst(int* indx, TMatrix<T>& b)" )  );
  }
//  if(rows()  != indx.rows())
//    error("wrong size indx vector passed to _lu_back_subst()");
  int i,j,ip;
  int ii = -1;
  T sum;
  T tmp;

  for(i=0;i < rows(); i++){
    ip = indx[i];
    sum = b._ml->_m[ip][0];
    b._ml->_m[ip][0] = b._ml->_m[i][0];
    if (ii >= 0) {
      for(j = ii; j <= i-1; j++) {
        tmp = _ml->_m[i][j] * b._ml->_m[j][0];
        sum -= tmp;
        if(std::abs(sum) < M_SMALL*std::abs(tmp))
          sum = T ();
      }
    } else if(sum != T() )
      ii = i;
    b._ml->_m[i][0] = sum;
  }
  for(i = rows() -1; i >= 0; i--){
    sum = b._ml->_m[i][0];
    if (i < cols() -1) {
      for (j = i + 1; j <= rows()-1; j++) {
        tmp = _ml->_m[i][j] * b._ml->_m[j][0];
        sum -= tmp;
        if(std::abs(sum) < M_SMALL*std::abs(tmp))
          sum = T();
      }
    }
    // store a component of the soln vector X:
    b._ml->_m[i][0] = sum/_ml->_m[i][i];
  }
}



// Implementation of exception subclasses

template<typename T>
TMatrix<T>::IndexRange::IndexRange( int a, int b, int c, int d, const char* f )
: i(a), j(b), im(c), jm(d)
{
  static bool firstTime = true;
  if( firstTime ) {
    cerr << "\n*** ERROR *** "
            "\n*** ERROR *** " << f
         << "\n*** ERROR *** limits are " << im << " " << jm
         << "\n*** ERROR *** You asked for "<< i << " " << j
         << "\n*** ERROR *** matrix limits exceeded "
            "\n*** ERROR *** "
            "\n*** ERROR *** This message is printed only once."
         << endl;
    firstTime = false;
  }
}


template<typename T>
const char* TMatrix<T>::IndexRange::what() const throw()
{
  return "Index out of range.";
}


template<typename T>
TMatrix<T>::NotVector::NotVector( int x, int y, int z, const char* f )
: i(x), r(y), c(z)
{
  static bool firstTime = true;
  if( firstTime ) {
    cerr << "\n*** ERROR *** "
            "\n*** ERROR *** " << f
         << "\n*** ERROR *** Matrix is not a vector:"
            "\n*** ERROR *** its dimenions are " << r << " x " << c
         << "\n*** ERROR *** "
            "\n*** ERROR *** "
            "\n*** ERROR *** This message is printed only once."
         << endl;
    firstTime = false;
  }
}


template<typename T>
const char* TMatrix<T>::NotVector::what() const throw()
{
  return "Matrix is not a vector.";
}


template<typename T>
TMatrix<T>::Incompatible::Incompatible( int x, int y, int z, int t, const char* f )
  : ra(x), ca(y), rb(z), cb(t)
{
  static bool firstTime = true;
  if( firstTime ) {
    cerr << "\n*** ERROR *** "
            "\n*** ERROR *** " << f
         << "\n*** ERROR *** Incompatible dimensions between matrices."
            "\n*** ERROR *** First  argument is " << ra << " x " << ca
         << "\n*** ERROR *** Second argument is " << rb << " x " << cb
         << "\n*** ERROR *** "
            "\n*** ERROR *** This message is printed only once."
         << endl;
    firstTime = false;
  }
}


template<typename T>
const char* TMatrix<T>::Incompatible::what() const throw()
{
  return "Incompatible arguments.";
}


template<typename T>
TMatrix<T>::NotSquare::NotSquare( int x, int y, const char* f )
  : r(x), c(y)
{
  static bool firstTime = true;
  if( firstTime ) {
    cerr << "\n*** ERROR *** "
            "\n*** ERROR *** " << f
         << "\n*** ERROR *** Matrix must be square: it's dimensions are "
         << r << " x " << c
         << "\n*** ERROR *** "
            "\n*** ERROR *** This message is printed only once."
         << endl;
    firstTime = false;
  }
}


template<typename T>
const char* TMatrix<T>::NotSquare::what() const throw()
{
  return "Matrix is not square.";
}


template<typename T>
TMatrix<T>::GenericException::GenericException( int x, int y, 
                                             const char* f, const char* _m )
: r(x), c(y)
{
  ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** " << f
      << "\n*** ERROR *** " << _m
      << "\n*** ERROR *** The dimensions are "
      << r << " x " << c
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    cerr << errorString;
    cerr << "\n*** ERROR *** This message is printed only once."
         << endl;
    firstTime = false;
  }
}


template<typename T>
const char* TMatrix<T>::GenericException::what() const throw()
{
  return errorString.c_str();
}


// Implementation of class MatrixD::RandomOrthogonal

RandomOrthogonal::RandomOrthogonal( int n )
: _dim(n), _passes(1)
{
  int i, j;

  _omitted = new bool*[ _dim ];
  for( i = 0; i < _dim; i++ ) {
    _omitted[i] = new bool[ _dim ];
    for( j = 0; j < _dim; j++ ) { _omitted[i][j] = false; }
    _omitted[i][i] = true;
  }

  _lowerTheta = new double*[ _dim ];
  for( i = 0; i < _dim; i++ ) {
    _lowerTheta[i] = new double[ _dim ];
    for( j = 0; j < _dim; j++ ) { _lowerTheta[i][j] = 0.0; }
  }

  _upperTheta = new double*[ _dim ];
  for( i = 0; i < _dim; i++ ) {
    _upperTheta[i] = new double[ _dim ];
    for( j = 0; j < _dim; j++ ) { _upperTheta[i][j] = M_TWOPI; }
    _upperTheta[i][i] = 0.0;
  }

  _rangeTheta = new double*[ _dim ];
  for( i = 0; i < _dim; i++ ) {
    _rangeTheta[i] = new double[ _dim ];
    for( j = 0; j < _dim; j++ ) { _rangeTheta[i][j] = M_TWOPI; }
    _rangeTheta[i][i] = 0.0;
  }
}


RandomOrthogonal::~RandomOrthogonal()
{
  for( int i = 0; i < _dim; i++ ) {
    delete [] _omitted[i];
    delete [] _lowerTheta[i];
    delete [] _upperTheta[i];
    delete [] _rangeTheta[i];
  }

  delete [] _omitted;
  delete [] _lowerTheta;
  delete [] _upperTheta;
  delete [] _rangeTheta;
}


void RandomOrthogonal::omitIndex( int i, int j )
{
  if( 0 <= i  &&  i < _dim  &&  0 <= j  &&  j < _dim ) {
    _omitted[i][j] = true;
    _omitted[j][i] = true;
  }
}


void RandomOrthogonal::omitIndex( int n )
{
  for( int i = 0; i < _dim; i++ ) {
    this->omitIndex( n, i );
  }
}


void RandomOrthogonal::includeIndex( int i, int j )
{
  if( 0 <= i  &&  i < _dim  &&  0 <= j  &&  j < _dim ) {
    _omitted[i][j] = false;
    _omitted[j][i] = false;
  }
}


void RandomOrthogonal::includeIndex( int n )
{
  for( int i = 0; i < _dim; i++ ) {
    this->includeIndex( n, i );
  }
}


void RandomOrthogonal::setNumberOfPasses( int n )
{
  if( 0 < n ) { _passes = n; }
}


void RandomOrthogonal::setRange( int i, int j, double lo, double hi )
{
  // Check and condition the arguments
  if( i < 0  ||  _dim <= i  ||
      j < 0  ||  _dim <= j  ||
      hi <= lo )
  { return; }

  if( std::abs(lo) < 1.0e-12            ) { lo = 0.0;     }
  if( std::abs(hi - M_TWOPI ) < 1.0e-12 ) { hi = M_TWOPI; }

  while( lo < 0.0      ) { lo += M_TWOPI; }
  while( lo > M_TWOPI  ) { lo -= M_TWOPI; }
  while( hi < 0.0      ) { hi += M_TWOPI; }
  while( hi > M_TWOPI  ) { hi -= M_TWOPI; }

  // Set the range
  _lowerTheta[i][j] = lo;
  _lowerTheta[j][i] = lo;
  _upperTheta[i][j] = hi;
  _upperTheta[j][i] = hi;
  _rangeTheta[i][j] = hi - lo;
  _rangeTheta[j][i] = hi - lo;
}


void RandomOrthogonal::setRange( int n, double lo, double hi )
{
  for( int i = 0; i < _dim; i++ ) {
    this->setRange( n, i, lo, hi );
  }
}


TMatrix<double> RandomOrthogonal::build()
{
  // This was programmed very inefficiently
  // but quickly. It will have to be redone
  // if used many times in an application.

  int i, j, p;
  double theta, cs, sn, tmp;
  TMatrix<double> R( "I", _dim );
  TMatrix<double> U( "I", _dim );
  TMatrix<double> W( "I", _dim );

  for( p = 0; p < _passes; p++ ) {
    for( i = 0; i < _dim-1; i++ ) {
      for( j = i+1; j < _dim; j++ ) {
        if( !(_omitted[i][j]) ) {
          theta = _lowerTheta[i][j] + drand48()*_rangeTheta[i][j];
          cs = cos( theta );
          sn = sin( theta );
          W(i,i) =  cs;  W(i,j) = sn;
          W(j,i) = -sn;  W(j,j) = cs;
          R = R*W;
          W = U;
        }
      }
    }
  }

  return R;
}


TMatrix<double> real( const TMatrix<FNAL::Complex>& x )
{
  int r = x.rows();
  int c = x.cols();
  TMatrix<double> ret( r, c );
  for( int i = 0; i < r; i++ ) {
    for( int j = 0; j < c; j++ ) {
      ret(i,j) = real((const FNAL::Complex&) (x(i,j)));
    }
  }
  return ret;
}


TMatrix<double> imag( const TMatrix<FNAL::Complex>& x )
{
  int r = x.rows();
  int c = x.cols();
  TMatrix<double> ret( r, c );
  for( int i = 0; i < r; i++ ) {
    for( int j = 0; j < c; j++ ) {
      ret(i,j) = imag((const FNAL::Complex&) (x(i,j)));
    }
  }
  return ret;
}


// The four eigen routines written here are anachronistic
// messes! They should be rewritten from scratch and
// fully templatized!

TMatrix<FNAL::Complex> TMatrix<double>::eigenValues() 
{
  int nm = rows();
  int n  = cols();
  int matz = 1;
  double* wr = new double[cols()];
  double* wi = new double[cols()];
  int* iv1 = new int[cols()];
  double* fv1 = new double[cols()];
  double* b = new double[rows()*cols()];
  double* c = new double[rows()*cols()];
  int ierr,i,j,k;
  int oddEven = nm/2;
  int realCount = 0;
  TMatrix<FNAL::Complex> tmp(rows(),cols(),complex_0);
  TMatrix<FNAL::Complex> tmp1(1,cols(),complex_0);

  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      b[k] = _ml->_m[j][i];       // the rg_ routine uses the transpose
      k++;
    }
  }
  rg_(&nm,&n,b,wr,wi,&matz,c,iv1,fv1,&ierr);

  if(ierr != 0) {
    cerr << "TMatrix<double>: Error in eigenvalue routine. error = ";
    cerr << ierr << endl;
    delete []wr;
    delete []wi;
    delete []iv1;
    delete []fv1;
    delete []b;
    delete []c;
    return tmp;
  }
  TMatrix<double> z1(nm,n,0.0);
  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      z1(i,j) = c[k];
      k++;
    }
  }

  i = 0;
  k = 0;
  j = 0;
  while( i< nm) {
    if(wi[i] == 0){
      realCount++;
      for(j=0; j<n; j++) {
        tmp(j,k) = Complex(z1(i,j),0.0);
      }
      tmp1(0,k) = Complex(wr[i],0.0);
      k++;
      i++;
    } else {
      tmp1(0,k)   = Complex(wr[i],wi[i]);
      tmp1(0,k+1) = Complex(wr[i+1],wi[i+1]);
      for(j=0; j<n; j++) {
        tmp(j,k)   = Complex(z1(i,j),z1(i+1,j));
        tmp(j,k+1) = Complex(z1(i,j),-z1(i+1,j));
      }
      k += 2;
      i += 2;
    }
  }
  if((nm == PHASESPACE) && (n == PHASESPACE)) {
    if(realCount == 0) {
      tmp._switch_columns(3,4);
      tmp1._switch_columns(3,4);
      tmp._switch_columns(1,2);
      tmp1._switch_columns(1,2);
      tmp._switch_columns(2,3);
      tmp1._switch_columns(2,3);
    }
    if(imag(tmp1(0,0)) == 0.0) {
      if(imag(tmp1(0,1)) == 0.0) {
        tmp._switch_columns(0,2);
        tmp1._switch_columns(0,2);
        tmp._switch_columns(1,4);
        tmp1._switch_columns(1,4);
        tmp._switch_columns(4,5);
        tmp1._switch_columns(4,5);
      } else if(imag(tmp1(0,3)) == 0.0) {
        tmp._switch_columns(2,3);
        tmp1._switch_columns(2,3);
        tmp._switch_columns(0,1);
        tmp1._switch_columns(0,1);
        tmp._switch_columns(2,4);
        tmp1._switch_columns(2,4);
        tmp._switch_columns(4,5);
        tmp1._switch_columns(4,5);
      } else if(imag(tmp1(0,5)) == 0.0) {
        tmp._switch_columns(2,3);
        tmp1._switch_columns(2,3);
        tmp._switch_columns(0,1);
        tmp1._switch_columns(0,1);
        tmp._switch_columns(1,2);
        tmp1._switch_columns(1,2);
      }
    } else if(imag(tmp1(0,2)) == 0.0) {
      if(imag(tmp1(0,5)) == 0.0) {
        tmp._switch_columns(1,3);
        tmp1._switch_columns(1,3);
      } else if(imag(tmp1(0,3)) == 0.0) {
        tmp._switch_columns(4,5);
        tmp1._switch_columns(4,5);
        tmp._switch_columns(1,5);
        tmp1._switch_columns(1,5);
        tmp._switch_columns(3,5);
        tmp1._switch_columns(3,5);
      }
    } else if(imag(tmp1(0,4)) == 0.0) {
      tmp._switch_columns(3,4);
      tmp1._switch_columns(3,4);
      tmp._switch_columns(1,2);
      tmp1._switch_columns(1,2);
      tmp._switch_columns(2,3);
      tmp1._switch_columns(2,3);
    }
  }

  delete []wr;
  delete []wi;
  delete []iv1;
  delete []fv1;
  delete []b;
  delete []c;

  int sortFlag = 1;
  int counter = 0;
  while ((sortFlag == 1)&& (counter < 10)) {
    sortFlag = 0;
    for(i=1; i < oddEven; i++) {
      if(abs(tmp(0,0)) < abs(tmp(0,i))) {
        tmp._switch_columns(0,i);
        tmp1._switch_columns(0,i);
        sortFlag = 1;
        if((oddEven*2) == nm) {
          tmp._switch_columns(oddEven,oddEven+i);
          tmp1._switch_columns(oddEven,oddEven+i);
        }
      }
    }
    counter++;
  }
  if(counter >= 10)
    cerr << "TMatrix<double>: Something is wrong with the eigenValue sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(tmp(1,1)) < abs(tmp(1,i))) {
      tmp._switch_columns(1,i);
      tmp1._switch_columns(1,i);
      sortFlag = 1;
      if((oddEven*2) == nm) {
        tmp._switch_columns(oddEven+1,oddEven+i);
        tmp1._switch_columns(oddEven+1,oddEven+i);
      }
    }
  }
  return tmp1;

}

TMatrix<FNAL::Complex> TMatrix<double>::eigenVectors() 
{
  int nm = rows();
  int n  = cols();
  int matz = 1;
  double* wr = new double[cols()];
  double* wi = new double[cols()];
  int* iv1 = new int[cols()];
  double* fv1 = new double[cols()];
  double* b = new double[rows()*cols()];
  double* c = new double[rows()*cols()];
  int ierr,i,j,k;
  int oddEven = nm/2;
  int realCount = 0;
  TMatrix<FNAL::Complex> tmp(rows(),cols(),complex_0);
  TMatrix<FNAL::Complex> tmp1(1,cols(),complex_0);

  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      b[k] = _ml->_m[j][i];       // the rg_ routine uses the transpose
      k++;
    }
  }
  rg_(&nm,&n,b,wr,wi,&matz,c,iv1,fv1,&ierr);

  if(ierr != 0) {
    cerr << "TMatrix<double>: Error in eigenvector routine. error = ";
    cerr << ierr<< endl;
    delete []wr;
    delete []wi;
    delete []iv1;
    delete []fv1;
    delete []b;
    delete []c;
    return tmp;
  }
  TMatrix<double> z1(nm,n,0.0);
  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      z1(i,j) = c[k];
      k++;
    }
  }

  i = 0;
  k = 0;
  j = 0;
  while( i< nm) {
    if(wi[i] == 0){
      realCount++;
      for(j=0; j<n; j++) {
        tmp(j,k) = Complex(z1(i,j),0.0);
      }
      tmp1(0,k) = Complex(wr[i],0.0);
      k++;
      i++;
    } else {
      tmp1(0,k)   = Complex(wr[i],wi[i]);
      tmp1(0,k+1) = Complex(wr[i+1],wi[i+1]);
      for(j=0; j<n; j++) {
        tmp(j,k)   = Complex(z1(i,j),z1(i+1,j));
        tmp(j,k+1) = Complex(z1(i,j),-z1(i+1,j));
      }
      k += 2;
      i += 2;
    }
  }
  if((nm == PHASESPACE) && (n == PHASESPACE)) {
    if(realCount == 0) {
      tmp._switch_columns(3,4);
      tmp._switch_columns(1,2);
      tmp._switch_columns(2,3);
    }
    if(imag(tmp1(0,0)) == 0.0) {
      if(imag(tmp1(0,1)) == 0.0) {
        tmp._switch_columns(0,2);
        tmp._switch_columns(1,4);
        tmp._switch_columns(4,5);
      } else if(imag(tmp1(0,3)) == 0.0) {
        tmp._switch_columns(2,3);
        tmp._switch_columns(0,1);
        tmp._switch_columns(2,4);
        tmp._switch_columns(4,5);
      } else if(imag(tmp1(0,5)) == 0.0) {
        tmp._switch_columns(2,3);
        tmp._switch_columns(0,1);
        tmp._switch_columns(1,2);
      }
    } else if(imag(tmp1(0,2)) == 0.0) {
      if(imag(tmp1(0,5)) == 0.0) {
        tmp._switch_columns(1,3);
      } else if(imag(tmp1(0,3)) == 0.0) {
        tmp._switch_columns(4,5);
        tmp._switch_columns(1,5);
        tmp._switch_columns(3,5);
      }
    } else if(imag(tmp1(0,4)) == 0.0) {
      tmp._switch_columns(3,4);
      tmp._switch_columns(1,2);
      tmp._switch_columns(2,3);
    }
  }

  delete []wr;
  delete []wi;
  delete []iv1;
  delete []fv1;
  delete []b;
  delete []c;
  int sortFlag = 1;
  int counter = 0;
  while ((sortFlag == 1)&& (counter < 10)) {
    sortFlag = 0;
    for(i=1; i < oddEven; i++) {
      if(abs(tmp(0,0)) < abs(tmp(0,i))) {
        tmp._switch_columns(0,i);
        sortFlag = 1;
        if((oddEven*2) == nm)
          tmp._switch_columns(oddEven,oddEven+i);
      }
    }
    counter++;
  }
  if(counter >= 10)
    cerr << "TMatrix<double>: Something is wrong with the eigenVector sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(tmp(1,1)) < abs(tmp(1,i))) {
      tmp._switch_columns(1,i);
      if((oddEven*2) == nm)
        tmp._switch_columns(1+oddEven,oddEven+i);
    }
  }
  return tmp;

}


TMatrix<FNAL::Complex> TMatrix<FNAL::Complex>::eigenValues() {
  int nm = rows();
  int n  = cols();
  int matz = 1;
  double* wr = new double[cols()];
  double* wi = new double[cols()];
  double* fv1 = new double[cols()];
  double* fv2 = new double[cols()];
  double* fv3 = new double[cols()];
  double* br = new double[rows()*cols()];
  double* bi = new double[rows()*cols()];
  double* cr = new double[rows()*cols()];
  double* ci = new double[rows()*cols()];
  int ierr,i,j,k;
  int oddEven = nm/2;
  TMatrix<FNAL::Complex> tmp(1,cols(),complex_0);

  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      br[k] = real(_ml->_m[j][i]); // the cg_ routine uses the transpose
      bi[k] = imag(_ml->_m[j][i]); // the cg_ routine uses the transpose
      k++;
    }
  }
  TMatrix<FNAL::Complex> z1(nm,n,complex_0);
  cg_(&nm,&n,br,bi,wr,wi,&matz,cr,ci,fv1,fv2,fv3,&ierr);
  if(ierr != 0) {
    cerr << "TMatrix<FNAL::Complex>: Error in eigenvalue routine. error = ";
    cerr << ierr<< endl;
    delete []wr;
    delete []wi;
    delete []fv1;
    delete []fv2;
    delete []fv3;
    delete []br;
    delete []bi;
    delete []cr;
    delete []ci;
    return tmp;
  }
  for(i = 0; i< cols(); i++)
    tmp._ml->_m[0][i] = Complex(wr[i],wi[i]);
  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      z1(i,j) = Complex(cr[k],ci[k]);
      k++;
    }
  }
  delete []wr;
  delete []wi;
  delete []fv1;
  delete []fv2;
  delete []fv3;
  delete []br;
  delete []bi;
  delete []cr;
  delete []ci;

  int sortFlag = 1;
  int counter = 0;
  while ((sortFlag == 1)&& (counter < 10)) {
    sortFlag = 0;
    for(i=1; i < oddEven; i++) {
      if(abs(z1(0,0)) < abs(z1(0,i))) {
        z1._switch_columns(0,i);
        tmp._switch_columns(0,i);
        sortFlag = 1;
        if((oddEven*2) == nm)
          z1._switch_columns(oddEven,oddEven+i);
          tmp._switch_columns(oddEven,oddEven+i);
      }
    }
    counter++;
  }
  if(counter >= 10)
    cerr << "TMatrix<FNAL::Complex>: Something is wrong with the eigenValue sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(z1(1,1)) < abs(z1(1,i))) {
      z1._switch_columns(1,i);
      tmp._switch_columns(1,i);
      if((oddEven*2) == nm)
        z1._switch_columns(1+oddEven,oddEven+i);
        tmp._switch_columns(1+oddEven,oddEven+i);
    }
  }
  return tmp;
}


TMatrix<FNAL::Complex> TMatrix<FNAL::Complex>::eigenVectors() {
  int nm = rows();
  int n  = cols();
  int matz = 1;
  double* wr = new double[cols()];
  double* wi = new double[cols()];
  double* fv1 = new double[cols()];
  double* fv2 = new double[cols()];
  double* fv3 = new double[cols()];
  double* br = new double[rows()*cols()];
  double* bi = new double[rows()*cols()];
  double* cr = new double[rows()*cols()];
  double* ci = new double[rows()*cols()];
  int ierr,i,j,k;
  int oddEven = nm/2;

  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      br[k] = real(_ml->_m[j][i]); // the cg_ routine uses the transpose
      bi[k] = imag(_ml->_m[j][i]); // the cg_ routine uses the transpose
      k++;
    }
  }
  TMatrix<FNAL::Complex> z1(nm,n,complex_0);
  cg_(&nm,&n,br,bi,wr,wi,&matz,cr,ci,fv1,fv2,fv3,&ierr);
  if(ierr != 0) {
    cerr << "TMatrix<FNAL::Complex>: Error in eigenvector routine. error = ";
    cerr << ierr<< endl;
    delete []wr;
    delete []wi;
    delete []fv1;
    delete []fv2;
    delete []fv3;
    delete []br;
    delete []bi;
    delete []cr;
    delete []ci;
    return z1;
  }
  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      z1(i,j) = Complex(cr[k],ci[k]);
      k++;
    }
  }
  delete []wr;
  delete []wi;
  delete []fv1;
  delete []fv2;
  delete []fv3;
  delete []br;
  delete []bi;
  delete []cr;
  delete []ci;

  int sortFlag = 1;
  int counter = 0;
  while ((sortFlag == 1)&& (counter < 10)) {
    sortFlag = 0;
    for(i=1; i < oddEven; i++) {
      if(abs(z1(0,0)) < abs(z1(0,i))) {
        z1._switch_columns(0,i);
        sortFlag = 1;
        if((oddEven*2) == nm)
          z1._switch_columns(oddEven,oddEven+i);
      }
    }
    counter++;
  }
  if(counter >= 10)
    cerr << "TMatrix<FNAL::Complex>: Something is wrong with the eigenVector sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(z1(1,1)) < abs(z1(1,i))) {
      z1._switch_columns(1,i);
      if((oddEven*2) == nm)
        z1._switch_columns(1+oddEven,oddEven+i);
    }
  }
  return z1;
}


// Implementation of derived class MatrixC
// Done to replicate member function dagger()

MatrixC::MatrixC()
: TMatrix<Complex>()
{
}

MatrixC::MatrixC( const MatrixC& x )
: TMatrix<Complex>( (const TMatrix<Complex>&) x )
{
}

MatrixC::MatrixC( const TMatrix<Complex>& x )
: TMatrix<Complex>( x )
{
}

MatrixC::MatrixC( int i )
: TMatrix<Complex>(i)
{
}

MatrixC::MatrixC( int rows, int columns )
: TMatrix<Complex>( rows, columns )
{
}

MatrixC::MatrixC( int rows, int columns, const FNAL::Complex& initval )
: TMatrix<Complex>( rows, columns, initval )
{
}

MatrixC::MatrixC( int rows, int columns, FNAL::Complex* initval )
: TMatrix<Complex>( rows, columns, initval )
{
}

MatrixC::MatrixC( const char* flag, int dimension )
: TMatrix<Complex>( flag, dimension )
{
}

MatrixC::MatrixC( const TMatrix<double>& x )
: TMatrix<Complex>( x.rows(), x.cols() )
{
  int i, j;
  for( i=0; i< _ml->_r; i++ ) {
    for( j=0; j< _ml->_c; j++ ) {
      _ml->_m[i][j] = Complex( x(i,j), 0.0 );
    }
  }
}

MatrixC::~MatrixC()
{
}


MatrixC MatrixC::dagger() const
{
  MatrixC z( cols(), rows(), complex_0 );
  TML<Complex>* zPtr = z._ml;

  for(int row = 0; row < rows(); row++) {
    for(int col = 0; col < cols(); col++) {
      zPtr->_m[col][row] = conj(_ml->_m[row][col]);
    }
  }
  z._stacked = 1;
  return z;
}

MatrixD real( const MatrixC& x )
{
  int r = x.rows();
  int c = x.cols();
  MatrixD ret( r, c );
  for( int i = 0; i < r; i++ ) {
    for( int j = 0; j < c; j++ ) {
      ret(i,j) = real((const FNAL::Complex&) (x(i,j)));
    }
  }
  return ret;
}


MatrixD imag( const MatrixC& x )
{
  int r = x.rows();
  int c = x.cols();
  MatrixD ret( r, c );
  for( int i = 0; i < r; i++ ) {
    for( int j = 0; j < c; j++ ) {
      ret(i,j) = imag((const FNAL::Complex&) (x(i,j)));
    }
  }
  return ret;
}

template class TMatrix<double>;
template class TMatrix<FNAL::Complex>;
template class TMatrix<int>;

template char operator==( const TMatrix<double>&, const TMatrix<double>& );
template char operator==( const TMatrix<double>&, const double& );
template char operator==( const double&, const TMatrix<double>& );
template std::ostream& operator<<(std::ostream&, const TMatrix<double>&);
template TMatrix<double> operator+(const TMatrix<double>&, const TMatrix<double>&);
template TMatrix<double> operator+(const TMatrix<double>&, const double&); 
template TMatrix<double> operator+(const double&, const TMatrix<double>&); 
template TMatrix<double> operator-(const TMatrix<double>&); 
template TMatrix<double> operator-(const TMatrix<double>&, const TMatrix<double>&); 
template TMatrix<double> operator-(const TMatrix<double>&, const double&); 
template TMatrix<double> operator-(const double&, const TMatrix<double>&); 
template TMatrix<double> operator*(const TMatrix<double>&, const TMatrix<double>&); 
template TMatrix<double> operator*(const TMatrix<double>&, const double);
template TMatrix<double> operator*(const double, const TMatrix<double>&);
template TMatrix<double> operator/(const TMatrix<double>&, const double);
template TMatrix<double> operator/(const double, TMatrix<double>&);
template TMatrix<double> operator/(TMatrix<double>&, TMatrix<double>&);

template char operator==( const TMatrix<int>&, const TMatrix<int>& );
template char operator==( const TMatrix<int>&, const int& );
template char operator==( const int&, const TMatrix<int>& );
template std::ostream& operator<<(std::ostream&, const TMatrix<int>&);
template TMatrix<int> operator+(const TMatrix<int>&, const TMatrix<int>&);
template TMatrix<int> operator+(const TMatrix<int>&, const int&); 
template TMatrix<int> operator+(const int&, const TMatrix<int>&); 
template TMatrix<int> operator-(const TMatrix<int>&); 
template TMatrix<int> operator-(const TMatrix<int>&, const TMatrix<int>&); 
template TMatrix<int> operator-(const TMatrix<int>&, const int&); 
template TMatrix<int> operator-(const int&, const TMatrix<int>&); 
template TMatrix<int> operator*(const TMatrix<int>&, const TMatrix<int>&); 
template TMatrix<int> operator*(const TMatrix<int>&, const int);
template TMatrix<int> operator*(const int, const TMatrix<int>&);
template TMatrix<int> operator/(const TMatrix<int>&, const int);
template TMatrix<int> operator/(const int, TMatrix<int>&);
template TMatrix<int> operator/(TMatrix<int>&, TMatrix<int>&);

template char operator==( const TMatrix<FNAL::Complex>&, const TMatrix<FNAL::Complex>& );
template char operator==( const TMatrix<FNAL::Complex>&, const FNAL::Complex& );
template char operator==( const FNAL::Complex&, const TMatrix<FNAL::Complex>& );
template std::ostream& operator<<(std::ostream&, const TMatrix<FNAL::Complex>&);
template TMatrix<FNAL::Complex> operator+(const TMatrix<FNAL::Complex>&, const TMatrix<FNAL::Complex>&);
template TMatrix<FNAL::Complex> operator+(const TMatrix<FNAL::Complex>&, const FNAL::Complex&); 
template TMatrix<FNAL::Complex> operator+(const FNAL::Complex&, const TMatrix<FNAL::Complex>&); 
template TMatrix<FNAL::Complex> operator-(const TMatrix<FNAL::Complex>&); 
template TMatrix<FNAL::Complex> operator-(const TMatrix<FNAL::Complex>&, const TMatrix<FNAL::Complex>&); 
template TMatrix<FNAL::Complex> operator-(const TMatrix<FNAL::Complex>&, const FNAL::Complex&); 
template TMatrix<FNAL::Complex> operator-(const FNAL::Complex&, const TMatrix<FNAL::Complex>&); 
template TMatrix<FNAL::Complex> operator*(const TMatrix<FNAL::Complex>&, const TMatrix<FNAL::Complex>&); 
template TMatrix<FNAL::Complex> operator*(const TMatrix<FNAL::Complex>&, const FNAL::Complex);
template TMatrix<FNAL::Complex> operator*(const FNAL::Complex, const TMatrix<FNAL::Complex>&);
template TMatrix<FNAL::Complex> operator/(const TMatrix<FNAL::Complex>&, const FNAL::Complex);
template TMatrix<FNAL::Complex> operator/(const FNAL::Complex, TMatrix<FNAL::Complex>&);
template TMatrix<FNAL::Complex> operator/(TMatrix<FNAL::Complex>&, TMatrix<FNAL::Complex>&);


TMatrix<FNAL::Complex > operator*(const TMatrix<FNAL::Complex >& x, const TMatrix<double>& y)
{
  TMatrix<FNAL::Complex > z(x.rows(),y.cols(),complex_0);
  TML<FNAL::Complex >* xPtr = x._ml;
  TML<double>* yPtr = y._ml;
  TML<FNAL::Complex >* zPtr = z._ml;
  if( x.cols() != y.rows()) {
    throw( TMatrix<double>::Incompatible( x.rows(), x.cols(), y.rows(), y.cols(),
           "TMatrix<FNAL::Complex > operator*(const TMatrix<FNAL::Complex >& x, const TMatrix<double>& y)" ) );
  }
  FNAL::Complex tmp;
  for(int row = 0; row < x.rows(); row++) {
    for(int col = 0; col < y.cols(); col++){
      FNAL::Complex sum = complex_0;
      for(int i = 0; i < x.cols(); i++) {
        tmp = xPtr->_m[row][i] * FNAL::Complex(yPtr->_m[i][col],0.0);
        sum += tmp;
        if(abs(sum) < M_SMALL*abs(tmp))
          sum = complex_0;
      }
      zPtr->_m[row][col] = sum;
    }
  }
  z._stacked = true;
  return z;
}


TMatrix<FNAL::Complex> operator*(const TMatrix<double>& y, const TMatrix<FNAL::Complex>& x)
{
  TMatrix<FNAL::Complex> z(y.rows(),x.cols(),complex_0);
  TML<FNAL::Complex>* xPtr = x._ml;
  TML<double>* yPtr = y._ml;
  TML<FNAL::Complex>* zPtr = z._ml;
  if( y.cols() != x.rows()) {
    throw( TMatrix<double>::Incompatible( x.rows(), x.cols(), y.rows(), y.cols(),
           "TMatrix<FNAL::Complex> operator*(const TMatrix<double>& x, const TMatrix<FNAL::Complex>& y)" ) );
  }
  FNAL::Complex tmp;
  for(int row = 0; row < y.rows(); row++) {
    for(int col = 0; col < x.cols(); col++){
      FNAL::Complex sum = complex_0;
      for(int i = 0; i < y.cols(); i++) {
        tmp = FNAL::Complex(yPtr->_m[row][i],0.0) * xPtr->_m[i][col];
        sum += tmp;
        if(abs(sum) < M_SMALL*abs(tmp))
          sum = complex_0;
      }
      zPtr->_m[row][col] = sum;
    }
  }
  z._stacked = true;
  return z;
}
