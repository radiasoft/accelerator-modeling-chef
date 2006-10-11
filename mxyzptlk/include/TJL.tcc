/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJL.cc
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             Batavia, IL   60510                                
******             Email: michelotti@fnal.gov                         
******
******  Revision History:
******
******  Feb 2005 - Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******  
******  - Efficiency improvements.
******  - new memory management scheme
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
****** Sep 2006  ostiguy@fnal.gov
******
****** -  eliminated doubly linked list representation for polynomials
****** -  _count now represents the no of stored monomials with weight > 1 
****** -  getCount() returns the no of monomials with a non-zero coefficient     
****** -  non-sparse representation for linear terms 
****** -  efficiency improvements taking advantage of the above
****** -  some members inlined on the basis of profiling    
****** 
**************************************************************************
*************************************************************************/
#ifndef TJL_TCC
#define TJL_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <complex>
#include <iomanip>
#include <fstream>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/utils.h>             // misc utils: nexcom(), bcfRec(), nearestInteger() ...  
#include <basic_toolkit/GenericException.h>
#include <mxyzptlk/TJLIterator.h>
#include <boost/scoped_ptr.hpp>


using FNAL::pcerr;
using FNAL::pcout;


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//  static data members
//------------------------------------------------------------------------

template<typename T>
const double TJL<T>::_mx_small   = 1.0e-12;      //  abs coeff threshold for removal of a JLterm

template<typename T>
const int TJL<T>::_mx_maxiter = 100;          // Maximum number of iterations allowed  
                                              //  in iterative routines (e.g. transcendental functions)

// ******  a global container for discarded Jets. 

template <typename T> 
std::vector<TJL<T>* >& TJL<T>::_thePool = * ( new std::vector<TJL<T>* > ); 



// ================================================================
//      Implementation of exceptions
//------------------------------------------------------------------

