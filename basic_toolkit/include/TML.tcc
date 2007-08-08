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
******
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <algorithm>
#include <vector>
#include <basic_toolkit/iosetup.h>

using std::cout;
using std::endl;

template<typename T> double TML<T>::tiny_                = 1.0e-20; // pivot threshold



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>::TML(): mdata_(0), nrows_(0), ncols_(0)  { } // default: an empty matrix


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>::TML(const char* flag, int dimension ): nrows_(dimension), ncols_(dimension)
{

  if( ( (dimension%2) != 0) &&
      (flag[0] == 'J') ) {
    throw( GenericMatrixException( dimension, dimension
                     , "TML<T>::TML<T>(const char* flag, int dimension)"
                     , "Dimension must be even for J matrix") );
  }

  mdata_ = new T* [nrows_];

  int i=0;

  int sz = nrows_*ncols_;
  T* dataPtr = new T [ sz ];

  for( i=0; i<nrows_; ++i ) { 
    mdata_[i] = dataPtr;
    dataPtr += ncols_;
  }

   // reset dataPtr

   dataPtr = mdata_[0];
 
   for( i=0;  i<sz;  ++i) {
      (*dataPtr) = T(); 
      ++dataPtr; 
   }


  if (flag[0]  == 'I') {
    for ( i=0; i<dimension; ++i) {
       mdata_[i][i] = T(1.0); 
    }
  } else if (flag[0] == 'J') {
    for (i = dimension/2; i< dimension; i++) {
      mdata_[i-dimension/2][i] = T(1.0); 
      mdata_[i][i-dimension/2] = T(-1.0);
    }
  } else {
      throw( GenericMatrixException( dimension, dimension
                     , "TML<T>::TML<T>(const char* flag, int dimension)"
                     , (std::string("Unknown flag: ")+flag).c_str() ));
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TML<T>::TML(int rows, int columns, T initval): nrows_(rows), ncols_(columns)
{

  mdata_ = new T* [nrows_];
 
  int sz = nrows_*ncols_;
  T* dataPtr = new T [ sz ];

  for( int i=0; i<nrows_; ++i ) { 
    mdata_[i] = dataPtr;
    dataPtr += ncols_;
  }

   T* colptr = mdata_[0];
 
   for( int i=0;  i<sz;  ++i) {
      (*colptr) = initval; 
       ++colptr; 
   }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>::TML(int rows, int columns, const T* values): nrows_(rows), ncols_(columns)
{
  mdata_ = new T* [nrows_];

  int sz = nrows_*ncols_;
  T* dataPtr = new T [ sz ];
  for( int i=0; i < nrows_; ++i) { 
    mdata_[i] = dataPtr;
    dataPtr += ncols_;
  }

  memcpy( (void*) (mdata_[0]), (void*) values, sz*sizeof(T) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>::TML( TML<T> const& X ): ReferenceCounter<TML<T> >(), nrows_(X.nrows_), ncols_(X.ncols_)
{

  mdata_ = new T* [nrows_];

  int sz = nrows_*ncols_;
  T* dataPtr = new T [ sz ];

  for( int i=0; i<nrows_; ++i) { 
     mdata_[i] = dataPtr;
     dataPtr += ncols_;
  }

  memcpy( (void*) (mdata_[0]), (void*) ((X.mdata_)[0]), sz*sizeof(T) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TML<T>::~TML() {
 
  nrows_ = 0;
  ncols_ = 0;

  if (!mdata_) return; // valid to delete a 0x0 matrix ... 
   
  if (mdata_[0]) delete [] mdata_[0];
  
  delete [] mdata_;
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TML<T>::clear() {

  int sz = nrows_*ncols_;

  T* dataptr = mdata_[0];

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

  int d = 0;
  if( nrows_ < ncols_ ) {
     d = nrows_;
  } else {                  
     d = ncols_;
  }

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
bool TML<T>::isOrthogonal() const
{
 if( nrows_ != ncols_ ) { return false; }

  MLPtr<T> self( new TML<T>(*this) ); // deepcopy  

  MLPtr<T> W( multiply<T>( self, self->transpose() ));

  int n = nrows_;

  for( int i=0; i<n; ++i ) {
    for( int j=0; j<n; ++j ) {
      if( i != j ) { if( 1.0e-12 < norm( (*W)(i,j) ) )       { return false; } }
      else         { if( 1.0e-12 < norm( (*W)(i,j) - 1.0 ) ) { return false; } }
    }
  }

  W = multiply<T>( self->transpose(),  self);

  for( int i=0; i<n; ++i ) {
    for( int j = 0; j<n; ++j ) {
      if( i != j ) { if( 1.0e-12 < norm( (*W)(i,j) ) )       { return false; } }
      else         { if( 1.0e-12 < norm( (*W)(i,j) - 1.0 ) ) { return false; } }
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
  if( nrows_ != ncols_) {
    throw( NotSquare( nrows_, ncols_, "TML<T>::determinant()" )  );
  }
  else {
    int* indx = new int[ncols_];  // create the "index vector
                              // see pp 38. in Numerical Recipes
    int d;
    // perform the decomposition once:

    MLPtr<T> decomp( lu_decompose(indx,d));
    det = d;
    for(int i=0; i<ncols_ ; ++i)
      det *= decomp->mdata_[i][i];
    delete [] indx;
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
  else {
  
    MLPtr<T> Y( new TML<T>("I",nrows_) );   // create an identity TMatrix<T>
    int* indx = new int[ncols_];                           // create the "index vector"
    MLPtr<T> B( new TML<T>( ncols_, ncols_, T()) );       // see Press & Flannery
    int d;

    // perform the decomposition once:

    MLPtr<T> decomp (lu_decompose(indx,d));

    for(int col = 0; col < ncols_; ++col){
      B->copy_column(Y,col,0);
      decomp->lu_back_subst(indx,B);
      Y->copy_column(B,0,col);
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

  if((i >= nrows_) || (j >= ncols_) ||
     (i < 0      ) || (j < 0      )    ) { 
       throw( IndexRange( i, j, nrows_-1, ncols_-1,
                       "T& TML<T>::operator()(int i, int j)" ) );
  }
  else { 
    return mdata_[i][j]; 
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T  TML<T>::operator()(int i, int j) const 
{
  
  if((i >= nrows_) || (j >= ncols_) ||
     (i < 0      ) || (j < 0      )    )
  { throw( IndexRange( i, j, nrows_-1, ncols_-1,
                       "T TML<T>::operator()(int i, int j) const" ) );
  }
  else { 
    return mdata_[i][j]; 
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T& TML<T>::operator()(int i) 
{
  // This body must stay in synch with
  // T TMatrix<T>::operator()(int i) const

  if( nrows_ == 1 ) {
    if( i >= 0 && i < ncols_ ) {
      return mdata_[0][i];
    }
    else {
      throw( IndexRange( 0, i, 0, ncols_-1,
                       "T& TML<T>::operator()(int i)" ) );
    }
  }
  else if( ncols_== 1 ) {
    if( i >= 0 && i < nrows_ ) {
      return mdata_[i][0];
    }
    else {
      throw( IndexRange( i, 0, nrows_-1, 0,
                       "T& TML<T>::operator()(int i)" ) );
    }
  }
  else {
    throw( NotVector( i, nrows_, ncols_,
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
  if( nrows_ == 1 ) {
    if( i >= 0 && i < ncols_ ) {
      return mdata_[0][i];
    }
    else {
      throw( IndexRange( 0, i, 0, ncols_-1,
                       "T& TML<T>::operator()(int i)" ) );
    }
  }
  else if( ncols_ == 1 ) {
    if( i >= 0 && i < nrows_ ) {
      return mdata_[i][0];
    }
    else {
      throw( IndexRange( i, 0, nrows_-1, 0,
                       "T& TMatrix<T>::operator()(int i)" ) );
    }
  }
  else {
    throw( NotVector( i, nrows_, ncols_,
                      "T& TMatrix<T>::operator()(int i)" ) );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
std::ostream& operator<<( std::ostream& os, const TML<T>& x)
{

  for(int i=0; i< x.nrows_; i++) {
    os << "( ";
    for(int j=0; j< x.ncols_; j++) {
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
MLPtr<T> multiply( MLPtr<T> const& x, TVector<T> const& y){ // right vector multiply


  MLPtr<T> z( new TML<T>( x->nrows_, 1 , T()) );

  if ( x->ncols_ != y.Dim() ) {
    throw( typename TML<T>::Incompatible( x->nrows_, x->ncols_, y.Dim(), 1,
           "multiply(MLPtr<T> const& x, Vector const& y)" ) );
  }

  T sum;

  for( int row=0; row< x->nrows_; ++row) {
      sum = T();
      for(int i=0; i<x->ncols_; ++i) {
        sum += x->mdata_[row][i] * y(i);
      }
      z->mdata_[row][0] = sum;
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
  
  MLPtr<T> temp( new TML<T>(nrows_, 1, T()) );

  for(int row=0; row< nrows_; ++row) {
    // temporarily store col 1:
    temp->mdata_[row][0] = mdata_[row][col1];
  }

  for(int row=0; row< nrows_; ++row) {
    mdata_[row][col1] = mdata_[row][col2];    // move col2 to col1
  }

  for(int row=0;  row< nrows_; ++row) {
    mdata_[row][col2] = temp->mdata_[row][0]; // move temp to col2
 }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TML<T>::switch_rows(int row1, int row2) 
{
  MLPtr<T> temp( new TML<T>(1,ncols_, T()) );

  for(int col=0; col < ncols_; ++col) {
    // temporarily store row 1:
    temp->mdata_[0][col] = mdata_[row1][col];
  }

  for(int col=0; col < ncols_; ++col) {
    mdata_[row1][col] = mdata_[row2][col];    // move row2 to row1
  }

  for(int col=0; col < ncols_; ++col) {
    mdata_[row2][col] = temp->mdata_[0][col]; // move temp to row2
  }
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
  MLPtr<T> scale_vector = lu_decomp->scale(); 

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

      if ( norm (dum) > norm(maximum) ){
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
TML<T>& TML<T>::operator=(const TML& x) 
{
  if( this == &x )  return *this;
 
  this->~TML(); // call the destructor. 

  nrows_ = x.nrows_;
  ncols_ = x.ncols_;
  mdata_ = new T* [nrows_];

  int sz = nrows_*ncols_;
  T* dataPtr = new T [ sz ];
  for( int i = 0; i< nrows_; ++i) { 
      mdata_[i] = dataPtr;
      dataPtr += ncols_;
  }

  for (int i=0; i< nrows_; ++i) {
      for (int j=0; j< ncols_; ++j) {

      mdata_[i][j] = x.mdata_[i][j];

      }
  }

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
bool operator==(const TML<T>& x, const TML<T>& y) 
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


