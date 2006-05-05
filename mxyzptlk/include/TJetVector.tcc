/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetVector.cc
******  Version:   1.0                    
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
******  Revision History
******   
******  Feb-May 2005   Jean-Francois Ostiguy
******                 ostiguy@fnal.gov 
****** 
****** - new memory management scheme 
******                                                            
******  Sep-Dec 2005  ostiguy@fnal.gov
******  
****** - refactored code to usea single class template parameter
******   instead of two. Mixed mode operations now handled using 
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - implementation details completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for implicit instantiations, define MXYZPTLK_IMPLICIT_TEMPLATES
******
******                                                                
**************************************************************************
*************************************************************************/

#ifndef TJETVECTOR_TCC
#define TJETVECTOR_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iosetup.h>
#include <GenericException.h>
#include <TJetVector.h>
#include <Jet.h>
#include <Matrix.h>


using FNAL::pcout;
using FNAL::pcerr;


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

template<typename T>
TJetVector<T>::TJetVector( int n, 
                              const TJet<T>* x, 
                              const typename EnvPtr<T>::Type pje )

{

  CHECKOUT(n <= 0, "TJetVector<T>::TJetVector<T>", "Dimension must be positive.")
  CHECKOUT(pje == 0, "TJetVector<T>::TJetVector<T>", "Environment is required.")

  if ( n < 0 ) { n = TJet<T>::lastEnvironment()->numVar(); }

  _myEnv = pje;
  _dim   = n;
  _comp  = new TJet<T>[ _dim ];

  if( x ) for ( int i = 0; i < _dim; i++ ) _comp[i] = x[i];
  else    for ( int i = 0; i < _dim; i++ ) {
     _comp[i].setEnvTo( pje );
     _comp[i] = T();    // ??? Unnecessary?
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T>::TJetVector( const TJetVector<T>& x )
{

  _myEnv = x._myEnv;
  _dim   = x._dim;

  _comp  = new TJet<T> [ _dim ];

  for ( int i = 0; i < _dim; i++ ) {

    _comp[i] = x._comp[i];           // Shallow copy is automatic because of lazy evaluation.
    CHECKOUT(  _comp[i].Env() != _myEnv , "TJetVector<T>::const TJetVector<T>& x", "Incompatible environments.")

  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T>::~TJetVector()
{
  delete [] _comp;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Assignment ...

template<typename T>
void TJetVector<T>::Set( const TJet<T>* x )
{
  typename EnvPtr<T>::Type pje;
  pje = x[0].Env();
#ifndef NOCHECKS
    CHECKOUT(pje != _myEnv, "TJetVector<T>::Set", "Wrong environment.")
#endif
  for ( int i = 0; i < 3; i++ ) { /// WHY is the no of variables assumed to be 3 ?  FIXME !!!
#ifndef NOCHECKS
    CHECKOUT(x[i].Env() != pje, "TJetVector<T>::Set", "Inequivalent environments.")
#endif
    _comp[i] = x[i];
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::SetComponent( int i, const TJet<T>& x )
{
#ifndef NOCHECKS
    CHECKOUT(x.Env() != _myEnv, "TJetVector<T>::Set", "Wrong environment.")
#endif
    _comp[i] = x;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJetVector<T>::operator() ( int i ) const
{
#ifndef NOCHECKS
    CHECKOUT(!(( 0 <= i ) && ( i < _dim )), "TJetVector<T>::operator()", "Argument out of range")
#endif
    return _comp[i];
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>& TJetVector<T>::operator() ( int i )
{
#ifndef NOCHECKS
    CHECKOUT(!(( 0 <= i ) && ( i < _dim )), "TJetVector<T>::operator()", "Argument out of range")
#endif
    return _comp[i];
}


// Algebraic functions ...

template<typename T>
TJetVector<T>& TJetVector<T>::operator= ( const TJetVector<T>& x )
{
  Reconstruct( x._dim, x._myEnv );

  for ( int i = 0; i < _dim; i++ ) _comp[i] = x._comp[i];
  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator+ ( const TJetVector<T>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TJetVector<T>::operator+", "Incompatible dimensions.")
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T>::operator+", "Incompatible environments.")
#endif

  TJetVector<T> z( _dim, 0, _myEnv );
  for ( int i = 0; i < _dim; i++ ) 
    z._comp[i] = _comp[i] + x._comp[i];
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJetVector<T> TJetVector<T>::operator+ ( const Vector& y ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != y.Dim(), "JetVector::operator+", "Incompatible dimensions.")
#endif

  TJetVector<T> z( *this );
  for ( int i = 0; i < _dim; ++i ) {
    z._comp[i] += y(i);
  }
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator+= ( const TJetVector<T>& x )
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TJetVector<T>::operator+=", "Incompatible dimensions.")
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T>::operator+=", "Incompatible environments.")
#endif

  for ( int i = 0; i < _dim; i++ ) _comp[i] += x._comp[i];
  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJetVector<T> TJetVector<T>::operator+= ( const Vector& x )
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x.Dim(), "JetVector::operator-=", "Incompatible dimensions.")
#endif

  for ( int i = 0; i < _dim; i++ ) {
    _comp[i] += x(i);
  }
  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator- ( const TJetVector<T>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TJetVector<T>::operator-", "Incompatible dimensions.")
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T>::operator-", "Incompatible environments.")
#endif

  TJetVector<T> z( _dim, 0, _myEnv );
  for ( int i = 0; i < _dim; i++ ) {
    z._comp[i] = _comp[i] - x._comp[i];
  }
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator- ( const Vector& y ) const
{
#ifndef NOCHECKS
  CHECKOUT(_dim != y.Dim(), "JetVector::operator-", "Incompatible dimensions.")
#endif

  TJetVector<T> z( *this );
  for ( int i = 0; i < _dim; i++ ) {
    z._comp[i] -= y(i);
  }
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> operator- ( const TJetVector<T>& x )
{
  TJetVector<T> z( x );
  for ( int i = 0; i < x._dim; i++ ) 
    z._comp[i].Negate();
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator-= ( const TJetVector<T>& x )
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x._dim, "TJetVector<T>::operator-=", "Incompatible dimensions.")
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T>::operator-=", "Incompatible environments.")
#endif

  for ( int i = 0; i < _dim; i++ ) 
    _comp[i] -= x._comp[i];
  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator-= ( const Vector& x )
{
#ifndef NOCHECKS
  CHECKOUT(_dim != x.Dim(), "JetVector::operator-=", "Incompatible dimensions.")
#endif

  for ( int i = 0; i < _dim; i++ ) {
    _comp[i] -= x(i);
  }
  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator* ( const TJet<T>& c ) const
{
  TJetVector<T> z( _dim, 0, _myEnv );
  for ( int i = 0; i < _dim; i++ ) z._comp[i] = c * _comp[i];
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator* ( const T& c ) const
{
  TJetVector<T> z( _dim, 0, _myEnv );
  for ( int i = 0; i < _dim; i++ ) z._comp[i] = c * _comp[i];
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> operator*( const TJet<T>& c, const Vector& x )
{
  int d = x.Dim();
  TJetVector<T> z( d, 0,  c.Env() );
  for ( int i = 0; i < d; i++ ) { z.SetComponent( i, c * x(i) ); }
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> operator* ( const TJet<T>& c, const TJetVector<T>& x )
{
  TJetVector<T> z( x._dim, 0, x._myEnv );
  for ( int i = 0; i < x._dim; i++ ) z._comp[i] = c * x._comp[i];
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> operator* ( const TJetVector<T>& x , const T& c )
{
  TJetVector<T> z( x._dim, 0, x._myEnv );
  for ( int i = 0; i < x._dim; i++ ) z._comp[i] = c * x._comp[i];
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> operator* ( const T& c, const TJetVector<T>& x )
{
  return operator*( x,c);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator*=( const TJet<T>& c ) 
{
 for( int i = 0; i < _dim; i++ ) _comp[i] *= c;
 return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator*=( T c ) 
{
 for( int i = 0; i < _dim; i++ ) _comp[i] *= c;
 return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator/ ( const TJet<T>& c ) const
{
 TJetVector<T> z( *this );  // ??? Can be speeded up by negate function.
 for( int i = 0; i < _dim; i++ ) z._comp[i] /= c;
 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator/ ( T c ) const
{
 TJetVector<T> z( *this );
 for( int i = 0; i < _dim; i++ ) z._comp[i] /= c;
 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator/=( const TJet<T>& c ) 
{
 for( int i = 0; i < _dim; i++ ) _comp[i] /= c;
 return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator/=( T c ) 
{
 for( int i = 0; i < _dim; i++ ) _comp[i] /= c;
 return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJetVector<T>::operator* ( const Vector& y ) const
{

  TJet<T> u( _myEnv );
  u = T(); // This should not be necessary. Why is it here?
  for ( int i = 0; i < _dim; i++ ) { u += _comp[i] * y(i); }
  return u;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator^ ( const TJetVector<T>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( x._dim != 3 ),
           "TJetVector<T>::operator^",
           "Dimension must be 3." )
#endif

  TJetVector<T> z( 3, 0, _myEnv );
  z._comp[ 0 ] = _comp[ 1 ] * x._comp[ 2 ] - _comp[ 2 ] * x._comp[ 1 ];
  z._comp[ 1 ] = _comp[ 2 ] * x._comp[ 0 ] - _comp[ 0 ] * x._comp[ 2 ];
  z._comp[ 2 ] = _comp[ 0 ] * x._comp[ 1 ] - _comp[ 1 ] * x._comp[ 0 ];
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator^ ( const Vector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( x.Dim() != 3 ),
           "TJetVector<T>::operator^",
           "Dimension must be 3." )
#endif

  TJetVector<T> z( 3, 0, _myEnv );
  z._comp[ 0 ] = _comp[ 1 ] * x( 2 ) - _comp[ 2 ] * x( 1 );
  z._comp[ 1 ] = _comp[ 2 ] * x( 0 ) - _comp[ 0 ] * x( 2 );
  z._comp[ 2 ] = _comp[ 0 ] * x( 1 ) - _comp[ 1 ] * x( 0 );
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> operator^ ( const Vector& y, const TJetVector<T>& x )
{
#ifndef NOCHECKS
  CHECKOUT(( y.Dim() != 3 ) || ( x._dim != 3 ),
           "TJetVector<T>::operator^",
           "Dimension must be 3." )
#endif

  TJetVector<T> z( 3, 0, x.Env() );
  z._comp[ 0 ] = y( 1 ) * x._comp[ 2 ] - y( 2 ) * x._comp[ 1 ];
  z._comp[ 1 ] = y( 2 ) * x._comp[ 0 ] - y( 0 ) * x._comp[ 2 ];
  z._comp[ 2 ] = y( 0 ) * x._comp[ 1 ] - y( 1 ) * x._comp[ 0 ];
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJetVector<T> operator*(  const TMatrix<T>& M, 
                              const TJetVector<T>& x ) 
{
 const typename EnvPtr<T>::Type pje(x._myEnv);
 TJetVector<T> z( M.rows(), 0, pje );
 int i, j, r, c;

 i = j = 0;
 r = M.rows();
 c = M.cols();

 if( M.cols() != x._dim ) {
  throw( GenericException( __FILE__, __LINE__, 
         "TJetVector<T> operator*(  const TMatrix<T>&, const TJetVector<T>& )",
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

template<typename T>
bool TJetVector<T>::operator== ( const TJetVector<T>& x ) const
{
  if( ( _dim != x._dim ) || _myEnv != x._myEnv ) return 0;
  for( int i = 0; i < _dim; i++ ) 
    if( _comp[i] != x._comp[i] ) return 0;
  return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator==( const T& x ) const 
{
 for( int i = 0; i < _dim; i++ )
   if( _comp[i] != x ) return 0;     // ??? WHAT???
 return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator!= ( const TJetVector<T>& x ) const
{
  return !( operator==( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator!=( const T& x ) const 
{
 return !( this->operator==( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator< ( const TJetVector<T>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T>::operator<", "Incompatible environments.")
  CHECKOUT(_dim != x._dim, "TJetVector<T>::operator<", "Incompatible dimensions.")
#endif

  for( int i=0; i < _dim; ++i ) 
    if( _comp[i].standardPart()  >= x._comp[i].standardPart() ) return false;
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator<= ( const TJetVector<T>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(_myEnv != x._myEnv, "TJetVector<T>::operator<", "Incompatible environments.")
  CHECKOUT(_dim != x._dim, "TJetVector<T>::operator<", "Incompatible dimensions.")
#endif

  for( int i = 0; i < _dim; ++i ) 
    if( _comp[i].standardPart() > x._comp[i].standardPart() ) return false;
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator> ( const TJetVector<T>& x ) const
{
  return !( operator<=( x ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator>= ( const TJetVector<T>& x ) const
{
  return !( operator<( x ) );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::IsNull() const
{
  int i = -1;
  while ( ++i < _dim ) if( _comp[i] != T() ) return 0;
  return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::IsUnit() const
{
  T x = T();
  for( int i = 0; i < _dim; i++ ) 
    x += _comp[i].standardPart()*_comp[i].standardPart();
  return x == 1.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::IsNilpotent() const 
{
 for( int i = 0; i <  _myEnv->spaceDim(); i++ ) 
  if( !(_comp[i].isNilpotent()) ) return 0;
 return 1;
}



// Utilities ..
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::Reconstruct()
{
  delete [] _comp;
  _comp = new TJet<T> [ _dim ];
  for ( int i = 0; i < _dim; i++ ) {
    _comp[i].setEnvTo( _myEnv );
    _comp[i].Reconstruct();
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::Reconstruct( int n, typename EnvPtr<T>::Type pje )
{
  delete [] _comp;
  _dim = n;
  _myEnv = pje;
  _comp = new TJet<T> [ _dim ];
  for ( int i = 0; i < _dim; i++ ) {
    _comp[i].Reconstruct( pje );
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::CopyFrom( const TJetVector& x )
{
  this->Reconstruct( x.Dim(), x.Env() );
  this->operator=( x );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::peekAt() const
{
  (*pcout) << "\n\nBegin TJetVector<T>::peekAt() ......\n";
  for ( int i = 0; i < _dim; i++ ) {
   (*pcout) << "TJetVector<T>::peekAt(): Component " << i << endl;
    _comp[i].peekAt();
  }
  (*pcout) << "End TJetVector<T>::peekAt() ......\n" << endl;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJetVector<T>::Norm () const
{
  TJet<T> x( _myEnv );
  x = T();
  for ( int i = 0; i < _dim; i++ ) x += _comp[i]*_comp[i];
  return sqrt(x);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::Unit () const
{
  TJet<T> x( _myEnv );
  x = Norm();
  TJetVector<T> z( *this );
  for ( int i = 0; i < _dim; i++ ) z._comp[i] /= x;
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
ostream& operator<<( ostream& os, const TJetVector<T>& v )
{
  os << "Begin TJetVector<T>:" << endl;
  os << v._dim << endl;
  os << *(v._myEnv);
  for( int i = 0; i < v._dim; i++ ) {
    os << "\nTJetVector<T> component " << i << ":" << endl;
    os << v._comp[i];
  }
  os << "\nEnd TJetVector<T>." << endl;
  return os;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
istream& operator>>( istream& is, TJetVector<T>& v )
{
  static char buf[100];
  is >> buf;
  is >> buf;
  is >> v._dim;
  CHECKOUT(v._dim <= 0, "TJetVector<T>::TJetVector<T>", "Dimension must be positive.")

  streamIn( is, v._myEnv );

  if( v._comp ) delete [] v._comp;
  v._comp = new TJet<T> [ v._dim ];

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

template<typename T>
TJetVector<T> TJetVector<T>::filter( bool (*f[]) ( const IntArray&, const T& ) ) const
{
 TJetVector<T> z( _dim, 0, _myEnv );

 for( int i=0; i<_dim;  ++i )
  z._comp[i] = _comp[i].filter( f[i] );

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::filter( int lower, int upper ) const
{
 TJetVector<T> z( _dim, 0, _myEnv );

 for( int i=0; i < _dim; ++i )
  z._comp[i] = _comp[i].filter( lower, upper );

 return z;
}



// Operations related to differentiation |||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::weightedDerivative( int* m, T* x ) 
{
 int i;
 for( i = 0; i < _dim; i++ ) x[i] = _comp[i].weightedDerivative( m );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::derivative( int* m, T* x ) 
{
 int i;
 for( i = 0; i < _dim; i++ ) x[i] = _comp[i].derivative( m );
}



// Query functions |||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
int TJetVector<T>::AccuWgt() const
{
  int accuWgt = _myEnv->maxWeight();
  for( int i = 0; i < _dim; i++ ) {
   if( _myEnv != _comp[i].Env() ) {
     (*pcerr) << "\n\n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** TJetVector<T>::AccuWgt()             \n"
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

template<typename T>
int TJetVector<T>::Weight()  const
{
  int weight = -1;
  for( int i = 0; i < _dim; i++ ) {
   if( _myEnv != _comp[i].Env() ) {
     (*pcerr) << "\n\n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** TJetVector<T>::Weight()               \n"
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

template<typename T>
void TJetVector<T>::standardPart( T* x ) const 
{
 int i;
 for( i = 0; i < _dim; i++ ) 
   x[i] = _comp[i].standardPart();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::getReference( T* r ) const 
{
 int i;
 for( i = 0; i < _myEnv->numVar(); i++ ) r[i] = _myEnv->getRefPoint()[i];
}


#endif // TJETVECTOR_TCC

