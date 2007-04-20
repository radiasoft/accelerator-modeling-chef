/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJL1.tcc
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
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
******  Authors: Jean-Francois Ostiguy   ostiguy@fnal.gov                                    
******           Leo Michelotti          michelotti@fnal.gov
******
******           Fermilab, Batavia IL 60510                                           
******  
******  NOTE:
******
******  The code in this file is a specialized first order implementation 
******  of the general class TJL, meant to integrate and interoperate as 
******  transparently as possible with it. It borrows liberally from 
******  an earlier specialized first order implementation authored by 
******  Leo Michelotti.  
******
******
**************************************************************************
*************************************************************************/
#ifndef TJL1_TCC
#define TJL1_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <complex>
#include <iomanip>
#include <fstream>
#include <basic_toolkit/iosetup.h>

#include <basic_toolkit/utils.h> // misc utils: nexcom(), bcfRec(), nearestInteger() ...  
#include <basic_toolkit/GenericException.h>
#include <functional>
#include <algorithm>
#include <numeric>

using FNAL::pcerr;
using FNAL::pcout;

// static class variable definition:


template <typename T>
std::vector<TJL1<T>* > TJL1<T>::thePool_; 


// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Implementation of Class TJL1
// 
// 
//    Constructors and destructors    |||||||||||||||||||||||||||

