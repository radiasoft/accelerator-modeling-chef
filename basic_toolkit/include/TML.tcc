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
******   - use boost::intrusive_pointer for reference counting
******   - eliminated private access to private internals of implementation 
******     class (TML) from class TMatrix.
******   - eliminated separate MatrixC class implementation (used to be derived from Matrix<T1,T2>)
******   - organized code to support both implicit and explicit template 
******     instantiations compilation models
******   - fixed incorrect complex version of eigenvalues/eigenvectors 
******   - isolated eigenvalue/eigenvector reordering functionality 
******   - eliminated code that attempted to discriminate between objects allocated
******     on the stack and objects allocated on the free store.
****** 
******  November 2007  Leo Michelotti
******                 michelotti@fnal.gov
******  
******  - Calls to TML<T>::scale() are now embedded in try-catch
******    blocks in order to handle matrices that are identically
******    zero.  In conjunction with other repairs made this month,
******    the  TML<T>::determinant()  should now handle correctly
******    any matrix whose determinant is zero.  In addition,
******    the  TML<T>::inverse()  catches these exceptions when
******    the matrix is singular and will return a zero matrix
******    rather than have the application program abort. A message
******    that a singular matrix has been encountered is still
******    written as a warning to the user.
******
****** Feb 2008 ostiguy@fnal.gov
****** 
****** - eliminated unsafe calls to memcpy
****** - fixed memory leak in destructor
****** - fixed bug in copy constructor and operator=  
****** 
****** Aug 2008 ostiguy@fnal
****** - added implementations for operator+=() and operator-=() 
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <algorithm>
#include <vector>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/iosetup.h>

using std::cout;
using std::endl;

template<typename T> double TML<T>::tiny_                = 1.0e-20; // pivot threshold



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>::TML()
 : mdata_(0), nrows_(0), ncols_(0)  

