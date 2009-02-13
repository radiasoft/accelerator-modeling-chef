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
******  Revision History:
****** 
******  Sep-Nov 2005   Jean-Francois Ostiguy
******                 ostiguy@fnal.gov
******  
******   - refactored code to use single template parameter instead of two
******   - introduced implicit conversions 
******   - using boost::intrusive_pointer for reference counting
******   - eliminated access to internals of implementation class (TML) 
******     from class TMatrix.
******   - eliminated separate MatrixC class implementation 
******     (used to inherit from Matrix<T1,T2>)
******   - organized code to support both implicit and 
******     explicit template instantiations
******   - fixed incorrect complex version of eigenvalues/eigenvectors 
******   - separated eigenvalue/eigenvector reordering function 
******   - eliminated code that attempted to discriminate between objects 
******     allocated on the stack and objects allocated from the free store.
******                                                                
****** May 2008 ostiguy@fnal.gov
******   - added general symplecticity test
******                                                               
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include <sstream>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/TML.h>
#include <basic_toolkit/TVector.h>

#ifdef WIN32
extern long   srand48(long int);
extern double drand48();
#endif

extern "C" { void rg_(int*, int*, double*, double*,double*, int*, double*,
                 int*, double*, int*); }

extern "C" { void cg_(int*, int*, double*, double*,double*,double*, int*,
                      double*,double*,double*, double*, double*,int*); }