// ****NOTE: **** for first order TJL1, capacity *must* be set to 
//                numVar()+1 !


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::TJL1( EnvPtr<T> const& pje, T x): 
count_( pje->numVar()+1 ),
weight_(1),                        
accuWgt_(1),
myEnv_( pje )  
{

   terms_ = new T[ count_ ]; 

   terms_[0] = x; // standard part 
   jcb_      = terms_+1;

   std::fill_n( &terms_[0]+1,  count_-1,  T()  ); 
   
 }

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T>  TJL1<T>::makeTJL( EnvPtr<T> const& pje, T x )
{

  if (!pje) return JL1Ptr<T>(); // this form is called by ONLY by Tcoord and Tparam

  if (thePool_.empty() ) 
     return (JL1Ptr<T>( new  TJL1<T>(pje, x )) ); 
 
  TJL1<T>* p   = thePool_.back();  thePool_.pop_back();

  if ( p->count_  != pje->numVar()+1 ) {
      
      delete [] p->terms_; 

      p->count_  =  pje->numVar()+1;
      p->terms_  =  new T[ p->count_ ];
      p->jcb_    =  p->terms_+1;
  }
 
  p->weight_   = 1;
  p->accuWgt_  = 1;
  p->myEnv_    = pje; 

  p->terms_[0] = x; // std part

  std::fill_n( &(p->terms_[0])+1,  p->count_-1,  T()  ); 

 if ( !p->myEnv_ ) {
 throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>:makeTJL( EnvPtr<T> const& pje, T x )",
          "Null Environment." ) );
 };

 return JL1Ptr<T>(p);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::TJL1( IntArray const& e, const T& x, EnvPtr<T> const& pje ) :
 count_(  pje->numVar()+1 ),
 weight_(1),
 accuWgt_(1),
 myEnv_(pje) 
{

  terms_ = new T[ count_ ];
  jcb_   = terms_+1;
 
  std::fill_n( &terms_[0]+1,  count_-1,  T()  ); 

  int weight = e.Sum(); 

  if ( weight > 1) return;

  if ( weight ==0 ) {
    terms_[0] = x;   
    return;
  }

  for (int i=0; i<count_-1; ++i) {
     if ( e(i) == 1){ 
       jcb_[i] = x;  
       break;
     }
  } 
  

 if ( !myEnv_ ) {
 throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::TJL1( const IntArray& e, const T& x, EnvPtr<T> const& pje )",
          "Null Environment." ) );
 };

 return;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::makeTJL( const IntArray& e, const T& x, EnvPtr<T> const& pje ) {

 if ( !pje ) {
 throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::makeTJL( const IntArray& e, const T& x, EnvPtr<T> const& pje )",
          "Null Environment." ) );
 };


  if (thePool_.empty() ) 
      return JL1Ptr<T>( new TJL1<T>(e,x,pje) );

  TJL1<T>* p = thePool_.back(); thePool_.pop_back();

  if (p->count_  != pje->numVar()+1 ) {
      delete [] p->terms_; 
      p->count_ =  pje->numVar()+1;
      p->terms_ = new T[ p->count_];
      p->jcb_   = p->terms_ + 1;
  }
 
  p->weight_   = 1;
  p->accuWgt_  = 1;
  p->myEnv_    = pje; 

  std::fill_n(  p->terms_+1,  p->count_-1, T() ); 

  int weight = e.Sum();

  if (weight > 1)  
     return JL1Ptr<T>(p) ;  // all terms of order 1 or lower are 0

  if (weight == 0) { 
     p->terms_[0] = x; 
     return JL1Ptr<T>(p);  
  }
 
  for (int i=0; i < p->count_-1; ++i) {
     if ( e(i) == 1){ 
       p->jcb_[i] = x;  
       break;
     }
  } 


 if ( !p->myEnv_ ) {
 throw( GenericException( __FILE__, __LINE__, 
          "JL1Ptr<T> TJL1<T>::makeTJL( const IntArray& e, const T& x, EnvPtr<T> const& pje )",
          "Null Environment." ) );
 };

 
  return JL1Ptr<T>(p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
template<typename U>
JL1Ptr<T> TJL1<T>::makeTJL( TJL1<U> const& x )
{

  if (thePool_.empty() ) {
     return JL1Ptr<T>( new TJL1<T>(x) );
  }
  
  TJL1<T>* p = thePool_.back(); thePool_.pop_back(); 
  
  if ( p->count_  != x.myEnv_->numVar()+1) {
      delete [] p->terms_; 
      p->count_ =  x.myEnv_->numVar()+1;
      p->terms_  = new T[ p->count_ ];
      p->jcb_    = p->terms_ + 1;
  }
 
  p->weight_   = x.weight_;  
  p->accuWgt_  = x.accuWgt_;
  p->myEnv_    = x.myEnv_;
 
  std::copy( x.terms_,  x.terms_+ x.count_, p->terms_ );
 
  if ( !p->myEnv_ ) {
  throw( GenericException( __FILE__, __LINE__, 
           "JL1Ptr<T> TJL1<T>::makeTJL( TJL1<U> const& x )",
           "Null Environment." ) );
  };


  return JL1Ptr<T>(p);

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::TJL1( TJL1 const& x ):
 count_(x.count_),       
 weight_(x.weight_),     
 accuWgt_(x.accuWgt_),
 myEnv_(x.myEnv_)

{
    if ( !myEnv_ ) {
     throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::TJL1( const TJL1& x )",
          "Null Environment." ) );
     };

     terms_ = new T [ count_];
     jcb_   = terms_+1;     

     std::copy( x.terms_, x.terms_+x.count_, terms_ );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::~TJL1() 
{
  
  delete [] terms_;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::discardTJL( TJL1<T>* p) 
{
  
   p->myEnv_ =   EnvPtr<T>();  // nullify the environment. 
   thePool_.push_back(p); 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//     Member functions(public)  |||||||||||||||||||||||||||||||


template<typename T>
void TJL1<T>::getReference( T* r ) const 
{

 for( int i=0;  i< count_-1; ++i ) r[i] = myEnv_->refPoint()[i];

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJL1<T>::isNilpotent() const 
{


 if(   std::abs(terms_[0]) > MX_SMALL ) 
      { return false; }
 else
      { return true; }

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL1<T>::writeToFile( std::ofstream& outStr ) const 
{}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::peekAt() const {

 (*pcout) << "\nCount  = "
      << count_
      << " Weight = "
      << weight_
      << " Max accurate weight = "
      << accuWgt_
      << std::endl;
 (*pcout) << "Reference point: ";

 for( int i=0; myEnv_->numVar(); ++i ) 
    (*pcout) << std::setprecision(12) << getEnv()->getRefPoint()[i] << "  ";
 (*pcout) << std::endl;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL1<T>::scaleBy( T y ) 
{ 

  std::transform( &terms_[0], &terms_[0]+count_ , &terms_[0], std::bind2nd(std::multiplies<T>(), y ));
  
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJL1<T>& TJL1<T>::Negate( ) 
{

  std::transform(  &terms_[0], &terms_[0]+count_, &terms_[0], std::negate<T>() );
  return *this;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::setVariable(  T const& x,  int const& j, EnvPtr<T> const& pje)
{
 
// this member function is meant to be called **ONLY** when a coordinate is instantiated

 if( ( j < 0 ) || ( j >= myEnv_->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const T&, const int&, TJetEnvironment<T>* )", 
     "Index out of range") );
 }
 
 myEnv_   = pje;
 accuWgt_ = myEnv_->maxWeight();
 weight_  = 1;
  
 terms_[0]   = x;
 jcb_[j]     = T (1.0);
 


 if ( !myEnv_ ) {
 throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::setVariable(  const T& x, const int& j, EnvPtr<T> const& pje)" ,
          "Null Environment." ) );
 };



}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL1<T>::setVariable(  const T& x, const int& j )
{
 
// this member function is meant to be called **ONLY** when a coordinate is instantiated

 if( ( j < 0 ) || ( j >= myEnv_->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const T&, const int&, TJetEnvironment<T>* )", 
     "Index out of range") );
 }
 
 accuWgt_ =  myEnv_->maxWeight();
 weight_  = 1;

 terms_[0]   = x;
 jcb_[j]     = T (1.0);

 if ( ! myEnv_ ) {
 throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::setVariable(  const T& x, const int& j )" ,
          "Null Environment." ) );
 };


 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
template<typename T>
void TJL1<T>::setVariable( const int& j,  EnvPtr<T> const& theEnv ) 
{

 if( !myEnv_ ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL1<T>::setVariable( const int&, TJetEnvironment<T>* )", 
     "Private data myEnv_ is null: object has no environment assigned.") );
 }

 if( ( j < 0 ) || ( j >= myEnv_->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL1<T>::setVariable( const int&, TJetEnvironment<T>* )", 
     "Index out of range") );

 }

 myEnv_   = theEnv;
 
 accuWgt_ =  theEnv->maxWeight();
 weight_  = 1;

 terms_[0] = theEnv->refPoint()[j];

 jcb_[j]   = (T) 1.0;

 if ( ! myEnv_ ) {
 throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::setVariable(  const T& x, EnvPtr<T> const& theEnv )" ,
          "Null Environment." ) );
 };


}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::insert( TJLterm<T> const& a) 
{

 if (a.weight_ > 1 ) return;  // weight > 1,  do nothing
 
 jcb_[a.offset_] = a.value_;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#if  0
template<typename T>
TJLterm<T>* TJL1<T>::get() 
{

 // this function should be eliminated and replaced by one that returns
 // a copy of a TJLterm object.

// THIS IS A LEAK ! *** BROKEN *** FIX ME ! 
  
 T value;
 
 IntArray index(count_-1);

 if ( std_  != T() ) { 
    value = std_;
    std_  = T(); 
    return new TJLterm<T>(index, value );  
 }

 for (int i=0; i< count_-1; ++i) { 
   if (jcb_[i] != T()  ) { 
     value   = jcb_[i]; 
     jcb_[i] = T();
     index(i) = 1;
     return new TJLterm<T>(index, value ); 
   }
 }

 return new TJLterm<T>(index, T() ); 

}
#endif


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJL1<T>::lowTerm() const 
{
 
 // returns the lowest order **non-zero**  term;  
 
 IntArray index( this->myEnv_->numVar() );

 T std = terms_[0];

 if (std != T() ) return TJLterm<T>( index, std, this->myEnv_ );

 for (int i=0; i< count_-1; ++i ) {
  
  if ( jcb_[i] != T() ) { 
     index(i) = 1; 
     return TJLterm<T>( index, jcb_[i], this->myEnv_ ); // all terms are zero 
   }
 }

 return TJLterm<T>( index, T(), this->myEnv_ ); // all terms are zero 

}

 


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJL1<T>::firstTerm() const 
{

 // returns the first non-zero term;  In the context of the non-sparse 1st order Jet
 // this is the same as lowTerm. For a sparse Jet representation, this would be the
 // first term in the list. Note that this term is allowed to have a zero coefficient. 
 // Normally 0.0 terms are omitted, but a term with a zero coeffient may arise as the
 // result of addition, for example.  

 return lowTerm();

 }


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::append(TJLterm<T> const& a) 
{

 if (a.weight_ >  1) return; 

 if (a.offset_ == 0 ) { 
    terms_[0] = a.value_;
 } 
 else { 
   jcb_[a.offset_] = a.value_; 
 }
 
 return;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::addTerm(const TJLterm<T>& a) 
{

 if (a.weight_ >  1) return; 

 if (a.offset_ == 0 ) {
    terms_[0] += a.value_;
 } 
 else {
    jcb_[a.offset_] += a.value_;
 } 

 weight_  = std::max(weight_, a.weight_);
 
 return;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL1<T>::standardPart() const 
{

return terms_[0];

}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::clear() 
{

    terms_[0] = T();

    for (int i=0; i<count_-1; ++i) { 
      jcb_[i] = T();
    }
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL1<T>::weightedDerivative( const int* ind ) const 
{

 int sum = 0;
 int j   = 0;
 int k   = 0;

 for( int i=0; i< myEnv_->numVar(); ++i) {
  k = ind[i];
  sum += k;
  if( k == 1 ) { j = i; }
 }

 if( 0 == sum ) { return terms_[0]; }
 if( 1 == sum ) { return jcb_[j];  }

 return T();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL1<T>::derivative( const int* ind ) const 
{
  return this->weightedDerivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::compose( JL1Ptr<T> const y[ ]) const 
{ 

 JL1Ptr<T> u[myEnv_->numVar()];

 // Check consistency of reference points and
 // subtract reference point prior to concatenation.

 for( int i=0;  i< myEnv_->numVar(); ++i ) {
   if(  (y[i]->myEnv_) != (y[0]->myEnv_) ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJL1::compose(JL1Ptr<T> const y[ ]) const ",
            "Inconsistent environments." ));
   }

   u[i] = y[i] + JL1Ptr<T>( makeTJL( y[0]->myEnv_, -myEnv_->getRefPoint()[i]));   // u[i] = y[i] - myEnv_->getRefPoint()[i];
 }

 JL1Ptr<T> z( makeTJL(y[0]->myEnv_) );
 
 z->terms_[0]  =  terms_[0];                                  

 for( int i=0; i <myEnv_->numVar(); ++i ) { // note: it is assumed that y[] has dimension myEnv_->NumVar()
 
    z->terms_[0] += ( jcb_[i] * u[i]->terms_[0] ); 

    for( int j=0; j < count_-1; ++j) { 

      z->jcb_[j]  += ( jcb_[i] * u[i]->jcb_[j] );   

    }
 }

 return z;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL1<T>::operator()( const T* x )  const 
{

 // This routine is linked to double TJL1::operator()( const Vector& x ) const
 // Any change made to this must be reflected in the other.

  T v             = terms_[0];

 for( int i=0; i< count_-1; ++i ) {
   v += jcb_[i] * ( x[i] - myEnv_->refPoint()[i] );
 }
 return v;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL1<T>::operator()( const Vector& x ) const 
{
  int n = x.Dim();
  T newarg [n];
  for( int i = 0; i < n; ++i ) {
    newarg[i] = x(i);
  }
  return this->operator()( newarg );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::filter( const int& wgtLo, const int& wgtHi ) const 
{ 

 if( wgtLo <= 0 && wgtHi >= 1 ) { return JL1Ptr<T>( makeTJL(*this) ); }

 if( wgtHi < wgtLo ) {
   return JL1Ptr<T>( makeTJL(myEnv_) );
 }

 JL1Ptr<T> z( makeTJL(*this) );

 if( 0 == wgtHi ) {
  for( int i=0; i < count_-1; ++i ) { z->jcb_[i] = T(); }
 }

 if( 1 == wgtLo ) {
  z->terms_[0]= T();
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::filter( bool (*f) ( IntArray const&, const T& ) ) const 
{ 

 JL1Ptr<T> z(makeTJL(myEnv_) );

 IntArray index(myEnv_->numVar());


 if( f( index, terms_[0] ) ) { z->terms_[0] = terms_[0]; }

 for( int i=0; i<count_-1; ++i) {
  index(i) = 1;
  if( f( index, terms_[0] ) ) { 
    z->jcb_[i] = jcb_[i];
  }
  index(i) = 0;
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>& TJL1<T>::operator=( const T& x ) 
{

 // FIX ME ++++ need to check env  !!!!!
 
 clear();
 terms_[0]  = x;
 return *this;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>& TJL1<T>::operator=( const TJL1<T>& x ) 
{

 if (&x == this) return *this;
 
 terms_[0] = x.terms_[0];

 for (int i=0; i< myEnv_->numVar(); ++i ) {

   jcb_[i] = x.jcb_[i];
 
 }

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJL1<T>& TJL1<T>::operator+=( const T& x ) {   

  terms_[0] += x;


 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::add( JL1Ptr<T> const& x) {

   terms_[0] += x->terms_[0];

 for (int i=0; i< myEnv_->numVar(); ++i ) {

   jcb_[i] += x->jcb_[i];
 
 } 

 return;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::sin() const
{ 

  JL1Ptr<T> z( makeTJL(myEnv_) ); 

  z->terms_[0] = std::sin( terms_[0] );

  for( int i=0; i<count_-1;  ++i) { 

    z->jcb_[i]  = std::cos( terms_[0] ) * jcb_[i];

  }
  return z;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::cos() const
{ 

 JL1Ptr<T> z( makeTJL(myEnv_) ); 

  z->terms_[0] = std::cos( terms_[0] );

  for( int i=0; i<count_-1;  ++i) { 

    z->jcb_[i]  = -std::sin( terms_[0] ) * jcb_[i];

  }
  return z;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 template<typename T>
 JL1Ptr<T> TJL1<T>::exp() const
 {

  JL1Ptr<T> z(makeTJL(myEnv_) );  

  z->terms_[0]  = std::exp( terms_[0] );

  for( int i=0; i <count_-1; ++i ) { 
    z->jcb_[i] = terms_[0] * jcb_[i]; 
  }

  return z;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::pow(const double& s )  const
{

  JL1Ptr<T> z( makeTJL(myEnv_) );  

  z->terms_[0] = std::pow( terms_[0], s );

  for( int i=0; i<count_-1;  ++i) { 
    z->jcb_[i] = jcb_[i] * s * std::pow( terms_[0], s-1 ); 
  }

  return z;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::pow(int n ) const
{

  JL1Ptr<T> z;  
  JL1Ptr<T> x (makeTJL(*this) );  
  
  int i = 0;

  if( n == 0 ) z =  JL1Ptr<T>( makeTJL(myEnv_, (T) 1.0 ) );
  else if( n > 0 ) {
    z = JL1Ptr<T>( makeTJL(*this) );
    for( i = 2; i <= n; i++ ) z = x*z;
  }
  else {
    JL1Ptr<T> xr; 
    z  = (T)1.0 / x ;
    xr = z;
    for( i = -2; i >= n; i-- ) z = z*xr;
  }

  return z;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::log() const
{

 if( std::abs(terms_[0]) <= 0.0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJL1::log() const",
           "Non-positive standard part.") );
  }

  JL1Ptr<T> z(makeTJL(myEnv_) );

  z->terms_[0] = std::log( terms_[0] );

  for( int i=0; i<count_-1; ++i ) { 

    z->jcb_[i] = ( 1.0 / terms_[0]  ) * jcb_[i]; 

  }
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::asin() const
{ 


JL1Ptr<T> z( makeTJL(*this) );
JL1Ptr<T> x( makeTJL(*this) );
JL1Ptr<T> dz = ( z->sin() - x ) / ( z->cos() );

 int iter = 0;

 while( iter++ < 2 ) // FIX ME !!!
 {
  z  = -dz;
  dz = (z->sin() - x) /  z->cos();

 }

 if( iter >= MX_MAXITER ) {
  (*pcout) << "*** WARNING ***                             \n";
  (*pcout) << "*** WARNING *** JL& asin( JL& x ) {         \n";
  (*pcout) << "*** WARNING *** Over " 
       << MX_MAXITER 
       << " iterations used;    \n";
  (*pcout) << "*** WARNING *** result may be incorrect.    \n";
  (*pcout) << "*** WARNING ***                             \n";
  (*pcout) << "*** WARNING *** Initial value:              \n";
  (*pcout) << "*** WARNING ***                             \n"
       << std::endl;
  x->printCoeffs();
  (*pcout) << "*** WARNING *** A few iterations:             " << std::endl;
  for( iter = 0; iter < 4; iter++ ) {
   (*pcout) << "*** WARNING *** Iteration No. " << ( iter+1 ) << std::endl;
   z = -dz;
   dz = (z->sin() - x) /  z->cos();
   (*pcout) << "*** WARNING *** dz = " << std::endl;
   dz->printCoeffs();
   (*pcout) << "*** WARNING *** z = " << std::endl;
   z->printCoeffs();
  }
 }

 return z;
}

 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::atan() const
{   
 
 JL1Ptr<T> z( makeTJL(*this) );
 JL1Ptr<T> x( makeTJL(*this) );
 JL1Ptr<T> c  = -z->cos();
 JL1Ptr<T> dz = c *( z->sin() +  (x*c) );

 int iter = 0;
 while( ++iter < 2 ) {  // FIXME !!!!
  z  = -dz;
  c = -(z->cos());
  dz = c *( z->sin() + x*c );
 }

 if( iter >= MX_MAXITER ) {
  (*pcerr) <<  "*** WARNING ***                             \n" 
           <<  "*** WARNING *** TJL1::atan()      {         \n" 
           <<  "*** WARNING *** Over " 
           << MX_MAXITER  << " iterations used;    \n"
           <<  "*** WARNING *** result may be incorrect.    \n" 
           <<  "*** WARNING ***                             \n" 
           << std::endl;
 }

 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::D( const int* n ) const 
{

 //----------------------------- 
 // Differentiation operator
 //-----------------------------

 int k   = 0;
 int sum = 0;
 for( int i = 0; i < myEnv_->numVar(); ++i ) {
  if( n[i] < 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL1::D( const int* n ) const ",
          "Component of argument is negative." ) );
  }
  if( n[i] > 1 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL1::D( const int* n ) const ",
          "Component of argument is too large." ) );
  }
  sum += n[i];
  if( 1 == n[i] ) { k = i; }
 }  

 JL1Ptr<T> z( makeTJL(myEnv_) );
 if( 1 == sum ) { z->terms_[0] = jcb_[k]; }
 return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
void TJL1<T>::printCoeffs() const {

 (*pcout) << "\nCount  = " << count_ 
      << ", Weight = " <<      weight_ 
      << ", Max accurate weight = " << accuWgt_ << std::endl;
 (*pcout) << "Reference point: " 
      << myEnv_->getRefPoint()[0];

 for( int i=0; i < count_-1; i++ ) {
   (*pcout) << ", ";
   (*pcout) << myEnv_->getRefPoint()[i];
 }

 (*pcout) << std::endl;

 IntArray index(count_-1);

 (*pcout) << "Index:  " 
        << index
        << "   Value: "
        << terms_[0]
        << std::endl;

 for( int i=0; i < count_-1; ++i ) {
  
  if( weight_ > accuWgt_ ) break;
 
  index(i) = 1; 
  (*pcout) << "Index:  " 
        << index
        << "   Value: "
        << jcb_[i]
        << std::endl;
  index(i) = 0; 
  }
  (*pcout) << "\n" << std::endl;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
template <typename U>
TJLterm<U>  TJL1<T>::iter_<U>::dereference() const  
{ 

   IntArray exponents(myEnv_->numVar() );

   if ( m_node == terms_ ) return TJLterm<T>(exponents, terms_[0] );    

   int idx = (m_node - terms_) - 1 ;
   exponents(idx) = 1;  

   return TJLterm<T>(exponents, jcb_[idx] );

}  

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
template <typename U>
void  TJL1<T>::iter_<U>::increment()          
{ 
   ++m_node; 
} 
        
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
template <typename U>
bool  TJL1<T>::iter_<U>::equal( iter_ const& other) const 
{
    return this->m_node == other.m_node;

}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



// ================================================================
//      Implementation of exceptions

template<typename T>
TJL1<T>::BadDimension::BadDimension( int i, int j, 
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: xdim(i), ydim(j)
{
  std::ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** First  argument has " << xdim << " coordinates."
      << "\n*** ERROR *** Second argument has " << ydim << " coordinates."
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << std::endl;
    firstTime = false;
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TJL1<T>::BadDimension::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::BadEnvironment::BadEnvironment( int a, int b, int c, 
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: i(a), env_1(b), env_2(c)
{
  std::ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** comp[" << i << "].Env = " << env_1
      << "\n*** ERROR *** this->myEnv = " << env_2
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << std::endl;
    firstTime = false;
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TJL1<T>::BadEnvironment::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::BadReference::BadReference( int ndx, double u,
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: i(ndx), im(u)
{
  std::ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** The imaginary part of _refPoint[ " << i << " ]"
      << "\n*** ERROR *** is equal to " << im <<  ", which is not zero."
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << std::endl;
    firstTime = false;
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TJL1<T>::BadReference::what() const throw()
{
  return errorString.c_str();
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//  ******   friends and operators  ***********

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
std::ostream& operator<<( std::ostream& os, const TJL1<T>& x ) 
{

 os << "\n Count = " << x.count_ << " , Weight = " << x.weight_;
 os << " , Max accurate weight = " << x.accuWgt_ << std::endl;

 if (x.myEnv_) {
 os << "Begin Environment: \n"
    << *(x.myEnv_)
    << "\nEnd Environment: " << std::endl;
 }
 else 
 os << "WARNING: null environment ! "<< std::endl; 
 
 IntArray index(x.count_-1);

 os << "Index: " << index << "   Value: " << x.terms_[0] << std::endl;  

 for (int i=0; i < x.count_-1;  ++i) {
   index(i) = 1;
   os << "Index: " << index <<"   Value: " << x.jcb_[i] << std::endl;  
   index(i) = 0;
 }

 return os << "\n" << std::endl;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
std::istream& operator>>( std::istream& is,  TJL1<T>& x ) 
{  
  char buf[100];
  int i,j;
  int count;
  T value;
  TJLterm<T>* q;

  x.clear();

  is >> buf;
  is >> buf;
  is >> count;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> x.weight_;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> x.accuWgt_;
  is >> buf;
  is >> buf;
  streamIn( is, x.myEnv_);
  is >> buf;
  is >> buf;
  IntArray ndx( x.getEnv()->numVar() );
  for( j = 0; j < count; ++j) {
    is >> buf;
    for( i = 0; i < x.getEnv()->numVar(); ++i )  {
     is >> ndx(i);
    }
    is >> buf;
    is >> value;
//     x.append(  TJLterm<T>( ndx, value, x.getEnv() ) );
  }

 return is;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const TJL1<T>& x, const TJL1<T>& y ) 
{
 if( x.myEnv_ != y.myEnv_ ) return false;

 if( !( x.terms_[0] == y.terms_[0] ) ) return false;

 for(  int i=0; i< x.count_-1; ++i) {
    for(  int j=0; j<y.count_-1; ++j) {
      if( !( x.jcb_[i] == y.jcb_[i] ) ) return false;
   }
 }
 return true;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const TJL1<T>& x, const T& y ) 
{

 bool result = true;
 result = result && ( x.terms_[0] == y );
 for( int i=0; i < x.myEnv_->numVar(); i++ ) {
  result = result && ( x.jcb_[i] == T() );
 }

 return result;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const T& y, const TJL1<T>& x )
{
 return x == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TJL1<T>& x, const TJL1<T>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TJL1<T>& x, const T& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const T& x, const TJL1<T>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> operator+(JL1Ptr<T> const& x, JL1Ptr<T> const& y ) {

// Check for consistency and set reference point of the sum.

  if( x->myEnv_ != y->myEnv_ ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::operator+(JL1Ptr<T> const& x)"
           "Inconsistent environments." ) );
  }

 JL1Ptr<T> z(TJL1<T>::makeTJL( x->myEnv_ ) );

 z->terms_[0]  = x->terms_[0] +  y->terms_[0];

 for (int i=0; i< z->myEnv_->numVar(); ++i ) {

   z->jcb_[i] = x->jcb_[i] +  y->jcb_[i];
 
 } 

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JL1Ptr<T>& operator+=(JL1Ptr<T> &x,      JL1Ptr<T> const& y  ){

 x->terms_[0] += y->terms_[0];
 for (int i=0; i< x->count_-1; ++i) {

    x->jcb_[i] += y->jcb_[i];

 } 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> operator-(JL1Ptr<T> const& x, JL1Ptr<T> const& y ) {

// Check for consistency and set reference point of the sum.

  if( x->myEnv_ != y->myEnv_ ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::operator-(JL1Ptr<T> const& x)"
           "Inconsistent environments." ) );
  }

 JL1Ptr<T> z(TJL1<T>::makeTJL( x->myEnv_ ) );

 z->terms_[0]  = x->terms_[0] -  y->terms_[0];

 for (int i=0; i< z->myEnv_->numVar(); ++i ) {

   z->jcb_[i] = x->jcb_[i] -  y->jcb_[i];
 
 } 

 return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JL1Ptr<T> operator-(JL1Ptr<T> const& x) {

 JL1Ptr<T> z(TJL1<T>::makeTJL( x->myEnv_ ));

 z->terms_[0]  = - x->terms_[0];

 for (int i=0; i< z->myEnv_->numVar(); ++i ) {

   z->jcb_[i] = -x->jcb_[i];
 } 

 return z;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> operator*(JL1Ptr<T> const& x, T const& y ) {

//---------------------------
// Multiplication by a scalar
//---------------------------

 JL1Ptr<T> z(TJL1<T>::makeTJL( *x ) ); // deep copy 
 z->scaleBy(y);
 
 return z;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> operator*(JL1Ptr<T> const& x, JL1Ptr<T> const& y ) {

//----------------------------
// Multiplication of two jets
//----------------------------

 // Consistency check 

 if( x->myEnv_ != y->myEnv_ ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL1Ptr<T> operator*( const JL1Ptr<T>&, const JL1Ptr<T>& ) ",
          "Inconsistent environments." ) );
 };

 JL1Ptr<T> z( TJL1<T>::makeTJL(x->myEnv_) );

 z->terms_[0]  = x->terms_[0] * y->terms_[0];

 for( int i=0; i< z->myEnv_->numVar();  ++i) { 

   z->jcb_[i]  = ( x->terms_[0] * y->jcb_[i] ) + ( y->terms_[0] * x->jcb_[i] ); 

 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JL1Ptr<T> operator/( T const& wArg, JL1Ptr<T> const& uArg ) 
{ 

 // Check for void operators ..

 if ( uArg->count_ < 1 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::operator/( JL1Ptr<T> const& wArg, JL1Ptr<T> const& uArg )",
          "Attempt to divide by a null TJL1<T> variable." ) );
 }
 
 JL1Ptr<T>  z( TJL1<T>::makeTJL(uArg->myEnv_) );

 z->terms_[0] =  wArg/(uArg->terms_[0]);

 for( int i=0; i < z->myEnv_->numVar(); ++i) { 

   z->jcb_[i] =   - ( wArg * uArg->jcb_[i] ) / ( uArg->terms_[0] * uArg->terms_[0] ); 

 };
 
 return z;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JL1Ptr<T> operator/( JL1Ptr<T> const& wArg, JL1Ptr<T> const& uArg ) 
{ 

 if ( (wArg->myEnv_) != (uArg->myEnv_) ) {

   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/( JL1Ptr<T> const& wArg, JL1Ptr<T> const& uArg )",
          "Inconsistent environments." ) );
 }

 // Check for void operators ..

 if ( uArg->count_ < 1 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::operator/( JL1Ptr<T> const& wArg, JL1Ptr<T> const& uArg )",
          "Attempt to divide by a null TJL1<T> variable." ) );
 }
 
 JL1Ptr<T>  z( TJL1<T>::makeTJL(wArg->myEnv_) );


 z->terms_[0] =  (wArg->terms_[0])/(uArg->terms_[0]);

 for( int i = 0; i < z->myEnv_->numVar(); i++ ) { 

   z->jcb_[i] = ( ( uArg->terms_[0] * wArg->jcb_[i] ) - ( wArg->terms_[0] * uArg->jcb_[i] ) )
                / ( uArg->terms_[0]* uArg->terms_[0] ); 

 };
 
 return z;

}



#endif // TJL1_TCC


