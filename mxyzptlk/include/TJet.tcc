/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJet.tcc
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab    
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
******  Revision History:
******
******  Feb 2005 - Jean-Francois Ostiguy
******             ostiguy@fnal.gov
******
****** - Efficiency improvements.
****** - new memory management scheme 
****** 
******  Sep-Dec 2005  ostiguy@fnal.gov
******
****** - refactored code to use a single class template parameter
******   instead of two. Mixed mode operations now handled using 
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - centralized TJetEnvironment management
****** - all implementation details now completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for implicit instantiations, define MXYZPTLK_IMPLICIT_TEMPLATES 
******  
**************************************************************************
*************************************************************************/
#ifndef TJET_TCC
#define TJET_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <fstream>

#include <utils.h> // misc utils: nexcom(), bcfRec(), nearestInteger() ...  
#include <iosetup.h>
#include <GenericException.h>
#include <MathConstants.h>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_array.hpp>


//-----------------------------------------------------------------
//      static variables
//-----------------------------------------------------------------

template<typename T> 
EnvPtr<T> TJet<T>::_lastEnv; // defaults to a null pointer


using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//            Implementation of the class TJet<T>
// 
//

template<typename T>
TJet<T>::TJet( typename TJet<T>::jl_t const& jl) : _jl( jl) 
{}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>::TJet( EnvPtr<T> const& pje ) :  _jl(   tjl_t::makeTJL( pje ) ){}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>::TJet( T x, EnvPtr<T> const& pje ): _jl(  tjl_t::makeTJL( pje,x ) ){}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>::TJet( TJet<T> const& x )
: _jl( x._jl )  // NOTE: ref count is incremented when JLPtr is instantiated. 
{}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
TJet<T>::~TJet() 
{}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
void TJet<T>::Reconstruct()
{
 // Combines destructor and constructor functions.
 // Use when initializing a static TJet variable.

 _jl = jl_t( tjl_t::makeTJL( _jl->getEnv() ) );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJet<T>::Reconstruct( EnvPtr<T> const& pje )
{
 // Combines destructor and constructor functions.
 // Use when initializing a static TJet variable.

 _jl = jl_t( tjl_t::makeTJL( pje ) );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJet<T>::Reconstruct( const IntArray& e, 
                               const T& x, 
                               EnvPtr<T> const& pje )
{
 // Combines destructor and constructor functions.
 // Use when initializing a static TJet variable.

 _jl = jl_t( tjl_t::makeTJL( e, x, pje ) ); 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJet<T>::setEnvTo( const TJet& x )
{
 
  if( _jl->getEnv() != x._jl->getEnv() ) 
  {
    if (_jl->count() > 1 ) _jl = _jl->clone();  // do not clone unless the ref count > 1  

    _jl->setEnv(x._jl->getEnv());
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJet<T>::setEnvTo( EnvPtr<T> const& pje )
{
  if( _jl->getEnv() != pje ) 
  {
     if (_jl->count() > 1 )  _jl = _jl->clone();
    _jl->setEnv(pje);
  }
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>                                  
void TJet<T>::getReference( T* r ) const
{
// get the reference point. The caller supplies 
// the vector r which is is expected to 
// have dimension numVar. 

 _jl->getReference( r );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::setVariable( const T& x,
                               const int& j, 
                               EnvPtr<T> const& pje )
{
  _jl = _jl->clone();
  _jl->setVariable( x, j, pje );  // !! Alters the environment!

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::setVariable( const T& x,
                               const int& j )
{
   _jl->setVariable( x, j);   // DANGER !! Alters the environment!
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJet<T>::setVariable( const int& j, EnvPtr<T> const& pje ) 
{

    _jl = _jl->clone();;
    _jl->setVariable( j, pje );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::setVariable( const int& j )
{

 _jl = _jl->clone();
 _jl->setVariable( j, _jl->getEnv() );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator=( const TJet& x ) 
{
  
  _jl = x._jl;
  return *this;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::DeepCopy( const TJet& x ) 
{

 if( this != &x ) {
   _jl = (x._jl)->clone();
}

 return *this; 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>& TJet<T>::operator=( const T& x ) 
{

 _jl = jl_t(tjl_t:: makeTJL( _jl->getEnv(), x)); 
 return *this; 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJet<T>::addTerm( const TJLterm<T>& a) 
{
 _jl = _jl->clone();
 _jl->addTerm( TJLterm<T>(a) );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
istream& operator>>( istream& is,  TJet<T>& x ) 
{
//  streams a TJet from into an existing instance

  x._jl = typename TJet<T>::jl_t( TJet<T>::tjl_t::makeTJL(x->getEnv()) );
  
 return operator>>( is, *(x._jl) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
ostream& operator<<( ostream& os, const TJet<T>& x ) 
{
 return operator<<( os, *(x._jl) );
}

// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//    Implementation of class Tcoord
//
//**************************************************************** 

template<typename T>
Tcoord<T>::Tcoord( T x ) : TJet<T>(0.0, EnvPtr<T>() ), _refpt(x) {
  
  // Note: passing a null env pointer to the TJet constructor
  //       results in a null JLPtr. Tcoord is not fully formed 
  //       until the instantiate() method is called. 
      
  TJetEnvironment<T>::_coordinates.push_back( this );
  
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void Tcoord<T>::instantiate( int index, EnvPtr<T> const& pje) {
 
 if (!pje)  { throw( GenericException( __FILE__, __LINE__, 
           "Tcoord<T>::instantiate( int index, const TJetEnvironment<T>* pje)",
           "Instantiating a coordinate requires a fully formed environment object." ) );
 }

 this->_index = index;
 this->_jl    = typename TJet<T>::jl_t( TJet<T>::tjl_t::makeTJL( pje) );
 this->_jl->setVariable( _refpt, index );  

} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
Tcoord<T>::~Tcoord() 
{ }

// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//    Implementation of class Tparam
//
// ***************************************************************
 
template<typename T>
Tparam<T>::Tparam( T x ) : TJet<T>(0.0, EnvPtr<T>() ), _refpt(x) {
 
  // Note: passing a null env pointer to the TJet constructor
  //       results in a null JLPtr. Tcoord is not fully formed 
  //       until the instantiate() method is called. 
      
TJetEnvironment<T>::_parameters.push_back( this );


}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
Tparam<T>::~Tparam() 
{  }  


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void Tparam<T>::instantiate( int index, EnvPtr<T> const& pje) {

 if (!pje)  { 
   throw( GenericException( __FILE__, __LINE__, 
           "Tcoord<T>::instantiate( int index, const TJetEnvironment<T>* pje)",
           "Instantiating a coordinate requires a fully formed environment object." ) );
  }

  this->_index = index;
  this->_jl   = typename TJet<T>::jl_t( TJet<T>::tjl_t::makeTJL( pje) );
  this->_jl->setVariable( _refpt, index );  
} 

//
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// 
// **************************************************************
// **************************************************************
// **************************************************************
//
//      The overloaded operators for class TJet<T>
//

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator+=( const TJet<T>& y ) 
{

  _jl = _jl->clone();
  _jl += y._jl; 
  return *this;
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>& TJet<T>::operator-=( const TJet<T>& y ) 
{

    _jl = _jl->clone();
    _jl +=( -y._jl );
    return *this; 


}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator*=( const TJet<T>& y ) 
{

 _jl = _jl->clone();
 _jl = _jl * y._jl;
 return *this;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator/=( const TJet<T>& y ) 
{

// no in-place division yet ...
// use std operator instead
 _jl = _jl->clone();
 _jl = _jl / (y._jl);  
 return *this;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator/=( const T& y ) 
{
  _jl = _jl->clone();
  _jl->scaleBy( ((T) 1.0)/ y);
  return *this;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
template<typename T> 
bool operator==( const TJet<T>& x, const TJet<T>& y ) 
{

 return *(x._jl) == *(y._jl);

}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
bool operator==( const TJet<T>& x, const T& y)
{

 return *(x._jl) == y;

}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
bool operator==( const T& x, const TJet<T>& y)
{

 return *(y._jl) == x;

}
#if 0 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
bool TJet<T>::operator==( const TJet<T>& y ) const
{
 return *(this->_jl) == *(y._jl);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJet<T>::operator==( const T& y ) const
{
 return *(this->_jl) == y;
}

#endif
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
bool operator!=( const TJet<T>& x, const TJet<T>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TJet<T>& x, const T& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const T& x, const TJet<T>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator+=( const T& x ) 
{   

  _jl  = _jl->clone();
  _jl->operator+=(x);

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator-=( const T& x ) 
{

 _jl  = _jl->clone();
 _jl->operator+=(-x);
 return *this;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator*=( const T& x ) 
{

 _jl = _jl->clone();
 _jl->scaleBy(x); 
 return *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator+( const TJet<T>& x, const TJet<T>& y ) 
{ 

  return TJet<T>( x._jl + y._jl);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator+( const TJet<T>& x, const T& y ) 
{

 return TJet<T>( x._jl + typename TJet<T>::jl_t( TJet<T>::tjl_t::makeTJL( x->getEnv(), y)) );    

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator+( const T& x, const TJet<T>& y ) 
{

 return  operator+(y,  x);

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator-( const TJet<T>& x, const T& y ) 
{
 return operator+(x, (-y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator-( const T& x, const TJet<T>& y ) 
{
 return operator+( (-y), x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
TJet<T> operator-( const TJet<T>& x) // Unary form of minus 
{
  typename TJet<T>::jl_t jl ( (x._jl)->clone() ); // deep copy
  jl->Negate();
  return TJet<T>( jl );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::Negate()    // ??? What is this for ???
{
  _jl = _jl->clone();
  _jl->Negate();

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::Mult( const T& x ) // ??? What is this for ???
{

 _jl = _jl->clone();
 _jl->scaleBy(x);

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator-( const TJet<T>& x, const TJet<T>& y ) 
{  
  return ( x + (-y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( const TJet<T>& x, const TJet<T>& y ) 
{

 return TJet<T>( x._jl * y._jl );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( const TJet<T>& x, const T& y ) 
{


 return TJet<T>( x._jl* y );


}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( const T& x, const TJet<T>& y ) 
{  
 
 return operator*( y, x);

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( const TJet<T>& x, const int& j ) 
{  
  return operator*( x, T(j) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( const int& j, const TJet<T>& x ) 
{ 
  return operator*( x, T(j));
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator/( const TJet<T>& x, const T& y ) 
{ 
  return operator*( x, (1.0/y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator/( const TJet<T>& x, const int& j ) 
{ 
 
  return ( operator*( x, 1.0/T(j) ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator/( const T& a, const TJet<T>& b ) 
{

 return TJet<T>( typename TJet<T>::jl_t( TJet<T>::tjl_t::makeTJL(b._jl->getEnv(), a) )/ b._jl  );    

} 


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T> operator/( const TJet<T>& wArg, const TJet<T>& uArg ) 
{ 

 return TJet<T>( wArg._jl/  uArg._jl );


}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator^( const TJet<T>& x, const TJet<T>& y ) 
{ 

//--------------------------
// Poisson bracket operator
//--------------------------

 // Check for consistency 
 if( x->getEnv() != y->getEnv() ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> operator^( const TJet<T>&, const TJet<T>& )",
          "Inconsistent environments." ) );
 }

 TJetEnvironment<T>* theEnv = x->getEnv();
 TJet<T> z (theEnv);

 int*  m = new int [ theEnv->numVar() ];
 int*  n = new int [ theEnv->numVar() ];

if( theEnv->_pbok ) {
 
   for( int i = 0; i < theEnv->numVar(); i++ ) {
     m[i] = 0;
     n[i] = 0;
   }
 
   for( int i = 0; i < theEnv->dof(); i++ ) {
     m[ i ] = 1;
     n[ i + theEnv->dof() ] = 1;

 
     z += ( ( x.D(m) * y.D(n) ) - ( x.D(n) * y.D(m) ) );
 
     m[ i ] = 0;
     n[ i + theEnv->dof() ] = 0;
   }
 
   delete [] m;
   delete [] n;
   return z;
 }
 else {
   delete [] m;
   delete [] n;
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> operator^( const TJet<T>&, const TJet<T>& )",
          "Environment not correct for performing bracket." ) );
 }
}

// **************************************************************
// **************************************************************
// **************************************************************
//
//      Overloaded analytic functions

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> acos( const TJet<T>& x ) 
{
  // Returns answer in (0,pi) if asin returns (-pi/2,pi/2).
  return ( ((T) M_PI_2) - asin(x) );
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> asin( const TJet<T>& x ) 
{ 

 return TJet<T>(x._jl->asin() );

}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> atan( const TJet<T>& x ) 
{   
 
 return TJet<T>(x._jl->atan() );

}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> cos( const TJet<T>& x ) 
{ 
 return TJet<T>( x._jl->cos() ); // Note:: TJL<T>::cos() does not affect its argument 
                                 // and returns a cloned jl.   
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> cosh( const TJet<T>& x ) 
{ 
 TJet<T> z = exp(x);
 z = ( z + ( 1.0 / z ) ) / 2.0;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> exp( const TJet<T>& x ) 
{ 
 
 return   TJet<T>( x->exp() ); // x->exp() returns a new instance

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> log ( const TJet<T>& x ) 
{ 

  return   TJet<T>( x->log() ); // x->log() returns a new instance

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> log10( const TJet<T>& x ) 
{
 static const T logE = 0.4342944819032518276511289;
 return  logE*log(x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> pow( const TJet<T>& x, const double& s ) 
{ 

 return TJet<T>( x->pow(s)); // pow(s) creates a new JL

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T> pow( const TJet<T>& x, int n ) 
{ 

 return TJet<T>( x->pow(n)); // pow(h) creates a new JL

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> sin( const TJet<T>& x ) 
{ 
  return TJet<T> ( x._jl->sin() ); // Note:: TJL<T>::sin() clones its argument 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> sinh( const TJet<T>& x ) 
{
 TJet<T> z;
 z = exp(x);
 return ( z - (1.0/z))/2.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> sqrt( const TJet<T>& x ) 
{
  return TJet<T>( x._jl->sqrt() ); // sqrt returns a copy 
  
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> tan( const TJet<T>& x ) 
{ 
 return sin(x) / cos(x) ;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> tanh( const TJet<T>& x ) 
{ 
 return sinh(x) / cosh(x);
}
 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>  erfc( const TJet<T>& z ) 
{
  return ( ((T) 1.0) - erf( z ) );
}



// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************************
// **************************************************************
// **************************************************************
//
//      Implementation of Class TJet<T>

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::peekAt() const 
{
_jl->peekAt( );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::printCoeffs() const 
{

 _jl->printCoeffs();

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::writeToFile( char* fileName ) const 
{
 ofstream outStr( fileName );
 this->writeToFile( outStr );
 outStr.close();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::writeToFile( ofstream& outStr ) const 
{
 _jl->writeToFile( outStr );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::scaleBy( T y ) 
{
 _jl = _jl->clone(); 
 _jl->scaleBy( y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>* TJet<T>::get() 
{
 _jl = _jl->clone();
 return _jl->get();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJet<T>::firstTerm() const 
{
 return _jl->firstTerm();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJet<T>::lowTerm() const 
{
 return _jl->lowTerm();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::standardPart() const 
{
 return _jl->standardPart();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::clear() 
{
 _jl = _jl->clone(); 
 _jl->clear();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::weightedDerivative( const int* ind ) const 
{
 return _jl->weightedDerivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::derivative( const int* ind ) const 
{
 return _jl->derivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::filter( const int& wgtLo, const int& wgtHi ) const 
{ 

 return TJet<T>( _jl->filter(wgtLo,wgtHi) ); 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::filter( bool (*f) ( const IntArray&, const T& ) ) const 
{ 

 return TJet<T>( _jl->filter(f) ); 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::operator() ( const TJetVector<T>& y ) const 
{
 
 int n = _jl->getEnv()->numVar();   

 TJet<T> z;      // ??? so as to be self-contained.

 TJet<T>* u =  new TJet<T> [ n ];

 for( int i = 0; i < n; i++ ) u[i] = y(i);
 
 z = operator()( u );

 delete[] u;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::operator() ( const TJet<T>* y ) const 
{ 

 jl_t yjl[ _jl->getEnv()->numVar() ];

 for(int i=0; i< _jl->getEnv()->numVar(); ++i) {

    yjl[i] = y[i]._jl;
 }

 return TJet<T>( _jl->compose( yjl ) ); 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::operator() ( const Vector& x ) const 
{
 return _jl->operator()( x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::operator() ( const T* x ) const 
{
 return _jl->operator()( x );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::D( const int* n ) const 
{

 return TJet<T> ( _jl->D(n) ); 

} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::D( const IntArray& n ) const 
{

  int narray[ n.Dim()];
  for (int i=0; i<n.Dim(); ++i) narray[i] = n(i); // copy the IntArray. 
                                                  // this needs to be done because the 
                                                  // internal representation of the array
                                                  // involve shorter ints    
  return TJet<T>( _jl->D(narray) );  

}

// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// ****** These functions need to be eliminated. 
//
// Because terms might have been added in between iteations 
// _jl ptr is modified, a deep copy needs to be performed first.
// This make the process of iterating **extremely** inefficient. 

template<typename T> 
const TJLterm<T>* TJet<T>::stepConstIteratorPtr() const
{
  if (_jl->count() > 1) _jl =  _jl->clone();
  return _jl->stepConstIteratorPtr(); 
}   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
template<typename T>
void  TJet<T>::resetConstIterator()
{
  if (_jl->count() > 1)_jl =  _jl->clone();
  _jl->resetConstIterator();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#if 0

template<typename T>
const TJLterm<T>& TJet<T>::stepConstIteratorRef() const
{   
  _jl =  _jl->clone();
  return _jl->stepConstIteratorRef();
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::resetIterator()
{
   _jl =  _jl->clone();
   _jl->resetIterator();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
template<typename T>
TJLterm<T>*  TJet<T>::stepIterator()
{
  _jl = _jl->clone();
  return _jl->stepIterator();

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  

#endif
#endif // TJET_TCC
