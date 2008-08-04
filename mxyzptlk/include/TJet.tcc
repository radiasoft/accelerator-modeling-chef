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
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES 
******
****** Mar 2007 ostiguy@fnal.gov  
****** - Introduced new compact monomial indexing scheme based on monomial ordering
******   to replace previous scheme based explicitly on monomial exponents tuple.
****** - monomial multiplication now handled via a lookup-table.
****** - added STL compatible monomial term iterators   
******  
******  Mar 2008 ostiguy@fnal
******  - Jet composition and evaluation code refactored and optimized. 
****** Aug 2008 ostiguy@fnal.gov 
******* - proxy class-based coefficient access using [] syntax
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <fstream>

#include <basic_toolkit/utils.h> // misc utils: nexcom(), bcfRec(), nearestInteger() ...  
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/MathConstants.h>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/iterator/transform_iterator.hpp>


//-----------------------------------------------------------------
//      static variables
//-----------------------------------------------------------------

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
class TJet<T>::coeff_proxy {  

public: 

   coeff_proxy( JLPtr<T> jl, IntArray const& index ) : jl_(jl), index_(index) {}

   operator T const () const { 
           return jl_->getTermCoefficient( index_);
   }

   coeff_proxy& operator=( T const& value ) 
   { 
       jl_->setTermCoefficient(value, index_ );
       return *this;
   }

private:
   