using FNAL::pcout;
using FNAL::pcerr;

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix()
 : ml_( MLPtr<T>( new TML<T> )) // , m1d_(this, 0)  
{ }

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix(int rows)
 : ml_( MLPtr<T>( new TML<T>( rows, 1, T() ) )) // , m1d_(this, 0) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix( TVector<T> const& v)
 : ml_( MLPtr<T>( new TML<T>( v.size() , 1 ,T() ) ) )// , m1d_(this, 0) 
{ 
  for(int irow=0; irow<rows(); ++irow) { (*this)[irow][0] = v[irow]; } 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix(int rows, int columns)
 : ml_( MLPtr<T>( new TML<T>( rows,columns,T() ) ) )// , m1d_(this, 0) 
{ }

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix(int rows, int columns, T initval)
 : ml_( MLPtr<T>( new TML<T> (rows,columns,initval) ) ) // , m1d_(this, 0) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix(int rows, int columns, T* initval)
: ml_( MLPtr<T>( new TML<T> ( rows,columns,initval) ) ) // , m1d_(this, 0) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix(TMatrix const& X)
 : ml_(X.ml_) // , m1d_(this, 0)  
{ }

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::~TMatrix<T>() 
{} /* ml_ automatically destroyed if refcount=0 */ 

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>*  TMatrix<T>::Clone() const   
{ 
   TMatrix<T>* p = new TMatrix<T>();
   p->ml_ = MLPtr<T>(ml_->clone()); 
   return p;
} 

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
template<typename T>
TMatrix<T>& TMatrix<T>::operator=(const TMatrix& x) 
{

    if (&x == this) return *this;

    ml_ = x.ml_;
    return *this;

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>& TMatrix<T>::operator=(TVector<T> const& x) 
{

  if ( std::min( rows(), cols()) != 1 ) { 
      throw( GenericException( __FILE__,  __LINE__, 
                              "TMatrix<T>::TMatrix<T>operator=(TVector<T> const& )"
                              "RHS Vector has dimension incompatible with LHS Matrix.") );
  }
 
  if ( rows() == 1) {
   
      for (int i=0; i<  cols(); ++i ) (*this)(0,i) = x[i];
      return *this;
   }

  for (int i=0; i< rows(); ++i )  (*this) (i,0) = x[i];

  return *this;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( TMatrix<T> const& x, TMatrix<T> const& y ) 
{
 return *(x.ml_) == *(y.ml_);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( TMatrix<T> const& x, T const& y ) 

{
 return *(x.ml_) == y;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( T const& y, TMatrix<T> const& x )
{
 return *(x.ml_) == y;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( TMatrix<T> const& x, TMatrix<T> const& y ) 
{
 return !( x == y );
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( TMatrix<T> const& x, T const& y ) 
{
 return !( x == y );
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( T const& x, TMatrix<T> const& y ) 
{
 return !( x == y );
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>& TMatrix<T>::operator+=( TMatrix<T> const& x ) 
{
 
  if (ml_->count() > 1) ml_= ml_->clone();
  ml_->operator+=( x.ml_ );
  return *this;

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>& TMatrix<T>::operator-=( TMatrix<T> const& x ) 
{
 
  if (ml_->count() > 1) ml_= ml_->clone();
  ml_->operator-=( x.ml_ );
  return *this;

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>& TMatrix<T>::operator+=( T const& x ) 
{
 
  if (ml_->count() > 1) ml_= ml_->clone();
  ml_->operator+=( x );
  return *this;

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>& TMatrix<T>::operator-=( T const& x ) 
{

 if (ml_->count() > 1) ml_= ml_->clone();
 ml_->operator+=( -x );
 return *this;

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMatrix<T>::Square() const 
{
  TMatrix<T> ret;
  ret.ml_ = ml_->Square();  
  return  ret;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMatrix<T>::transpose() const 
{
  TMatrix<T> ret;
  ret.ml_= ml_->transpose();
  return ret;

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TMatrix<T>::trace() const
{
  return ml_->trace();
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
double TMatrix<T>::maxNorm() const
{
   return ml_->maxNorm();
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TMatrix<T>::isOrthogonal( double eps) const
{ 
  TMatrix<T> W = ( this->dagger() * (*this) - TMatrix<T>::Imatrix( this-> rows() ) );  
  return ( W.maxNorm() < eps ) ;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool  TMatrix<T>::isSymplectic(char convention, double eps) const
{ 
  
   // 'S' = beam dynamics ordering convention 
   // 'J' = hamiltonian pertubation theory convention  

   if (rows() != cols() ) return false;   
   if (rows() % 2 != 0  ) return false;   
   if (cols() % 2 != 0  ) return false;   

   TMatrix<T> S; 

   if ( (convention == 's') || (convention == 'S' )) { 
      S = Smatrix( rows() ); 
   } 
   else if ( (convention == 'j') || convention == 'J') { 
      S = Jmatrix( rows() ); 
   }
   else {
     throw( GenericException( __FILE__,  __LINE__, 
                              "TMatrix<T>::isSymplectic"
                              "Unknown state variable ordering convention") );
   }     

   TMatrix<T> M(*this);
  
   M = ( M.dagger()*S*M ) - S; 

   return ( M.maxNorm() < eps );
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TMatrix<T>::determinant() const
{
  return  ml_->determinant();
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMatrix<T>::inverse() const 
{
  TMatrix<T> ret;
  ret.ml_ = ml_->inverse(); // ml_->inverse() returns a new matrix
  return ret;
}


// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void    TMatrix<T>:: switch_columns( int col1, int col2) {

  if (ml_->count() > 1) ml_= ml_->clone();
  ml_->switch_columns(col1,col2);

};


// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// operators

template<typename T>
T& TMatrix<T>::operator()(int const& i, int const& j) 
{

  if ( ml_->count() > 1 ) ml_ = ml_->clone();  
  return (*ml_)(i,j); 

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T const&  TMatrix<T>::operator()(int const& i, int const& j) const 
{
  
  return (*ml_)(i,j);
 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T & TMatrix<T>::operator()(int const& i) 
{
  if ( ml_->count() > 1 ) ml_ = ml_->clone();  
  return (*ml_)(i);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T const& TMatrix<T>::operator()(int const& i) const 
{
return ml_->operator()(i);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
std::ostream& operator<<(std::ostream& os, TMatrix<T> const& x)
{
   return os << *(x.ml_); 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator+(TMatrix<T> const& x, TMatrix<T> const& y) 
{
  TMatrix<T> res;
  res.ml_ = add<T>(x.ml_, y.ml_); // add returns a new matrix
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator+(TMatrix<T> const& x, T const& y) 
{
  TMatrix<T> res;
  res.ml_ = add<T>(x.ml_, y); // add returns a new matrix
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator+( T const& y, TMatrix<T> const& x )  // add returns a new matrix
{
  TMatrix<T> res;
  res.ml_ =  add<T>(x.ml_, y);
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator-(TMatrix<T> const& x) 
{
  TMatrix<T> res;
  res.ml_ =  Negate<T>(x.ml_); // Note: Negate does a deepcopy   
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator-(TMatrix<T> const& x, TMatrix<T> const& y) 
{
  TMatrix<T> res;
  res.ml_ = subtract<T>(x.ml_, y.ml_);
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator-(TMatrix<T> const& x, T const& y) 
{
  TMatrix<T> res;
  res.ml_ = add<T>(x.ml_, -y);
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator-( T const& y, TMatrix<T> const& x ) 
{
  TMatrix<T> res;
  res.ml_ =  Negate<T>( add<T>(x.ml_, -y) );
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator*( TMatrix<T> const& x, T const& y) 
{
  TMatrix<T> res; 
  res.ml_ = multiply<T>(x.ml_, y);
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator*( T const& y, TMatrix<T> const& x) 
{
  TMatrix<T> res; 
  res.ml_ = multiply<T> (x.ml_, y);
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator*(TMatrix<T> const& x, TMatrix<T> const& y)  
{
  TMatrix<T> res; 
  res.ml_ =  multiply<T>(x.ml_, y.ml_);
  return res;
}


// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T> 
TVector<T> operator*(TMatrix<T>  const& x,  TVector<T> const& v) // right multiply
{
   return  multiply<T>(x.ml_, v);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T> 
TMatrix<T> operator*(TVector<T>  const& v,  TMatrix<T> const& y) // left multiply
{
  TMatrix<T> res; 
  res.ml_ = multiply<T>(v, y.ml_);
  return res;
}
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator/( TMatrix<T> const& x, T const& y) 
{
  TMatrix<T> res; 
  res.ml_= divide<T>(x.ml_, y);
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator/( T const& x, TMatrix<T> const& y) 
{
  TMatrix<T> res; 
  res.ml_ = divide<T>(x, y.ml_);
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator/( TMatrix<T> const& x, TMatrix<T> const& y) 
{
  TMatrix<T> res; 
  res.ml_ = divide<T>(x.ml_, y.ml_);
  return res;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
void TMatrix<T>::GaussJordan( TMatrix& A, TVector<T> & rhs)
{
  if ( A.ml_->count() > 1 ) A.ml_ = A.ml_->clone();  
  TML<T>::GaussJordan( *A.ml_ , rhs ); 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
void TMatrix<T>::GaussJordan( TMatrix& A, TMatrix& rhs)
{
  if (   A.ml_->count() > 1 )    A.ml_   =     A.ml_->clone();  
  if ( rhs.ml_->count() > 1 )  rhs.ml_   =   rhs.ml_->clone();  

  TML<T>::GaussJordan( *A.ml_ , *rhs.ml_); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<std::complex<double> > TMatrix<T>::eigenValues() const
{
  return ml_->eigenValues(); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<std::complex<double> > TMatrix<T>::eigenVectors() const
{
  TMatrix<std::complex<double> > ret;
  ret.ml_ = ml_->eigenVectors(); 
  return  ret; 
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMatrix<T>::scale() const
{
  // returns a col vector; Each row is the max of the corresponding matrix row elements. 
  TMatrix<T> result;
  result.ml_ = ml_->scale();
  return result;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMatrix<T>::lu_decompose( int* permutations, int& d) const
{
  TMatrix<T> result;
  result.ml_ = ml_->lu_decompose( permutations, d);
  return result;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TMatrix<T>::lu_back_subst( int* permutations, TMatrix& rhs)
{
 ml_->lu_back_subst( permutations, rhs.ml_ );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> const TMatrix<T>::Smatrix(int n)
{   
   if ( n%2 != 0 ) {
      throw( GenericException( __FILE__,  __LINE__, 
                              "TMatrix<T>::JMatrix(int n) "
                              "A Symplectic matrix must have even dimensionality.") );
   }     
  
   TMatrix<T> S(n,n);
   for (int i=0; i<n; ++i) {  
     for (int j=0; j<n; ++j) {
       if ( ((i%2)== 0) && ((j-i)==1) ) S[i][j] = T( 1.0); 
       if ( ((i%2)!= 0) && ((i-j)==1) ) S[i][j] = T(-1.0); 
     }
   } 

   return S;
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> const TMatrix<T>::Jmatrix(int n)   
{
   if ( n%2 != 0 ) {
      throw( GenericException( __FILE__,  __LINE__, 
                              "TMatrix<T>::JMatrix(int n) "
                              "A Symplectic matrix must have even dimensionality.") );
   }     
  
   TMatrix<T> J(n,n);
   for (int i=0; i<n; ++i) {  
     for (int j=0; j<n; ++j) {
       if (j == i+n/2)  J[i][j] =  T(  1.0 ); 
       if (i == j+n/2)  J[i][j] =  T( -1.0 ); 
     }
   } 

   return J;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
   
template<typename T>
TMatrix<T> const TMatrix<T>::Imatrix(int n)   
{
  
   TMatrix<T> I(n,n);
   for (int i=0; i<n; ++i) { I[i][i] = T(1.0); } 

   return I;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
   
template<typename T>
TMatrix<std::complex<double> > const  TMatrix<T>::Qmatrix(int n) 
{
   if ( n%2 != 0 ) {
      throw( GenericException( __FILE__,  __LINE__, 
                              "TMatrix<T>::QMatrix(int n) "
                              "A Symplectic matrix must have even dimensionality.") );
   }     
  
   TMatrix<std::complex<double> > Q(n,n);
   
   for (int i=0; i <6; i+=2 ) { 
          Q[i  ][i]  = 1.0/sqrt(2.0);   Q[i  ][i+1] =  std::complex<double>(0.0,  1.0)/sqrt(2.0); 
          Q[i+1][i]  = 1.0/sqrt(2.0);   Q[i+1][i+1] =  std::complex<double>(0.0, -1.0)/sqrt(2.0); 
   }

   return Q;
}   
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
   
template<typename T>
TMatrix<std::complex<double> > const TMatrix<T>::Rmatrix(int n) 
{
   if ( n%2 != 0 ) {
      throw( GenericException( __FILE__,  __LINE__, 
                              "TMatrix<T>::RMatrix(int n) "
                              "A Symplectic matrix must have even dimensionality.") );
   }     
  
   TMatrix<std::complex<double> > R(n,n);

   for (int i=0; i<n/2; ++i ){
          R[i][i    ]       =  1.0/sqrt(2.0);   
          R[i][i+n/2]       =  std::complex<double>(0.0,  1.0)/sqrt(2.0);

          R[i+n/2][i]       =  1.0/sqrt(2.0); 
          R[i+n/2][i+n/2]   =  std::complex<double>(0.0, -1.0)/sqrt(2.0); 
   }

   return R;
}   
