/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetVector.cc
******  Version:   1.0                    
******                                                                
******  Copyright (c) 1990, 2004 Universities Research Association, Inc.    
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

#ifndef TJETVECTOR_TCC
#define TJETVECTOR_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>

#include "GenericException.h"
#include "TJetVector.h"
#include "TMatrix.h"

// ================================================================
//      Global variables
//

using namespace std;

#define CHECKOUT(test,fcn,message)    \
  if( test ) {                        \
    throw( GenericException(          \
             __FILE__, __LINE__,      \
             fcn, message        ) ); \
  }

// ================================================================
//      Constructors and the destructor ...
//

template<typename T1, typename T2>
TJetVector<T1,T2>::TJetVector<T1,T2>( int n, 
                        const TJet<T1,T2>* x, 
                        const TJetEnvironment<T1,T2>* pje )
{
  int i;

  CHECKOUT(n <= 0, "TJetVector<T1,T2>::TJetVector<T1,T2>", "Dimension must be positive.")
  CHECKOUT(pje == 0, "TJetVector<T1,T2>::TJetVector<T1,T2>", "Environment is required.")

  _myEnv = (TJetEnvironment<T1,T2>*) pje;
  _dim = n;
  _comp = new TJet<T1,T2> [ _dim ];

  if( x ) for ( i = 0; i < _dim; i++ ) _comp[i] = x[i];
  else    for ( i = 0; i < _dim; i++ ) {
    _comp[i].setEnvTo( (TJetEnvironment<T1,T2>*) pje );
    _comp[i] = ((T1) 0.0);    // ??? Unnecessary?
  }
}

template<typename T1, typename T2>
TJetVector<T1,T2>::TJetVector<T1,T2>( const TJetVector<T1,T2>& x )
{
  static int i;

  _myEnv = x._myEnv;
  _dim = x._dim;
  _comp = new TJet<T1,T2> [ _dim ];

  for ( i = 0; i < _dim; i++ ) {
    _comp[i] = x._comp[i];    // Shallow copy is automatic because
                            // of lazy evaluation.
    #ifndef NOCHECKS
    if( _comp[i].Env() != _myEnv ) {
      throw( TJL<T1,T2>::BadEnvironment( 
               i, (int) _comp[i].Env(), (int) _myEnv, 
               __FILE__, __LINE__, 
               "TJetVector<T1,T2>::TJetVector<T1,T2>( const TJetVector<T1,T2>& )",
               "Inconsistent environment at a component." ));
    }
    #endif
  }
}

template<typename T1, typename T2>
TJetVector<T1,T2>::~TJetVector<T1,T2>()
{
  delete [] _comp;
}


// Assignment ...

template<typename T1, typename T2>
void TJetVector<T1,T2>::Set( const TJet<T1,T2>* x )
{
  TJetEnvironment<T1,T2>* pje;
  pje = x[0].Env();
#ifndef NOCHECKS
    CHECKOUT(pje != _myEnv, "TJetVector<T1,T2>::Set", "Wrong environment.")
#endif
  for ( int i = 0; i < 3; i++ ) {
#ifndef NOCHECKS
    CHECKOUT(x[i].Env() != pje, "TJetVector<T1,T2>::Set", "Inequivalent environments.")
#endif
    _comp[i] = x[i];
  }
}


template<typename T1, typename T2>
void TJetVector<T1,T2>::SetComponent( int i, const TJet<T1,T2>& x )
{
#ifndef NOCHECKS
    CHECKOUT(x.Env() != _myEnv, "TJetVector<T1,T2>::Set", "Wrong environment.")
#endif
    _comp[i] = x;
}


template<typename T1, typename T2>
TJet<T1,T2> TJetVector<T1,T2>::operator() ( int i ) const
{
#ifndef NOCHECKS
    CHECKOUT(!(( 0 <= i ) && ( i < _dim )), "TJetVector<T1,T2>::operator()", "Argument out of range")
#endif
    return _comp[i];
}


template<typename T1, typename T2>
TJet<T1,T2>& TJetVector<T1,T2>::operator() ( int i )
{
#ifndef NOCHECKS
    CHECKOUT(!(( 0 <= i ) && ( i < _dim )), "TJetVector<T1,T2>::operator()", "Argument out of range")
#endif
    return _comp[i];
}


// Algebraic functions ...