    JLPtr<T>     jl_; 
    IntArray  index_;
};


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>::TJet( typename TJet<T>::jl_t const& jl) : jl_( jl) 
{}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>::TJet( EnvPtr<T> const& pje ) : jl_(   TJet<T>::tjl_t::makeTJL( pje ) ){}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>::TJet( T x, EnvPtr<T> const& pje ): jl_( TJet<T>::tjl_t::makeTJL( pje,x ) ){}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>::TJet( TJet<T> const& x )
: gms::FastAllocator(), jl_( x.jl_ ) 
{}
// NOTE: ref count is incremented when JLPtr is instantiated. 


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>::TJet( TJet<T> const& arg,  EnvPtr<T> const&  env)
 : gms::FastAllocator()
{
   //-------------------------------------------------------------
   // Conversion between two jets with different environments
   // Number of variables must be the same, but maximum order and/or 
   // reference points may be different.
   //--------------------------------------------------------------

   if ( arg.Env() == env ) { jl_ = arg.jl_; return; } 

   jl_ = tjl_t::makeTJL( env );
 
   jl_ = arg.jl_; // performs conversion if needed
   
   return;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
TJet<T>::~TJet() 
{}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::setEnvTo( const TJet& x )
{
 
  if( jl_->getEnv() != x.jl_->getEnv() ) 
  {
    if (jl_->count() > 1 ) jl_ = jl_->clone();  

    jl_->setEnv(x.jl_->getEnv());
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJet<T>::setEnvTo( EnvPtr<T> const& pje )
{
  if( jl_->getEnv() != pje ) 
  {
     if (jl_->count() > 1 )  jl_ = jl_->clone();
    jl_->setEnv(pje);
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

 jl_->getReference( r );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::setVariable(  int const& j, T const& x, EnvPtr<T> pje )
{
  if (jl_.count() > 1 ) jl_ = jl_->clone();
  jl_->setVariable( j, x, pje );  

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::setVariable( int const& j, T const& x )
{
   if (jl_.count() > 1 ) jl_ = jl_->clone();
   jl_->setVariable( j, x );   // DANGER !! Alters the environment!
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJet<T>::setVariable( int const& j, EnvPtr<T> pje ) 
{

    if (jl_.count() > 1 ) jl_ = jl_->clone();
    jl_->setVariable( j, pje);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::setVariable( int const& j )
{

 if (jl_.count() > 1 ) jl_ = jl_->clone();
 jl_->setVariable( j, jl_->getEnv() );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void  TJet<T>::setStandardPart( T const& std ) { 

  if (jl_.count() > 1 ) jl_ = jl_->clone();
  jl_->setStandardPart(std);     

} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator=( TJet const& x ) 
{

  if (&x == this) return *this;
  
  jl_ = x.jl_;
  return *this;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator=( T const& x ) 
{

 jl_ = jl_t(tjl_t:: makeTJL( jl_->getEnv(), x)); 
 return *this; 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::addTerm( TJLterm<T> const& a) 
{
  if (jl_.count() > 1 ) jl_ = jl_->clone();
  jl_->addTerm( TJLterm<T>(a) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::getTermCoefficient(IntArray const& exp) const
{
  return jl_->getTermCoefficient(exp);
} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::setTermCoefficient(T const& value, IntArray const& exp) 
{

  if (jl_.count() > 1 ) jl_ = jl_->clone();
  jl_->setTermCoefficient(value, exp);

} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0 
=========================================================================
FIXME
=========================================================================
template<typename T>
T operator[ IntArray const& ] const
{  
  return jl_->getTermCoefficient(exp);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T&      operator[]( IntArray const& exp)
{
  if (jl_.count() > 1 ) jl_ = jl_->clone();
  return jl_->setTermCoefficient( exp );

}   
=========================================================================
#endif
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
istream& operator>>( istream& is,  TJet<T>& x ) 
{
//  streams a TJet from into an existing instance

  x.jl_ = typename TJet<T>::jl_t( TJet<T>::tjl_t::makeTJL(x->getEnv()) );
  
 return operator>>( is, *(x.jl_) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
ostream& operator<<( ostream& os, TJet<T> const& x ) 
{
 return operator<<( os, *(x.jl_) );
}

// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//    Implementation of class Tcoord
//
//**************************************************************** 

template<typename T>
Tcoord<T>::Tcoord( T x ) : TJet<T>(0.0, EnvPtr<T>() ), refpt_(x) {
  
  // Note: passing a null env pointer to the TJet constructor
  //       results in a null JLPtr. Tcoord is not fully formed 
  //       until the instantiate() method is called. 
      
  TJetEnvironment<T>::coordinates_.push_back( this );
  
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void Tcoord<T>::instantiate( int index, EnvPtr<T> const& pje) {
 
 if (!pje)  { throw( GenericException( __FILE__, __LINE__, 
           "Tcoord<T>::instantiate( int index, const TJetEnvironment<T>* pje)",
           "Instantiating a coordinate requires a fully formed environment object." ) );
 }

 this->index_ = index;
 this->jl_    = typename TJet<T>::jl_t( TJet<T>::tjl_t::makeTJL( pje) );
 this->jl_->setVariable( index, refpt_);  

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
Tparam<T>::Tparam( T x ) : TJet<T>(0.0, EnvPtr<T>() ), refpt_(x) {
 
  // Note: passing a null env pointer to the TJet constructor
  //       results in a null JLPtr. Tcoord is not fully formed 
  //       until the instantiate() method is called. 
      
TJetEnvironment<T>::parameters_.push_back( this );


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

  this->index_ = index;
  this->jl_   = typename TJet<T>::jl_t( TJet<T>::tjl_t::makeTJL( pje) );
  this->jl_->setVariable( index, refpt_ );  
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
TJet<T>& TJet<T>::operator+=( TJet<T> const& rhs ) 
{
  if (jl_.count() > 1 ) jl_ = jl_->clone();
  jl_ += rhs.jl_; 

  //jl_ = jl_ + rhs.jl_; 

  return *this;
 }

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>& TJet<T>::operator-=( TJet<T> const& rhs ) 
{
 if (jl_.count() > 1 ) jl_ = jl_->clone();
 jl_ -= rhs.jl_ ;

 //jl_ = jl_ - rhs.jl_ ;

  return *this; 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator*=( TJet<T> const& y ) 
{

 if (jl_.count() > 1 ) jl_ = jl_->clone();
 jl_ = jl_ * y.jl_;
 return *this;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator/=( TJet<T> const& y ) 
{

// no in-place division yet ...
// use std operator instead

 if (jl_.count() > 1 ) jl_ = jl_->clone();
 jl_ = jl_ / (y.jl_);  
 return *this;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator/=( const T& y ) 
{
 if (jl_.count() > 1 ) jl_ = jl_->clone();
 jl_ *=  ( T(1.0)/ y );
 return *this;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
bool operator==( TJet<T> const& lhs, TJet<T> const& rhs ) 
{
 return *lhs.jl_ == *rhs.jl_;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
bool operator==( TJet<T> const& lhs, T const& rhs)
{
 return  *(lhs.jl_) == rhs;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
bool operator==( T const& lhs, TJet<T> const& rhs)
{
  return  *(rhs.jl_) == lhs;
}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T> 
bool operator!=( TJet<T> const& lhs, TJet<T> const& rhs ) 
{
 return !( lhs==rhs );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
bool operator!=( TJet<T> const& lhs, T const& rhs)
{
 return !( lhs==rhs );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
bool operator!=( T const& lhs, TJet<T> const& rhs)
{
 return !( rhs==lhs);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>& TJet<T>::operator+=( T const& x ) 
{   
 if (jl_.count() > 1 ) jl_ = jl_->clone();
 jl_+= x;

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator-=( T const& x ) 
{
 if (jl_.count() > 1 ) jl_ = jl_->clone();
 jl_ -=(x);

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator*=( T const& x ) 
{

 if (jl_.count() > 1 ) jl_ = jl_->clone();
 jl_ *= x; 
 return *this;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator+( TJet<T> const& x, TJet<T> const& y ) 
{ 

  return TJet<T>( x.jl_ + y.jl_);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator+( TJet<T> const& x, const T& y ) 
{

 return TJet<T>( x.jl_ + typename TJet<T>::jl_t( TJet<T>::tjl_t::makeTJL( x->getEnv(), y)) );    

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator+( const T& x, TJet<T> const& y ) 
{

 return  operator+(y,  x);

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator-( TJet<T> const& x, const T& y ) 
{
 return operator+(x, (-y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator-( const T& x, TJet<T> const& y ) 
{
 return operator+( (-y), x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
TJet<T> operator-( TJet<T> const& x) // Unary form of minus 
{
  typename TJet<T>::jl_t jl( (x.jl_)->clone() );
  return TJet<T>( -jl );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator-( TJet<T> const& x, TJet<T> const& y ) 
{  
  return ( x + (-y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( TJet<T> const& x, TJet<T> const& y ) 
{
 return TJet<T>( x.jl_ * y.jl_ );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( TJet<T> const& x, const T& y ) 
{
 return TJet<T>( x.jl_* y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( const T& x, TJet<T> const& y ) 
{  
 return (y*x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( TJet<T> const& x, int const& j ) 
{  
  return operator*( x, T(j) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( int const& j, TJet<T> const& x ) 
{ 
  return operator*( x, T(j));
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator/( TJet<T> const& x, const T& y ) 
{ 
  return operator*( x, (1.0/y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator/( TJet<T> const& x, int const& j ) 
{ 
 
  return ( operator*( x, 1.0/T(j) ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator/( const T& a, TJet<T> const& b ) 
{

 return TJet<T>( typename TJet<T>::jl_t( TJet<T>::tjl_t::makeTJL(b.jl_->getEnv(), a) )/ b.jl_  );    

} 


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T> operator/( TJet<T> const& wArg, TJet<T> const& uArg ) 
{ 

 return TJet<T>( wArg.jl_/  uArg.jl_ );


}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator^( TJet<T> const& x, TJet<T> const& y ) 
{ 

//--------------------------
// Poisson bracket operator
//--------------------------

 // Check for consistency 
 if( x.Env() != y.Env() ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> operator^( TJet<T> const&,  TJet<T> const& )",
          "Inconsistent environments." ) );
 }

 EnvPtr<T> theEnv(x.Env());
 TJet<T>   z (theEnv);

 IntArray m( theEnv->numVar() );
 IntArray n( theEnv->numVar() );

 if( theEnv->spaceDim()%2 != 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> operator^( TJet<T> const&, TJet<T> const& )",
          "Environment not correct for performing bracket." ) );
  }

   
 for( int i=0; i< theEnv->dof(); ++i) {

    m[ i ] = 1;
    n[ i + theEnv->dof() ] = 1;

 
    z += ( ( x.D(m) * y.D(n) ) - ( x.D(n) * y.D(m) ) );
 
    m[ i ] = 0;
    n[ i + theEnv->dof() ] = 0;
 }

 return z;

}

// **************************************************************
// **************************************************************
// **************************************************************
//
//      Overloaded analytic functions

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> acos( TJet<T> const& x ) 
{
  // Returns answer in (0,pi) if asin returns (-pi/2,pi/2).
  return ( T(M_PI_2) - asin(x) );
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> asin( TJet<T> const& x ) 
{ 
 return TJet<T>(x.jl_->asin() );
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> atan( TJet<T> const& x ) 
{   
 return TJet<T>(x.jl_->atan() );
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> cos( TJet<T> const& x ) 
{ 
 return TJet<T>( x.jl_->cos() ); // Note:: TJL<T>::cos() does not affect its argument 
                                 // and returns a cloned jl.   
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> cosh( TJet<T> const& x ) 
{ 
 TJet<T> z = exp(x);
 z += ( T(1.0) / z );
 z /= T (2.0);
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> exp( TJet<T> const& x ) 
{ 
 return  TJet<T>( x->exp() ); // x->exp() returns a new instance
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> log ( TJet<T> const& x ) 
{ 
  return   TJet<T>( x->log() ); // x->log() returns a new instance
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> log10( TJet<T> const& x ) 
{
 static const T log10E = log10(exp(1.0)); // 0.4342944819032518276511289;
 return  log10E*log(x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> pow( TJet<T> const& x, const double& s ) 
{ 
  return TJet<T>( x.jl_->pow( x.jl_, s) ); // pow(s) returns a copy 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T> pow( TJet<T> const& x, int n ) 
{ 
 return TJet<T>( x.jl_->pow( x.jl_, n) );  // pow(n) returns a cloned jl.   
}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> sin( TJet<T> const& x ) 
{ 
 return TJet<T>( x.jl_->sin() ); // Note:: TJL<T>::cos() does not affect its argument 
                                 // and returns a cloned jl.   
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> sinh( TJet<T> const& x ) 
{
 TJet<T> z = exp(x);
 z  -= T(1.0)/z;
 z  /= T(2.0);
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> sqrt( TJet<T> const& x ) 
{
  return TJet<T>( x.jl_->sqrt() ); // sqrt returns a copy 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> tan( TJet<T> const& x ) 
{ 
 return sin(x) / cos(x) ;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> tanh( TJet<T> const& x ) 
{ 
 return sinh(x) / cosh(x);
}
 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>  erfc( TJet<T> const& z ) 
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
typename TJet<T>::iterator        TJet<T>::begin()       
{  
   if (jl_.count() > 1) jl_ = jl_->clone();
   return       iterator(  jl_->begin() ); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJet<T>::const_iterator  TJet<T>::begin() const 
{ 
   return const_iterator( static_cast<TJL<T> const&>(*jl_).begin() ); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJet<T>::iterator        TJet<T>::end()         
{  
  if (jl_.count() > 1) jl_ = jl_->clone();
  return       iterator( jl_->end() );   
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJet<T>::const_iterator  TJet<T>::end() const { 

  return const_iterator(  static_cast<TJL<T> const&>(*jl_).end() );   

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJet<T>::reverse_iterator   TJet<T>::rbegin() 
{
  return boost::make_reverse_iterator( end() );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJet<T>::const_reverse_iterator  TJet<T>::rbegin() const
{
  return boost::make_reverse_iterator( end() );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJet<T>::reverse_iterator        TJet<T>::rend()
{
  return boost::make_reverse_iterator( begin() );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJet<T>::const_reverse_iterator  TJet<T>::rend()   const
{
  return boost::make_reverse_iterator( begin() );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::printCoeffs() const 
{
 jl_->printCoeffs();
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
 jl_->writeToFile( outStr );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::clear() 
{
 if (jl_.count() > 1) jl_ = jl_->clone();
 jl_->clear();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::weightedDerivative( IntArray const& ind ) const 
{
 return jl_->weightedDerivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::derivative( IntArray const& ind ) const 
{
 return jl_->derivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::filter( int const& wgtLo, int const& wgtHi ) const 
{ 
 return TJet<T>( jl_->filter(wgtLo,wgtHi) ); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::filter( bool (*f) ( IntArray const&, T const& ) ) const 
{ 
 return TJet<T>( jl_->filter(f) ); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>  TJet<T>::operator() ( TJetVector<T>          const& y) const // composition 
{
 typedef typename boost::transform_iterator< JetToJL, typename TJetVector<T>::const_iterator>  const_jl_iterator; 

 const_jl_iterator it_b(  y.begin(), (JetToJL()) );
 const_jl_iterator it_e(  y.end(),   (JetToJL()) );

 return  jl_->compose(it_b, it_e); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>  TJet<T>::operator() ( std::vector<TJet<T> >  const& y ) const // composition 
{

 typedef typename boost::transform_iterator< JetToJL, typename std::vector<TJet<T> >::const_iterator >  const_jl_iterator; 

 const_jl_iterator it_b(  y.begin(), (JetToJL()) );
 const_jl_iterator it_e(  y.end(),   (JetToJL()) );

 return jl_->compose(it_b, it_e); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::operator()( std::vector<T> const& y ) const 
{ 
 return  jl_->evaluate( y.begin(), y.end()); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::operator()( TVector<T> const& y ) const 
{ 
 return  jl_->evaluate( y.begin(), y.end() ); 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T> TJet<T>::D( IntArray const& n ) const 
{
 return TJet<T> ( jl_->D(n) ); 
} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
double sobolev_norm ( TJet<T> const& arg) 
{
  return arg.jl_->maxAbs();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
template<typename T>
TJet<T> TJet<T>::makeCoordinate( EnvPtr<T> env, int index )
{
  TJet<T> jet(env);
  jet.setVariable( index, env ); 
  return jet;
}   

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJet<T>::coeff_proxy const 
TJet<T>::operator[]( IntArray const& index ) const
{
   return coeff_proxy( this->jl_, index);
}   

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename TJet<T>::coeff_proxy 
TJet<T>::operator[]( IntArray const& index)
{
  return  coeff_proxy( this->jl_, index );   
}    

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

