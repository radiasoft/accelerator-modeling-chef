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

#include <iosetup.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <MathConstants.h>
#include <PhysicsConstants.h>
#include <TMatrix.h>
#include <TML.h>

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


static const int PHASESPACE = 6;
#define M_SMALL  1.0e-30

static double tiny = double(1e-20);


template<typename T>
TMatrix<T>::TMatrix(): _ml( typename MLPtr<T>::Type( new TML<T> ) ) 
{ }

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix(int rows): _ml( typename MLPtr<T>::Type( new TML<T>( rows, 1, T() ) )) {}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix(int rows, int columns):_ml( typename MLPtr<T>::Type( new TML<T>( rows,columns,T() ) ) ){ }

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix(int rows, int columns, T initval): _ml( typename MLPtr<T>::Type( new TML<T> (rows,columns,initval) )){}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix(int rows, int columns, T* initval): _ml( typename MLPtr<T>::Type( new TML<T> ( rows,columns,initval) )){}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix(const char* flag, int dimension): _ml( typename MLPtr<T>::Type( new TML<T>( flag,dimension) ))  {} 

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TMatrix<T>::TMatrix(const TMatrix& X): _ml(X._ml) { }

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::TMatrix(typename MLPtr<T>::Type& ml): _ml(ml) { }

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>::~TMatrix<T>() {/* _ml automatically destroyed if refcount=0 */ }

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>& TMatrix<T>::operator=(const TMatrix& x) 
{

    _ml = x._ml;
    return *this;

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>& TMatrix<T>::DeepCopy(const TMatrix<T>& x) 
{
  _ml = typename MLPtr<T>::Type( new TML<T>( *x._ml ) );

  return *this;
}


// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const TMatrix<T>& x, const TMatrix<T>& y ) 
{
 return *(x._ml) == *(y._ml);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const TMatrix<T>& x, const T& y ) 

{
 return *(x._ml) == y;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const T& y, const TMatrix<T>& x )
{
 return *(x._ml) == y;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TMatrix<T>& x, const TMatrix<T>& y ) 
{
 return !( x == y );
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TMatrix<T>& x, const T& y ) 
{
 return !( x == y );
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const T& x, const TMatrix<T>& y ) 
{
 return !( x == y );
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>& TMatrix<T>::operator+=( const T& x ) 
{
 
  _ml = _ml->clone();
  _ml->operator+=( x );
  return *this;

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T>& TMatrix<T>::operator-=( const T& x ) 
{

 _ml = _ml->clone();
 _ml->operator+=( -x );
 return *this;

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMatrix<T>::Square() const 
{

  typename MLPtr<T>::Type p( _ml->Square() );  
  return  TMatrix<T>( p);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMatrix<T>::transpose() const 
{

  typename MLPtr<T>::Type p( _ml->transpose() );
  return TMatrix<T> (p);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TMatrix<T>::trace() const
{

  return _ml->trace();

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TMatrix<T>::isOrthogonal() const
{
  
   return _ml->isOrthogonal();
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TMatrix<T>::determinant() const
{
 
  return  _ml->determinant();

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMatrix<T>::inverse() const 
{
  typename MLPtr<T>::Type p(_ml->inverse()); // _ml->inverse() returns a new matrix
  return TMatrix<T>( p );
}


// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void    TMatrix<T>:: switch_columns( int col1, int col2) {

  _ml = _ml->clone();
  _ml->_switch_columns(col1,col2);

};


// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// operators

template<typename T>
T& TMatrix<T>::operator()(int i, int j) 
{

  if ( _ml->count() > 1 ) _ml = _ml->clone();  // This may be bad ! 
  return (*_ml)(i,j); 

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T  TMatrix<T>::operator()(int i, int j) const 
{
  
  return (*_ml)(i,j);
 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T TMatrix<T>::getElement(int i, int j) const 
{
  return (*_ml)(i,j);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T& TMatrix<T>::operator()(int i) 
{

if ( _ml->count() > 1 ) _ml = _ml->clone();  // THIS IS BAD !
return (*_ml)(i);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TMatrix<T>::operator()(int i) const 
{
return _ml->operator()(i);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
std::ostream& operator<<(std::ostream& os, const TMatrix<T>& x)
{

   return os << *(x._ml); 

}


// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator+(const TMatrix<T>& x, const TMatrix<T>& y) 
{
  typename MLPtr<T>::Type p = add<T>(x._ml, y._ml); // add returns a new matrix
  return TMatrix<T>(p);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator+(const TMatrix<T>& x, const T& y) 
{
  typename MLPtr<T>::Type p = add<T>(x._ml, y); // add returns a new matrix
  return TMatrix<T>(p);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator+( const T& y, const TMatrix<T>& x )  // add returns a new matrix
{
  typename MLPtr<T>::Type p = add<T>(x._ml, y);
  return TMatrix<T>( p);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator-(const TMatrix<T>& x) 
{

  typename MLPtr<T>::Type p( Negate<T>(x._ml) ); // Note: Negate does a deepcopy   
  return TMatrix<T>(p);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator-(const TMatrix<T>& x, const TMatrix<T>& y) 
{
  typename MLPtr<T>::Type p( subtract<T>(x._ml, y._ml));
  return TMatrix<T>(p);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator-(const TMatrix<T>& x, const T& y) 
{
  typename MLPtr<T>::Type p(add<T>(x._ml, -y));
  return TMatrix<T>(p);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator-( const T& y, const TMatrix<T>& x ) 
{
  typename MLPtr<T>::Type p(  Negate<T>( add<T>(x._ml, -y) ) );
  return TMatrix<T>(p);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator*( const TMatrix<T>& x, const T& y) 
{
  typename MLPtr<T>::Type p(multiply<T>(x._ml, y));
  return TMatrix<T>(p);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator*( const T& y, const TMatrix<T>& x) 
{
  typename MLPtr<T>::Type p( multiply<T> (x._ml, y));
  return TMatrix<T>(p);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator*(const TMatrix<T>& x, const TMatrix<T>& y)  
{

  typename MLPtr<T>::Type p(multiply<T>(x._ml, y._ml));
  return TMatrix<T>( p);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator/( const TMatrix<T>& x, const T& y) 
{

  typename MLPtr<T>::Type p(divide<T>(x._ml, y) );
  return TMatrix<T>( p);

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator/( const T& x, TMatrix<T> const& y) 
{
  typename MLPtr<T>::Type p(divide<T>(x, y._ml) );
  return TMatrix<T>(p);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> operator/( TMatrix<T> const& x, TMatrix<T> const& y) 
{
  typename MLPtr<T>::Type p(divide<T>(x._ml, y._ml));
  return TMatrix<T>(p);

}


// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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


