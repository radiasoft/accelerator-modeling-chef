/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BASIC TOOLKIT:  Low level utility C++ classes.
******
******  File:      TMatrix.tcc
******
******  Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Leo Michelotti                                     
******             Email: michelotti@fnal.gov                         
****** 
******  Revision (Sep 2005):
******
******             Jean-Francois Ostiguy
******             ostiguy@fnal.gov                                                   
******             
******  - reprganized code to support explicit template instantiations
******
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iosetup.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <MathConstants.h>
#include <PhysicsConstants.h>
#include <TMatrix.h>

#ifdef WIN32
extern long   srand48(long int);
extern double drand48();
#endif

extern "C" { void rg_(int*, int*, double*, double*,double*, int*, double*,
                 int*, double*, int*); }

extern "C" { void cg_(int*, int*, double*, double*,double*,double*, int*,
                      double*,double*,double*, double*, double*,int*); }


using FNAL::Complex;
using FNAL::pcout;
using FNAL::pcerr;


static const int PHASESPACE = 6;
#define M_SMALL  1.0e-30

static double tiny = double(1e-20);


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
bool operator==( const TMatrix<T>& x, const TMatrix<T>& y ) 
{
 return *(x._ml) == *(y._ml);
}

template<typename T>
bool operator==( const TMatrix<T>& x, const T& y ) 
{
 return *(x._ml) == y;
}

template<typename T>
bool operator==( const T& y, const TMatrix<T>& x )
{
 return *(x._ml) == y;
}

template<typename T>
bool operator!=( const TMatrix<T>& x, const TMatrix<T>& y ) 
{
 return !( x == y );
}

template<typename T>
bool operator!=( const TMatrix<T>& x, const T& y ) 
{
 return !( x == y );
}

template<typename T>
bool operator!=( const T& x, const TMatrix<T>& y ) 
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
T TMatrix<T>::determinant() const
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
    (*pcerr) << "TMatrix<T>:operator/ divide by zero" << endl;
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
      (*pcerr) << "\n*** ERROR *** Matrix = \n" << *this << endl;
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
    (*pcerr) << "\n*** ERROR *** "
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
    (*pcerr) << "\n*** ERROR *** "
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
    (*pcerr) << "\n*** ERROR *** "
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
    (*pcerr) << "\n*** ERROR *** "
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
  std::ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** " << f
      << "\n*** ERROR *** " << _m
      << "\n*** ERROR *** The dimensions are "
      << r << " x " << c
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
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