template<typename T1, typename T2>
TJetVector<T1,T2>& TJetVector<T1,T2>::operator= ( const TJetVector<T1,T2>& x )
{
  Reconstruct( x._dim, x._myEnv );

  for ( int i = 0; i < _dim; i++ ) _comp[i] = x._comp[i];
  return *this;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator+ ( const TJetVector<T1,T2>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TJetVector<T1,T2>::operator+", "Incompatible dimensions.")
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T1,T2>::operator+", "Incompatible environments.")
#endif

  TJetVector<T1,T2> z( _dim, 0, _myEnv );
  for ( int i = 0; i < _dim; i++ ) 
    z._comp[i] = _comp[i] + x._comp[i];
  return z;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator+ ( const Vector& y ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != y.Dim(), "JetVector::operator+", "Incompatible dimensions.")
#endif

  TJetVector<T1,T2> z( *this );
  for ( int i = 0; i < _dim; i++ ) {
    z._comp[i] += y(i);
  }
  return z;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator+= ( const TJetVector<T1,T2>& x )
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TJetVector<T1,T2>::operator+=", "Incompatible dimensions.")
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T1,T2>::operator+=", "Incompatible environments.")
#endif

  for ( int i = 0; i < _dim; i++ ) _comp[i] += x._comp[i];
  return *this;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator+= ( const Vector& x )
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.Dim(), "JetVector::operator-=", "Incompatible dimensions.")
#endif

  for ( int i = 0; i < _dim; i++ ) {
    _comp[i] += x(i);
  }
  return *this;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator- ( const TJetVector<T1,T2>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TJetVector<T1,T2>::operator-", "Incompatible dimensions.")
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T1,T2>::operator-", "Incompatible environments.")
#endif

  TJetVector<T1,T2> z( _dim, 0, _myEnv );
  for ( int i = 0; i < _dim; i++ ) {
    z._comp[i] = _comp[i] - x._comp[i];
  }
  return z;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator- ( const Vector& y ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != y.Dim(), "JetVector::operator-", "Incompatible dimensions.")
#endif

  TJetVector<T1,T2> z( *this );
  for ( int i = 0; i < _dim; i++ ) {
    z._comp[i] -= y(i);
  }
  return z;
}


template<typename T1, typename T2>
TJetVector<T1,T2> operator- ( const TJetVector<T1,T2>& x )
{
  TJetVector<T1,T2> z( x );
  for ( int i = 0; i < x._dim; i++ ) 
    z._comp[i].Negate();
  return z;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator-= ( const TJetVector<T1,T2>& x )
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TJetVector<T1,T2>::operator-=", "Incompatible dimensions.")
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T1,T2>::operator-=", "Incompatible environments.")
#endif

  for ( int i = 0; i < _dim; i++ ) 
    _comp[i] -= x._comp[i];
  return *this;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator-= ( const Vector& x )
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x.Dim(), "JetVector::operator-=", "Incompatible dimensions.")
#endif

  for ( int i = 0; i < _dim; i++ ) {
    _comp[i] -= x(i);
  }
  return *this;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator* ( const TJet<T1,T2>& c ) const
{
  TJetVector<T1,T2> z( _dim, 0, _myEnv );
  for ( int i = 0; i < _dim; i++ ) z._comp[i] = c * _comp[i];
  return z;
}

template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator* ( const T1& c ) const
{
  TJetVector<T1,T2> z( _dim, 0, _myEnv );
  for ( int i = 0; i < _dim; i++ ) z._comp[i] = c * _comp[i];
  return z;
}



template<typename T1, typename T2>
TJetVector<T1,T2> operator*( const TJet<T1,T2>& c, const Vector& x )
{
  int d = x.Dim();
  TJetVector<T1,T2> z( d, 0, ((TJetEnvironment<T1,T2>*) c.Env()) );
  for ( int i = 0; i < d; i++ ) { z.SetComponent( i, c * x(i) ); }
  return z;
}






template<typename T1, typename T2>
TJetVector<T1,T2> operator* ( const TJet<T1,T2>& c, const TJetVector<T1,T2>& x )
{
  TJetVector<T1,T2> z( x._dim, 0, x._myEnv );
  for ( int i = 0; i < x._dim; i++ ) z._comp[i] = c * x._comp[i];
  return z;
}

template<typename T1, typename T2>
TJetVector<T1,T2> operator* ( const T1& c, const TJetVector<T1,T2>& x )
{
  TJetVector<T1,T2> z( x._dim, 0, x._myEnv );
  for ( int i = 0; i < x._dim; i++ ) z._comp[i] = c * x._comp[i];
  return z;
}

template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator*=( const TJet<T1,T2>& c ) 
{
 for( int i = 0; i < _dim; i++ ) _comp[i] *= c;
 return *this;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator*=( T1 c ) 
{
 for( int i = 0; i < _dim; i++ ) _comp[i] *= c;
 return *this;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator/ ( const TJet<T1,T2>& c ) const
{
 TJetVector<T1,T2> z( *this );  // ??? Can be speeded up by negate function.
 for( int i = 0; i < _dim; i++ ) z._comp[i] /= c;
 return z;
}

template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator/ ( T1 c ) const
{
 TJetVector<T1,T2> z( *this );
 for( int i = 0; i < _dim; i++ ) z._comp[i] /= c;
 return z;
}

template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator/=( const TJet<T1,T2>& c ) 
{
 for( int i = 0; i < _dim; i++ ) _comp[i] /= c;
 return *this;
}

template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator/=( T1 c ) 
{
 for( int i = 0; i < _dim; i++ ) _comp[i] /= c;
 return *this;
}


template<typename T1, typename T2>
TJet<T1,T2> TJetVector<T1,T2>::operator* ( const TJetVector<T1,T2>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T1,T2>::operator*", "Incompatible environments.")
  CHECKOUT(_dim != x._dim, "TJetVector<T1,T2>::operator*", "Incompatible dimensions.")
#endif

  TJet<T1,T2> u( _myEnv );
  u = ((T1) 0.0);
  int i;
  for ( i = 0; i < _dim; i++ ) u += _comp[i] * x._comp[i];
  return u;
}


template<typename T1, typename T2>
TJet<T1,T2> TJetVector<T1,T2>::operator* ( const Vector& y ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != y.Dim(), "JetVector::operator*", "Incompatible dimensions.")
#endif

  TJet<T1,T2> u( _myEnv );
  u = 0.0; // This should not be necessary. Why is it here?
  for ( int i = 0; i < _dim; i++ ) { u += _comp[i] * y(i); }
  return u;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator^ ( const TJetVector<T1,T2>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( x._dim != 3 ),
           "TJetVector<T1,T2>::operator^",
           "Dimension must be 3." )
#endif

  TJetVector<T1,T2> z( 3, 0, _myEnv );
  z._comp[ 0 ] = _comp[ 1 ] * x._comp[ 2 ] - _comp[ 2 ] * x._comp[ 1 ];
  z._comp[ 1 ] = _comp[ 2 ] * x._comp[ 0 ] - _comp[ 0 ] * x._comp[ 2 ];
  z._comp[ 2 ] = _comp[ 0 ] * x._comp[ 1 ] - _comp[ 1 ] * x._comp[ 0 ];
  return z;
}


template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::operator^ ( const Vector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( x.Dim() != 3 ),
           "TJetVector<T1,T2>::operator^",
           "Dimension must be 3." )
#endif

  TJetVector<T1,T2> z( 3, 0, _myEnv );
  z._comp[ 0 ] = _comp[ 1 ] * x( 2 ) - _comp[ 2 ] * x( 1 );
  z._comp[ 1 ] = _comp[ 2 ] * x( 0 ) - _comp[ 0 ] * x( 2 );
  z._comp[ 2 ] = _comp[ 0 ] * x( 1 ) - _comp[ 1 ] * x( 0 );
  return z;
}

template<typename T1, typename T2>
TJetVector<T1,T2> operator^ ( const Vector& y, const TJetVector<T1,T2>& x )
{
#ifndef NOCHECKS
  CHECKOUT(( y.Dim() != 3 ) || ( x._dim != 3 ),
           "TJetVector<T1,T2>::operator^",
           "Dimension must be 3." )
#endif

  TJetVector<T1,T2> z( 3, 0, x.Env() );
  z._comp[ 0 ] = y( 1 ) * x._comp[ 2 ] - y( 2 ) * x._comp[ 1 ];
  z._comp[ 1 ] = y( 2 ) * x._comp[ 0 ] - y( 0 ) * x._comp[ 2 ];
  z._comp[ 2 ] = y( 0 ) * x._comp[ 1 ] - y( 1 ) * x._comp[ 0 ];
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJetVector<T1,T2> operator*(  const TMatrix<T1>& M, 
                              const TJetVector<T1,T2>& x ) 
{
 const TJetEnvironment<T1,T2>* pje = x._myEnv;
 TJetVector<T1,T2> z( M.rows(), 0, pje );
 int i, j, r, c;

 i = j = 0;
 r = M.rows();
 c = M.cols();

 if( M.cols() != x._dim ) {
  throw( GenericException( __FILE__, __LINE__, 
         "TJetVector<T1,T2> operator*(  const TMatrix<T1>&, const TJetVector<T1,T2>& )",
         "Rows and/or columns of the matrix are not correct." ) );
 }

 for( i = 0; i < r; i++ ) {
  z._comp[i] = M( i, 0 ) * x._comp[0];
  j = 1;
  while( j < c )   { 
    z._comp[i] += M( i, j ) * ( x._comp[j++] ) ;
  }
 }

 return z;
}


// Boolean functions |||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool TJetVector<T1,T2>::operator== ( const TJetVector<T1,T2>& x ) const
{
  if( ( _dim != x._dim ) || _myEnv != x._myEnv ) return 0;
  for( int i = 0; i < _dim; i++ ) 
    if( _comp[i] != x._comp[i] ) return 0;
  return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool TJetVector<T1,T2>::operator==( const T1& x ) const 
{
 for( int i = 0; i < _dim; i++ )
   if( _comp[i] != x ) return 0;     // ??? WHAT???
 return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool TJetVector<T1,T2>::operator!= ( const TJetVector<T1,T2>& x ) const
{
  return !( operator==( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool TJetVector<T1,T2>::operator!=( const T1& x ) const 
{
 return !( this->operator==( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool TJetVector<T1,T2>::operator< ( const TJetVector<T1,T2>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T1,T2>::operator<", "Incompatible environments.")
  CHECKOUT(_dim != x._dim, "TJetVector<T1,T2>::operator<", "Incompatible dimensions.")
#endif

  for( int i = 0; i < _dim; i++ ) 
    if( _comp[i].standardPart() >= x._comp[i].standardPart() ) return 0;
  return 1;
}

template<typename T1, typename T2>
bool TJetVector<T1,T2>::operator<= ( const TJetVector<T1,T2>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T1,T2>::operator<", "Incompatible environments.")
  CHECKOUT(_dim != x._dim, "TJetVector<T1,T2>::operator<", "Incompatible dimensions.")
#endif

  for( int i = 0; i < _dim; i++ ) 
    if( _comp[i].standardPart() > x._comp[i].standardPart() ) return 0;
  return 1;
}

template<typename T1, typename T2>
bool TJetVector<T1,T2>::operator> ( const TJetVector<T1,T2>& x ) const
{
  return !( operator<=( x ) );
}

template<typename T1, typename T2>
bool TJetVector<T1,T2>::operator>= ( const TJetVector<T1,T2>& x ) const
{
  return !( operator<( x ) );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool TJetVector<T1,T2>::IsNull() const
{
  int i = -1;
  while ( ++i < _dim ) if( _comp[i] != ((T1) 0.0) ) return 0;
  return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool TJetVector<T1,T2>::IsUnit() const
{
  T1 x = ((T1) 0.0);
  for( int i = 0; i < _dim; i++ ) 
    x += _comp[i].standardPart()*_comp[i].standardPart();
  return x == 1.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool TJetVector<T1,T2>::IsNilpotent() const 
{
 for( int i = 0; i <  _myEnv->_spaceDim; i++ ) 
  if( !(_comp[i].isNilpotent()) ) return 0;
 return 1;
}



// Utilities ..
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJetVector<T1,T2>::Reconstruct()
{
  delete [] _comp;
  _comp = new TJet<T1,T2> [ _dim ];
  for ( int i = 0; i < _dim; i++ ) {
    _comp[i].setEnvTo( _myEnv );
    _comp[i].Reconstruct();
  }
}


template<typename T1, typename T2>
void TJetVector<T1,T2>::Reconstruct( int n, TJetEnvironment<T1,T2>* pje )
{
  delete [] _comp;
  _dim = n;
  _myEnv = pje;
  _comp = new TJet<T1,T2> [ _dim ];
  for ( int i = 0; i < _dim; i++ ) {
    _comp[i].Reconstruct( pje );
  }
}


template<typename T1, typename T2>
void TJetVector<T1,T2>::CopyFrom( const TJetVector& x )
{
  this->Reconstruct( x.Dim(), ((TJetEnvironment<T1,T2>*) x.Env()) );
  this->operator=( x );
}


template<typename T1, typename T2>
void TJetVector<T1,T2>::peekAt() const
{
  cout << "\n\nBegin TJetVector<T1,T2>::peekAt() ......\n";
  for ( int i = 0; i < _dim; i++ ) {
    cout << "TJetVector<T1,T2>::peekAt(): Component " << i << endl;
    _comp[i].peekAt();
  }
  cout << "End TJetVector<T1,T2>::peekAt() ......\n" << endl;
}


template<typename T1, typename T2>
TJet<T1,T2> TJetVector<T1,T2>::Norm () const
{
  TJet<T1,T2> x( _myEnv );
  x = ((T1) 0.0);
  for ( int i = 0; i < _dim; i++ ) x += _comp[i]*_comp[i];
  return sqrt(x);
}

template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::Unit () const
{
  TJet<T1,T2> x( _myEnv );
  x = Norm();
  TJetVector<T1,T2> z( *this );
  for ( int i = 0; i < _dim; i++ ) z._comp[i] /= x;
  return z;
}


template<typename T1, typename T2>
ostream& operator<<( ostream& os, const TJetVector<T1,T2>& v )
{
  os << "Begin TJetVector<T1,T2>:" << endl;
  os << v._dim << endl;
  os << *(v._myEnv);
  for( int i = 0; i < v._dim; i++ ) {
    os << "\nTJetVector<T1,T2> component " << i << ":" << endl;
    os << v._comp[i];
  }
  os << "\nEnd TJetVector<T1,T2>." << endl;
  return os;
}


template<typename T1, typename T2>
istream& operator>>( istream& is, TJetVector<T1,T2>& v )
{
  static char buf[100];
  is >> buf;
  is >> buf;
  is >> v._dim;
  CHECKOUT(v._dim <= 0, "TJetVector<T1,T2>::TJetVector<T1,T2>", "Dimension must be positive.")

  streamIn( is, &(v._myEnv) );

  if( v._comp ) delete [] v._comp;
  v._comp = new TJet<T1,T2> [ v._dim ];

  for ( int i = 0; i < v._dim; i++ ) {
    is >> buf;
    is >> buf;
    is >> buf;
    is >> v._comp[i];
  }

  is >> buf;
  is >> buf;

  return is;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::filter( bool (*f[]) ( const IntArray&, const T1& ) )
{
 TJetVector<T1,T2> z( _dim, 0, _myEnv );
 int i;

 for( i = 0; i < _dim; i++ )
  z._comp[i] = _comp[i].filter( f[i] );

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJetVector<T1,T2> TJetVector<T1,T2>::filter( int lower, int upper ) 
{
 TJetVector<T1,T2> z( _dim, 0, _myEnv );
 int i;        

 for( i = 0; i < _dim; i++ )
  z._comp[i] = _comp[i].filter( lower, upper );

 return z;
}



// Operations related to differentiation |||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJetVector<T1,T2>::weightedDerivative( int* m, T1* x ) 
{
 int i;
 for( i = 0; i < _dim; i++ ) x[i] = _comp[i].weightedDerivative( m );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJetVector<T1,T2>::derivative( int* m, T1* x ) 
{
 int i;
 for( i = 0; i < _dim; i++ ) x[i] = _comp[i].derivative( m );
}



// Query functions |||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
int TJetVector<T1,T2>::AccuWgt() const
{
  int accuWgt = _myEnv->_maxWeight;
  for( int i = 0; i < _dim; i++ ) {
   if( _myEnv != _comp[i].Env() ) {
     cerr << "\n\n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** TJetVector<T1,T2>::AccuWgt()             \n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** Inconsistent environments at      \n"
 	  << "*** WARNING *** index " << i << "                 \n"
 	  << "*** WARNING ***                                   \n"
 	  << endl;
   }
   if( accuWgt > _comp[i].getAccuWgt() ) accuWgt = _comp[i].getAccuWgt();
  }
  return accuWgt;
}

template<typename T1, typename T2>
int TJetVector<T1,T2>::Weight()  const
{
  int weight = -1;
  for( int i = 0; i < _dim; i++ ) {
   if( _myEnv != _comp[i].Env() ) {
     cerr << "\n\n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** TJetVector<T1,T2>::Weight()               \n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** Inconsistent environments at      \n"
 	  << "*** WARNING *** index " << i << "                 \n"
 	  << "*** WARNING ***                                   \n"
 	  << endl;
   }
   if( weight  < _comp[i].getWeight()  ) weight  = _comp[i].getWeight();
  }
  return weight;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJetVector<T1,T2>::standardPart( T1* x ) const 
{
 int i;
 for( i = 0; i < _dim; i++ ) 
   x[i] = _comp[i].standardPart();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJetVector<T1,T2>::getReference( T1* r ) const 
{
 int i;
 for( i = 0; i < _myEnv->_numVar; i++ ) r[i] = _myEnv->_refPoint[i];
}


#endif // TJETVECTOR_TCC
