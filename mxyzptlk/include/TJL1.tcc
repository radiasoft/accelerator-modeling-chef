/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJL1.cc
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
#include <iosetup.h>

#include <utils.h> // misc utils: nexcom(), bcfRec(), nearestInteger() ...  
#include <GenericException.h>
#include <TJL1.h>

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
TJL1<T>::TJL1(typename EnvPtr<T>::Type pje, T x): 
_count( pje->numVar()+1 ),
_weight(1),                        
_accuWgt( pje->maxWeight() ),
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
TJL1<T>* TJL1<T>::makeTJL( typename EnvPtr<T>::Type pje, T x )
{

  if (!pje) return 0; // this form is called by TCoord and Tparam

  if (_thePool.empty() ) return ( new  TJL1<T>(pje, x )); 
 
  TJL1<T>* p   = _thePool.back();  _thePool.pop_back();

  if ( p->_count  != pje->numVar()+1 ) {
      
      delete [] p->_jcb; 
      p->_jcb   = new T[ pje->numVar() ];
      p->_count =  pje->numVar()+1;
  }
 
  p->_weight   = 1;
  p->_accuWgt  = pje->maxWeight();
  p->_myEnv    = pje; 


 return p;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::TJL1( const IntArray& e, const T& x, typename EnvPtr<T>::Type pje ) :
 _count(  pje->numVar()+1 ),
 _weight(1),
 _accuWgt(pje->maxWeight() ),
 _myEnv(pje) 
{

  _jcb = new T [ _count-1 ];
 
  for (int i=0; i<_count-1; ++i) { _jcb[i] = T(); }

  int i =0; 
  for (i=0; i<_count-1; ++i) {
     if ( e(i) == 1){ 
       _jcb[i] = x;  
       break;
     }
  } 
 
  if (i==_count-1) _std = x;   

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>* TJL1<T>::makeTJL( const IntArray& e, const T& x, typename EnvPtr<T>::Type pje ) 
{

  if (!pje) return 0;  // cannot create a TJL without a properly constructed environemnt 

  if (_thePool.empty() ) return new TJL1<T>(e,x,pje);

  TJL1<T>* p = _thePool.back(); _thePool.pop_back();

  if (p->_count  != pje->numVar()+1) {
      delete [] p->_jcb; 
      p->_jcb   = new T[ pje->numVar() ];
      p->_count =  pje->numVar()+1;
  }
 
  p->_weight   = 1;
  p->_accuWgt  = pje->maxWeight();
  p->_myEnv    = pje; 

  int i =0; 
  for (i=0; i < p->_count-1; ++i) {
     if ( e(i) == 1){ 
       p->_jcb[i] = x;  
       break;
     }
  } 
 
  if ( i == (p->_count-1) ) p->_std = x;   

  return p;
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

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>* TJL1<T>::makeTJL( const TJL1& x )
{

  if (_thePool.empty() ) return new TJL1<T>(x);
 
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

 return p;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::~TJL1() 
{

 delete _jcb;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::discardTJL( TJL1<T>* p) 
{
  
   typename EnvPtr<T>::Type nullEnv;
   p->_myEnv = nullEnv;   // nullify the environment. 
   _thePool.push_back(p); 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//     Member functions(public)  |||||||||||||||||||||||||||||||


template<typename T>
void TJL1<T>::getReference( T* r ) const 
{

 for( int i=0;  i<_count; ++i ) r[i] = _myEnv->refPoint()[i];

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
void TJL1<T>::setVariable(  const T& x, const int& j, typename EnvPtr<T>::Type pje)
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
  
 _std   = x;
 _jcb[j]= T (1.0);
 
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
 _weight = 1;

 _std   = x;
 _jcb[j]= T (1.0);

 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
template<typename T>
void TJL1<T>::setVariable( const int& j, 
                              typename EnvPtr<T>::Type theEnv ) 
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

 _myEnv = theEnv;
 
 _accuWgt =  theEnv->maxWeight();
 _weight = 1;

 T x = theEnv->refPoint()[j];

 _std    = x;
 _jcb[j] = (T) 1.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::insert( const TJLterm<T>& a) 
{

 // if (a._weight > 1 ) throw GenericException
 
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
  

 return 0;

}

#endif

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if  0
template<typename T>
TJLterm<T> TJL1<T>::lowTerm() const 
{
 
 // returns the lowest order **non-zero**  term;  Returns 0.0 if all terms are zero.  

 TJLterm<T>* p =0;
 for( p = _jltermStore; p < _jltermStoreCurrentPtr; ++p  ) {
  if ( p->_value != T() ) break;
 }

 if     ( p == _jltermStoreCurrentPtr ) return TJLterm<T>( IntArray(this->_myEnv->numVar()), T(), this->_myEnv );
 else  return TJLterm<T>( *p );
}
#endif

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
template<typename T>
TJLterm<T> TJL1<T>::firstTerm() const 
{

 // returns the first term i.e. the lowest order. Note that this term may have a zero coefficient;

 return _jltermStore[0];
 

}
#endif

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0 

template<typename T>
void TJL1<T>::append(const TJLterm<T>& a) 
{

 for (int i=0; i< a._index.Dim(); ++i) { 
  if (a._index(i) != 0) _jltermStore[i] = a;
 }
 _weight  = max(_weight, a._weight);

}
#endif


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
 for( int i=0; i< _myEnv->numVar(); i++ ) {
  k = ind[i];
  sum += k;
  if( 1 == k ) { j = i; }
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
typename JL1Ptr<T>::Type TJL1<T>::compose( typename JL1Ptr<T>::Type const y[ ]) const 
{ 

 typename JL1Ptr<T>::Type u[_myEnv->numVar()];

 // Check consistency of reference points and
 // subtract reference point prior to concatenation.

 for( int i=0;  i< _myEnv->numVar(); ++i ) {
   if(  (y[i]->_myEnv) != (y[0]->_myEnv) ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJL1::compose(typename JL1Ptr<T>::Type const y[ ]) const ",
            "Inconsistent environments." ));
   }

   u[i] = y[i] + typename JL1Ptr<T>::Type( makeTJL( y[0]->_myEnv, -_myEnv->getRefPoint()[i]));   // u[i] = y[i] - _myEnv->getRefPoint()[i];
 }

 typename JL1Ptr<T>::Type z( makeTJL(y[0]->_myEnv) );
 
 z->_std  =  _std;                                  

 for( int i=0; i <_count-1; ++i ) {
 
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
typename JL1Ptr<T>::Type TJL1<T>::filter( const int& wgtLo, const int& wgtHi ) const 
{ 

 if( wgtLo <= 0 && wgtHi >= 1 ) { return *this; }
 if( wgtHi < wgtLo ) {
   return typename JL1Ptr<T>::Type( makeTJL(_myEnv) );
 }

 typename JL1Ptr<T>::Type z( makeTJL(*this) );

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
typename JL1Ptr<T>::Type TJL1<T>::filter( bool (*f) ( const IntArray&, const T& ) ) const 
{ 

 typename JL1Ptr<T>::Type z(makeTJL(_myEnv) );

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
std::ostream& operator<<( std::ostream& os, const TJL1<T>& x ) 
{

 os << "\n Count = " << x._count << " , Weight = " << x._weight;
 os << " , Max accurate weight = " << x._accuWgt << std::endl;
 os << "Begin Environment: \n"
    << *(x._myEnv)
    << "\nEnd Environment: " << std::endl;

 return os << "\n" << std::endl;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const TJL1<T>& x, const TJL1<T>& y ) 
{
 if( x._myEnv != y._myEnv ) return false;

 if( !( x->_std == y->_std ) ) return false;

 for(  int i=0; i< x._count-1; ++i) {
    for(  int j=0; j<y._count-1; ++j) {
      if( !( x->_jcb[i] == y->_jcb[i] ) ) return false;
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
void TJL1<T>::add( typename JL1Ptr<T>::Type const& x) {

   _std += x->_std;

 for (int i=0; i< _myEnv->numVar(); ++i ) {

   _jcb[i] += x->_jcb[i];
 
 } 

 return;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename boost::intrusive_ptr<TJL1<T> > operator+(boost::intrusive_ptr<TJL1<T> > const& x, boost::intrusive_ptr<TJL1<T> > const& y ) {

// Check for consistency and set reference point of the sum.

  if( x->_myEnv != y->_myEnv ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::operator+(typename JL1Ptr<T>::Type const& x)"
           "Inconsistent environments." ) );
  }

 typename JL1Ptr<T>::Type z(TJL1<T>::makeTJL( x->_myEnv ) );

 z->_std  = x->_std +  y->_std;

 for (int i=0; i< z->_myEnv->numVar(); ++i ) {

   z->_jcb[i] = x->_jcb[i] +  y->_jcb[i];
 
 } 

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
boost::intrusive_ptr<TJL1<T> > operator-(boost::intrusive_ptr<TJL1<T> > const& x, boost::intrusive_ptr<TJL1<T> > const& y ) {

// Check for consistency and set reference point of the sum.

  if( x->_myEnv != y->_myEnv ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::operator-(typename JL1Ptr<T>::Type const& x)"
           "Inconsistent environments." ) );
  }

 typename JL1Ptr<T>::Type z(TJL1<T>::makeTJL( x->_myEnv ) );

 z->_std  = x->_std -  y->_std;

 for (int i=0; i< z->_myEnv->numVar(); ++i ) {

   z->_jcb[i] = x->_jcb[i] -  y->_jcb[i];
 
 } 

 return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
boost::intrusive_ptr<TJL1<T> > operator-(boost::intrusive_ptr<TJL1<T> > const& x) {

 typename JL1Ptr<T>::Type z(TJL1<T>::makeTJL( x->_myEnv ));

 z->_std  = - x->_std;

 for (int i=0; i< z->_myEnv->numVar(); ++i ) {

   z->_jcb[i] = -x->_jcb[i];
 } 

 return z;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
boost::intrusive_ptr<TJL1<T> > operator*(boost::intrusive_ptr<TJL1<T> > const& x, T const& y ) {

//---------------------------
// Multiplication by a scalar
//---------------------------

 typename JL1Ptr<T>::Type z(TJL1<T>::makeTJL( *x ) ); // deep copy 
 z->scaleBy(y);
 
 return z;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
boost::intrusive_ptr<TJL1<T> > operator*(boost::intrusive_ptr<TJL1<T> > const& x, boost::intrusive_ptr<TJL1<T> > const& y ) {

//----------------------------
// Multiplication of two jets
//----------------------------

 // Consistency check 

 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL1Ptr<T> operator*( const JL1Ptr<T>&, const JL1Ptr<T>& ) ",
          "Inconsistent environments." ) );
 };

 typename JL1Ptr<T>::Type z( TJL1<T>::makeTJL(x->_myEnv) );

 z->_std  = x->_std * y->_std;

 for( int i=0; i< z->_myEnv->numVar();  ++i) { 

   z->_jcb[i]  = ( x->_std * y->_jcb[i] ) + ( y->_std * x->_jcb[i] ); 

 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
boost::intrusive_ptr<TJL1<T> > operator/( T const& wArg, boost::intrusive_ptr<TJL1<T> > const& uArg ) 
{ 

 // Check for void operators ..

 if ( uArg->_count < 1 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::operator/( typename JL1Ptr<T>::Type const& wArg, typename JL1Ptr<T>::Type const& uArg )",
          "Attempt to divide by a null TJL1<T> variable." ) );
 }
 
 typename JL1Ptr<T>::Type  z( TJL1<T>::makeTJL(uArg->_myEnv) );

 z->_std =  wArg/(uArg->_std);

 for( int i=0; i < z->_myEnv->numVar(); ++i) { 

   z->_jcb[i] =   - ( wArg * uArg->_jcb[i] ) / ( uArg->_std * uArg->_std ); 

 };
 
 return z;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
boost::intrusive_ptr<TJL1<T> > operator/( boost::intrusive_ptr<TJL1<T> > const& wArg, boost::intrusive_ptr<TJL1<T> > const& uArg ) 
{ 

 if ( (wArg->_myEnv) != (uArg->_myEnv) ) {

   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/( typename JL1Ptr<T>::Type const& wArg, typename JL1Ptr<T>::Type const& uArg )",
          "Inconsistent environments." ) );
 }

 // Check for void operators ..

 if ( uArg->_count < 1 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL1<T>::operator/( typename JL1Ptr<T>::Type const& wArg, typename JL1Ptr<T>::Type const& uArg )",
          "Attempt to divide by a null TJL1<T> variable." ) );
 }
 
 typename JL1Ptr<T>::Type  z( TJL1<T>::makeTJL(wArg->_myEnv) );


 z->_std =  (wArg->_std)/(uArg->_std);

 for( int i = 0; i < z->_myEnv->numVar(); i++ ) { 

   z->_jcb[i] = ( ( uArg->_std * wArg->_jcb[i] ) - ( wArg->_std * uArg->_jcb[i] ) )
                / ( uArg->_std* uArg->_std ); 

 };
 
 return z;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename JL1Ptr<T>::Type TJL1<T>::sin() const
{ 

  typename JL1Ptr<T>::Type z( makeTJL(_myEnv) ); 

  z->_std = std::sin( _std );

  for( int i=0; i<_myEnv->numVar();  ++i) { 

    z->_jcb[i]  = std::cos( _std ) * _jcb[i];

  }
  return z;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename JL1Ptr<T>::Type TJL1<T>::cos() const
{ 

 typename JL1Ptr<T>::Type z( makeTJL(_myEnv) ); 

  z->_std = std::cos( _std );

  for( int i=0; i<_myEnv->numVar();  ++i) { 

    z->_jcb[i]  = -std::sin( _std ) * _jcb[i];

  }
  return z;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 template<typename T>
 typename JL1Ptr<T>::Type TJL1<T>::exp() const
 {

  typename JL1Ptr<T>::Type z(makeTJL(_myEnv) );  

  z->_std  = std::exp( _std );

  for( int i=0; i <_myEnv->numVar(); ++i ) { 
    z->_jcb[i] = _std * _jcb[i]; 
  }

  return z;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename JL1Ptr<T>::Type TJL1<T>::pow(const double& s )  const
{

  typename JL1Ptr<T>::Type z( makeTJL(_myEnv) );  

  z->_std = std::pow( _std, s );

  for( int i=1; i< _count;  ++i) { 
    z->_jcb[i] = _jcb[i] * s * std::pow( _std, s-1 ); 
  }

  return z;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename JL1Ptr<T>::Type TJL1<T>::pow(int n ) const
{

  typename JL1Ptr<T>::Type z;  
  typename JL1Ptr<T>::Type x (makeTJL(*this) );  
  
  int i = 0;

  if( n == 0 ) z =  typename JL1Ptr<T>::Type( makeTJL(_myEnv, (T) 1.0 ) );
  else if( n > 0 ) {
    z = typename JL1Ptr<T>::Type( makeTJL(*this) );
    for( i = 2; i <= n; i++ ) z = x*z;
  }
  else {
    typename JL1Ptr<T>::Type xr; 
    z  = (T)1.0 / x ;
    xr = z;
    for( i = -2; i >= n; i-- ) z = z*xr;
  }

  return z;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
typename JL1Ptr<T>::Type TJL1<T>::log() const
{

 if( std::abs(_std) <= 0.0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJL1::log() const",
           "Non-positive standard part.") );
  }

  typename JL1Ptr<T>::Type z(makeTJL(_myEnv) );

  z->_std = std::log( _std );

  for( int i=0; i<_myEnv->numVar(); ++i ) { 

    z->_jcb[i] = ( 1.0 / _std  ) * _jcb[i]; 

  }
  return z;
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
  streamIn( is, &(x.getEnv()));
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
typename JL1Ptr<T>::Type TJL1<T>::asin() const
{ 


typename JL1Ptr<T>::Type z( makeTJL(*this) );
typename JL1Ptr<T>::Type x( makeTJL(*this) );
typename JL1Ptr<T>::Type tmp = operator-<T>(z - x);
typename JL1Ptr<T>::Type dz = ( z->sin() - x ) / ( z->cos() );

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
typename JL1Ptr<T>::Type TJL1<T>::atan() const
{   
 
 typename JL1Ptr<T>::Type z( makeTJL(*this) );
 typename JL1Ptr<T>::Type x( makeTJL(*this) );
 typename JL1Ptr<T>::Type c  = -z->cos();
 typename JL1Ptr<T>::Type dz = c *( z->sin() +  (x*c) );

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
typename JL1Ptr<T>::Type TJL1<T>::D( const int* n ) const 
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

 typename JL1Ptr<T>::Type z( makeTJL(_myEnv) );
 if( 1 == sum ) { z->_std = _jcb[k]; }
 return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
void TJL1<T>::printCoeffs() const {

 std::cout << "\nCount  = " << _count 
      << ", Weight = " <<      _weight 
      << ", Max accurate weight = " << _accuWgt << std::endl;
 std::cout << "Reference point: " 
      << _myEnv->getRefPoint()[0];
 for( int i=1; i < _myEnv->numVar(); i++ ) {
   std::cout << ", ";
   std::cout << _myEnv->getRefPoint()[i];
 }
 std::cout << std::endl;

#if 0
 while((  p = (TJLterm<T>*) getNext()  )) {
   if( p->_weight > _accuWgt ) break;
   std::cout << "Index:  " 
        << p->_index
        << "   Value: "
        << p->_value
        << std::endl;
 }

 std::cout << "\n" << std::endl;
#endif

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
//    **** Iteration routines **** 
//------------------------------------------------------------ 

#if  0


template<typename T>
void TJL1<T>::resetConstIterator() 
{

  _constIterPtr = 0;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJL1<T>::stepConstIterator()  const
{
    return _jltermStore[_constIterPtr]; 
    ++_constIterPtr;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const TJLterm<T>& TJL1<T>::stepConstIteratorRef()  const
{
    return _jltermStore[_constIterPtr]; 
    ++_constIterPtr;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const TJLterm<T>* TJL1<T>::stepConstIteratorPtr()  const
{

    return &_jltermStore[_constIterPtr]; 
    ++_constIterPtr;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL1<T>::resetIterator()
{

  _iterPtr = 0;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>* TJL1<T>::stepIterator()
{
    return &_jltermStore[_constIterPtr]; 
    ++_constIterPtr;

}


#endif

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

template<typename T>
TJL1<T>::HorribleException::HorribleException( int i, int j, int k, 
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: curIndex(i), coordSize(j), valueSize(k)
{
  std::ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** A horrible, inexplicable error    "
         "\n*** ERROR *** has occurred. This is beyond      "
         "\n*** ERROR *** the realm of human understanding. "
         "\n*** ERROR *** Please consult an exorcist.       "
         "\n*** ERROR *** _currentIndex:      " << curIndex
      << "\n*** ERROR *** _newCoords.size() : " << coordSize
      << "\n*** ERROR *** _newValues.size() : " << valueSize
      << "\n*** ERROR *** " << msg
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
const char* TJL1<T>::HorribleException::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL1<T>::HideousException::HideousException( int x, int y, 
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: i(x), n(y)
{
  std::ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** An unbelievably hideous error     "
      << "\n*** ERROR *** has occurred.                     "
      << "\n*** ERROR *** " << i << " != " << n << "        "
      << "\n*** ERROR ***               This is beyond      "
      << "\n*** ERROR *** the realm of human understanding. "
      << "\n*** ERROR *** " << msg
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
const char* TJL1<T>::HideousException::what() const throw()
{
  return errorString.c_str();
}

#endif // TJL1_TCC