template<typename T>
TJL<T>::BadDimension::BadDimension( int i, int j, 
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
const char* TJL<T>::BadDimension::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::BadEnvironment::BadEnvironment( int a, int b, int c, 
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
const char* TJL<T>::BadEnvironment::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::BadReference::BadReference( int ndx, double u,
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
const char* TJL<T>::BadReference::what() const throw()
{
  return errorString.c_str();
}


// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Implementation of Class TJL
// 
// 




//    Constructors and destructors    |||||||||||||||||||||||||||



template<typename T>
void TJL<T>::initStore( int capacity) {

  _jltermStoreCapacity    = capacity;
  _jltermStore            =  TJLterm<T>::array_allocate( _jltermStoreCapacity ); 
  _jltermStoreCurrentPtr  = _jltermStore;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL(EnvPtr<T> const& pje, T x): 
_myEnv( pje ),  
_count(0),
_weight(0),                        
_accuWgt( pje->maxWeight() ),
_lowWgt(0)
{
   initStore( pje->numVar() +1 );    // max no of terms at order 1  
   appendLinearTerms( pje->numVar());
   setStandardPart(x);


}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::makeTJL( EnvPtr<T> const& pje, T x )
{

  if (!pje) return 0; // this form is called by TCoord and Tparam

  if (_thePool.empty() ) return (  JLPtr<T>(new  TJL<T>(pje, x ) )); 
 
  TJL<T>* p    = _thePool.back();  _thePool.pop_back();
  p->_count    = 0;
  p->_weight   = 0;
  p->_accuWgt  = pje->maxWeight();
  p->_lowWgt   = 0;
  p->_myEnv    = pje; 

  p->appendLinearTerms( pje->numVar());
  p->setStandardPart(x); 

  return  JLPtr<T>(p);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL( IntArray const& e, const T& x, EnvPtr<T> const& pje ) :
 _myEnv(pje), 
 _count(0),
 _weight(0),
 _accuWgt(pje->maxWeight() ),
 _lowWgt(0),
 _jltermStoreCapacity(0), 
 _jltermStore(0), 
_jltermStoreCurrentPtr(0)
{

 initStore( pje->numVar() +1 );    // max no of terms at order 1  
 appendLinearTerms( pje->numVar() );
 
 TJLterm<T> term(e, x, pje);  

 int indy = 0;

 if (term._weight == 0) {  
  _jltermStore[0]._value = x;
  return;
 }
   
 if (term._weight == 1) {  
   indy = _myEnv->offsetIndex(term._index); 	 
   _jltermStore[indy]._value = x; 
   return; 
 }
 
 if ( term._weight > 1 )   
   append( TJLterm<T>(term) ); 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

template<typename T>
 JLPtr<T> TJL<T>::makeTJL( const IntArray& e, const T& x, EnvPtr<T> const& pje ) 
{

  if (!pje) return 0;  // cannot create a TJL without a properly constructed environemnt 

  if (_thePool.empty() ) return  JLPtr<T>( new TJL<T>(e,x,pje) );

  TJL<T>* p = _thePool.back(); _thePool.pop_back();

  p->_count     = 0;
  p->_weight    = 0;
  p->_accuWgt   = pje->maxWeight();
  p->_lowWgt    = 0;
  p->_myEnv     = pje; 


  TJLterm<T> term(e, x, pje);
  int indy = 0;

  p->appendLinearTerms( pje->numVar() );

  if (term._weight == 0) {  
   p->_jltermStore[0]._value = x;
   return JLPtr<T>(p);
  }
   
  if (term._weight == 1) {  
    indy = pje->offsetIndex(term._index); 	 
    p->_jltermStore[indy]._value = x; 
    return  JLPtr<T>(p);
  }
 
  if ( term._weight > 1)   
   p->append( term ); 

  return JLPtr<T>(p);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL( TJL<T> const& x ): ReferenceCounter<TJL<T> >(),  
 _myEnv(x._myEnv),
 _count(x._count),                     
 _weight(x._weight),     
 _accuWgt(x._accuWgt),
 _lowWgt(x._lowWgt),
 _jltermStore(0), 
 _jltermStoreCurrentPtr(0), 
 _jltermStoreCapacity(0)
{
 
 initStore( x._jltermStoreCapacity );
 memcpy( _jltermStore, x._jltermStore, (x._jltermStoreCurrentPtr-x._jltermStore)*sizeof(TJLterm<T>) );
 _jltermStoreCurrentPtr = _jltermStore + (x._jltermStoreCurrentPtr - x._jltermStore);


} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::~TJL() 
{
 clear();
 TJLterm<T>::array_deallocate( _jltermStore );

 _jltermStore         = _jltermStoreCurrentPtr = 0;
 _jltermStoreCapacity = 0;
 

 // _myEnv is purposely NOT deleted, of course.
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::growStore( ) {
 
TJLterm<T>* old_jltermStore           = 0;
TJLterm<T>* old_jltermStoreCurrentPtr = 0;
int             old_jltermStoreCapacity  = 0;


  // grow the allocation vector ....

  // save the current store ptrs 

    old_jltermStore           = _jltermStore;
    old_jltermStoreCurrentPtr = _jltermStoreCurrentPtr;
    old_jltermStoreCapacity   = _jltermStoreCapacity;

    _jltermStoreCapacity *= 2;  // double the capacity
    _jltermStore = _jltermStoreCurrentPtr = TJLterm<T>::array_allocate( _jltermStoreCapacity );
 

    memcpy( _jltermStore, old_jltermStore, (old_jltermStoreCurrentPtr-old_jltermStore)*sizeof(TJLterm<T>) );
    _jltermStoreCurrentPtr = _jltermStore + (old_jltermStoreCurrentPtr - old_jltermStore);


    // now OK to zap the old store
 
    TJLterm<T>::array_deallocate( old_jltermStore );

}   

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//     Member functions(public)  |||||||||||||||||||||||||||||||


template<typename T>
int TJL<T>::getCount() const { 

  // returns the true term count 

  
  int count = _count;

  int lterms = _myEnv->numVar(); 
  for (TJLterm<T> const * p =_jltermStore;  p < _jltermStore + lterms +1; ++p ) {

       if (p->_value != T() ) ++count;   

  }   
  return count; 

}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::removeTerm( TJLterm<T> const& a) 
{

  for ( TJLterm<T> * p = _jltermStore; p < _jltermStoreCurrentPtr; ++p) { 
    
     _weight = 0; 

      if ( p->_index == a._index ) {
         p->_value = T(); 
         continue;  // ignores the line below when term has been removed
       }
        
  _weight  = std::max(_weight,  p->_weight);

  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::addTerm( TJLterm<T> const& a )
{

 int indy = 0;
 TJLterm<T>* const  tjlmml = _myEnv->TJLmml(); 

 //------------------------
 // transfer to scratchpad
 //------------------------

 for (TJLterm<T> const* p = _jltermStore; p < _jltermStoreCurrentPtr; ++p) { 
 
    indy = _myEnv->offsetIndex(p->_index); 	 
    tjlmml[indy]._value = p->_value; 	 
 } 	 
 

 // add the term argument to value in scratch pad
 //----------------------------------------------

 indy = _myEnv->offsetIndex( a._index );
 tjlmml[indy]._value += a._value;

 
 // -----------------------------------------------------------
 // clear current TJL , transfer back result from scratch pad,  
 // and cleanup
 //------------------------------------------------------------

 transferFromScratchPad();   

 return;
 
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::getReference( T* r ) const 
{

 for( int i=0;  i < _myEnv->numVar(); ++i ) r[i] = _myEnv->refPoint()[i];
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJL<T>::isNilpotent() const 
{

 if ( std::abs(standardPart()) > _mx_small ) 
   return false;
 else
   return true; 
  
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::writeToFile( std::ofstream& outStr ) const 
{

 for ( TJLterm<T> const* p = _jltermStore; p < _jltermStoreCurrentPtr; ++p) { 

   outStr << "Weight: " << p -> _weight
          << "   Value: " << p -> _value
          << " || ";
   outStr << "Addresse: " 
          <<  ((int) p)
          << std::endl;
   outStr << "Index:  ";

   for( int i=0; i < _myEnv->numVar(); ++i ) {
     outStr << ((p->_index)(i)) << "  ";
   }
   outStr << std::endl;
   }
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::peekAt() const {

 (*pcout) << "\nCount  = "
      << getCount()
      << " Weight = "
      << _weight
      << " Max accurate weight = "
      << _accuWgt
      << std::endl;
 (*pcout) << "Reference point: ";

 for( int i= 0; i < getEnv()->numVar(); ++i) 
    (*pcout) << std::setprecision(12) << getEnv()->getRefPoint()[i] << "  ";

 (*pcout) << std::endl;

 for ( TJLterm<T> const* p = _jltermStore; p < _jltermStoreCurrentPtr; ++p) { 

   (*pcout) << "Weight: "
        << p -> _weight
        << "   Value: "
        << p -> _value
        << " || ";
   (*pcout) << "Address: "
        << (int) p            << "  "
        << std::endl;
   (*pcout) << "Index:  " << (p -> _index) << std::endl;

 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::scaleBy( T y ) 
{ 

 for ( TJLterm<T>* p = _jltermStore; p < _jltermStoreCurrentPtr; ++p) {
   p->_value *= y;               
 }

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJL<T>& TJL<T>::Negate( ) {

 for(  TJLterm<T>* p = _jltermStore; p< _jltermStoreCurrentPtr; ++p  ) {
   p->_value = -p->_value;
 }
 return *this;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::setVariable(  const T& x, const int& j, EnvPtr<T> const& pje)
{
 
// this member function is meant to be called **ONLY** when a coordinate is instantiated

 if( ( j < 0 ) || ( j >= _myEnv->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const T&, const int&, TJetEnvironment<T>* )", 
     "Index out of range") );
 }
 
  clear(); 

 _myEnv = pje;
 _accuWgt = pje->maxWeight();
 _lowWgt  = 0;
 _weight  = 0;
  
 appendLinearTerms(  _myEnv->numVar() );
 setStandardPart(x);
 _jltermStore[j+1]._value =  T(1.0);

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::setVariable(  const T& x, const int& j )
{
 
// this member function is meant to be called **ONLY** when a coordinate is instantiated

 if( ( j < 0 ) || ( j >= _myEnv->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const T&, const int&, TJetEnvironment<T>* )", 
     "Index out of range") );
 }
 
  clear(); 
 _accuWgt =  _myEnv->maxWeight();
 _lowWgt  =  0;
 _weight  =  0;


 appendLinearTerms(  _myEnv->numVar() );
 setStandardPart(x);
 _jltermStore[j+1]._value =  T(1.0);

 }

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
template<typename T>
void TJL<T>::setVariable( const int& j, 
                              EnvPtr<T> const& theEnv ) 
{

 if( _myEnv == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const int&, TJetEnvironment<T>* )", 
     "Private data _myEnv is null: object has no environment assigned.") );
 }

 if( ( j < 0 ) || ( j >= _myEnv->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const int&, TJetEnvironment<T>* )", 
     "Index out of range") );

 }

 _myEnv = theEnv;
 
  clear();
 _accuWgt =  theEnv->maxWeight();
 _lowWgt  =  0;
 _weight   = 0;

 T x = theEnv->refPoint()[j];  
 
 appendLinearTerms(  _myEnv->numVar() );
 setStandardPart(x);
 _jltermStore[j+1]._value =  T(1.0);

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::clear() 
{
 
 _weight = 0;
 _count  = 0;
 _jltermStoreCurrentPtr  = _jltermStore; // do not delete the store here !
 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL<T>::weightedDerivative( const int* ind ) const 
{

 for ( TJLterm<T>* p = _jltermStore; p < _jltermStoreCurrentPtr; ++p) {
   
   bool theOne = true; 
   for ( int i=0;  i < _myEnv->numVar(); ++i ) {
      theOne  = theOne && ( (p->_index)(i) == ind[i] );
   }
   if (theOne) return p->_value;
 }
 
 // not found; return 0;

 return T();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL<T>::derivative( const int* ind ) const 
{
 T d = weightedDerivative( ind );
 if ( d == T() ) return 0.0;
 
 double n;
 double multiplier = 1.0;

 for ( int i=0; i < _myEnv->numVar(); ++i ) {
   n = ind[i];
   while(n)  multiplier *= n--;
   }
 
 return multiplier*d;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL<T>::operator()( const T* x )  const 
{
 // This routine is linked to double TJL<T>::operator()( const Vector& x ) const
 // Any change made to this must be reflected in the other.

 TJLterm<T>* p = 0;
   T v   = T();
 int w   = 0;
 int i   = 0;
 T term  = T();
 T* u    = 0;
 
 u = new T[ _myEnv->numVar() ];
 
 // Subtract off the reference point.
 for( i = 0; i < _myEnv->numVar(); ++i ) u[i] = x[i] - _myEnv->refPoint()[i];
 
 // Evaluate and store _monomials.
 
 // The zeroth one.

 _myEnv->monomial()[0] = 1.0;

 int* exponent = _myEnv->exponent();
 
 // For all higher weights ...
 for( w = 1; w <= _weight; w++ )
 
   // Get the next set of _exponents of weight w.
   while( nexcom( w, _myEnv->numVar(), exponent ) ) {
 
     // Find the first non-zero _exponent.
     i = 0;
     while( !exponent[i++]  ) ;
     i--;
 
     // The value of the _monomial associated with this composition
     // is obtained by multiplying a factor into a previously
     // computed _monomial.

     exponent[i]--;

     term = (_myEnv->monomial())[ _myEnv->offsetIndex( exponent ) ];

     exponent[i]++;

     _myEnv->monomial()[ _myEnv->offsetIndex( exponent ) ] = term * u[i];
 
   }
 
 // Monomials have been now stored at this point.
 // Now traverse the JL variable and evaluate.
 
 v = T();
 
 for ( TJLterm<T>* p = _jltermStore; p < _jltermStoreCurrentPtr; ++p) {

   v += ( p->_value ) * ( _myEnv->monomial()[ _myEnv->offsetIndex( p->_index ) ] );
 }
 
 delete [] u;
 
 return v;
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL<T>::operator()( Vector const& x ) const 
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
JLPtr<T> TJL<T>::filter( const int& wgtLo, const int& wgtHi ) const 
{ 
 //----------------------------------------------------------------------------
 // Filters the terms with weights included in the closed interval [wgtLo,wgtHi]
 //----------------------------------------------------------------------------

 // Trivial case: nothing to filter ... return a *deep copy* of the current jl.
 
 if( ( wgtLo <= 0 ) && ( wgtHi >= _weight ) ) {
   return JLPtr<T>( TJL<T>::makeTJL( *this ));
 } 

 // Begin filtering ... 

 JLPtr<T> z( TJL<T>::makeTJL( _myEnv) );
 
 int      wgt = 0;
 int upperWgt = 0;
 
 for ( TJLterm<T> const *p = _jltermStore; p < _jltermStoreCurrentPtr; ++p) {

   wgt = p->_weight;
   if( (wgt < 2) && (wgt >= wgtLo) && ( wgt <= wgtHi ) ) {
     int indy = _myEnv->offsetIndex( p->_index );
     z->_jltermStore[indy]._value = p->_value;
   }
   else if( ( wgt >= wgtLo ) && ( wgt <= wgtHi ) ) {
     z->append(*p);
     if( wgt > upperWgt ) upperWgt = wgt;
   }
 }
 
 z->_weight  = upperWgt;             // ??? Where is the copy of the re
 z->_accuWgt = _accuWgt;             // ??? reference point???

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::filter( bool (*f) ( const IntArray&, const T& ) ) const 
{ 

 JLPtr<T> z( TJL<T>::makeTJL(_myEnv) );

 int nv = _myEnv->numVar();

 for ( TJLterm<T>* p = _jltermStore; p < _jltermStoreCurrentPtr; ++p) {

  if( (*f)( p->_index, p->_value ) ) {

     if( p->_weight < 2 ) {
       int indy = _myEnv->offsetIndex( p->_index );
       z->_jltermStore[indy]._value = p->_value;
     }
     else {
         z->append(*p);
     }
  }
 }
 return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::truncMult( JLPtr<T> const& v, const int& wl ) const 
{ 
 //
 // Truncated multiplication: used only by the division operator.
 // No checks are made on the environments.
 //

 // If one of the arguments is void, return a *deep copy* of it ..

 if ( (getCount()    == 0) )     return  JLPtr<T>( TJL<T>::makeTJL(*this) );   
 if ( (v->getCount() == 0) )     return  JLPtr<T>( TJL<T>::makeTJL(*v) );        

 // .. otherwise continue normal operations.

 // Initializations

 JLPtr<T> z( TJL<T>::makeTJL( _myEnv) );

 TJLterm<T>* tjlmml =  _myEnv->TJLmml(); 

 T  dummy   = T();
 T  product = T();

 int indy = 0;

 TJLterm<T> const * const vstart    = v->_jltermStore; 
 TJLterm<T> const * const vend      = v->_jltermStoreCurrentPtr; 
 TJLterm<T> const * const thisstart = _jltermStore;
 TJLterm<T> const * const thisend   = _jltermStoreCurrentPtr;

 for (  TJLterm<T> const* p = vstart; p < vend; p++ ) {

    for ( TJLterm<T> const* q = thisstart; q < thisend; q++ ) {

      if( ( p->_weight + q->_weight ) > wl ) continue;

      indy = _myEnv->offsetIndex( p->_index + q->_index );

   // Will work even when the exponents are all zero.

      product = p->_value * q->_value;
      dummy = tjlmml[indy]._value + product;

      if( std::abs( dummy ) < _mx_small*std::abs( product ) ) {
        tjlmml[indy]._value = T();
      }
      else {
        tjlmml[indy]._value = dummy;
      }
    }
 }

//--------------------------------------
// Transfer answer from the scratchpad.
//--------------------------------------

 z->transferFromScratchPad();
 
 // Determine the maximum weight computed accurately.
 // ??? Is this step necessary???
 
 z->_accuWgt = std::min(_accuWgt, v->_accuWgt);
 
 return  z;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Operators for class JL
//------------------------------------------------------------------

template<typename T>
std::ostream& operator<<( std::ostream& os, const TJL<T>& x ) 
{


 os << "\n Count  = " << x.getCount() << " , Weight = " << x._weight;
 os << " , Max accurate weight = " << x._accuWgt << std::endl;
 os << "Begin Environment: \n"
    << *(x._myEnv)
    << "\nEnd Environment: " << std::endl;

 
 for ( TJLterm<T> const* p = x._jltermStore; p < x._jltermStoreCurrentPtr; ++p) {

   if ( p->_value == T() ) continue;

   os << "Index: ";
   os << p->_index << " ";
   os << "   Value: " << std::setprecision(30) << p -> _value << std::endl;
 }
 return os << "\n" << std::endl;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const TJL<T>& x, const TJL<T>& y ) 
{
 
// Note: as curently implemented, this is a computationally VERY expensive opeation. 

if( x._myEnv != y._myEnv ) return false;

if( (  x._weight  != y._weight )  ||
     ( x._accuWgt != y._accuWgt )
   ) return false;
 
TJLterm<T> const* q = 0;   
for ( TJLterm<T> const* p = x._jltermStore; p < x._jltermStoreCurrentPtr; ++p, ++q) {

   if(  !( *p == *q)  ) return false;
}

return true;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( TJL<T> const& x, T const& y ) 
{


 if (x.standardPart() != y )   return false;

 // all terms of weight  >0 must also have _value == 0 (coefficient)

 for (  TJLterm<T> const* p = x._jltermStore+1; p < x._jltermStoreCurrentPtr; ++p) {
   if ( p->_value != T() ) return false;
 } 

 return true;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const T& y, const TJL<T>& x )
{
 return x == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TJL<T>& x, const TJL<T>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TJL<T>& x, const T& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const T& x, const TJL<T>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
template<typename T>
TJL<T>& TJL<T>::operator=( const T& x ) 
{
 
 if( _myEnv ) _accuWgt = _myEnv->maxWeight();
 else {
   throw( GenericException( __FILE__, __LINE__, 
     "TJL<T>& TJL<T>::operator=( const T& x ) {", 
     "Private data _myEnv is null: object has no environment assigned.") );
 }

 clear();
 appendLinearTerms( _myEnv->numVar() ); 
 setStandardPart(x);

 return *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>& TJL<T>::operator=( const TJL<T>& x )  {

 if( this == &x ) return *this;

 TJLterm<T>* old_jltermStore = 0;

 clear(); 

 // fast copy 

 if ( _jltermStoreCapacity < x._jltermStoreCapacity )  {
   old_jltermStore = _jltermStore;
      _jltermStore = _jltermStoreCurrentPtr =  TJLterm<T>::array_allocate(x._jltermStoreCapacity);
      _jltermStoreCapacity = x._jltermStoreCapacity;
 };

 
 memcpy( _jltermStore, x._jltermStore, (x._jltermStoreCurrentPtr-x._jltermStore)*sizeof(TJLterm<T>) );
 _jltermStoreCurrentPtr = _jltermStore + (x._jltermStoreCurrentPtr-x._jltermStore);

 
 if (old_jltermStore) {
    TJLterm<T>::array_deallocate( old_jltermStore );
 }


 _count   = x._count;    
 _weight  = x._weight;
 _accuWgt = x._accuWgt;
 _myEnv   = x._myEnv;

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>& TJL<T>::operator+=( T const& x ) {   

  _jltermStore->_value += x; 

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::sin() const
{ 

 JLPtr<T> epsilon( TJL<T>::makeTJL( *this) ); // deep copy 


 if( epsilon->standardPart() != T() ) {            // jet has non-zero standard part
   T cs = std::cos( epsilon->standardPart()  );
   T sn = std::sin( epsilon->standardPart()  );
   epsilon->setStandardPart( T() );                  // zero out the standard part;

   // ---------------------------------------------
   // at this point, epsilon is a pure differential    
   // ---------------------------------------------

     return  ( _epsCos(epsilon)*sn + _epsSin(epsilon)*cs);
  
 }

 else {                               

   return _epsSin( epsilon ); // argument is already a pure infinitesimal

 }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::cos() const
{ 

 JLPtr<T> epsilon( makeTJL( *this) ); // deep copy 

 
 if( epsilon ->standardPart() != T() ) {                // jet has non-zero standard part
   T cs = std::cos( epsilon ->standardPart() );
   T sn = std::sin( epsilon ->standardPart() );
   epsilon->setStandardPart( T() );                            // zero out the standard part;

   // ---------------------------------------------
   // at this point, epsilon is a pure differential    
   // ---------------------------------------------
   
     return  (_epsCos( epsilon )*cs - _epsSin( epsilon )*sn );
 }

 else {                               

   return _epsCos( epsilon ); // x is pure infinitesimal

   }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::_epsSin( JLPtr<T> const& epsilon ) 
{ 
 
 JLPtr<T> epsq;
 epsq = epsilon*epsilon;
 epsq->Negate();                                              //   epsq = -epsilon*epsilon 
 JLPtr<T> z( epsilon->clone() );               //    z = epsilon -- deep copy 



 JLPtr<T> term =  epsilon*epsq;                // term = 1/6 * epsilon*epsq
 term->scaleBy(1.0/6.0);
 
 double n = 3.0;
 double n1,n2;

  int nsteps = 0;
  while( term->getCount() > 0 ) {
   z    += term;                                // z += term;
   term *= epsq;                                // term->multiply(epsq);
   n1 = ++n; n2 = ++n;
   term->scaleBy( 1.0/static_cast<T>(n1*n2) );  // term = ( ( term*epsq ) / ++n ) / ++n;
   

   if (++nsteps > epsilon->_accuWgt) break;                            // expansion terminates after at most  epsilon->_accuWgt steps


 }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::_epsCos( JLPtr<T> const& epsilon ) 
{ 
 

 JLPtr<T> z( makeTJL( epsilon->_myEnv, ((T) 1.0) ));             // z   = 1    

 JLPtr<T> epsq = (epsilon*epsilon);
 epsq->Negate();                                         // epsq = -epsilon*epsilon

 JLPtr<T> term(epsq->clone());
 term->scaleBy(0.5);                                     // term = epsq/2.0

 double n = 2.0;
 double n1, n2;
 
  int nsteps =0;
  while( term->getCount() > 0 ) {
   z += term;                                   // z += term;
   term *= epsq;                                // term->multiply(epsq);
   n1 = ++n; n2 = ++n;
   term->scaleBy( 1.0/static_cast<T>(n1*n2) );  // term = ( ( term*epsq ) / ++n ) / ++n;  
   if (++nsteps > epsilon->_accuWgt) break;     // expansion terminates after at most  epsilon->_accuWgt steps
   }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::sqrt() const 
{

 if( getCount() == 0 ) {

   // * the code below should be replaced by a more permanent 
   // * solution; possibly a function that clears the scratchpads for all
   // * existing environments. 
   // * The scratchpads should be cleared when *any* exception is thrown
   // * within mxyzptlk because the TJLterm _value fields may contain  
   // * "Nan"s. 
   // *DO NOT REMOVE OR COMMENT OUT UNLESS YOU HAVE IMPLEMENTED A BETTER SOLUTION !* 
   //   
   // -JFO 

   //--------------------------------------------------------------------------

   (*pcout) << "Resetting Environment Scratchpad" << std::endl;
   for (int i=0; i< _myEnv->maxTerms(); i++ ) {
         _myEnv->TJLmml()[i]._value = 0.0;
   }          
   
   //--------------------------------------------------------------------------

   int *p = 0;    
   *p = 1; 

   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> sqrt() { ",
          "Argument is zero." ) );
 }
 


 if( standardPart() !=  T() )   // non-zero standard part
 {
   T factor = std::sqrt( standardPart() );
   if( getCount() == 1 )           // ... operand may have no derivatives
   {
     return JLPtr<T>( makeTJL(_myEnv, factor)); 
   }
   else                       // ... normal case
   {
     JLPtr<T> epsilon( makeTJL(*this) ); // deep copy 
     epsilon->setStandardPart( T() );                     
     epsilon->scaleBy( 1.0/standardPart() );
     return ( _epsSqrt(epsilon)*factor );   
     }
 }
 else                                 // nilpotent argument
 {
   
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> sqrt() ",
          "Argument's standard part vanishes; it is nilpotent." ) );
  
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::_epsSqrt( JLPtr<T> const& epsilon ) 
{  

 // This function is identical to epsPow
 // with the substitution  s = 1/2

 JLPtr<T>      z(makeTJL(epsilon->_myEnv,(T) 1.0)); // z = 1.0 
 JLPtr<T>      term(makeTJL(*epsilon));             // deep copy;  term = epsilon 
 
 double f    = 1.0 / 2.0;
 double n    = 1.0;

 term->scaleBy(f);                      // term = f*epsilon;
 
 int nsteps = 0;
 while( term->getCount() > 0 ) {

   z    += term;                                     // z += term; in place add
   term *= epsilon;                                  // term->multiply(epsilon);
   term->scaleBy( ((T)--f)/static_cast<T>(++n) );    // term *= ( ((T) (--f)) * epsilon ) / ++n;

   if (++nsteps > epsilon->_accuWgt) break;                            // expansion terminates after at most  epsilon->_accuWgt steps
                                                                       // NOTE: "term->count" may not reach zero due to round-off
 }

 z->_accuWgt = epsilon->_accuWgt;

 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 template<typename T>
 JLPtr<T> TJL<T>::exp() const
 {
 
 T factor = T();
 JLPtr<T> epsilon( makeTJL(*this) );  //deep copy

 if( standardPart() !=  T() ) {                   // x has non-zero standard part
   factor = std::exp( standardPart() );
   epsilon->setStandardPart( T() );               // zero out the standard part off epsilon
   return ( _epsExp( epsilon )*factor );
   }
 else {                               // x is already a pure infinitesimal
   return _epsExp( epsilon );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JLPtr<T>  TJL<T>::_epsExp( JLPtr<T> const& epsilon ) 
{ 

 JLPtr<T> z( makeTJL(epsilon->_myEnv, ((T) 1.0)  ));    // z    = 1.0;
 JLPtr<T> term( epsilon->clone() );                     // term = epsilon

 double n = 1.0;
 
 int nsteps = 0;
 while( term->getCount() > 0 ) {
   z += term;                                             // z += term;
   term *= epsilon;  
   term->scaleBy(1.0/static_cast<T>(++n));                // term = ( term*epsilon ) / ++n;
   if (++nsteps > epsilon->_accuWgt) break;               // this should not be necessary
                                                          // unless the scratchpad is poluted.   
 }
  
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::pow(const double& s )  const
{

 if( getCount() == 0 ) return JLPtr<T>( makeTJL(*this) ); // deepcopy  pow(0, s) = 0  
 
 int u = 0;

 if(  standardPart()  !=  T()   ) // x has non-zero standard part
 {

   if( getCount() == 0 ) {             // x may have no derivatives
     return JLPtr<T>( makeTJL( _myEnv, std::pow( standardPart(),s ) ) );
   }

   JLPtr<T> z( makeTJL(*this) );   //   deep copy

   z->setStandardPart( T() );   //  zero the standard part off z 

   z->scaleBy( 1.0/ standardPart() );
   z = _epsPow( z, s );
   z->scaleBy( std::pow( standardPart(), s ) );
   return z;
   }
 else                                 // x is pure infinitesimal
   {
   u = nearestInteger( s );
   if( s != (T) u ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T> pow( const TJet<T>&, const double& )",
            "Cannot use infinitesimal as base with non-integer _exponent." ) );
   }

   JLPtr<T> z(makeTJL(_myEnv, ((T) 1.0))); // z = 1.0;

   if ( u == 0 ) {
     return z;
   }
   if ( u > 0 ) {
     JLPtr<T> x(makeTJL(*this));      // x, deepcopy;
     while( u-- > 0 )   z *= x;
     return z;
     }
   else {
     JLPtr<T> x(makeTJL(*this));               //  x, deepcopy;
     while( u++ < 0 )   z *= x;
     JLPtr<T> tmp(makeTJL(_myEnv, ((T) 1.0))); // tmp = 1.0;
     z = tmp/z;                                //  z = 1.0/z;
     return z;
     }
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::pow(int n ) const
{

  if( n == 0 ) 
     return JLPtr<T>(makeTJL( _myEnv, ((T) 1.0)));    // z = 1
  else if( n > 0 ) {
     JLPtr<T> z(makeTJL( *this));              // z =  x
     JLPtr<T> x(makeTJL( *this));              // z =  x
     for( int i = 2; i <= n; ++i ) z *= x;
     return z;
  }
  else { // exponent is negative

    JLPtr<T> xr( makeTJL( _myEnv, ((T) 1.0) ));
    JLPtr<T> x(  makeTJL(*this) );               // deep copy of argument; this step is not really necessary
                                                                // and should be eliminated
    xr =  xr/x;                                                 // xr  = 1/x 
    x  = xr;                                                    // deep copy 
    for( int i = -2; i >= n; i-- ) x *= xr;
    return x;
  }

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::_epsPow( JLPtr<T> const& epsilon, const double& s ) 
{ 
 JLPtr<T> z( makeTJL(epsilon->_myEnv, ((T) 1.0)) ); // z = 1.0;
 JLPtr<T> term;

 double f = s;
 double n = 1.0;

 term  = epsilon;
 term->scaleBy(s);                   //term = s*epsilon;
 
 z->_myEnv = epsilon->_myEnv;
 
 int ncount = 0;
 while( term->getCount() > 0 ) {
   z    += term;                                     
   term *= epsilon;                                  // term = ( ((T) (--f)) * term * epsilon ) / ++n;
   term->scaleBy(((T) (--f))/ static_cast<T>(++n) );
   if (++ncount > epsilon->_accuWgt ) break;
   }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::log() const
{

 if( getCount() == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> log () { ",
          "Argument is zero." ) );
 }
 

 if( standardPart() != T() )  // x has non-zero standard part
 {
   if( getCount() == 0 ) {             // x may have no derivatives
     return JLPtr<T>( makeTJL( _myEnv, std::log( standardPart() )));

   }

   JLPtr<T> z( makeTJL(*this)   );
   JLPtr<T> u( makeTJL(_myEnv) );
   JLPtr<T> w( makeTJL(_myEnv) );
   
   z->setStandardPart( T() );         //   zero out the standard part 

   z->scaleBy( - 1.0/ standardPart() );

   w = z;
   u = z;                                         // We use here the formulae

   double n = 1.0;                                // ln( s + e )   = ln s + ln( 1 + e/s ), and
   while( u->getCount() > 0 ) {                       // ln( 1 + e/s ) = ln( 1 - ( -e/s )) 
     u *= z;                                      //  = - sum ( - e/s )^n / n
     w += (u / static_cast<T>(++n));              // w += ( u / ++n );
   }
   
   JLPtr<T> tmp( makeTJL(_myEnv, std::log(standardPart()) ));
   return ( tmp - w );     // std::log(standardPart()) - w
 
   }
 else                                 // operand has zero standard part
   {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> log() ",
          "operand's standard part vanishes; it is nilpotent." ) );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::compose( JLPtr<T> const y[ ]) const 
{

 //-------------------------------------------------
 //  **** Composition operator ****
 //-------------------------------------------------

 JLPtr<T>  u[ _myEnv->numVar() ];

 //------------------------------------------------
 // Check consistency of reference points and
 // subtract reference point prior to concatenation.
 //-------------------------------------------------

 for( int i= 0; i< _myEnv->numVar(); ++i ) {
 
  if( y[i]->_myEnv !=  y[0]->_myEnv ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJL<T>::compose( JLPtr<T> const y[] ) const ",
            "Inconsistent environments." ) );
   }
 
   u[i] =  y[i] + JLPtr<T>( makeTJL( y[0]->_myEnv, -_myEnv->getRefPoint()[i])) ;  // u[i] = y[i] - _myEnv->getRefPoint()[i];
 }


 JLPtr<T> term( makeTJL(_myEnv) ) ; // Initializing term should not be necessary. 

 JLPtr<T>* tjlmonomial = _myEnv->TJLmonomial();
 int*                     exponent    = _myEnv->exponent();

 // -----------------------------
 // Evaluate and store monomials.
 // -----------------------------

 // The zeroth one ...

 tjlmonomial[0] = JLPtr<T>(makeTJL( u[0]->_myEnv, ((T) 1.0) ));

 // For all higher weights ...

 int  i = 0;
 int  w = 1;

 for( w=1; w <= _accuWgt; w++ ) {

  // Get the next set of _exponents of weight w.

   while( nexcom( w, _myEnv->numVar(), exponent ) ) {
 
     // Find the first non-zero _exponent.
     i = 0;
     
     while( !(exponent[i++]) ) ;
     i--;
 
     // The value of the monomial associated with this composition
     // is obtained by multiplying a factor into a previously
     // computed monomial.

     (exponent[i])--;
     
     term = tjlmonomial[ _myEnv->offsetIndex( exponent ) ];

     exponent[i]++;

     tjlmonomial[ _myEnv->offsetIndex( exponent )] = u[i]*term; 
 

   } // End while loop.
  }  // End for loop
 
 // -----------------------------------------------
 // Monomials have been stored.
 // Now traverse the TJL<T> variable and evaluate.
 // Formerly done by using concat routine, which
 // is reproduced here
 //------------------------------------------------
 
 JLPtr<T> z( makeTJL(u[0]->_myEnv) ); 

 for(  TJLterm<T> const* p = _jltermStore; p< _jltermStoreCurrentPtr; ++p  ) {

    if( p->_weight > _accuWgt ) break;

    z += ( tjlmonomial[ _myEnv->offsetIndex(p->_index) ] *  (p->_value) );
 } 
 
 // Finish...
  
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
std::istream& operator>>( std::istream& is,  TJL<T>& x ) 
{  // ??? This function shouldn't
   // ??? be here.
  char buf[100];
  buf[99] = '\0';

  int count;
  T value;

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

  x.appendLinearTerms( x._myEnv->numVar() ); 

  IntArray ndx( x.getEnv()->numVar() );

  TJLterm<T> term;

  for( int j=0; j < count; ++j) {
    is >> buf;
    is >> ndx;
    is >> buf;
    is >> value;
    term = TJLterm<T>( ndx, value, x._myEnv );

    if ( term._weight == 0)  x.setStandardPart(value);
    if ( term._weight == 1)  { 
            int indy = x._myEnv->offsetIndex( term._index ); 
            x._jltermStore[indy]._value = value;
    }  
    if ( term._weight >  1)  x.append(term);

  }

 return is;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::asin() const
{ 

 const T zero = T(); 	 
 EnvPtr<T>  pje(_myEnv);
  	 
 // Initial Newton's step 	 
 
 JLPtr<T> z(makeTJL(*this));        // deep copy	        
 JLPtr<T> dz; 
 JLPtr<T> x( makeTJL(*this) );

 dz = ( z->sin() - x ) / z->cos(); 	 

 // Setting up the iteration 	 

 int upperBound = 8; 	 
 
 int iter = 0;	   
 int indy = 0; 	 

 double compValue; 	 
 bool repeat = true; 	 
 
 TJLterm<T>  *const tjlmml = pje->TJLmml(); 	 
 	 
 // Iterated Newton's steps
 
 
 TJLIterator<T>  iter_z  (*z);
 TJLIterator<T>  iter_dz (*dz);

 while( repeat && (iter < _mx_maxiter) ) 	 
 { 	 
     while( ++iter < upperBound ) { 	 
      // These two lines are the heart of the calculation: 	 
      z  -= dz; 	 
      dz = ( z->sin() - x ) / z->cos(); 	 
     }
	 
     // The rest is just determining when to stop. 	  
     //   This procedure could be improved, but it's better 	 
     //   than the previous one, which was just comparing 	 
     //   dz to zero. 	 
     repeat = false; 	 
 
  	 
     // Load the monomials of the current answer into a scratchpad 	   

     TJLterm<T>  const*     p  = 0; 	 

     iter_z.reset(); 	 
     while(  (p = ++iter_z) ) { 	 
       indy = pje->offsetIndex(p->_index); 	 
       tjlmml[indy]._value = p->_value; 	 
     } 	 
 	 
     // Compare to the increment monomials, one coefficient at a time 	   
    
     iter_dz.reset();

     while( (p = ++iter_dz) ) { 	 
       indy = pje->offsetIndex(p->_index); 	 
       compValue = std::abs( tjlmml[indy]._value ); 	 
       if( compValue < _mx_small ) { 	 
         if( std::abs(p->_value) > _mx_small ) { 	 
           repeat = true; 	 
           break; 	 
         } 	 
       } 	 
     } 	 

     // Clean the scratchpad. 	 

     iter_z.reset(); 	 
     while( (p = ++iter_z) ) { 	 
       indy = pje->offsetIndex(p->_index); 	 
       tjlmml[indy]._value = zero; 	 
     }
 	 
     // And continue 	 
     upperBound += 8; 	 
 } 	 
 
 // If stopped because too many iterations,
 // print a warning message


 if( iter >= _mx_maxiter ) {
  (*pcerr) << "*** WARNING ***                             \n";
  (*pcerr) << "*** WARNING *** JLPtr<T>& asin()      \n";
  (*pcerr) << "*** WARNING *** Over " 
       << _mx_maxiter 
       << " iterations used;    \n";
  (*pcerr) << "*** WARNING *** result may be incorrect.    \n";
  (*pcerr) << "*** WARNING ***                             \n";
  (*pcerr) << "*** WARNING *** Initial value:              \n";
  (*pcerr) << "*** WARNING ***                             \n"
       << std::endl;

   this->printCoeffs();
  (*pcerr) << "*** WARNING *** A few iterations:             " << std::endl;
  for( iter = 0; iter < 4; ++iter ) {
   (*pcerr) << "*** WARNING *** Iteration No. " << ( iter+1 ) << std::endl;
   z -= dz;
   dz = ( z->sin() - x ) / z->cos();
   (*pcerr) << "*** WARNING *** dz = " << std::endl;
   dz->printCoeffs();
   (*pcerr) << "*** WARNING *** z = " << std::endl;
   z->printCoeffs();
  }
 }


 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::atan() const
{   
 
 JLPtr<T> z( makeTJL(*this));      // deep copy
 JLPtr<T> c;
 JLPtr<T> dz;

 JLPtr<T> x( makeTJL(*this)); 

 int iter = 0;

 c = z->cos();                                                    // c = cos( z );
 dz = c*( z->sin() - x*c );                                       // in-place multiply would help 

 while( ( dz->standardPart() != T() ) && ( iter++ < _mx_maxiter ) ) { 
  z -= dz;
  c = z->cos();                                                   // c = cos( z );
  dz = c*( z->sin() - x*c );
 }

 if( iter >= _mx_maxiter ) {
  (*pcerr) <<  "*** WARNING ***                         \n" 
       <<  "*** WARNING *** JLPtr<T>& atan()            \n" 
       <<  "*** WARNING *** Over " 
                << _mx_maxiter  << " iterations used;    \n"
       <<  "*** WARNING *** result may be incorrect.    \n" 
       <<  "*** WARNING ***                             \n" 
       << std::endl;
 }

 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::printCoeffs() const {


 (*pcout) << "\nCount  = " << getCount() 
          << ", Weight = " << _weight 
          << ", Max accurate weight = " << _accuWgt << std::endl;
 (*pcout) << "Reference point: " 
      << _myEnv->getRefPoint()[0];

 for( int i=0; i < _myEnv->numVar(); ++i) {
   (*pcout) << ", ";
   (*pcout) << _myEnv->getRefPoint()[i];
 }
 (*pcout)   << std::endl;


 for(  TJLterm<T> const* p = _jltermStore; p < _jltermStoreCurrentPtr; ++p  ) {

   if( p->_weight > _accuWgt ) break;
   (*pcout) << "Index:  " 
            << p->_index
            << "   Value: "
            << p->_value
            << std::endl;
 }

 std::cout << "\n" << std::endl;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::D( const int* n ) const 
{

 //----------------------------- 
 // Differentiation operator
 //-----------------------------

 bool doit = false;

 // --- Preliminary check of _index set.
 
 int w = 0;
 
 for( int i=0; i < _myEnv->numVar(); ++i ) {
   if( n[i] < 0 ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJL<T>::D( const int* ) const",
            "Cannot differentiate with negative _index." ) );
     }
   w += n[i];
 }
 
 if( w > _accuWgt ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::D( const int* ) const",
          "Differentiation request beyond accuracy allowed." ) );
 }
 
 if( w == 0 ) return JLPtr<T>(makeTJL(_myEnv));

 JLPtr<T> z(makeTJL(_myEnv)); 

 // --- Construct the derivative one link at a time.
 // --- ( See note Obs.4 )
 

 int f = 0;
 int j = 0;

 for(  TJLterm<T>* p = _jltermStore; p< _jltermStoreCurrentPtr; ++p  ) {
 
   boost::scoped_ptr<TJLterm<T> > q( new TJLterm<T>( *p ) );
 
   doit = true;
   // -- Reset the _index.
   for( int i=0; i< _myEnv->numVar(); ++i) 
     doit = doit && ( ( q->_index(i) -= n[i] ) >= 0 );
 
   if( doit ) {
     // -- Build factorial multiplier.
     f = 1;
     for( int k = 0; k < _myEnv->numVar(); ++k ) {
       j = q->_index(k);
       for( int i = 0; i < n[k]; i++ ) f *= ++j;
     }
     if( f <= 0 ) {
       throw( GenericException( __FILE__, __LINE__, 
              "TJet<T> TJet<T>::D( const int* ) const",
              "Horrible, unexplainable error while differentiating!" ) );
     }                           // Super fussbudget!! ( f "must" be positive )
   
     // -- Make final changes in private data of the TJLterm<T> and
     //    absorb it into the answer.

     (q->_value)   *= f;              // OK. So I'm a fussbudget with parentheses again ..
     (q->_weight ) -= w;
   
     if (q->_weight < 2 ) {
      int indy = _myEnv->offsetIndex(q->_index);  
      z->_jltermStore[indy]._value = q->_value;  
     }
       else {  
      z->append( *q );
     } //  if (q->_weight < 2 )
   } //  if( doit )
 }
 
 // --- Finally, adjust _accuWgt and return value
 
 z->_accuWgt = _accuWgt - w;       // ??? Is this correct ???

 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//                 *** friend functions ******
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
template <typename T>
template <void T_function(T&, T const&)>
JLPtr<T>  TJL<T>::add(JLPtr<T> const & x, JLPtr<T> const& y  ){  


// Check for consistency and set reference point of the sum.

  if( x->_myEnv != y->_myEnv ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::add(JLPtr<T> const& x, JLPtr<T> const& y)"
           "Inconsistent environments." ) );
  }

  JLPtr<T> z( TJL<T>::makeTJL(x->_myEnv) ); 

 //-------------------------------------------------
 // Is this 1st order only ?
 //------------------------------------------------ 

  if ( x->_myEnv->maxWeight() == 1) { 

   TJLterm<T>* px = x->_jltermStore;
   TJLterm<T>* py = y->_jltermStore;
   TJLterm<T>* pz = z->_jltermStore;
 
   for( int i=0; i < z->_myEnv->numVar()+1;  ++i, ++px, ++py, ++pz ) { // NOTE: index starts at 0 since std part is at 0 ! 
     pz->_value = px->_value;
     T_function( pz->_value, py->_value);
   }

   // In principle, accur. weight should be adjusted before returning

   return z;
 }


 //  -----------------------------------------------------------------
 //  Loop over the terms and accumulate monomials in the scrach pad.
 //  Use direct sequential access to access terms since order is
 //  irrelevant in this context.
 //  ------------------------------------------------------------------ 
 
 TJLterm<T>* const tjlmml =  x->_myEnv->TJLmml(); // the environment scratchpad
 int indy      = 0;


 TJLterm<T> const * const xstart    = x->_jltermStore; 
 TJLterm<T> const * const xend      = x->_jltermStoreCurrentPtr; 
 TJLterm<T> const * const ystart    = y->_jltermStore;
 TJLterm<T> const * const yend      = y->_jltermStoreCurrentPtr;

 //------------------------------------------------------------
 // accumulate terms of the lhs argument (i.e. x) into the scratchpad
 //------------------------------------------------------------

  int lterms = 1 + x->_myEnv->numVar() ; 
  int idx = 0;

  for(TJLterm<T> const* p = xstart; p < (xstart+lterms); ++p, ++idx  ) {
   tjlmml[idx]._value = p->_value;
  }

  for(TJLterm<T> const* p = (xstart+ lterms); p < xend; ++p  ) {

   indy = x->_myEnv->offsetIndex( p->_index);
   tjlmml[indy]._value = p->_value;
 }

 //-------------------------------------------------------------------
 // accumulate terms of the rhs argument (i.e. y)  into the scratchpad
 // ------------------------------------------------------------------

  lterms = 1 + y->_myEnv->numVar() ; 
  idx = 0;

  for( TJLterm<T> const* p = ystart; p < (ystart+lterms); ++p, ++idx  ) {
   T_function( tjlmml[idx]._value, p->_value);
  }

  for(TJLterm<T> const* p = (ystart+lterms); p< yend; ++p  ) {

   indy   = x->_myEnv->offsetIndex( p->_index);
   T_function( tjlmml[indy]._value, p->_value); // T_function(x, y) either adds or subtract y from x in place.   
 }
 //------------------------------------------------
 // At this point, the result is in the scratchpad!
 //------------------------------------------------


 // Transfer result from the scratchpad. 


  z->transferFromScratchPad(); 

  z->_lowWgt  = std::min(x->_lowWgt,   y->_lowWgt);   
  z->_accuWgt = std::min(x->_accuWgt, y->_accuWgt);

 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
template <void T_function( T& x, T const& y) >
JLPtr<T>&  TJL<T>::inplace_add(JLPtr<T>& x, JLPtr<T> const& y  ){  

 //---------------
 // In place add()
 //---------------

// Check for consistency and set reference point of the sum.

  if( y->_myEnv != x->_myEnv ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::inplace_add(JLPtr<T> const& y)"
           "Inconsistent environments." ) );
  }

 //-------------------------------------------------
 // Is this 1st order only ?
 //------------------------------------------------ 

  if ( x->_myEnv->maxWeight() == 1) { 

   TJLterm<T>* px = x->_jltermStore;
   TJLterm<T>* py = y->_jltermStore;

   for( int i=0; i < x->_myEnv->numVar()+1;  ++i, ++px, ++py ) { // NOTE: index starts at 0 since std part is at 0 ! 
     T_function( px->_value, py->_value);
   }

   // In principle, accur. weight should be adjusted before returning

   return x;
 }

 //  -----------------------------------------------------------------
 //  Loop over the terms and accumulate monomials in the scrach pad.
 //  Use direct sequential access to access terms since order is
 //  irrelevant in this context.
 //  ----------------------------------------------------------------- 
 

 TJLterm<T> * const tjlmml =  x->_myEnv->TJLmml(); // the environment scratchpad
 
 TJLterm<T> const * const xstart    = x->_jltermStore; 
 TJLterm<T> const * const xend      = x->_jltermStoreCurrentPtr; 
 TJLterm<T> const * const ystart    = y->_jltermStore;
 TJLterm<T> const * const yend      = y->_jltermStoreCurrentPtr;
 T result      = T();

 int indy      = 0;

 //------------------------------------------------------------
 // accumulate terms of the current object into the scratchpad
 //------------------------------------------------------------

 int lterms = 1 + x->_myEnv->numVar() ; 
 int idx = 0;

 for( TJLterm<T> const* p = xstart; p < (xstart+lterms); ++p,++idx  ) {
   tjlmml[idx]._value = p->_value;
 }

 for( TJLterm<T> const* p = xstart+lterms; p < xend; ++p  ) {

   indy = x->_myEnv->offsetIndex( p->_index);
   tjlmml[indy]._value = p->_value;
 }

 //-------------------------------------------------------------------
 // accumulate terms of the rhs argument (i.e. y)  into the scratchpad
 // ------------------------------------------------------------------

 lterms = 1 + y->_myEnv->numVar() ; 
 idx = 0;

 for(  TJLterm<T> const* p = ystart; p< ystart+lterms; ++p,++idx  ) {

   result   = tjlmml[idx]._value;
   T_function (  result, p->_value);


   if (std::abs(result) > TJL<T>::_mx_small) 
        tjlmml[idx]._value = result; // should not be needed !
   else 
        tjlmml[idx]._value = T(); 
 }

 for(  TJLterm<T> const* p = ystart+lterms; p< yend; ++p  ) {

   indy   =  x->_myEnv->offsetIndex( p->_index);
   result = tjlmml[indy]._value;
   T_function (  result, p->_value);

   if (std::abs(result) > TJL<T>::_mx_small) 
        tjlmml[indy]._value = result; // should not be needed !
   else 
        tjlmml[indy]._value = T(); 
 }
 

 //------------------------------------------------
 // At this point, the result is in the scratchpad!
 //------------------------------------------------

 // -----------------------------
 // zero out  the current object 
 // -----------------------------

 int accuWgt = x->_accuWgt; // save lhs accurate weight value before clearing it ...   
 int lowWgt  = x->_lowWgt; 
 
 x->transferFromScratchPad();

 x->_lowWgt  = std::min(lowWgt,     y->_lowWgt);   
 x->_accuWgt = std::min(accuWgt,    y->_accuWgt);


 return x;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator+(JLPtr<T> const & x, JLPtr<T> const& y  ){  

  return TJL<T>::template add<TJL<T>::op_add >(x,y); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>&  operator+=(JLPtr<T>& x,      JLPtr<T> const& y  ) {

 return  TJL<T>::template inplace_add<TJL<T>::op_add>(x,y); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator-(JLPtr<T> const & x,  JLPtr<T> const& y  ){
  
 return   TJL<T>::template add<TJL<T>::op_sub>(x,y); 

}
  

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>&  operator-=(JLPtr<T>& x,      JLPtr<T> const& y  ) {

 return   TJL<T>::template inplace_add<TJL<T>::op_sub>(x,y); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator-(JLPtr<T> const &x) {

 JLPtr<T> z( x->clone() );
 z->Negate();
 return z;

}
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator*(JLPtr<T> const & x,  T const& y  ){  

  JLPtr<T> z (x->clone() );
  z->scaleBy( y );  
  return z;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator*( T const & x,             JLPtr<T> const& y  ){  

  return y*x;

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator*(JLPtr<T> const & x,  JLPtr<T> const& y  ){  

//----------------------------
// Multiplication of two jets
//----------------------------

 // Consistency check 

 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJLPtr<T> operator*( const JLPtr<T>&, const JLPtr<T>& ) ",
          "Inconsistent environments." ) );
 };

 EnvPtr<T> pje(x->_myEnv);
 JLPtr<T> z( TJL<T>::makeTJL( pje ) );

 int  indy                = 0;
 T  dummy                 = T();
 T  product               = T();


 // ---------------------------------------------------------------------------------------
 // Determine the lowest weight of the product and the maximum weight computed accurately.
 // ---------------------------------------------------------------------------------------

 z->_lowWgt  =  x->_lowWgt + y->_lowWgt;   

 int testWeight = std::min( (x->_accuWgt + y->_lowWgt), (y ->_accuWgt+ x->_lowWgt) );
     testWeight = std::min( testWeight,  pje->maxWeight() ); 
 
 //------------------------------------------------------------------------------
 // Is the max accurate weight == 1 ?  If so, no sparsity. perform the multiplication directly 
 //-------------------------------------------------------------------------------

 if (testWeight == 1 ) {

    T result;

    TJLterm<T>* px = x->_jltermStore;
    TJLterm<T>* py = y->_jltermStore;
    TJLterm<T>* pz = z->_jltermStore;
    
    T std_x =  px->_value;
    T std_y =  py->_value;
 
    pz->_value = std_x * std_y; // std part
    ++px; ++py; ++pz; 
    
    for( int i=1; i < z->_myEnv->numVar()+1;  ++i, ++px, ++py, ++pz) { // NOTE: index starts at 1 since std part is at 0 ! 

     result  = ( std_x * py->_value ) + ( std_y * px->_value ); 

     if( std::abs( result ) < MX_ABS_SMALL )   result = T();

     pz->_value = result;
   }

   z->_accuWgt = testWeight;

   return z;
 }
 
 //  -----------------------------------------------------------------
 //  Loop over the terms and accumulate monomials in the scrach pad.
 //  Use direct sequential access to access terms since order is
 //  irrelevant in this context.
 //  ------------------------------------------------------------------ 

 TJLterm<T>* const tjlmml =  x->_myEnv->TJLmml(); 

 TJLterm<T> const * const xstart    = x->_jltermStore; 
 TJLterm<T> const * const xend      = x->_jltermStoreCurrentPtr; 
 TJLterm<T> const * const ystart    = y->_jltermStore;
 TJLterm<T> const * const yend      = y->_jltermStoreCurrentPtr;

 
for(   TJLterm<T> const* p = ystart; p < yend; ++p  ) {
  for(   TJLterm<T> const* q = xstart; q < xend; ++q  ) {


     if( ( p->_weight + q->_weight ) > testWeight ) continue;   

     indy = pje->offsetIndex( p->_index + q->_index );

     // Will work even when the exponents are all zero.

     product = p->_value * q->_value;

     dummy = tjlmml[indy]._value + product;

     if( (std::abs( dummy ) < TJL<T>::_mx_small*std::abs( product )) || 
         (std::abs( dummy ) < MX_ABS_SMALL) ) {
       tjlmml[indy]._value = T();
     } 
     else {
       tjlmml[indy]._value = dummy;
     }
 }}

 // -----------------------------------------------------------------------------------------------
 // At this point, the terms are ordered in the scratchpad
 //------------------------------------------------------------------------------------------------
   
 z->transferFromScratchPad();
 z->_accuWgt = testWeight;

 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>&  operator*=(JLPtr<T> & x,     JLPtr<T> const& y  )  
{

//------------------------------------
// In place Multiplication of two jets
//------------------------------------

 // Consistency check 

 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "multiply(JLPtr<T> const& ) ",
          "Inconsistent environments." ) );
 };

 // if one of the arguments is zero, clear x before returning 

 if( x->getCount() == 0 ) return x;
 if( y->getCount() == 0 ) { 

   x->clear();
   x->_accuWgt = x->_myEnv->maxWeight();
   x->_lowWgt  = 0;
   x->_weight  = 0; 
   return x;
 }

 
 int  indy                = 0;
 T  dummy                 = T();
 T  product               = T();


 // -------------------------------------------------
 // Determine the maximum weight computed accurately.
 // -------------------------------------------------

 int lowWgt =  x->_lowWgt + y->_lowWgt;   

 int testWeight = std::min( (x->_accuWgt + y->_lowWgt), (y->_accuWgt+ x->_lowWgt) );
     testWeight = std::min( testWeight,  x->_myEnv->maxWeight() ); 
 
 x->_accuWgt = testWeight;


 // -------------------------------------------------
 // Is this first order ?
 // -------------------------------------------------

 if (testWeight == 1 ) {

    T result;

    TJLterm<T>* px = x->_jltermStore;
    TJLterm<T>* py = y->_jltermStore;
    
    T std_x =  px->_value;
    T std_y =  py->_value;
 
    std_x *= std_y; // std part

    ++px; ++py;  
    
    for( int i=1; i < x->_myEnv->numVar()+1;  ++i, ++px, ++py ) { // NOTE: index starts at 1 since std part is at 0 ! 

     result  = ( std_x * py->_value ) + ( std_y * px->_value ); 

     if( std::abs( result ) < MX_ABS_SMALL )   result = T();

     px->_value = result;
   }

   x->_accuWgt = testWeight;

   return x;
 }

 //  -----------------------------------------------------------------
 //  Loop over the terms and accumulate monomials in the scrach pad.
 //  Use direct sequential access to access terms since order is
 //  irrelevant in this context.
 //  ------------------------------------------------------------------ 
 
 EnvPtr<T> pje(x->_myEnv);

 TJLterm<T>* const tjlmml =  x->_myEnv->TJLmml(); 

 TJLterm<T> const * const xstart = x->_jltermStore; 
 TJLterm<T> const * const xend   = x->_jltermStoreCurrentPtr; 
 TJLterm<T> const * const ystart = y->_jltermStore;
 TJLterm<T> const * const yend   = y->_jltermStoreCurrentPtr;

 
 for(   TJLterm<T> const* p = ystart; p < yend; ++p  ) {
   for(  TJLterm<T> const* q = xstart; q < xend; ++q  ) {


     if( ( p->_weight + q->_weight ) > testWeight ) continue;   

     indy = pje->offsetIndex( p->_index + q->_index );

     // Will work even when the exponents are all zero.

     product = p->_value * q->_value;

     dummy = tjlmml[indy]._value + product;

     if( (std::abs( dummy ) < TJL<T>::_mx_small*std::abs( product )) || 
         (std::abs( dummy ) < MX_ABS_SMALL) ) {
       tjlmml[indy]._value = T();
     } 
     else {
       tjlmml[indy]._value = dummy;
     }
 }}

 // -----------------------------------------------------------------------------------------------
 // At this point, the terms are ordered in the scratchpad!
 //-------------------------------------------------------------------------------------------------

 x->transferFromScratchPad();

 x->_accuWgt  = testWeight;
 x->_lowWgt   = lowWgt;

 return x;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename T>
JLPtr<T>  operator/(JLPtr<T> const& wArg,  JLPtr<T> const& uArg  ){ 

 if ( (wArg->_myEnv) != (uArg->_myEnv) ) {

   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/ ( TJL<T> const& wArg, TJL<T> const& uArg )",
          "Inconsistent environments." ) );
 }

 if ( wArg->getCount() == 0) return JLPtr<T>( TJL<T>::makeTJL( wArg->_myEnv ) );  // numerator is zero
 

 // Check for void operators ..

 if ( (uArg->getCount() == 0) ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/ ( TJL<T> const& wArg, TJL<T> const& uArg )",
          "Attempt to divide by a null TJL<T> variable." ) );
 }
 

 // ------------------------------------------
 // Is this a first order calculation ?
 // ------------------------------------------

 if ( uArg->_accuWgt < 2 ) {

     JLPtr<T>  z( TJL<T>::makeTJL(uArg->_myEnv) );

     TJLterm<T>* pu = uArg->_jltermStore;
     TJLterm<T>* pw = wArg->_jltermStore;
     TJLterm<T>* pz =    z->_jltermStore;

     T u_std =  pu->_value; 
     T w_std =  pw->_value;
 
     pz->_value =  (w_std)/(u_std);
 
     ++pz; ++pu; ++pw;

     for( int i = 0; i < z->_myEnv->numVar(); ++i, ++pu, ++pw, ++pz ) { 

     pz->_value = ( ( u_std * pw->_value ) - ( w_std  * pu->_value ) )
                  / ( u_std * u_std); 

   };
 
   return z;

 }

 //-------------------------------------------------------------------
 // Initialize local variables and set the environment of the answer.
 // (These steps are not necessary, but they enforce a discipline.)
 //---------------------------------------------------------------------

 JLPtr<T>  v( TJL<T>::makeTJL(wArg->_myEnv) );
 JLPtr<T> vn( TJL<T>::makeTJL(wArg->_myEnv) );

 JLPtr<T>  w( TJL<T>::makeTJL(wArg->_myEnv) );
 JLPtr<T>  u( TJL<T>::makeTJL(*uArg )       );

 // Normalize the denominator

 if( uArg->standardPart() == T() ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/( JLPtr<T> const& wArg, JLPtr<T> const& uArg )",
          "Attempt to divide by nilpotent element." ) );
 }

 T   u0  = uArg->standardPart();
 // int wgt = uArg->_weight; // not used for the moment

 u->scaleBy( T(1.0)/ u0 );

 //  -------------------
 //  Recursive algorithm
 //  ------------------- 

 int wl  = 0;
 int mw  =  wArg->_myEnv->maxWeight();

 v->setStandardPart( wArg->standardPart()); 
 
 wl = 0;
 while ( wl < mw ) {

   wl++;
   vn  = u->truncMult( v, wl );
   vn  = vn->filter( wl, wl );
    w  = wArg->filter( wl, wl );
     
  // compute v = v + w - vn 

   v += (w-vn);      // v + w- vn                       
 
 }                                     
 
 // Correct normalization of the answer ..

 v->scaleBy( ((T) 1.0)/ u0); 
 
 // Determine the maximum weight computed accurately. // IS THIS NEEDED HERE ??? FIXME !
 // -------------------------------------------------

 v->_lowWgt  = std::min(  wArg->_lowWgt,  uArg->_lowWgt);
 v->_accuWgt = std::min(  wArg->_accuWgt, uArg->_accuWgt);

 // .. and return _value.

 return v;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator/( JLPtr<T> const & x,  T const& y  ){  

  JLPtr<T> z (x->clone() );
  z->scaleBy( ((T) 1.0) / y );  
  return z;

}


#endif // TJL_TCC
