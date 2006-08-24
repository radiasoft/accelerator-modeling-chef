/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Basic TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TML.tcc
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
******  Revision History 
******
******  Sep-Nov 2005   Jean-Francois Ostiguy
******                 ostiguy@fnal.gov                                   
******               
******   - refactored code to use single template parameter instead of two
******   - introduced implicit conversions 
******   - using boost::intrusive_pointer for reference counting
******   - eliminated access to internals of implementation class (TML) from class TMatrix.
******   - eliminated separate MatrixC class implementation (used to be derived from Matrix<T1,T2>)
******   - organized code to support both implicit and explicit template instantiations
******   - fixed incorrect complex version of eigenvalues/eigenvectors 
******   - separated eigenvalue/eigenvector reordering function 
******   - eliminated code that attempted to discriminate between objects allocated
******     on the stack and objects allocated from the free store.
****** 
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <TML.h>

using namespace std;

TML::tiny = double(1e-20); // pivot threshold


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>::TML(): _mdata(0), _nrows(0), _ncols(0)  { } // default: an empty matrix


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>::TML(const char* flag, int dimension ): _nrows(dimension), _ncols(dimension)
{

  if( ( (dimension%2) != 0) &&
      (flag[0] == 'J') ) {
    throw( GenericMatrixException( dimension, dimension
                     , "TML<T>::TML<T>(const char* flag, int dimension)"
                     , "Dimension must be even for J matrix") );
  }

  _mdata = new T* [_nrows];

  int i=0;

  int sz = _nrows*_ncols;
  T* dataPtr = new T [ sz ];

  for( i=0; i<_nrows; ++i ) { 
    _mdata[i] = dataPtr;
    dataPtr += _ncols;
  }

   // reset dataPtr

   dataPtr = _mdata[0];
 
   for( i=0;  i<sz;  ++i) {
      (*dataPtr) = T(); 
      ++dataPtr; 
   }


  if (flag[0]  == 'I') {
    for ( i=0; i<dimension; ++i) {
       _mdata[i][i] = T(1); 
    }
  } else if (flag[0] == 'J') {
    for (i = dimension/2; i< dimension; i++) {
      _mdata[i-dimension/2][i] = T(1); 
      _mdata[i][i-dimension/2] = T(-1);
    }
  } else {
      throw( GenericMatrixException( dimension, dimension
                     , "TML<T>::TML<T>(const char* flag, int dimension)"
                     , strcat("Unknown flag: ", flag) ) );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>::TML(int rows, int columns, T initval): _nrows(rows), _ncols(columns)
{

  _mdata = new T* [_nrows];
 
  int sz = _nrows*_ncols;
  T* dataPtr = new T [ sz ];

  for( int i=0; i<_nrows; ++i ) { 
    _mdata[i] = dataPtr;
    dataPtr += _ncols;
  }

   T* colptr = _mdata[0];
 
   for( int i=0;  i<sz;  ++i) {
      (*colptr) = initval; 
       ++colptr; 
   }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>::TML(int rows, int columns, const T* values): _nrows(rows), _ncols(columns)
{
  _mdata = new T* [_nrows];

  int sz = _nrows*_ncols;
  T* dataPtr = new T [ sz ];
  for( int i=0; i < _nrows; ++i) { 
    _mdata[i] = dataPtr;
    dataPtr += _ncols;
  }

  memcpy( (void*) (_mdata[0]), (void*) values, sz*sizeof(T) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>::TML( TML<T> const& X ):  _nrows(X._nrows), _ncols(X._ncols)
{

  _mdata = new T* [_nrows];

  int sz = _nrows*_ncols;
  T* dataPtr = new T [ sz ];

  for( int i=0; i<_nrows; ++i) { 
     _mdata[i] = dataPtr;
     dataPtr += _ncols;
  }

  memcpy( (void*) (_mdata[0]), (void*) ((X._mdata)[0]), sz*sizeof(T) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>::~TML() {
  _clear();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TML<T>::_clear() {

  if((_nrows == 0) && (_ncols == 0)) return;

  if (_mdata[0]) delete [] _mdata[0];
  if (_mdata)    delete [] _mdata;
  _nrows = 0;
  _ncols = 0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> TML<T>::Square() const 
{

  int d = 0;
  if( _nrows < _ncols ) {
     d = _nrows;
  } else {                  
     d = _ncols;
  }

  MLPtr<T> z(  new TML<T>(d, d, T()) ); 

  for( int i=0; i<d; ++i )
    for( int j=0; j<d; ++j )
      z->_mdata[i][j] = _mdata[i][j];

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
MLPtr<T> TML<T>::transpose() const 
{
  MLPtr<T> zPtr( new TML<T>( _ncols,_nrows, T()) ); 

  for (int row=0; row<_nrows;  ++row)  {
    for(int col=0; col<_ncols; ++col)  {
      zPtr->_mdata[col][row] = _mdata[row][col];
    }
  }
  return zPtr;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
T TML<T>::trace() const {
  
  if( _nrows != _ncols) throw( NotSquare( _nrows, _ncols, "T TMatrix<T>::trace()" )  );

  T temp = T();

  for (int i=0; i<_nrows; ++i)  { 
    temp += _mdata[i][i];  
  }  

  return temp;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
bool TML<T>::isOrthogonal() const
{
 if( _nrows != _ncols ) { return false; }

  MLPtr<T> self( new TML<T>(*this) ); // deepcopy  

  MLPtr<T> W( multiply<T>( self, self->transpose() ));

  int n = _nrows;

  for( int i=0; i<n; ++i ) {
    for( int j=0; j<n; ++j ) {
      if( i != j ) { if( 1.0e-12 < std::abs( (*W)(i,j) ) )       { return false; } }
      else         { if( 1.0e-12 < std::abs( (*W)(i,j) - 1.0 ) ) { return false; } }
    }
  }

  W = multiply<T>( self->transpose(),  self);

  for( int i=0; i<n; ++i ) {
    for( int j = 0; j<n; ++j ) {
      if( i != j ) { if( 1.0e-12 < std::abs( (*W)(i,j) ) )       { return false; } }
      else         { if( 1.0e-12 < std::abs( (*W)(i,j) - 1.0 ) ) { return false; } }
    }
  }

  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T TML<T>::determinant() const
{
  T det = T();
  if( _nrows != _ncols) {
    throw( NotSquare( _nrows, _ncols, "TML<T>::determinant()" )  );
  }
  else {
    int* indx = new int[_ncols];  // create the "index vector
                              // see pp 38. in Numerical Recipes
    int d;
    // perform the decomposition once:

    MLPtr<T> decomp(_lu_decompose(indx,d));
    det = d;
    for(int i=0; i<_ncols ; ++i)
      det *= decomp->_mdata[i][i];
    delete [] indx;
  }
  return det;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> TML<T>::inverse() const 
{

  if( _nrows  != _ncols ) {
    throw( NotSquare( _nrows, _ncols, "TML<T>::inverse()" )  );
  }
  else {
  
    MLPtr<T> Y( new TML<T>("I",_nrows) );   // create an identity TMatrix<T>
    int* indx = new int[_ncols];                           // create the "index vector"
    MLPtr<T> B( new TML<T>( _ncols, _ncols, T()) );       // see Press & Flannery
    int d;

    // perform the decomposition once:

    MLPtr<T> decomp (_lu_decompose(indx,d));

    for(int col = 0; col < _ncols; ++col){
      B->_copy_column(Y,col,0);
      decomp->_lu_back_subst(indx,B);
      Y->_copy_column(B,0,col);
    }
    delete []indx;
    return Y;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T& TML<T>::operator()(int i, int j) 
{

  if((i >= _nrows) || (j >= _ncols) ||
     (i < 0      ) || (j < 0      )    ) { 
       throw( IndexRange( i, j, _nrows-1, _ncols-1,
                       "T& TML<T>::operator()(int i, int j)" ) );
  }
  else { 
    return _mdata[i][j]; 
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T  TML<T>::operator()(int i, int j) const 
{
  
  if((i >= _nrows) || (j >= _ncols) ||
     (i < 0      ) || (j < 0      )    )
  { throw( IndexRange( i, j, _nrows-1, _ncols-1,
                       "T TML<T>::operator()(int i, int j) const" ) );
  }
  else { 
    return _mdata[i][j]; 
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T& TML<T>::operator()(int i) 
{
  // This body must stay in synch with
  // T TMatrix<T>::operator()(int i) const

  if( _nrows == 1 ) {
    if( i >= 0 && i < _ncols ) {
      return _mdata[0][i];
    }
    else {
      throw( IndexRange( 0, i, 0, _ncols-1,
                       "T& TML<T>::operator()(int i)" ) );
    }
  }
  else if( _ncols== 1 ) {
    if( i >= 0 && i < _nrows ) {
      return _mdata[i][0];
    }
    else {
      throw( IndexRange( i, 0, _nrows-1, 0,
                       "T& TML<T>::operator()(int i)" ) );
    }
  }
  else {
    throw( NotVector( i, _nrows, _ncols,
                      "T& TML<T>::operator()(int i)" ) );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T TML<T>::operator()(int i) const 
{
  // This body must stay in synch with
  // T& TMatrix<T>::operator()(int i)
  if( _nrows == 1 ) {
    if( i >= 0 && i < _ncols ) {
      return _mdata[0][i];
    }
    else {
      throw( IndexRange( 0, i, 0, _ncols-1,
                       "T& TML<T>::operator()(int i)" ) );
    }
  }
  else if( _ncols == 1 ) {
    if( i >= 0 && i < _nrows ) {
      return _mdata[i][0];
    }
    else {
      throw( IndexRange( i, 0, _nrows-1, 0,
                       "T& TMatrix<T>::operator()(int i)" ) );
    }
  }
  else {
    throw( NotVector( i, _nrows, _ncols,
                      "T& TMatrix<T>::operator()(int i)" ) );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
std::ostream& operator<<( std::ostream& os, const TML<T>& x)
{
  int i,j;

  for(i=0; i< x._nrows; i++) {
    os << "( ";
    for(j=0; j< x._ncols; j++) {
      os  << x._mdata[i][j] << ", ";
    }
    os << " )\n";
  }
  return os << endl;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> add( MLPtr<T> const& x, MLPtr<T> const& y) 
{

  MLPtr<T> z( new TML<T>(x->_nrows, x->_ncols, T()) );

  if( ( x->_nrows != y->_nrows ) || ( x->_ncols != y->_ncols ) ) {
    throw( typename TML<T>::Incompatible( x->_nrows, x->_ncols, y->_nrows, y->_ncols,
           "TML<T> add(MLPtr<T> const& x, MLPtr<T>TMatrix<T> const& y)" ));
  }

  int sz = z->_nrows * z->_ncols;
  
  T* const *const xp = x->_mdata; 
  T* const *const yp=  y->_mdata; 
  T** zp            =  z->_mdata;
  
  for(int i=0;  i<x->_nrows; ++i) {
    for(int j=0;  j<x->_ncols; ++j) {

      zp[i][j] = xp[i][j]  + yp[i][j];
      //if(std::abs(zp[i][j]) < M_SMALL*std::abs(yp[i][j]) ) zp[i][j] = T();
    }
  }

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> add(MLPtr<T> const& x, const T& y) 
{
  MLPtr<T> z( new TML<T>( *x ) ); // deep copy
 

  for(int i=0;  i< x->_nrows; ++i) {
    z->_mdata[i][i] += y;
  }

  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> Negate( MLPtr<T> const& x) // unary minus 
{
  MLPtr<T> z( new TML<T>( *x) ); //deep copy

  T* p = z->_mdata[0];

  int sz = z->_nrows * z->_ncols;

  for( int i=0; i<sz; ++i ) { (*p) = -(*p); ++p; }

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> subtract(MLPtr<T> const& x, MLPtr<T> const& y) 
{
  MLPtr<T> z( new TML<T>(x->_nrows, x->_ncols ,T()) );

  if(( x->_nrows != y->_nrows ) || (x->_ncols != y->_ncols )) {
    throw( typename TML<T>::Incompatible( x->_nrows, x->_ncols, y->_nrows, y->_ncols,
           "subtract(MLPtr<T> const& x, MLPtr<T> const& y) " ));
  }

 
  T** zp             = z->_mdata;
  const T* const* xp = x->_mdata;
  const T* const* yp = y->_mdata;


  for( int i=0; i<x->_nrows; ++i ) {
    for( int j=0; j<x->_ncols; ++j ) {

       zp[i][j] = xp[i][j] - yp[i][j];
       //if(std::abs(zp[i][j]) < M_SMALL*std::abs(yp[i][j]) ) (zp[i][j]) = T();

    }
  } 
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> multiply( MLPtr<T> const& x, T const& y) 
{
  
  MLPtr<T> z( new TML<T>( *x ) );

  T* p = z->_mdata[0];
  int sz = x->_nrows * x->_ncols;
  
  for(int i=0; i<sz; ++i) {
        (*p) *= y;              
        ++p;
  }
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> multiply(MLPtr<T> const& x, MLPtr<T> const& y)  
{
  MLPtr<T> z( new TML<T>( x->_nrows, y->_ncols, T()) );

  if( x->_ncols != y->_nrows) {
    throw( typename TML<T>::Incompatible( x->_nrows, x->_ncols, y->_nrows, y->_ncols,
           "multiply(MLPtr<T> const& x, MLPtr<T> const& y)" ) );
  }
  T tmp;
  for(int row=0; row< x->_nrows; ++row) {
    for(int col = 0; col <y->_ncols; ++col) {
      T sum = T();
      for(int i=0; i<x->_ncols; ++i) {
        tmp = x->_mdata[row][i] * y->_mdata[i][col];
        sum += tmp;
        //if(std::abs(sum) < M_SMALL*std::abs(tmp) ) sum = T();
      }
      z->_mdata[row][col] = sum;
    }
  }
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> divide (MLPtr<T> const& x, T const& y) 
{
  MLPtr<T> z ( new TML<T>( *x) );
  if(y == T())
    (*pcerr) << "TMatrix<T>:operator/ divide by zero" << endl;

  for(int i=0;  i<x->_nrows ; ++i) {
     for(int j=0; j< x->_ncols; ++j) {

      z->_mdata[i][j] /= y;

    }
  }
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> divide( T const& y, MLPtr<T> const & x) 
{
  return multiply<T>( x->inverse(), y);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
MLPtr<T> divide( MLPtr<T> const& y, MLPtr<T> const& x) 
{
  return multiply<T>( x->inverse(), y);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// copy the from_col of mm to the to_col of "this"

template<typename T>
void TML<T>::_copy_column(MLPtr<T> const& mm, int from_col, int to_col)
{
  if(_nrows  != mm->_nrows) {
    throw( Incompatible( _nrows, _ncols, mm->_nrows, mm->_ncols,
       "void TML<T>::_copy_column(MLPtr<T> const& mm, int from_col, int to_col)" )  );
  }
  else {
    for(int row=0; row < _nrows; ++row)
      _mdata[row][to_col] = mm->_mdata[row][from_col];
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TML<T>::_switch_columns(int col1, int col2) 
{
  
  MLPtr<T> temp( new TML<T>(_nrows, 1, T()) );

  for(int row=0; row< _nrows; ++row) {
    // temporarily store col 1:
    temp->_mdata[row][0] = _mdata[row][col1];
  }

  for(int row=0; row< _nrows; ++row) {
    _mdata[row][col1] = _mdata[row][col2];    // move col2 to col1
  }

  for(int row=0;  row< _nrows; ++row) {
    _mdata[row][col2] = temp->_mdata[row][0]; // move temp to col2
 }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TML<T>::_switch_rows(int row1, int row2) 
{
  MLPtr<T> temp( new TML<T>(1,_ncols, T()) );

  for(int col=0; col < _ncols; ++col) {
    // temporarily store row 1:
    temp->_mdata[0][col] = _mdata[row1][col];
  }

  for(int col=0; col < _ncols; ++col) {
    _mdata[row1][col] = _mdata[row2][col];    // move row2 to row1
  }

  for(int col=0; col < _ncols; ++col) {
    _mdata[row2][col] = temp->_mdata[0][col]; // move temp to row2
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// scale a matrix (used in L-U decomposition)

template<typename T>
MLPtr<T> TML<T>::_scale() const 
{
  T temp;
  if (_nrows <= 0 || _ncols <= 0) {
    throw( GenericMatrixException( _nrows, _ncols
                   , "TMatrix<T> TMatrix<T>::_scale()"
                   , "Bad TMatrix<T> for _scale()" ) );
  }
  if (_nrows != _ncols) {
    throw( NotSquare( _nrows, _ncols, "TMatrix<T> TMatrix<T>::_scale()" )  );
  }

  MLPtr<T> scale_vector( new TML<T>( _ncols, 1, T()) );

  double maximum = 0.0;
  for (int row=0; row< _nrows; ++row){

    maximum = 0.0;

    for(int col=0; col< _ncols; ++col) {
      maximum = std::max( maximum, std::abs(_mdata[row][col]) );
    }

    if(maximum == 0.0 ) {
      (*pcerr) << "\n*** ERROR *** Matrix = \n" << *this << endl;
      throw( GenericMatrixException( _nrows, _ncols
                     , "TML<T>::_scale()"
                     , "Matrix is singular." ) );
    }
    // save scaling

    scale_vector->_mdata[row][0] = ((T) 1)/ maximum;
  }
  return scale_vector;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
 MLPtr<T> TML<T>::_lu_decompose(int* indx, int& d ) const
{
  // Returns the L-U decomposition of a matrix. indx is an ouput
  // vector which records the row permutation affected by the
  // partial pivoting, d is output as +-1 depending on whether the
  // number of row interchanges was even or odd, respectively.
  // This routine is used in combination with _lu_back_subst to
  // solve linear equations or invert a matrix.

  if(_nrows  != _ncols) {
    throw( NotSquare( _nrows, _ncols,
      "TML<T>::_lu_decompose(int* indx, int& d ) const " )  );
  }
  d = 1; // parity check
  T dum; // from the book - I don't know signj
  T sum;
  T maximum;
  T tmp;

  // make a direct copy of the original matrix:
  MLPtr<T> lu_decomp( new TML<T>( *this) );

  // scale the matrix
  MLPtr<T> scale_vector = lu_decomp->_scale(); 

  // The loop over columns of Crout's method:

  int col_max; 
  int i,j,k; 
  for( j=0; j< _nrows; ++j) {
    if (j > 0) {
      // eqn 2.3.12 except for j=i:
      for (i=0; i<= j-1; ++i) {
        sum = lu_decomp->_mdata[i][j];
        if(i > 0) {
          for(k=0; k <= i-1; ++k) {
            tmp = lu_decomp->_mdata[i][k]*lu_decomp->_mdata[k][j];
            sum -= tmp;
          }
          lu_decomp->_mdata[i][j] = sum;
        }
      }
    }

// Initialize the search for the largest pivot element:
    maximum = T();
    // i=j of eq 2.3.12 & i=j+I..N of 2.3.13:
    for(i=j; i <= _ncols-1; ++i) {
      sum = lu_decomp->_mdata[i][j];

      if(j > 0) {
        for(k=0; k <= j-1; ++k) {
          tmp =  lu_decomp->_mdata[i][k] * lu_decomp->_mdata[k][j];
          sum -= tmp;
        }
        lu_decomp->_mdata[i][j] = sum;
      }
      // figure of merit for pivot:
      dum = scale_vector->_mdata[i][0] * std::abs(sum);
      if (dum >= maximum){
        // is it better than the best so far ?
        col_max = i;
        maximum = std::abs(dum);
      }
    }
    // Do we need to interchange rows?

    if(j != col_max)  {
      lu_decomp->_switch_rows(col_max,j); // Yes, do so ...
      d *= -1;  // ... and change the parity of d
      // also interchange the scale factor:
      dum = scale_vector->_mdata[col_max][0];
      scale_vector->_mdata[col_max][0] = scale_vector->_mdata[j][0];
      scale_vector->_mdata[j][0] = dum;
    }

    indx[j] = col_max;
    // Now, finally, divide by the pivot element:
    if(j != _nrows -1){
      // If the pivot element is .zero the matrix is
      // singular (at least to the precision of the
      // algorithm). For some applications on singular
      // matrices, it is desirable to substitute _tiny for 0
      if(lu_decomp->_mdata[j][j] == T())
        lu_decomp->_mdata[j][j] = T(_tiny); /// this may not work for all types
                                            
      dum = T(1)/lu_decomp->_mdata[j][j];
      for(i=j+1; i <= _ncols-1; i++)
        lu_decomp->_mdata[i][j] *= dum;
    }

  }
  if(lu_decomp->_mdata[_nrows-1][_ncols-1] == T())
    lu_decomp->_mdata[_nrows-1][_ncols-1] = T(_tiny);

  return lu_decomp;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TML<T>::_lu_back_subst(int* indx, MLPtr<T>& b)  
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

  if(_nrows  != _ncols) {
    throw( NotSquare( _nrows, _ncols,
      "void TML<T>::_lu_back_subst(int* indx, MLPtr<T>& b)" )  );
  }
  if(_nrows  != b->_nrows) {
    throw( Incompatible( _nrows, _ncols, b->_nrows, b->_ncols,
      "void TML<T>::_lu_back_subst(int* indx,MLPt<T>& b)" )  );
  }
//  if(_nrows  != indx->_nrows)
//    error("wrong size indx vector passed to _lu_back_subst()");
  int i,j,ip;
  int ii = -1;
  T sum;
  T tmp;

  for(i=0; i< _nrows; ++i){
    ip = indx[i];
    sum = b->_mdata[ip][0];
    b->_mdata[ip][0] = b->_mdata[i][0];
    if (ii >= 0) {
      for(j = ii; j <= i-1; j++) {
        tmp = _mdata[i][j] * b->_mdata[j][0];
        sum -= tmp;
       }
    } else if(sum != T() )
      ii = i;
    b->_mdata[i][0] = sum;
  }
  for(i = _nrows-1; i >= 0; --i){
    sum = b->_mdata[i][0];
    if (i < _ncols -1) {
      for (j = i+1; j <= _nrows-1; ++j) {
        tmp = _mdata[i][j] * b->_mdata[j][0];
        sum -= tmp;
      }
    }
    // store a component of the soln vector X:
    b->_mdata[i][0] = sum/_mdata[i][i];
  }
}



// Implementation of exception subclasses

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
TML<T>& TML<T>::operator=(const TML& x) 
{
  if( this == &x )  return *this;
 
  _clear();

  _nrows = x._nrows;
  _ncols = x._ncols;
  _mdata = new T* [_nrows];

  int sz = _nrows*_ncols;
  T* dataPtr = new T [ sz ];
  for( int i = 0; i< _nrows; ++i) { 
      _mdata[i] = dataPtr;
      dataPtr += _ncols;
  }

  for (int i=0; i< _nrows; ++i) {
      for (int j=0; j< _ncols; ++j) {

      _mdata[i][j] = x._mdata[i][j];

      }
  }

  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
bool TML<T>::operator==(const TML<T>& x)
{
  if((x._nrows != _nrows) || (x._ncols != _ncols)) { return false; }
  for(int i=0; i < _nrows; ++i) {
    for(int j=0; j < _ncols; ++j) {
      if(x._mdata[i][j] != _mdata[i][j]) { return false; }
    }
  }
  return true;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
bool TML<T>::operator==(const T& x) 
{
  if(_nrows != _ncols) return false;
  for(int i=0; i< _nrows; i++) {
    if(_mdata[i][i] != x) return false;
    for(int j =0; j < _ncols; j++) {
      if((i != j) && (_mdata[i][j] != 0.0)) { return false; }
    }
  }
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>& TML<T>::operator+=( const T& x) 
{
  for(int i=0;  i<_nrows; ++i) {
    _mdata[i][i] += x;
  }
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
bool operator==(const TML<T>& x, const TML<T>& y) 
{
  if((x->_nrows != y->_nrows) || (x->_ncols != y->_ncols)) { return false; }
  for(int i=0;  i< x->_nrows; ++i) {
    for(int j=0;  j<x->_ncols; ++j) {

      if(x.m[i][j] != y.m[i][j]) return false;
    }
  }
  return true;
}

// Implementation of exception subclasses

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>::IndexRange::IndexRange( int a, int b, int c, int d, const char* f )
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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
const char* TML<T>::IndexRange::what() const throw()
{
  return "Index out of range.";
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>::NotVector::NotVector( int x, int y, int z, const char* f )
: i(x), r(y), c(z)
{
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** ERROR *** "
            "\n*** ERROR *** " << f
         << "\n*** ERROR *** Matrix is not a vector:"
            "\n*** ERROR *** its dimensions are " << r << " x " << c
         << "\n*** ERROR *** "
            "\n*** ERROR *** "
            "\n*** ERROR *** This message is printed only once."
         << endl;
    firstTime = false;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TML<T>::NotVector::what() const throw()
{
  return "Matrix is not a vector.";
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>::Incompatible::Incompatible( int x, int y, int z, int t, const char* f )
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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TML<T>::Incompatible::what() const throw()
{
  return "Incompatible arguments.";
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>::NotSquare::NotSquare( int x, int y, const char* f )
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


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TML<T>::NotSquare::what() const throw()
{
  return "Matrix is not square.";
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>::GenericMatrixException::GenericMatrixException( int x, int y, 
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


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TML<T>::GenericMatrixException::what() const throw()
{
  return errorString.c_str();
}


