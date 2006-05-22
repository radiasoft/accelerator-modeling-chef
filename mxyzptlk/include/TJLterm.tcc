/*************************************************************************
**************************************************************************
**************************************************************************
******
******  MXYZPTLK:  A C++ implementation of differential algebra.
******
******  File:      TJLterm.tcc
******
******  Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******* Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
******  Author:    Leo Michelotti
******             Fermilab
******             Batavia, IL   60510
******             Email: michelotti@fnal.gov
******
******  Major Revision History:
******
******  Feb 2005   Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******  
****** - Efficiency improvements
****** - Major memory management redesign. 
******  
******  Sep-Dec 2005  ostiguy@fnal.gov
******
****** - refactored code to usea single class template parameter
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
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef TJLTERM_TCC
#define TJLTERM_TCC

#include <iosetup.h>

using FNAL::pcerr;
using FNAL::pcout;

//---------------------------------------------------------------
//*** Static member variable instantiations for the TJLterm class
//---------------------------------------------------------------
//
// NOTE::
// 
//  __gnu_cxx::hash_map is an extension to the STL. 
//  There is equivalent functionality in tr1 ...
//  boost::hash<TJLterm<T>*> is used to get a portable
//  pointer hash fnct. 

template <typename T>
boost::pool<> TJLterm<T>::_ordered_memPool( sizeof(TJLterm<T>), 2048 );

template <typename T>
__gnu_cxx::hash_map< TJLterm<T>*, unsigned int, boost::hash<TJLterm<T>*> > TJLterm<T>::_array_sizes;


// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Implementation of Class TJLterm<T>

template<typename T>
TJLterm<T>::TJLterm() 
: _index(6), _weight(0), _value(T()), _deleted(false) /// index argument necessary ???
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm(  EnvPtr<T> const& pje ) 
: _index( pje->numVar() ), _weight(0), _value(T()), _deleted(false) 
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm( const IntArray& l, 
                const T& x, EnvPtr<T> const& pje ) :   
_index( l ), _weight(l.Sum()), _value(x), _deleted(false)
{ }
//------------------------------------------------------------------------------------------------------
// The code below has been commented out for efficiency. It performs sanity checks and can be re-enabled
// for debugging pruposes.
// Remember: TJLterm<>::TJLterm() is called on a massive scale ! 
//------------------------------------------------------------------------------------------------------
#if 0
{

 // I think that it is not necessary to use a try block
 // with this constructor, even though it throws exceptions,
 // because the destructor does no subsidiary garbage collection.
 // - Leo Michelotti
 //   Thursday, January 22, 2004


 int i, dpt;

 if( pje ) {

   int n = l.Dim();

   if( n != pje->numVar() ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJLterm<T>::TJLterm<T>( const IntArray&, const T&, TJetEnvironment<T>*",
            "Dimensions are wrong.") );
   }
  
   // ??? These checks could be removed for speed.
   dpt = 0;
   for( i = 0; i < n; ++i ) {
     if( (l(i) < 0) ) {
       throw( GenericException( __FILE__, __LINE__, 
              "TJLterm<T>::TJLterm<T>( const IntArray&, const T&, TJetEnvironment<T>*",
              "Bad index in JLTerm.") );
     }
     dpt += l(i);
   }
  
   if( dpt > pje->maxWeight() ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJLterm<T>::TJLterm<T>( const IntArray&, const T&, TJetEnvironment<T>*",
            "Attempt to load a JLTerm with too large a weight.") );
   }
   
   // ??? REMOVE: _index = l;
   _weight = dpt;
   _value = x;
 }

 else {
   if( l.Dim() != 1 ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJLterm<T>::TJLterm<T>( const IntArray&, const T&, TJetEnvironment<T>*",
            "Inconsistency between l and pje") );
   }
   if( l(0) != 0 ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJLterm<T>::TJLterm<T>( const IntArray&, const T&, TJetEnvironment<T>*",
            "Bad value of the index when pje = 0.") );
   }

   _index = l;
   _weight = l.Sum();
   _value = x;
 }
}
#endif
//--------------------------------------------------------------------------------------------------------
// end of commented out code
//---------------------------------------------------------------------------------------------------------


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm( const IntArray& l, const T& x )
: _index(l), _weight(l.Sum()), _value(x),_deleted(false)
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm( const TJLterm<T>& x ) 
: _index( x._index ), _weight(x._weight), _value(x._value), _deleted(x._deleted)
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJLterm<T>::Reconstruct( const IntArray& e, const T& x )
{
  _index.Reconstruct(e);
  _weight   = e.Sum();
  _value    = x;
  _deleted  = false;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// This destructor should **never** be called 
// doing so will break the specialized allocation.

#if  0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template<typename T>
TJLterm<T>::~TJLterm<T>() 
{
 

}
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif 

template<typename T>
TJLterm<T>* TJLterm<T>::array_allocate(int n) {

// ----------------------------------------------------------------------
// To use the system allocator instead of boost:pool to allocate JLterms
// define JLTERM_ALLOCATOR_MALLOC. This is often useful for debugging.  
//-----------------------------------------------------------------------

#ifdef JLTERM_ALLOCATOR_MALLOC
    TJLterm<T>* p = (TJLterm<T>*) malloc(n*sizeof(TJLterm<T>));
#else
    TJLterm<T>* p = 
     static_cast<TJLterm<T>*>(_ordered_memPool.ordered_malloc( n ));
    _array_sizes[ p ] = n;
#endif

    return p;
}

template<typename T>
void TJLterm<T>::array_deallocate(TJLterm<T>* p) {

#ifdef JLTERM_ALLOCATOR_MALLOC
    free(p)
#else 
    _ordered_memPool.ordered_free( p, _array_sizes[p] ); 
    _array_sizes.erase(p );
#endif

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
//      ******** Overloaded operators for class TJLterm<T> ******

template<typename T>
TJLterm<T>& TJLterm<T>::operator=( const TJLterm<T>& x ) 
{
 _deleted = x._deleted;
 _weight  = x._weight;
 _value   = x._value;
 _index   = x._index;
 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJLterm<T>::operator*( const TJLterm<T>& y ) 
{
 TJLterm<T> z(*this);
 int n = 0;

 if((  ( n = this->_index.Dim() ) != y._index.Dim()  )) {
   throw( typename TJLterm<T>::BadDimension( this->_index.Dim(), y._index.Dim(),
                            __FILE__, __LINE__, 
                            "TJLterm<T> TJLterm<T>::operator*( const TJLterm<T>& y ) ",
                            "Inconsistent number of coordinates." ) );
 }

 z._weight = this->_weight + y._weight;
 for( int i = 0; i < n; ++i ) {  z._index(i) = this->_index(i) + y._index(i);  }
 z._value = this->_value * y._value;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJLterm<T>::operator+( const TJLterm<T>& y ) 
{
 if( this->_index != y._index ) {
   throw( typename TJLterm<T>::BadDimension( this->_index.Dim(), y._index.Dim(),
                            __FILE__, __LINE__, 
                            "TJLterm<T> TJLterm<T>::operator*( const TJLterm<T>& y ) ",
                            "Inconsistent number of coordinates." ) );
 }
 TJLterm<T> z(*this);
 z._value += y._value;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const TJLterm<T>& a, const TJLterm<T>& b ) 
{
 if( a._weight != b._weight ) return false;
 if( a._value  != b._value  ) return false;
 if( a._index  != b._index  ) return false;
 return true;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TJLterm<T>& a, const TJLterm<T>& b ) 
{
 return !( a == b );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator<=( const TJLterm<T>& a, const TJLterm<T>& b ) 
{
 int i;
 if( a._index.Dim() != b._index.Dim() ) {
   throw( typename TJLterm<T>::BadDimension( a._index.Dim(), b._index.Dim(), 
          __FILE__, __LINE__, 
          "char operator<=( const TJLterm<T>&, const TJLterm<T>& )",
          "Dimensions don't match.") );
 }

 if( a._weight != b._weight ) { return ( a._weight < b._weight ); }
 
// the code below should work, but at the moment it does not for some reason ... . 
// 
#if 0
 return (a._index <= b._index) ;
#endif

// do this instead ... 

#if 1

 // for( i = 0; i < a._index.Dim(); ++i ) {
 //   if( a._index(i) == b._index(i) ) continue;
 //   return ( a._index(i) < b._index(i) );
 // }

 i = a._index.Dim();
 
 int k,l;

 IntArrayReverseIterator getNext_a(a._index); // this should go away. Use 
 IntArrayReverseIterator getNext_b(b._index); // IntArray operator <= instead

 while( 0 < i-- ) {
   k = getNext_a(); l = getNext_b();
   if( k  != l ) {
     return (  k < l );
   }
 }

// while( 0 < i ) {
//   i--;
//   if( a._index(i) != b._index(i) ) {
//     return ( a._index(i) < b._index(i) );
//   }
// }

 return true;  // when all indices are the same.
#endif
//////////////////////////////////////////////////////////////////////////////
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator%=( const TJLterm<T>& a, const TJLterm<T>& b ) 
{
 if( a._weight != b._weight ) return false;
 return a._index == b._index;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T> 
std::ostream& operator<<(std::ostream& os, TJLterm<T> const& term) {

 os << "Index: "   << term._index  
    << " Weight: " << term._weight 
    << " Value: "  << term._value;

 return os;
} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJLterm<T>::BadDimension::BadDimension( int i, int j, 
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
const char* TJLterm<T>::BadDimension::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#endif // TJLTERM_TCC
