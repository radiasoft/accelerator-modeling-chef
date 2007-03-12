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
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES
******
******  Dec 2006 ostiguy@fnal.gov    
****** 
******  - New implementation. TJetVector is now based on a vector<Jet>
******    container. The previous version was based on a dynamically allocated 
******    raw array of Jet*. Since a Jet is now basically an envelope for a 
******    smart ptr to implementation, its mem footprint is negligible. 
******    This fact enables the use of value semantics for
******    the stored Jet components and vastly simplifies memory management.       
******  
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef TJETVECTOR_TCC
#define TJETVECTOR_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/Matrix.h>



using FNAL::pcout;
using FNAL::pcerr;
using namespace std;


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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T>::TJetVector( EnvPtr<T> const& pje )
: myEnv_(pje)
{
  
  if (!pje) return; // empty vector is OK.
  comp_.resize( pje->numVar(), TJet<T>(T(), pje) ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T>::TJetVector( int n, EnvPtr<T> const& pje )
: myEnv_(pje)
{

  comp_.resize( n, TJet<T>(T(), pje) ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T>::TJetVector( TVector<T> const& x , EnvPtr<T>  const& env)
:  myEnv_(env)

{
 
  comp_.resize(x.Dim()); 

  int i=0;
  for ( typename vector<TJet<T> >::iterator it = comp_.begin(); it != comp_.end(); ++it, ++i) {
     
           *it = x[i]; 
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T>::TJetVector( TJetVector<T> const& x )
: myEnv_(x.myEnv_), comp_(x.comp_) { }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T>::TJetVector( TJetVector<T> const& x,  int i1, int i2 ): myEnv_(x.myEnv_)  
{

 comp_ = std::vector<TJet<T> >( x.comp_.begin()+ i1,  x.comp_.begin() + i2 ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T>::~TJetVector(){ }


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Assignment ...


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::SetComponent( int i, TJet<T> const& x )
{

#ifndef NOCHECKS
CHECKOUT( (x.Env()  != myEnv_) , "TJetVector<T>::Set", "Wrong environment.")
CHECKOUT( ( !x.Env() )         , "TJetVector<T>::Set", "Null environment.")
#endif

    comp_[i] = x;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJetVector<T>::operator() ( int i ) const
{
    return comp_[i];
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T>& TJetVector<T>::operator() ( int i )
{
#ifndef NOCHECKS
    CHECKOUT(!(( 0 <= i ) && ( i < comp_.size() )), "TJetVector<T>::operator()", "Argument out of range")
#endif
    return comp_[i];
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Algebraic functions ...

template<typename T>
TJetVector<T>& TJetVector<T>::operator= ( TJetVector<T> const& x ) {

  if  ( &x == this)  return *this;

  myEnv_ =  x.myEnv_;
  comp_  =  x.comp_;

  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator+ ( TJetVector<T> const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(comp_.size() != x.comp_.size(), "TJetVector<T>::operator+", "Incompatible dimensions.")
  CHECKOUT(myEnv_ != x.myEnv_, "TJetVector<T>::operator+", "Incompatible environments.")
#endif

  TJetVector<T> z(*this);

  typename vector<TJet<T> >::const_iterator  itx  = x.comp_.begin(); 

  for ( typename vector<TJet<T> >::iterator  itz = z.comp_.begin(); itz != z.comp_.end(); ++itz, ++itx) {
    (*itz) +=  (*itx);
  }

  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJetVector<T> TJetVector<T>::operator+ ( Vector const& y ) const
{
#ifndef NOCHECKS
  CHECKOUT(comp_.size() != y.Dim(), "TJetVector<T>::operator+", "Incompatible dimensions.")
#endif

  TJetVector<T> z( *this );

  int i = 0;
  for ( typename vector<TJet<T> >::iterator  itz = z.comp_.begin(); itz != z.comp_.end(); ++itz, ++i) {
    (*itz) += y(i);
  }

  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator+= ( TJetVector<T> const& x )
{
#ifndef NOCHECKS
  CHECKOUT(comp_.size() != x.comp_.size(), "TJetVector<T>::operator+=", "Incompatible dimensions.")
  CHECKOUT(myEnv_ != x.myEnv_, "TJetVector<T>::operator+=", "Incompatible environments.")
#endif

  typename vector<TJet<T> >::const_iterator  itx = x.comp_.begin(); 

  for ( typename vector<TJet<T> >::iterator  it = comp_.begin(); it != comp_.end(); ++it, ++itx) {
    (*it) += (*itx);
  }

  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJetVector<T> TJetVector<T>::operator+= ( Vector const& x )
{
#ifndef NOCHECKS
  CHECKOUT(comp_.size() != x.Dim(), "TJetVector<T>::operator+=(const Vector& x)", "Incompatible dimensions.")
#endif

  int i=0 ;
  for ( typename vector<TJet<T> >::iterator  it = comp_.begin(); it != comp_.end(); ++it, ++i) {
    (*it) += x(i);
  }

  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator- ( TJetVector<T> const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(comp_.size()   != x.comp_.size(),   "TJetVector<T>::operator-", "Incompatible dimensions.")
  CHECKOUT(myEnv_ != x.myEnv_, "TJetVector<T>::operator-", "Incompatible environments.")
#endif


  TJetVector<T> z(*this);

  typename vector<TJet<T> >::const_iterator  itx = x.comp_.begin(); 

  for ( typename vector<TJet<T> >::iterator  itz = z.comp_.begin(); itz != z.comp_.end(); ++itz, ++itx) {
    (*itz) -= (*itx);
  }

  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator- ( Vector const& y ) const
{
#ifndef NOCHECKS
  CHECKOUT(comp_.size() != y.Dim(), "TJetVector::operator-(const Vector& y)", "Incompatible dimensions.")
#endif

  TJetVector<T> z( *this );

  int i=0 ;
  for ( typename vector<TJet<T> >::iterator  itz = z.comp_.begin(); itz != z.comp_.end(); ++itz, ++i) {
    (*itz) -= y(i);
  }

  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> operator- ( TJetVector<T> const& x ) // unary minus
{
  TJetVector<T> z( x );

  for ( typename vector<TJet<T> >::iterator  itz = z.comp_.begin(); itz != z.comp_.end(); ++itz) {
    itz->Negate();
  }

  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator-= ( TJetVector<T> const& x )
{
#ifndef NOCHECKS
  CHECKOUT(comp_.size() != x.comp_.size(), "TJetVector<T>::operator-=", "Incompatible dimensions.")
  CHECKOUT(myEnv_ != x.myEnv_, "TJetVector<T>::operator-=", "Incompatible environments.")
#endif


  typename vector<TJet<T> >::const_iterator  itx = x.comp_.begin(); 

  for ( typename vector<TJet<T> >::iterator  it = comp_.begin(); it != comp_.end(); ++it, ++itx) {
    (*it) -= (*itx);
  }

  return *this;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator-= ( Vector const& x )
{
#ifndef NOCHECKS
  CHECKOUT(comp_.size() != x.Dim(), "TJetVector::operator-=( const Vector& x )", "Incompatible dimensions.")
#endif

  int i =0;
  for ( typename vector<TJet<T> >::iterator  it = comp_.begin(); it != comp_.end(); ++it, ++i) {
    (*it) -= x(i);
  }

  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator* ( TJet<T> const& x ) const
{
  TJetVector<T> z( *this );

  for ( typename vector<TJet<T> >::iterator  itz = z.comp_.begin(); itz != z.comp_.end(); ++itz) {
    (*itz) *=  x;
  }

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator* ( T  const& c ) const
{
  TJetVector<T> z( *this );

  for ( typename vector<TJet<T> >::iterator  itz = z.comp_.begin(); itz != z.comp_.end(); ++itz) {
    (*itz) *=  c;
  }

  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> operator*(  TJet<T> const& c, Vector const& x )
{

  TJetVector<T> z( x.Dim(), c.Env());

  for ( int i=0;  i < x.Dim(); ++i ) {
     z[i] *= x[i];
  }  

  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJetVector<T> operator* (  TJet<T> const& c,  TJetVector<T> const& x )
{

  TJetVector<T> z( x );

  for ( typename vector<TJet<T> >::iterator  itz = z.comp_.begin(); itz != z.comp_.end(); ++itz) {
    (*itz) *= c;
  }

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T, typename U>
TJetVector<T> operator* ( TJetVector<T> const& x , U const& c )
{
  TJetVector<T> z( x );

  for ( typename vector<TJet<T> >::iterator  itz = z.comp_.begin(); itz != z.comp_.end(); ++itz) {
    (*itz) *= c;
  }

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T,  typename U>
TJetVector<T> operator* ( U const& c, TJetVector<T> const& x )
{
  return operator*( x,c);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator*=( TJet<T> const& c ) 
{

  for ( typename vector<TJet<T> >::iterator  it = comp_.begin(); it != comp_.end(); ++it) {
    (*it) *= c;
  }

 return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator*=( T c ) 
{

 for ( typename vector<TJet<T> >::iterator  it = comp_.begin(); it != comp_.end(); ++it) {
    (*it) *= c;
 }

 return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator/ ( TJet<T> const& c ) const
{

 TJetVector<T> z( *this );  

 for ( typename vector<TJet<T> >::iterator  itz = z.comp_.begin(); itz != z.comp_.end(); ++itz) {
    (*itz) /= c;
 }

 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator/ ( T c ) const
{
 TJetVector<T> z( *this );

 for ( typename vector<TJet<T> >::iterator  itz = z.comp_.begin(); itz != z.comp_.end(); ++itz) {
    (*itz) /= c;
 }

 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator/=( TJet<T> const& c ) 
{

 for ( typename vector<TJet<T> >::iterator  it = comp_.begin(); it != comp_.end(); ++it) {
    (*it) /= c;
 }

 return *this;


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator/=( T c ) 
{

 for ( typename vector<TJet<T> >::iterator  it = comp_.begin(); it != comp_.end(); ++it) {
    (*it) /= c;
 }

 return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJetVector<T>::operator* ( Vector const& y ) const
{

  TJet<T> u( myEnv_);

  int i=0;
  for ( typename vector<TJet<T> >::const_iterator  it = comp_.begin(); it != comp_.end(); ++it, ++i) {
    u += ((*it) * y(i)); 
  }
  return u;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator^ ( const TJetVector<T>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT((comp_.size() != 3) || ( x.comp_.size() != 3 ),
           "TJetVector<T>::operator^",
           "Dimension must be 3." )
#endif

  TJetVector<T> z( myEnv_ );

  z.comp_.push_back( comp_[ 1 ] * x.comp_[ 2 ] - comp_[ 2 ] * x.comp_[ 1 ] );
  z.comp_.push_back( comp_[ 2 ] * x.comp_[ 0 ] - comp_[ 0 ] * x.comp_[ 2 ] );
  z.comp_.push_back( comp_[ 0 ] * x.comp_[ 1 ] - comp_[ 1 ] * x.comp_[ 0 ]);

  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::operator^ ( Vector const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT((comp_.size() != 3) || ( x.Dim() != 3 ),
           "TJetVector<T>::operator^",
           "Dimension must be 3." )
#endif

  TJetVector<T> z( myEnv_ );

  z.comp_.push_back( comp_[ 1 ] * x( 2 ) - comp_[ 2 ] * x( 1 ) );
  z.comp_.push_back( comp_[ 2 ] * x( 0 ) - comp_[ 0 ] * x( 2 ) );
  z.comp_.push_back( comp_[ 0 ] * x( 1 ) - comp_[ 1 ] * x( 0 ) );

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> operator^ ( Vector const& y, TJetVector<T> const& x )
{
#ifndef NOCHECKS
  CHECKOUT(( y.Dim() != 3 ) || ( x.comp_.size() != 3 ),
           "TJetVector<T>::operator^",
           "Dimension must be 3." )
#endif

  return -(x^y);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJetVector<T> operator*(  TMatrix<T> const& M, TJetVector<T> const& x ) 
{
 TJetVector<T> z( M.rows(), x.myEnv_ );

 int r = M.rows();
 int c = M.cols();

 if( M.cols() != x.comp_.size() ) {
  throw( GenericException( __FILE__, __LINE__, 
         "TJetVector<T> operator*(  const TMatrix<T>&, const TJetVector<T>& )",
         "Rows and/or columns of the matrix are not correct." ) );
 }

 int j=0;
 for( int i=0; i < r; ++i) {
   z.comp_[i] = M( i, 0 ) * x.comp_[0];
   j = 1;

   while( j < c )   { 
     z.comp_[i] += M( i, j ) * ( x.comp_[j] ) ;  
     ++j;
   }
 }

 return z;
}


// Boolean functions |||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator== ( TJetVector<T> const& x ) const
{
  if( ( comp_.size() != x.comp_.size() ) || myEnv_ != x.myEnv_ ) return 0;

  for( int i=0; i < comp_.size(); ++i) 
    if( comp_[i] != x.comp_[i] ) return 0;
  return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator==( T const& x ) const 
{
 for( int i=0; i < comp_.size(); ++i ){
   if( comp_[i] != x ) return false; 
 }

 return true;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator!= ( TJetVector<T> const & x ) const
{
  return !( operator==( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator!=( T const& x ) const 
{
 return !( operator==( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator< ( TJetVector<T> const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(myEnv_ != x.myEnv_, "TJetVector<T>::operator<", "Incompatible environments.")
  CHECKOUT(comp_.size() != x.comp_.size(), "TJetVector<T>::operator<", "Incompatible dimensions.")
#endif

  for( int i=0; i < comp_.size(); ++i ) 
    if( comp_[i].standardPart()  >= x.comp_[i].standardPart() ) return false;
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator<= ( TJetVector<T> const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(myEnv_ != x.myEnv_, "TJetVector<T>::operator<", "Incompatible environments.")
  CHECKOUT(comp_.size() != x.comp_.size(), "TJetVector<T>::operator<", "Incompatible dimensions.")
#endif

  for( int i = 0; i < comp_.size(); ++i ) 
    if( comp_[i].standardPart() > x.comp_[i].standardPart() ) return false;
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator> ( TJetVector<T> const& x ) const
{
  return !( operator<=( x ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::operator>= ( TJetVector<T> const& x ) const
{
  return !( operator<( x ) );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::IsNull() const
{
  int i = -1;
  while ( ++i < comp_.size() ) if( comp_[i] != T() ) return false;
  return true;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::IsUnit() const
{
  T x = T();
  for( int i = 0; i < comp_.size(); i++ ) 
    x += comp_[i].standardPart()*comp_[i].standardPart();

  return ( x == 1.0 );  // NOT RELIABLE !!!! FIXME !!!!

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJetVector<T>::IsNilpotent() const 
{

 for( int i=0; i <  myEnv_->spaceDim(); ++i) {
  if( !(comp_[i].isNilpotent()) ) return false;
 }

 return true;
}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Utilities ..


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::peekAt() const
{
  (*pcout) << "\n\nBegin TJetVector<T>::peekAt() ......\n";
  for ( int i = 0; i < comp_.size(); i++ ) {
   (*pcout) << "TJetVector<T>::peekAt(): Component " << i << endl;
    comp_[i].peekAt();
  }
  (*pcout) << "End TJetVector<T>::peekAt() ......\n" << endl;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJetVector<T>::Norm () const
{
  TJet<T> x( myEnv_ );
  x = T();
  for ( int i = 0; i < comp_.size(); ++i) x += comp_[i]*comp_[i];
  return sqrt(x);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::Unit () const
{
  TJet<T> x( myEnv_ );
  x = Norm();
  TJetVector<T> z( *this );
  for ( int i = 0; i < comp_.size(); ++i) z.comp_[i] /= x;
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
ostream& operator<<( ostream& os, TJetVector<T> const& v )
{
  os << "Begin TJetVector<T>:" << endl;
  os << v.comp_.size() << endl;
  os << *(v.myEnv_);
  for( int i = 0; i < v.comp_.size(); i++ ) {
    os << "\nTJetVector<T> component " << i << ":" << endl;
    os << v.comp_[i];
  }
  os << "\nEnd TJetVector<T>." << endl;
  return os;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
istream& operator>>( istream& is, TJetVector<T>& v )
{
  char buf[100];

  int  notused;

  is >> buf;
  is >> buf;
  is >> notused;

  CHECKOUT(v.comp_.size() <= 0, "TJetVector<T>::TJetVector<T>", "Dimension must be positive.")

  streamIn( is, v.myEnv_ );

  TJet<T> tmp;
  for ( int i = 0; i < v.comp_.size(); i++ ) {
    is >> buf;
    is >> buf;
    is >> buf;
    is >> tmp;  v.comp_.push_back(tmp);
  }

  is >> buf;
  is >> buf;

  return is;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::filter( bool (*f[]) ( const IntArray&, const T& ) ) const
{
 TJetVector<T> z( TVector<T>(comp_.size()), myEnv_ );

 for( int i=0; i<comp_.size();  i++ ) {
    z.comp_[i] = comp_[i].filter( f[i] );
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TJetVector<T>::filter( int lower, int upper ) const
{

 TJetVector<T> z( TVector<T>(comp_.size()), myEnv_ );

 for( int i=0; i < comp_.size(); ++i ) {
   z.comp_[i] = comp_[i].filter( lower, upper );
 }

 return z;
}



// Operations related to differentiation |||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::weightedDerivative( int* m, T* x ) 
{

 for( int i=0;  i<comp_.size(); i++ ) x[i] = comp_[i].weightedDerivative( m );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetVector<T>::derivative( int* m, T* x ) 
{

 for( int i = 0; i < comp_.size(); i++ ) x[i] = comp_[i].derivative( m );
}



// Query functions |||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
int TJetVector<T>::AccuWgt() const
{
  int accuWgt = myEnv_->maxWeight();
  for( int i = 0; i < comp_.size(); i++ ) {
   if( myEnv_ != comp_[i].Env() ) {
     (*pcerr) << "\n\n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** TJetVector<T>::AccuWgt()             \n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** Inconsistent environments at      \n"
 	  << "*** WARNING *** index " << i << "                 \n"
 	  << "*** WARNING ***                                   \n"
 	  << endl;
   }
   if( accuWgt > comp_[i].getAccuWgt() ) accuWgt = comp_[i].getAccuWgt();
  }
  return accuWgt;
}

template<typename T>
int TJetVector<T>::Weight()  const
{
  int weight = -1;
  for( int i = 0; i < comp_.size(); i++ ) {
   if( myEnv_ != comp_[i].Env() ) {
     (*pcerr) << "\n\n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** TJetVector<T>::Weight()               \n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** Inconsistent environments at      \n"
 	  << "*** WARNING *** index " << i << "                 \n"
 	  << "*** WARNING ***                                   \n"
 	  << endl;
   }
   if( weight  < comp_[i].getWeight()  ) weight  = comp_[i].getWeight();
  }
  return weight;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TJetVector<T>::standardPart() const 
{

 TVector<T> x( comp_.size() );

 for(  int i=0; i< comp_.size(); ++i ) { 
   x[i] = comp_[i].standardPart();
 }
 
 return x;

}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TJetVector<T>::getReference() const 
{

 TVector<T> r( myEnv_->numVar() );

 for( int i= 0; i < myEnv_->numVar(); ++i) { 
   r[i] = myEnv_->getRefPoint()[i];
 }

 return r;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#endif // TJETVECTOR_TCC