{ } // default: an empty matrix

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>::TML(int rows, int columns, T initval)
 : nrows_(rows), ncols_(columns)
{

  mdata_ = new T* [nrows_];
 
  int sz = nrows_*ncols_;
  T* dataPtr = data_ = new T [ sz ];
  
  for( int i=0; i<nrows_; ++i ) { 
    mdata_[i] = dataPtr;
    dataPtr += ncols_;
  }

   dataPtr = data_;
 
   for( int i=0;  i<sz;  ++i) {
      (*dataPtr) = initval; 
       ++dataPtr; 
   }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>::TML(int rows, int columns, const T* values)
 : nrows_(rows), ncols_(columns)
{
  mdata_ = new T* [nrows_];

  int sz = nrows_*ncols_;
  T* dataPtr = new T [ sz ];
       data_ = dataPtr;

  for( int i=0; i < nrows_; ++i) { 
    mdata_[i] = dataPtr;
    dataPtr += ncols_;
  }

  std:copy ( values, values+sz, data_ ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>::TML( TML<T> const& X )
 : ReferenceCounter<TML<T> >(), nrows_(X.nrows_), ncols_(X.ncols_)
{

  mdata_ = new T* [nrows_];

  int sz = nrows_*ncols_;

  T* dataPtr = data_ = new T [ sz ];

  for( int i=0; i<nrows_; ++i) { 
     mdata_[i] = data_ + ( X.mdata_[i] - X.data_);
  }

  std:copy ( X.data_, X.data_+ sz,  data_); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>::~TML() {
 
  nrows_ = 0;
  ncols_ = 0;

  if (!mdata_) return; // valid to delete a 0x0 matrix ... 
   
  if (  data_ ) delete [] data_;
  if ( mdata_ ) delete [] mdata_;
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TML<T>::clear() {

  int sz = nrows_*ncols_;

  T* dataptr = data_;

  for( int i=0;  i<sz;  ++i) {
    *dataptr = T(); 
    ++dataptr; 
  }

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> TML<T>::Square() const 
{

  int d = std::min(nrows_,ncols_ );

  MLPtr<T> z(  new TML<T>(d, d, T()) ); 

  for( int i=0; i<d; ++i )
    for( int j=0; j<d; ++j )
      z->mdata_[i][j] = mdata_[i][j];

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
MLPtr<T> TML<T>::transpose() const 
{
  MLPtr<T> zPtr( new TML<T>( ncols_,nrows_, T()) ); 

  for (int row=0; row<nrows_;  ++row)  {
    for(int col=0; col<ncols_; ++col)  {
      zPtr->mdata_[col][row] = mdata_[row][col];
    }
  }
  return zPtr;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TML<T>::trace() const {
  
  if( nrows_ != ncols_) throw( NotSquare( nrows_, ncols_, "T TMatrix<T>::trace()" )  );

  T temp = T();

  for (int i=0; i<nrows_; ++i)  { 
    temp += mdata_[i][i];  
  }  

  return temp;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
double TML<T>::maxNorm() const 
{
  return std::abs( *( std::max_element( data_, data_+(nrows_*ncols_), typename TML<T>::abs_less() )) ); 
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TML<T>::determinant() const
{

  if( nrows_ != ncols_) {
    throw( NotSquare( nrows_, ncols_, "TML<T>::determinant()" )  );
  }

  int indx[ncols_];  // create the "index vector" used to keep (row permutations)
                     // see pp 38. in Numerical Recipes

  int d = 1; // d is used as output parameter for lu_decompose.
             // it is set to +-1 depending on whether the no of 
             // row permutations is even or odd.

  // Perform the decomposition once

  MLPtr<T> decomp( new TML<T>(nrows_,ncols_,T()) );
  try {
      decomp = lu_decompose(indx,d);
      }
      catch( GenericException const& ge ) {
      // The matrix is almost certainly singular.
      // This will set the returned value of the
      // determinant to zero.
      for(int i=0; i<ncols_ ; ++i) {
        decomp->mdata_[i][i] = 0.0;
      }      
    }

  // Finish calculating the determinant
 
  T det(d);

  for(int i=0; i<ncols_ ; ++i) {
      det *= decomp->mdata_[i][i];
  }

  return det;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> TML<T>::inverse() const 
{
  if( nrows_  != ncols_ ) {
    throw( NotSquare( nrows_, ncols_, "TML<T>::inverse()" )  );
  }

  MLPtr<T> Y( new TML<T>(nrows_, nrows_, T() ));   // create an identity matrix
  for (int i=0; i< nrows_; ++i) (*Y)(i,i) = T(1.0);

  MLPtr<T> B( new TML<T>( ncols_, ncols_, T()) );  // see Press & Flannery

  // perform the decomposition once:


  int indx[ncols_];                                // create the "index vector"
  int d;

  MLPtr<T> decomp( new TML<T>(nrows_,ncols_,T()) );
    try {
      decomp = lu_decompose(indx,d);
    }
    catch( GenericException const& ge ) {
      // The matrix is almost certainly singular.
      // This will set the returned value of the
      // "inverse" to zero.
      std::ostringstream uic;
      uic << "\n*** WARNING *** "
             "\n*** WARNING *** " << __FILE__ << ", line " << __LINE__ 
          << ": MLPtr<T> TML<T>::inverse() const "
             "\n*** WARNING *** -------------------------------------"
             "\n*** WARNING *** Attempt made to invert singular matrix.\n"
          << (*this)
          << "\n*** WARNING *** Function is returning zero matrix."
             "\n*** WARNING *** "
          << endl;
      (*FNAL::pcerr) << uic.str() << endl;

      for(int i=0; i<nrows_ ; ++i) {
        for(int j=0; j<ncols_ ; ++j) {
          Y->mdata_[i][j] = 0.0;
        }
      }      
      return Y;
    }

    // Finish calculating the inverse

    for(int col= 0; col< ncols_; ++col){
      B->copy_column(Y,col,0);
      decomp->lu_back_subst(indx,B);
      Y->copy_column(B,0,col);
    }

    return Y;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T& TML<T>::operator()(int const& i, int const& j) 
{

  if((i >= nrows_) || (j >= ncols_) ||
     (i < 0      ) || (j < 0      )    ) { 
       throw( IndexRange( i, j, nrows_-1, ncols_-1,
                       "T& TML<T>::operator()(int i, int j)" ) );
  }

  return mdata_[i][j]; 
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T  TML<T>::operator()(int const& i, int const&  j) const 
{
  
  if((i >= nrows_) || (j >= ncols_) ||
     (i < 0      ) || (j < 0      )    )
  { throw( IndexRange( i, j, nrows_-1, ncols_-1,
                       "T TML<T>::operator()(int i, int j) const" ) );
  }
 
  return mdata_[i][j]; 
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T& TML<T>::operator()(int const& i) 
{
  // This body must stay in synch with
  // T TMatrix<T>::operator()(int i) const

  if ( (nrows_ != 1) || (ncols_!= 1) ) {
    throw( NotVector( i, nrows_, ncols_,
                      "T& TML<T>::operator()(int i)" ) );
  }

 
  if( (i < 0) || (i>= ncols_) ) {
      throw( IndexRange( 0, i, 0, ncols_-1,
                       "T& TML<T>::operator()(int i)" ) );
  }

   

  if( (i < 0) || (i >= nrows_) ) {
      throw( IndexRange( i, 0, nrows_-1, 0,
                       "T& TML<T>::operator()(int i)" ) );
  }

  return ( nrows_ == 1 ) ? mdata_[0][i] : mdata_[i][0];


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TML<T>::operator()(int const& i) const 
{
  
  if ( (nrows_!= 1) || (ncols_!= 1) ) {
    throw( NotVector( i, nrows_, ncols_,
                      "T& TML<T>::operator()(int i)" ) );
  }

 
  if( (i<0) || (i>= ncols_) ){
      throw( IndexRange( 0, i, 0, ncols_-1,
                       "T& TML<T>::operator()(int i)" ) );
  }

   

  if( (i<0) || (i>= nrows_) ) {
      throw( IndexRange( i, 0, nrows_-1, 0,
                       "T& TML<T>::operator()(int i)" ) );
  }

  return ( nrows_ == 1 ) ? mdata_[0][i] : mdata_[i][0];

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
std::ostream& operator<<( std::ostream& os, const TML<T>& x)
{

  for(int i=0; i< x.nrows_; ++i) {
    os << "( ";
    for(int j=0; j< x.ncols_; ++j) {
      os  << x.mdata_[i][j] << ", ";
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

  MLPtr<T> z( new TML<T>(x->nrows_, x->ncols_, T()) );

  if( ( x->nrows_ != y->nrows_ ) || ( x->ncols_ != y->ncols_ ) ) {
    throw( typename TML<T>::Incompatible( x->nrows_, x->ncols_, y->nrows_, y->ncols_,
           "TML<T> add(MLPtr<T> const& x, MLPtr<T>TMatrix<T> const& y)" ));
  }

  int sz = z->nrows_ * z->ncols_;
  
  T* const *const xp = x->mdata_; 
  T* const *const yp=  y->mdata_; 
  T** zp            =  z->mdata_;
  
  for(int i=0;  i<x->nrows_; ++i) {
    for(int j=0;  j<x->ncols_; ++j) {

      zp[i][j] = xp[i][j]  + yp[i][j];
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
 

  for(int i=0;  i< x->nrows_; ++i) {
    z->mdata_[i][i] += y;
  }

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
MLPtr<T> Negate( MLPtr<T> const& x) // unary minus 
{
  MLPtr<T> z( new TML<T>( *x) ); //deep copy

  T* p = z->mdata_[0];

  int sz = z->nrows_ * z->ncols_;

  for( int i=0; i<sz; ++i ) { (*p) = -(*p); ++p; }

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
MLPtr<T> subtract(MLPtr<T> const& x, MLPtr<T> const& y) 
{
  MLPtr<T> z( new TML<T>(x->nrows_, x->ncols_ ,T()) );

  if(( x->nrows_ != y->nrows_ ) || (x->ncols_ != y->ncols_ )) {
    throw( typename TML<T>::Incompatible( x->nrows_, x->ncols_, y->nrows_, y->ncols_,
           "subtract(MLPtr<T> const& x, MLPtr<T> const& y) " ));
  }

 
  T** zp             = z->mdata_;
  const T* const* xp = x->mdata_;
  const T* const* yp = y->mdata_;


  for( int i=0; i<x->nrows_; ++i ) {
    for( int j=0; j<x->ncols_; ++j ) {

       zp[i][j] = xp[i][j] - yp[i][j];
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

  T* p = z->mdata_[0];
  int sz = x->nrows_ * x->ncols_;
  
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
  MLPtr<T> z( new TML<T>( x->nrows_, y->ncols_, T()) );

  if( x->ncols_ != y->nrows_) {
    throw( typename TML<T>::Incompatible( x->nrows_, x->ncols_, y->nrows_, y->ncols_,
           "multiply(MLPtr<T> const& x, MLPtr<T> const& y)" ) );
  }

  T sum;

  for( int row=0; row< x->nrows_; ++row) {
    for(int col=0; col <y->ncols_; ++col) {
      sum = T();
      for(int i=0; i<x->ncols_; ++i) {
        sum += x->mdata_[row][i] * y->mdata_[i][col];
      }
      z->mdata_[row][col] = sum;
    }
  }
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
TVector<T> multiply( MLPtr<T> const& x, TVector<T> const& y){ // right vector multiply


  TVector<T> z(  x->nrows_ );

  if ( x->ncols_ != y.Dim() ) {
    throw( typename TML<T>::Incompatible( x->nrows_, x->ncols_, y.Dim(), 1,
           "multiply(MLPtr<T> const& x, Vector const& y)" ) );
  }

  T sum;

  for( int row=0; row< x->nrows_; ++row) {
      sum = T();
      for(int i=0; i<x->ncols_; ++i) {
        sum += x->mdata_[row][i] * y[i];
      }
      z[row] = sum;
  }
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> MLPtr<T> 
multiply( TVector<T>  const& x, MLPtr<T> const& y) {        // left vector multiply 

  MLPtr<T> z( new TML<T>( 1 , y->ncols_, T()) );

  if( x.Dim() != y->nrows_) {
    throw( typename TML<T>::Incompatible( 1, x.Dim(), y->nrows_, y->ncols_,
           "multiply(Vector const& x, MLPtr<T> const& y)" ) );
  }

  T sum;

    for(int col=0; col < x.Dim(); ++col) {
      sum = T();
      for(int i=0; i< y->nrows_; ++i) {
        sum += x(i) * y->mdata_[i][col];
      }
      z->mdata_[0][col] = sum;
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
    (*FNAL::pcerr) << "TMatrix<T>:operator/ divide by zero" << endl;

  for(int i=0;  i<x->nrows_ ; ++i) {
     for(int j=0; j< x->ncols_; ++j) {

      z->mdata_[i][j] /= y;

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
void TML<T>::copy_column(MLPtr<T> const& mm, int from_col, int to_col)
{
  if(nrows_  != mm->nrows_) {
    throw( Incompatible( nrows_, ncols_, mm->nrows_, mm->ncols_,
       "void TML<T>::copy_column(MLPtr<T> const& mm, int from_col, int to_col)" )  );
  }
  else {
    for(int row=0; row < nrows_; ++row)
      mdata_[row][to_col] = mm->mdata_[row][from_col];
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TML<T>::switch_columns(int col1, int col2) 
{
  
  std::vector<T> tmp_col;

  // temporarily store col1:

  for(int row=0; row< nrows_; ++row) {
    tmp_col.push_back ( mdata_[row][col1] );
  }

  for(int row=0; row< nrows_; ++row) {
    mdata_[row][col1] = mdata_[row][col2];    // move col2 to col1
  }

  for(int row=0;  row< nrows_; ++row) {
    mdata_[row][col2] = tmp_col[row];          // move temp to col2
 }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TML<T>::switch_rows(int row1, int row2) 
{

  // mdata_ is an array of T**;  data is stored row-wise.

  std::swap( mdata_[row1], mdata_[row2] ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// scale a matrix (used in L-U decomposition)

template<typename T>
MLPtr<T> TML<T>::scale() const 
{
  if (nrows_ <= 0 || ncols_ <= 0) {
    throw( GenericMatrixException( nrows_, ncols_
                   , "TMatrix<T> TMatrix<T>::scale()"
                   , "Bad TMatrix<T> for scale()" ) );
  }
  if (nrows_ != ncols_) {
    throw( NotSquare( nrows_, ncols_, "TMatrix<T> TMatrix<T>::scale()" )  );
  }

  MLPtr<T> scale_vector( new TML<T>( ncols_, 1, T()) );

  double maximum = 0.0;
  for (int row=0; row< nrows_; ++row){

    maximum = 0.0;

    for(int col=0; col< ncols_; ++col) {
      maximum = std::max( maximum, norm(mdata_[row][col]) );
    }

    if(maximum == 0.0 ) {
      (*FNAL::pcerr) << "\n*** ERROR *** Matrix = \n" << *this << endl;
      throw( GenericMatrixException( nrows_, ncols_
                     , "TML<T>::scale()"
                     , "Matrix is singular." ) );
    }
    // save scaling

    scale_vector->mdata_[row][0] = ((T) 1)/ maximum;
  }
  return scale_vector;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
 MLPtr<T> TML<T>::lu_decompose( int* indx, int& d ) const
{
  //----------------------------------------------------------------
  // Returns the L-U decomposition of a matrix. indx is an ouput
  // vector which records the row permutation affected by the
  // partial pivoting, d is output as +-1 depending on whether the
  // number of row interchanges was even or odd, respectively.
  // This routine is used in combination with lu_back_subst to
  // solve linear equations or invert a matrix.
  //-----------------------------------------------------------------
 
  if(nrows_  != ncols_) {
    throw( NotSquare( nrows_, ncols_,
      "TML<T>::lu_decompose(int* indx, int& d ) const " )  );
  }

  d = 1; // parity check
  T dum; // from the book - I don't know signj
  T sum;
  T maximum;
  T tmp;

  // clone the original matrix:
  MLPtr<T> lu_decomp( new TML<T>( *this) );

  // scale the matrix
  MLPtr<T> scale_vector;
  try {
    scale_vector = lu_decomp->scale(); 
  }
  catch( GenericMatrixException const& ge ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TML<T>::lu_decompose( int*, int& )",
           ge.what() ) );
  }
  catch( NotSquare const& ge ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TML<T>::lu_decompose( int*, int& )",
           ge.what() ) );
  }

  // The loop over columns of Crout's method:

  int col_max = 0; 
  int i       = 0;
  int j       = 0;

  for( j=0; j< nrows_; ++j) {
    if (j > 0) {
      // eqn 2.3.12 except for j=i:
      for ( i=0; i<= j-1; ++i) {
        sum = lu_decomp->mdata_[i][j];
        if(i > 0) {
          for(int k=0; k <= i-1; ++k) {
            tmp = lu_decomp->mdata_[i][k]*lu_decomp->mdata_[k][j];
            sum -= tmp;
          }
          lu_decomp->mdata_[i][j] = sum;
        }
      }
    }

    // Initialize the search for the largest pivot element:
    maximum = T();
    // i=j of eq 2.3.12 & i=j+I..N of 2.3.13:
    for( i=j; i <= ncols_-1; ++i) {
      sum = lu_decomp->mdata_[i][j];

      if(j > 0) {
        for(int k=0; k <= j-1; ++k) {
          tmp =  lu_decomp->mdata_[i][k] * lu_decomp->mdata_[k][j];
          sum -= tmp;
        }
        lu_decomp->mdata_[i][j] = sum;
      }
      // figure of merit for pivot:
      dum = scale_vector->mdata_[i][0] * norm(sum);

      if ( norm (dum) >= norm(maximum) ){
        // is it better than the best so far ?
        col_max = i;
        maximum =norm(dum);
      }
    }

    // Do we need to interchange rows?

    if(j != col_max)  {
      lu_decomp->switch_rows(col_max,j); // Yes, do so ...
      d *= -1;  // ... and change the parity of d
      // also interchange the scale factor:
      dum = scale_vector->mdata_[col_max][0];
      scale_vector->mdata_[col_max][0] = scale_vector->mdata_[j][0];
      scale_vector->mdata_[j][0] = dum;
    }

    indx[j] = col_max;
    // Now, finally, divide by the pivot element:
    if(j != nrows_ -1){
      // If the pivot element is .zero the matrix is
      // singular (at least to the precision of the
      // algorithm). For some applications on singular
      // matrices, it is desirable to substitute tiny_ for 0
      if(lu_decomp->mdata_[j][j] == T())
        lu_decomp->mdata_[j][j] = T(tiny_); /// this may not work for all types
                                            
      dum = T(1)/lu_decomp->mdata_[j][j];
      for(i=j+1; i <= ncols_-1; i++)
        lu_decomp->mdata_[i][j] *= dum;
    }

  }
  if(lu_decomp->mdata_[nrows_-1][ncols_-1] == T())
    lu_decomp->mdata_[nrows_-1][ncols_-1] = T(tiny_);

  return lu_decomp;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TML<T>::lu_back_subst(int* indx, MLPtr<T>& b)  
{
//------------------------------------------------------------------
// Solves the set of N linear equations A*X = B. Here "this"
// is the LU-decomposition of the matrix A, determined by the
// routine lu_decompose(). Indx is input as the permutation
// vector returned  by lu_decompose(). B is input as the
// right-hand side vector B, and returns with the solution
// vector X. This routine takes into  account the possibility
// that B will begin with many zero elements, so it is efficient
// for use in matrix inversion. See pp 36-37 in
// Press & Flannery.
//--------------------------------------------------------------------

  if(nrows_  != ncols_) {
    throw( NotSquare( nrows_, ncols_,
      "void TML<T>::lu_back_subst(int* indx, MLPtr<T>& b)" )  );
  }
  if(nrows_  != b->nrows_) {
    throw( Incompatible( nrows_, ncols_, b->nrows_, b->ncols_,
      "void TML<T>::lu_back_subst(int* indx,MLPt<T>& b)" )  );
  }
//  if(nrows_  != indx->nrows_)
//    error("wrong size indx vector passed to lu_back_subst()");
  int i,j,ip;
  int ii = -1;
  T sum;
  T tmp;

  for(i=0; i< nrows_; ++i){
    ip = indx[i];
    sum = b->mdata_[ip][0];
    b->mdata_[ip][0] = b->mdata_[i][0];
    if (ii >= 0) {
      for(j = ii; j <= i-1; j++) {
        tmp = mdata_[i][j] * b->mdata_[j][0];
        sum -= tmp;
       }
    } else if(sum != T() )
      ii = i;
    b->mdata_[i][0] = sum;
  }
  for(i = nrows_-1; i >= 0; --i){
    sum = b->mdata_[i][0];
    if (i < ncols_ -1) {
      for (j = i+1; j <= nrows_-1; ++j) {
        tmp = mdata_[i][j] * b->mdata_[j][0];
        sum -= tmp;
      }
    }
    // store a component of the soln vector X:
    b->mdata_[i][0] = sum/mdata_[i][i];
  }
}



// Implementation of exception subclasses

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
TML<T>& TML<T>::operator=(TML<T> const& x) 
{
  if( this == &x )  return *this;

  ReferenceCounter<TML<T> >::operator=(x);
 
  //-----------------------------------------------------
  // if the LHS and RHS do not have the same dimensions, 
  // wipe LHS out and allocate a copy of RHS instead 
  // ---------------------------------------------------- 

  if ( ( nrows_ != x.nrows_) || ( ncols_ != x.ncols_ ) ) {

     this->~TML(); 
     new (this) TML(x);
     return *this;
  }

  //-----------------------------------------------------
  // Otherwise, just copy the data and the (adjusted) 
  // row pointers. 
  // ---------------------------------------------------- 

  for( int i=0; i<nrows_; ++i) { 
     mdata_[i] = data_ + ( x.mdata_[i] - x.data_);
  }

  std:copy ( x.data_, x.data_+ (x.nrows_*x.ncols_),  data_); 

  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
bool TML<T>::operator==(const TML<T>& x)
{
  if((x.nrows_ != nrows_) || (x.ncols_ != ncols_)) { return false; }
  for(int i=0; i < nrows_; ++i) {
    for(int j=0; j < ncols_; ++j) {
      if(x.mdata_[i][j] != mdata_[i][j]) { return false; }
    }
  }
  return true;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
bool TML<T>::operator==(const T& x) 
{
  if(nrows_ != ncols_) return false;
  for(int i=0; i< nrows_; i++) {
    if(mdata_[i][i] != x) return false;
    for(int j =0; j < ncols_; j++) {
      if((i != j) && (mdata_[i][j] != 0.0)) { return false; }
    }
  }
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>& TML<T>::operator+=( const T& x) 
{
  for(int i=0;  i<nrows_; ++i) {
    mdata_[i][i] += x;
  }
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>&  TML<T>::operator+=( MLPtr<T> const& rhs)
{

  T**               lrowptr; 
  T const * const * rrowptr; 

  for ( lrowptr = &mdata_[0], rrowptr = &(rhs->mdata_[0]); lrowptr != &mdata_[0] + nrows_; ++lrowptr, ++rrowptr ) {
    std::transform( *lrowptr, (*lrowptr)+ncols_, *rrowptr, *lrowptr, std::plus<T>());
  }
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>&  TML<T>::operator-=( MLPtr<T> const& rhs)
{
  T**               lrowptr;
  T const * const * rrowptr;

  for ( lrowptr = &mdata_[0], rrowptr = &(rhs->mdata_[0]); lrowptr != &mdata_[0] + nrows_; ++lrowptr, ++rrowptr ) {
    std::transform( *lrowptr, (*lrowptr)+ncols_, *rrowptr, *lrowptr, std::minus<T>());
  }
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
bool operator==( TML<T> const& x, TML<T> const& y) 
{
  if((x->nrows_ != y->nrows_) || (x->ncols_ != y->ncols_)) { return false; }
  for(int i=0;  i< x->nrows_; ++i) {
    for(int j=0;  j<x->ncols_; ++j) {

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
    (*FNAL::pcerr) << "\n*** ERROR *** "
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
    (*FNAL::pcerr) << "\n*** ERROR *** "
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
    (*FNAL::pcerr) << "\n*** ERROR *** "
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
    (*FNAL::pcerr) << "\n*** ERROR *** "
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
    (*FNAL::pcerr) << errorString;
    (*FNAL::pcerr) << "\n*** ERROR *** This message is printed only once."
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


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename T>
void TML<T>::GaussJordan( TML<T>& a, TML<T>& rhs)
{

  int n =   a.rows();
  int m = rhs.cols();

  std::vector<int> indxr(n, 0);
  std::vector<int> indxc(n, 0);
  std::vector<int>  ipiv(n, 0);

  int icol =0;
  int irow =0;

  for (int i=0; i<n; ++i) {

    norm_return_type big = norm( T() );

    for (int j=0; j<n; ++j)

      if (ipiv[j] != 1)

        for (int k=0; k<n; ++k) {
          if (ipiv[k] == 0) {
            if ( norm( a(j,k) ) >= big) {
              big = norm(a(j,k));
              irow = j;
              icol = k;
            }
          } else if (ipiv[k] > 1) 
            (*FNAL::pcerr) << "TML::GaussJordan: Singular Matrix-1 " << ipiv[k] << endl;
        }

    ++(ipiv[icol]);

    if (irow != icol) {
      for (int l=0; l<n; ++l)  swap(    a(irow,l), a(icol,l) );
      for (int l=0; l<m; ++l)  swap(  rhs(irow,l), rhs(icol,l) );
    }
    indxr[i]=irow;
    indxc[i]=icol;

    if (a(icol,icol) == 0.0) 
      (*FNAL::pcerr) << ": TML::GaussJordan::Singular Matrix-2 " << a << endl;

    T pivinv =  T(1.0)/ a(icol,icol);
 
    a(icol,icol) = T(1.0);
 
    for ( int l=0;  l<n;   ++l) a(icol,l) *= pivinv;
    for ( int l=0;  l<m;   ++l) rhs(icol,l) *= pivinv;
    for ( int ll=0; ll<n; ++ll)
      if (ll != icol) {
        T dum = a(ll,icol);
        a(ll,icol)= T();
        for (int l=0; l<n; ++l)  a(ll,l)   -= a(icol,l)*dum;
        for (int l=0; l<m; ++l)  rhs(ll,l) -= rhs(icol,l)*dum;
      }
  }

  for ( int l=n-1; l>=0 ; --l) {
    if (indxr[l] != indxc[l])
      for ( int k=0; k<n; ++k)
        swap(a(k,indxr[l]),a(k,indxc[l]));
  }
}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TML<T>::GaussJordan( TML<T>& a, TVector<T>& b)
{

  TML<T> btmp(b.Dim(), 1);

  for (int i=0; i<b.Dim(); ++i ) btmp(i,0) = b[i]; 

  GaussJordan( a, btmp ); 

  for (int i=0;  i < b.Dim(); ++i ) b[i] = btmp(i,0); 
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


