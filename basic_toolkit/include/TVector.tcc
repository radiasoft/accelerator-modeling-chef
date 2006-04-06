/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TVector.tcc
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
******  Copyright  Universities Research Association, Inc.    
******             All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Revision History:
******
******  Dec 2005   Jean-Francois Ostiguy
******             ostiguy@fnal.gov
******
******  - templated version
******                                                                
**************************************************************************
*************************************************************************/
#ifndef TVECTOR_TCC
#define TVECTOR_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>   // for memcpy()
#include <VectorD.h>
#include <Matrix.h>

#include <iomanip>
 
using namespace std;


// ================================================================
//      Globals
//

template<typename T>
OutputFormat* TVector<T>::_defOFPtr = 0;

template<typename T>
void TVector<T>::setDefaultFormat( const OutputFormat& x )
{
  if( ( (OutputFormat*) 0 ) != TVector<T>::_defOFPtr ) {
    delete TVector<T>::_defOFPtr;
  }
  _defOFPtr = new OutputFormat( x );
}

// ================================================================
//      Exception handlers
//

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>::GenericException::GenericException( const char* fcn, const char* msg )
: w( msg )
{
  static bool firstTime = true;
  if( firstTime ) {
    cerr << "\n*** ERROR *** "
            "\n*** ERROR *** " << fcn
         << "\n*** ERROR *** " << msg
         << "\n*** ERROR *** This message is printed only once." 
         << endl;
    firstTime = false;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TVector<T>::GenericException::what() const throw()
{
  return strcat( "Vector::GenericException: ", w.c_str() );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


#define CHECKOUT(test,fcn,message)                       \
  if( test ) {                                           \
    throw( typename TVector<T>::GenericException( fcn, message ) ); \
  }


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// ================================================================
//      Constructors and the destructor ...
//

template<typename T>
TVector<T>::TVector( int n, const T* x, OutputFormat* fmtPtr )
{

  CHECKOUT(n <= 0, "TVector<T>::TVector", "Dimension must be positive.")

  _dim = n;
  _comp = new T[ _dim ];
  _loComp = _comp;
  _hiComp = &( _comp[_dim-1] );

  T* q; 
  const T* r = x;
  if(x) { for( q = _loComp; q <= _hiComp; q++,r++ ) { *q = *r;   } }
  else  { for( q = _loComp; q <= _hiComp; q++,r++ ) { *q =  T(); } }

  // REMOVE: if( x ) { for ( int i=0; i< _dim; ++i) { _comp[i] = x[i]; } }
  // REMOVE: else    { for ( int i=0; i< _dim; ++i) { _comp[i] = 0.0;  } }

  _ofPtr = fmtPtr;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
TVector<T>::TVector( const TVector& x )
{
  _dim = x._dim;
  _comp = new T[ _dim ];
  _loComp = _comp;
  _hiComp = &( _comp[_dim-1] );

  T* q; 
  T* r = x._comp;
  for( q = _loComp; q <= _hiComp; q++,r++ ) { *q = *r; }

  // REMOVE: for ( int i = 0; i < _dim; i++ ) _comp[i] = x._comp[i];

  _ofPtr = x._ofPtr;  // Note: shallow copy of output format
                      //       may or may not be a good thing.
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
void TVector<T>::reset()
{
  for( T* q = _loComp; q <= _hiComp; q++ ) { *q = T(); }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>::TVector::~TVector()
{
  delete [] _comp;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TVector<T>::Set( const T* x )
{
  for ( int i=0; i< _dim; ++i) _comp[i] = x[i];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void  TVector<T>::set( T x, T y, T z )
{
  if( _dim >= 3 ) {
    _comp[0] = x;
    _comp[1] = y;
    _comp[2] = z;
  }
  else for( int i = 0; i < _dim; i++ ) _comp[i] = 0.0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TVector<T>::operator() ( int i ) const
{
  if ( ( 0 <= i ) && ( i < _dim ) ) { return _comp[i]; }
  else {
    throw( GenericException( "double TVector::operator() ( int i ) const",
                             "Index out of range.") );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T& TVector<T>::operator() ( int i )
{
  if ( ( 0 <= i ) && ( i < _dim ) ) { return _comp[i]; }
  else {
    throw( GenericException( "double& Vector::operator() ( int i )",
                             "Index out of range.") );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Algebraic functions ...
template<typename T>
TVector<T>& TVector<T>::operator= ( const TVector& x )
{
  if( this != &x ) {
    #ifndef NOCHECKS
    CHECKOUT(_dim != x._dim, "TVector::operator=", "Incompatible dimensions.")
    #endif
    memcpy((void *)_comp, (const void *)x._comp, _dim * sizeof(T));
  }
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::operator+ ( const TVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TVector::operator+", "Incompatible dimensions.")
#endif

  TVector z( _dim );
  for ( int i = 0; i < _dim; ++i) 
    z._comp[i] = _comp[i] + x._comp[i];
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::operator+= ( const TVector& x )
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TVector::operator+=", "Incompatible dimensions.")
#endif

  for ( int i=0; i < _dim;  ++i) _comp[i] += x._comp[i];
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TVector<T> TVector<T>::operator- ( const TVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TVector::operator-", "Incompatible dimensions.")
#endif

  TVector z( _dim );
  for ( int i=0; i< _dim; ++i) 
    z._comp[i] = _comp[i] - x._comp[i];
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> operator- ( const TVector<T>& x )
{
  TVector<T> z( x._dim );
  for ( int i=0; i< x._dim; ++i) 
    z._comp[i] = - x._comp[i];
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::operator-= ( const TVector<T>& x )
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TVector<T>::operator-=", "Incompatible dimensions.")
#endif

  for ( int i = 0; i < _dim; i++ ) _comp[i] -= x._comp[i];
  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>  operator* ( const TVector<T>& x,  T c ) 
{
  TVector<T> z( x._dim );
  for ( int i = 0; i < x._dim;  ++i) z._comp[i] = c * x._comp[i];
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> operator* ( T c, const TVector<T>& x )
{
  TVector<T> z( x._dim );
  for ( int i = 0; i < x._dim; i++ ) z._comp[i] = c * x._comp[i];
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::operator*=( T c ) {
 for( int i=0; i < _dim; ++i) _comp[i] *= c;
 return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>  TVector<T>::operator/ ( T c ) const
{
 TVector z( _dim );
 for( int i = 0; i < _dim; ++i) z._comp[i] = _comp[i] / c;
 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::operator/=( T c ) {
 for( int i = 0; i < _dim; ++i) _comp[i] /= c;
 return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::operator^ ( const TVector<T>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( x._dim != 3 ),
           "TVector<T>::operator^",
           "_Dimension must be 3." )
#endif

  TVector<T> z( 3 );
  z._comp[ 0 ] = _comp[ 1 ] * x._comp[ 2 ] - _comp[ 2 ] * x._comp[ 1 ];
  z._comp[ 1 ] = _comp[ 2 ] * x._comp[ 0 ] - _comp[ 0 ] * x._comp[ 2 ];
  z._comp[ 2 ] = _comp[ 0 ] * x._comp[ 1 ] - _comp[ 1 ] * x._comp[ 0 ];
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// Boolean functions ...

template<typename T>
bool TVector<T>::operator== ( const TVector<T>& x ) const
{
  if( _dim != x._dim ) return 0;
  for( int i = 0; i < _dim; i++ ) 
    if( _comp[i] != x._comp[i] ) return 0;
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TVector<T>::operator!= ( const TVector<T>& x ) const
{
  return !( operator==( x ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TVector<T>::operator< ( const TVector<T>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TVector<T>::operator<", "Dimensions incompatible.")
#endif
  for( int i = 0; i < _dim; i++ ) if( _comp[i] >= x._comp[i] ) return false;
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TVector<T>::operator<= ( const TVector<T>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TVector<T>::operator<=", "Dimensions incompatible.")
#endif
  for( int i = 0; i < _dim; i++ ) if( _comp[i] > x._comp[i] ) return false;
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TVector<T>::operator>= ( const TVector<T>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TVector<T>::operator>=", "Dimensions incompatible.")
#endif
  for( int i = 0; i < _dim; i++ ) if( _comp[i] < x._comp[i] ) return false;
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TVector<T>::IsNull() const
{
  int i = -1;
  while ( ++i < _dim ) if( _comp[i] != 0.0 ) return false;
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// Utilities ..

template<typename T>
TVector<T> TVector<T>::Abs() const
{
  TVector<T> z( _dim );
  for( int i = 0; i < _dim; i++ ) z._comp[i] = std::abs( _comp[i] );
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::Unit () const
{
  double x = Norm();
  TVector<T> z( _dim );

  for ( int i = 0; i < _dim; ++i) z._comp[i] = _comp[i] / x;
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TVector<T>::Rotate ( TVector& v, double theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( v._dim != 3 ),
           "Vector::Rotate",
           "_Dimension must be 3." )
#endif

  TVector<T> e  = Unit();
  T c = cos( theta );
  T s = sin( theta );
  TVector<T> u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; ++i) v._comp[i] = u._comp[i];
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
ostream& operator<<( std::ostream & os, const TVector<T>& v ) 
{
  OutputFormat* q;
  q = v._ofPtr;
  if( ( (OutputFormat*) 0 ) == q  ) {
    q = TVector<T>::_defOFPtr;
  }

  if( ( (OutputFormat*) 0 ) == q  ) {
    os << "( ";
    for( int i = 0; i < v._dim - 1; i++ )
      os << v._comp[i] << ", ";
    os << v._comp[ v._dim - 1 ] << " )";
    return os;
  }

  else {
    os << "( ";
    for( int i = 0; i < v._dim - 1; i++ ) {
      os << setw(q->width) 
         << setprecision(q->precision) 
         << resetiosflags((std::_Ios_Fmtflags) (014002 | 04)) 
         << setiosflags((std::_Ios_Fmtflags) (q->flags)) 
//         << resetiosflags((014002 | 04)) 
//         << setiosflags((q->flags)) 
         << v._comp[i] << ", "
         << q->getPadding();
    }
    os << setw(q->width) 
       << setprecision(q->precision) 
       << resetiosflags((std::_Ios_Fmtflags) (014002 | 04)) 
       << setiosflags((std::_Ios_Fmtflags) (q->flags)) 
//       << resetiosflags((014002 | 04)) 
//       << setiosflags((q->flags)) 
       << v._comp[ v._dim - 1 ] << " )";

    return os;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// Utilities

template<typename T>
TVector<T> operator*( const TMatrix<T>& A, const TVector<T>& x )
{
  int r = A.rows();
  int c = A.cols();

  if( c != x.Dim() ) {
    throw( typename TVector<T>::GenericException( 
                        "Vector operator*( const TMatrix<T>& A, const TVector<T>& x )",
                        "Incompatible dimensions.") );
  }
  TVector<T> z( r );
  for( int i=0; i < r; ++i ) {
    z(i) = T();
    for( int j=0; j<c; ++j ) {
      z(i) += A( i, j ) * x(j);
    }
  }
  return z;
}


template<typename T>
TVector<T>& TVector<T>::operator=( const TMatrix<T>& x )
{
  T* u = _comp;

  if( 1 == x.rows() ) {
    if( _dim == x.cols() ) {
      for( int i = 0; i < _dim; i++ ) {
        *(u++) = x(0,i);
      }
      // A better way of doing this requires writing
      // a TMatrix iterator, which probably should be
      // done anyway.
    }
    else {
      throw( typename TVector<T>::GenericException( 
               "TVector& TVector::operator=( const TMatrix& x )", 
               "Incompatible dimensions."  ) );
    }
  }
  if( 1 == x.cols() ) {
    if( _dim == x.rows() ) {
      for( int i = 0; i < _dim; i++ ) {
        *(u++) = x(i,0);
      }
      // A better way of doing this requires writing
      // a TMatrix iterator, which probably should be
      // done anyway.
    }
    else {
      throw( typename TVector<T>::GenericException( 
               "TVector& TVector::operator=( const TMatrix& x )", 
               "Incompatible dimensions."  ) );
    }
  }

  return *this;
}


#endif // TVECTOR_TCC
