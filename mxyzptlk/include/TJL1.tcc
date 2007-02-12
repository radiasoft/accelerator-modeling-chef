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

using FNAL::pcerr;
using FNAL::pcout;

// static class variable definition:


template <typename T>
std::vector<TJL1<T>* > TJL1<T>::_thePool; 


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
_count( pje->numVar()+1 ),
_weight(1),                        
_accuWgt(1),
_myEnv( pje )  
{

   _std = x; 
   _jcb = new T[ _count-1 ];

   for (int i=0; i<_count-1; ++i )  
        _jcb[i] = T();
   
 }

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T>  TJL1<T>::makeTJL( EnvPtr<T> const& pje, T x )
{

  if (!pje) return JL1Ptr<T>(); // this form is called by ONLY by Tcoord and Tparam

  if (_thePool.empty() ) 
     return (JL1Ptr<T>( new  TJL1<T>(pje, x )) ); 
 
  TJL1<T>* p   = _thePool.back();  _thePool.pop_back();

  if ( p->_count  != pje->numVar()+1 ) {
      
      delete [] p->_jcb; 
      p->_jcb   = new T[ pje->numVar() ];
      p->_count =  pje->numVar()+1;
  }
 
  p->_weight   = 1;
  p->_accuWgt  = 1;
  p->_myEnv    = pje; 

  p->_std = x;
  for (int i=0; i<p->_count-1; ++i) p->_jcb[i]= T();


 if ( !p->_myEnv ) {
 throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>:makeTJL( EnvPtr<T> const& pje, T x )",
          "Null Environment." ) );
 };

 return JL1Ptr<T>(p);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::TJL1( const IntArray& e, const T& x, EnvPtr<T> const& pje ) :
 _count(  pje->numVar()+1 ),
 _weight(1),
 _accuWgt(1),
 _myEnv(pje) 
{

  _jcb = new T[ _count-1 ];
 
  for (int i=0; i<_count-1; ++i) { _jcb[i] = T(); }

  int weight = 0;
  for (int i=0; i < _count-1; ++i) {
    weight  += e(i);
  }

  if (weight > 1) return;

  if (weight ==0) {
    _std = x;   
    return;
  }

  for (int i=0; i<_count-1; ++i) {
     if ( e(i) == 1){ 
       _jcb[i] = x;  
       break;
     }
  } 
  

 if ( !_myEnv ) {
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


  if (_thePool.empty() ) 
      return JL1Ptr<T>( new TJL1<T>(e,x,pje) );

  TJL1<T>* p = _thePool.back(); _thePool.pop_back();

  if (p->_count  != pje->numVar()+1 ) {
      delete [] p->_jcb; 
      p->_jcb   = new T[ pje->numVar() ];
      p->_count =  pje->numVar()+1;
  }
 
  p->_weight   = 1;
  p->_accuWgt  = 1;
  p->_myEnv    = pje; 

  int weight = 0;

  for (int i=0; i<  p->_count-1; ++i) { p->_jcb[i] = T(); }

  for (int i=0; i < p->_count-1; ++i) {
    weight  += e(i);
  }

  if (weight > 1)  
     return JL1Ptr<T>(p) ;  // all terms of order 1 or lower are 0

  if (weight == 0) { 
     p->_std = x; 
     return JL1Ptr<T>(p);  
  }
 
  for (int i=0; i < p->_count-1; ++i) {
     if ( e(i) == 1){ 
       p->_jcb[i] = x;  
       break;
     }
  } 


 if ( !p->_myEnv ) {
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

  if (_thePool.empty() ) {
     return JL1Ptr<T>( new TJL1<T>(x) );
  }
  
  TJL1<T>* p = _thePool.back(); _thePool.pop_back(); 
  
  if ( p->_count  != x._myEnv->numVar()+1) {
      delete [] p->_jcb; 
      p->_jcb   = new T[ x._myEnv->numVar() ];
      p->_count =  x._myEnv->numVar()+1;
  }
 
  p->_weight   = x._weight;  
  p->_accuWgt  = x._accuWgt;
  p->_myEnv    = x._myEnv;
 
  p->_std      = x._std;

  for (int i=0; i < (p->_count-1); ++i) 
       p->_jcb[i] = x._jcb[i];

 if ( !p->_myEnv ) {
 throw( GenericException( __FILE__, __LINE__, 
          "JL1Ptr<T> TJL1<T>::makeTJL( TJL1<U> const& x )",
          "Null Environment." ) );
 };


 return JL1Ptr<T>(p);

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::TJL1( const TJL1& x ):
 _count(x._count),       
 _weight(x._weight),     
 _accuWgt(x._accuWgt),
 _myEnv(x._myEnv)

{
    _std = x._std;
     _jcb = new T [ _count-1 ];

     for (int i=0; i<_count-1; ++i) 
       _jcb[i] = x._jcb[i];

 if ( !_myEnv ) {
 throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::TJL1( const TJL1& x )",
          "Null Environment." ) );
 };



}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::~TJL1() 
{
  
  delete [] _jcb;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::discardTJL( TJL1<T>* p) 
{
  
   p->_myEnv =   EnvPtr<T>();  // nullify the environment. 
   _thePool.push_back(p); 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//     Member functions(public)  |||||||||||||||||||||||||||||||


template<typename T>
void TJL1<T>::getReference( T* r ) const 
{

 for( int i=0;  i< _count-1; ++i ) r[i] = _myEnv->refPoint()[i];

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJL1<T>::isNilpotent() const 
{


 if(   std::abs(_std) > MX_SMALL ) 
      { return false; }
 else
      { return true; }

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL1<T>::writeToFile( std::ofstream& outStr ) const 
{


}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::peekAt() const {

 (*pcout) << "\nCount  = "
      << _count
      << " Weight = "
      << _weight
      << " Max accurate weight = "
      << _accuWgt
      << std::endl;
 (*pcout) << "Reference point: ";

 for( int i = 0; _myEnv->numVar(); ++i ) 
    (*pcout) << std::setprecision(12) << getEnv()->getRefPoint()[i] << "  ";
 (*pcout) << std::endl;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL1<T>::scaleBy( T y ) 
{ 

  _std *= y;
 
  for (int i=0; i<_count-1; ++i) {
   _jcb[i] *= y;               
  }

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJL1<T>& TJL1<T>::Negate( ) {

  _std = -_std;
 
  for (int i=0; i<_count-1; ++i) {
   _jcb[i] = - _jcb[i];               
  }

 return *this;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::setVariable(  const T& x, const int& j, EnvPtr<T> const& pje)
{
 
// this member function is meant to be called **ONLY** when a coordinate is instantiated

 if( ( j < 0 ) || ( j >= _myEnv->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const T&, const int&, TJetEnvironment<T>* )", 
     "Index out of range") );
 }
 
 _myEnv   = pje;
 _accuWgt = _myEnv->maxWeight();
 _weight  = 1;
  
 _std     = x;
 _jcb[j]  = T (1.0);
 


 if ( !_myEnv ) {
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

 if( ( j < 0 ) || ( j >= _myEnv->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const T&, const int&, TJetEnvironment<T>* )", 
     "Index out of range") );
 }
 
 _accuWgt =  _myEnv->maxWeight();
 _weight  = 1;

 _std     = x;
 _jcb[j]  = T (1.0);

 if ( ! _myEnv ) {
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

 if( !_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL1<T>::setVariable( const int&, TJetEnvironment<T>* )", 
     "Private data _myEnv is null: object has no environment assigned.") );
 }

 if( ( j < 0 ) || ( j >= _myEnv->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL1<T>::setVariable( const int&, TJetEnvironment<T>* )", 
     "Index out of range") );

 }

 _myEnv   = theEnv;
 
 _accuWgt =  theEnv->maxWeight();
 _weight  = 1;

 _std     = theEnv->refPoint()[j];

 _jcb[j]  = (T) 1.0;

 if ( ! _myEnv ) {
 throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::setVariable(  const T& x, EnvPtr<T> const& theEnv )" ,
          "Null Environment." ) );
 };


}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::insert( const TJLterm<T>& a) 
{

 if (a._weight > 1 ) return;  // weight > 1,  do nothing
 
 if (a._weight == 0) _std = a._value;

 for (int i=0; i< a._index.Dim(); ++i) { 
  if (a._index(i) == 1 ) _jcb[i] = a._value;
  break;
 }

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
 
 IntArray index(_count-1);

 if ( _std  != T() ) { 
    value = _std;
    _std  = T(); 
    return new TJLterm<T>(index, value );  
 }

 for (int i=0; i< _count-1; ++i) { 
   if (_jcb[i] != T()  ) { 
     value   = _jcb[i]; 
     _jcb[i] = T();
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
 
 IntArray index( this->_myEnv->numVar() );

 if (_std != T() ) return TJLterm<T>( index, _std, this->_myEnv );

 for (int i=0; i< _count-1; ++i ) {
  
  if ( _jcb[i] != T() ) { 
     index(i) = 1; 
     return TJLterm<T>( index, _jcb[i], this->_myEnv ); // all terms are zero 
   }
 }

 return TJLterm<T>( index, T(), this->_myEnv ); // all terms are zero 

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
void TJL1<T>::append(const TJLterm<T>& a) 
{

 if (a._weight >  1) return; 

 if (a._weight == 0) {
   _std = a._value;
   return;
 }

 if (a._weight ==  1) {
   for (int i=0; i< a._index.Dim(); ++i) { 
     if (a._index(i) == 1) { 
       _jcb[i] = a._value; 
       break;
     }
   }
    _weight  = std::max(_weight, a._weight);
   return;
 }
 
}



// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::addTerm(const TJLterm<T>& a) 
{

 if (a._weight >  1) return; 

 if (a._weight == 0) {
   _std += a._value;
   return;
 }

 if (a._weight ==  1) {
   for (int i=0; i< a._index.Dim(); ++i) { 
     if (a._index(i) == 1) { 
       _jcb[i] += a._value; 
       break;
     }
   }
    _weight  = std::max(_weight, a._weight);
   return;
 }
 
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL1<T>::standardPart() const 
{

return _std;

}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::clear() 
{

    _std = T();

    for (int i=0; i<_count-1; ++i) { 
      _jcb[i] = T();
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

 for( int i=0; i< _myEnv->numVar(); ++i) {
  k = ind[i];
  sum += k;
  if( k == 1 ) { j = i; }
 }

 if( 0 == sum ) { return _std; }
 if( 1 == sum ) { return _jcb[j]; }

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

 JL1Ptr<T> u[_myEnv->numVar()];

 // Check consistency of reference points and
 // subtract reference point prior to concatenation.

 for( int i=0;  i< _myEnv->numVar(); ++i ) {
   if(  (y[i]->_myEnv) != (y[0]->_myEnv) ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJL1::compose(JL1Ptr<T> const y[ ]) const ",
            "Inconsistent environments." ));
   }

   u[i] = y[i] + JL1Ptr<T>( makeTJL( y[0]->_myEnv, -_myEnv->getRefPoint()[i]));   // u[i] = y[i] - _myEnv->getRefPoint()[i];
 }

 JL1Ptr<T> z( makeTJL(y[0]->_myEnv) );
 
 z->_std  =  _std;                                  

 for( int i=0; i <_myEnv->numVar(); ++i ) { // note: it is assumed that y[] has dimension _myEnv->NumVar()
 
    z->_std += ( _jcb[i] * u[i]->_std ); 

    for( int j=0; j < _count-1; ++j) { 

      z->_jcb[j]  += ( _jcb[i] * u[i]->_jcb[j] );   

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

  T v             = _std;

 for( int i=0; i< _count-1; ++i ) {
   v += _jcb[i] * ( x[i] - _myEnv->refPoint()[i] );
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
   return JL1Ptr<T>( makeTJL(_myEnv) );
 }

 JL1Ptr<T> z( makeTJL(*this) );

 if( 0 == wgtHi ) {
  for( int i=0; i < _count-1; ++i ) { z->_jcb[i] = T(); }
 }

 if( 1 == wgtLo ) {
  z->_std= T();
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::filter( bool (*f) ( const IntArray&, const T& ) ) const 
{ 

 JL1Ptr<T> z(makeTJL(_myEnv) );

 IntArray index(_myEnv->numVar());


 if( f( index, _std ) ) { z->_std = _std; }

 for( int i=0; i<_count-1; ++i) {
  index(i) = 1;
  if( f( index, _std ) ) { 
    z->_jcb[i] = _jcb[i];
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
 _std  = x;
 return *this;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>& TJL1<T>::operator=( const TJL1<T>& x ) 
{

 if (&x == this) return *this;
 
 _std = x._std;

 for (int i=0; i< _myEnv->numVar(); ++i ) {

   _jcb[i] = x._jcb[i];
 
 }

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJL1<T>& TJL1<T>::operator+=( const T& x ) {   

  _std += x;


 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::add( JL1Ptr<T> const& x) {

   _std += x->_std;

 for (int i=0; i< _myEnv->numVar(); ++i ) {

   _jcb[i] += x->_jcb[i];
 
 } 

 return;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::sin() const
{ 

  JL1Ptr<T> z( makeTJL(_myEnv) ); 

  z->_std = std::sin( _std );

  for( int i=0; i<_count-1;  ++i) { 

    z->_jcb[i]  = std::cos( _std ) * _jcb[i];

  }
  return z;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::cos() const
{ 

 JL1Ptr<T> z( makeTJL(_myEnv) ); 

  z->_std = std::cos( _std );

  for( int i=0; i<_count-1;  ++i) { 

    z->_jcb[i]  = -std::sin( _std ) * _jcb[i];

  }
  return z;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 template<typename T>
 JL1Ptr<T> TJL1<T>::exp() const
 {

  JL1Ptr<T> z(makeTJL(_myEnv) );  

  z->_std  = std::exp( _std );

  for( int i=0; i <_count-1; ++i ) { 
    z->_jcb[i] = _std * _jcb[i]; 
  }

  return z;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> TJL1<T>::pow(const double& s )  const
{

  JL1Ptr<T> z( makeTJL(_myEnv) );  

  z->_std = std::pow( _std, s );

  for( int i=0; i<_count-1;  ++i) { 
    z->_jcb[i] = _jcb[i] * s * std::pow( _std, s-1 ); 
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

  if( n == 0 ) z =  JL1Ptr<T>( makeTJL(_myEnv, (T) 1.0 ) );
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

 if( std::abs(_std) <= 0.0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJL1::log() const",
           "Non-positive standard part.") );
  }

  JL1Ptr<T> z(makeTJL(_myEnv) );

  z->_std = std::log( _std );

  for( int i=0; i<_count-1; ++i ) { 

    z->_jcb[i] = ( 1.0 / _std  ) * _jcb[i]; 

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
 for( int i = 0; i < _myEnv->numVar(); ++i ) {
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

 JL1Ptr<T> z( makeTJL(_myEnv) );
 if( 1 == sum ) { z->_std = _jcb[k]; }
 return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
void TJL1<T>::printCoeffs() const {

 (*pcout) << "\nCount  = " << _count 
      << ", Weight = " <<      _weight 
      << ", Max accurate weight = " << _accuWgt << std::endl;
 (*pcout) << "Reference point: " 
      << _myEnv->getRefPoint()[0];

 for( int i=0; i < _count-1; i++ ) {
   (*pcout) << ", ";
   (*pcout) << _myEnv->getRefPoint()[i];
 }

 (*pcout) << std::endl;

 IntArray index(_count-1);

 (*pcout) << "Index:  " 
        << index
        << "   Value: "
        << _std
        << std::endl;

 for( int i=0; i < _count-1; ++i ) {
  
  if( _weight > _accuWgt ) break;
 
  index(i) = 1; 
  (*pcout) << "Index:  " 
        << index
        << "   Value: "
        << _jcb[i]
        << std::endl;
  index(i) = 0; 
  }
  (*pcout) << "\n" << std::endl;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
//    **** Iteration routines **** 
//------------------------------------------------------------ 

template<typename T>
void TJL1<T>::resetConstIterator() 
{

  _constIterPtr = 0;

 (*pcout) << "void TJL1<T>::resetConstIterator() called." << std::endl;
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

 os << "\n Count = " << x._count << " , Weight = " << x._weight;
 os << " , Max accurate weight = " << x._accuWgt << std::endl;

 if (x._myEnv) {
 os << "Begin Environment: \n"
    << *(x._myEnv)
    << "\nEnd Environment: " << std::endl;
 }
 else 
 os << "WARNING: null environment ! "<< std::endl; 
 
 IntArray index(x._count-1);

 os << "Index: " << index << "   Value: " << x._std << std::endl;  

 for (int i=0; i < x._count-1;  ++i) {
   index(i) = 1;
   os << "Index: " << index <<"   Value: " << x._jcb[i] << std::endl;  
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
  is >> x._weight;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> x._accuWgt;
  is >> buf;
  is >> buf;
  streamIn( is, x._myEnv);
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
 if( x._myEnv != y._myEnv ) return false;

 if( !( x._std == y._std ) ) return false;

 for(  int i=0; i< x._count-1; ++i) {
    for(  int j=0; j<y._count-1; ++j) {
      if( !( x._jcb[i] == y._jcb[i] ) ) return false;
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
 result = result && ( x._std == y );
 for( int i=0; i < x._myEnv->numVar(); i++ ) {
  result = result && ( x._jcb[i] == T() );
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

  if( x->_myEnv != y->_myEnv ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::operator+(JL1Ptr<T> const& x)"
           "Inconsistent environments." ) );
  }

 JL1Ptr<T> z(TJL1<T>::makeTJL( x->_myEnv ) );

 z->_std  = x->_std +  y->_std;

 for (int i=0; i< z->_myEnv->numVar(); ++i ) {

   z->_jcb[i] = x->_jcb[i] +  y->_jcb[i];
 
 } 

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JL1Ptr<T>& operator+=(JL1Ptr<T> &x,      JL1Ptr<T> const& y  ){

 x->_std += y->_std;
 for (int i=0; i< x->_count-1; ++i) {

    x->_jcb[i] += y->_jcb[i];

 } 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JL1Ptr<T> operator-(JL1Ptr<T> const& x, JL1Ptr<T> const& y ) {

// Check for consistency and set reference point of the sum.

  if( x->_myEnv != y->_myEnv ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::operator-(JL1Ptr<T> const& x)"
           "Inconsistent environments." ) );
  }

 JL1Ptr<T> z(TJL1<T>::makeTJL( x->_myEnv ) );

 z->_std  = x->_std -  y->_std;

 for (int i=0; i< z->_myEnv->numVar(); ++i ) {

   z->_jcb[i] = x->_jcb[i] -  y->_jcb[i];
 
 } 

 return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JL1Ptr<T> operator-(JL1Ptr<T> const& x) {

 JL1Ptr<T> z(TJL1<T>::makeTJL( x->_myEnv ));

 z->_std  = - x->_std;

 for (int i=0; i< z->_myEnv->numVar(); ++i ) {

   z->_jcb[i] = -x->_jcb[i];
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

 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL1Ptr<T> operator*( const JL1Ptr<T>&, const JL1Ptr<T>& ) ",
          "Inconsistent environments." ) );
 };

 JL1Ptr<T> z( TJL1<T>::makeTJL(x->_myEnv) );

 z->_std  = x->_std * y->_std;

 for( int i=0; i< z->_myEnv->numVar();  ++i) { 

   z->_jcb[i]  = ( x->_std * y->_jcb[i] ) + ( y->_std * x->_jcb[i] ); 

 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JL1Ptr<T> operator/( T const& wArg, JL1Ptr<T> const& uArg ) 
{ 

 // Check for void operators ..

 if ( uArg->_count < 1 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::operator/( JL1Ptr<T> const& wArg, JL1Ptr<T> const& uArg )",
          "Attempt to divide by a null TJL1<T> variable." ) );
 }
 
 JL1Ptr<T>  z( TJL1<T>::makeTJL(uArg->_myEnv) );

 z->_std =  wArg/(uArg->_std);

 for( int i=0; i < z->_myEnv->numVar(); ++i) { 

   z->_jcb[i] =   - ( wArg * uArg->_jcb[i] ) / ( uArg->_std * uArg->_std ); 

 };
 
 return z;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JL1Ptr<T> operator/( JL1Ptr<T> const& wArg, JL1Ptr<T> const& uArg ) 
{ 

 if ( (wArg->_myEnv) != (uArg->_myEnv) ) {

   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/( JL1Ptr<T> const& wArg, JL1Ptr<T> const& uArg )",
          "Inconsistent environments." ) );
 }

 // Check for void operators ..

 if ( uArg->_count < 1 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::operator/( JL1Ptr<T> const& wArg, JL1Ptr<T> const& uArg )",
          "Attempt to divide by a null TJL1<T> variable." ) );
 }
 
 JL1Ptr<T>  z( TJL1<T>::makeTJL(wArg->_myEnv) );


 z->_std =  (wArg->_std)/(uArg->_std);

 for( int i = 0; i < z->_myEnv->numVar(); i++ ) { 

   z->_jcb[i] = ( ( uArg->_std * wArg->_jcb[i] ) - ( wArg->_std * uArg->_jcb[i] ) )
                / ( uArg->_std* uArg->_std ); 

 };
 
 return z;

}



#endif // TJL1_TCC


