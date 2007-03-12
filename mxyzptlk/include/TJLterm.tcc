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
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES 
******
****** Sep 2006 ostiguy@fnal.gov
******  
****** - eliminated archaic "Reconstruct" member(s). 
******   Use placement new syntax instead.
******  
**************************************************************************
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef TJLTERM_TCC
#define TJLTERM_TCC

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>

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
boost::pool<>&  TJLterm<T>::_ordered_memPool = *( new boost::pool<>( sizeof(TJLterm<T>), 2048 ));

template <typename T>
__gnu_cxx::hash_map< TJLterm<T>*, unsigned int, boost::hash<TJLterm<T>*> >& TJLterm<T>::_array_sizes = 
* (new __gnu_cxx::hash_map< TJLterm<T>*, unsigned int, boost::hash<TJLterm<T>*> >()  );  




// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Implementation of Class TJLterm<T>

template<typename T>
TJLterm<T>::TJLterm( int nvar) 
:value_(T()), weight_(0),  index_(nvar)
{}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm(  EnvPtr<T> const& pje ) 
: value_( T() ), weight_(0), index_( pje->numVar() )
{}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm( IntArray const& l, T const& x, EnvPtr<T> const& pje ) :   
value_(x),  weight_(l.Sum()), index_( l )
{
   if( !pje ) return; 
   if (l.Dim() != pje->numVar() ) {
       throw( GenericException( __FILE__, __LINE__, 
              "TJLterm<T>::TJLterm<T>( IntArray const &, T const&, EnvPtr<T> const&)",
              "Dimensions are inconsistent.") );
   }
} 

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
   
   // ??? REMOVE: index_ = l;
   weight_ = dpt;
   value_ = x;
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

   index_ = l;
   weight_ = l.Sum();
   value_ = x;
 }
}
#endif
//--------------------------------------------------------------------------------------------------------
// end of commented out code
//---------------------------------------------------------------------------------------------------------


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm(  IntArray const& l, const T& x )
: value_(x), weight_(l.Sum()), index_(l)
{}


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
// -----------------------------------------------------------------------
// This code allocates an extra block and uses it to store the array size
//------------------------------------------------------------------------   
    TJLterm<T>* p = 
     static_cast<TJLterm<T>*>(_ordered_memPool.ordered_malloc( n+1 ));
     int * psize = (int *) p;
     *psize = n+1;
     ++p;
//-------------------------------------------------------------------------------------
// Alternately, one could save the array size in a hash table indexed with the pointer
// allocated block.
//------------------------------------------------------------------------------------

/***********

       TJLterm<T>* p = 
       static_cast<TJLterm<T>*>(_ordered_memPool.ordered_malloc( n ));
       _array_sizes[ p ] = n;

************/ 

#endif

    return p;
}

template<typename T>
void TJLterm<T>::array_deallocate(TJLterm<T>* p) {

#ifdef JLTERM_ALLOCATOR_MALLOC
    free(p)
#else 
// --------------------------------------------------------------------------------------------------
// This code deallocates an array of blocks assuming that the size has been stored in an extra block
//---------------------------------------------------------------------------------------------------   
      --p;
      int*  psize = (int *) p;
      _ordered_memPool.ordered_free( p, *psize ); 

//-----------------------------------------------------------------------------------------------------
// Alternately, one could retrieve the array size from a hash table indexed with the pointer to the
// allocated block
//------------------------------------------------------------------------------------------------------

/***********

     _ordered_memPool.ordered_free( p, _array_sizes[p] ); 
     _array_sizes.erase(p );

*************/

#endif

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
//      ******** Overloaded operators for class TJLterm<T> ******

template<typename T>
TJLterm<T>& TJLterm<T>::operator=( TJLterm<T> const& x ) 
{

 if (&x == this) return *this;

 weight_  = x.weight_;
 value_   = x.value_;
 index_   = x.index_;

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJLterm<T>::operator*( TJLterm<T> const& y ) 
{
 TJLterm<T> z(*this);

#if 0 

 if((  ( n = this->index_.Dim() ) != y.index_.Dim()  )) {
   throw( typename TJLterm<T>::BadDimension( this->index_.Dim(), y.index_.Dim(),
                            __FILE__, __LINE__, 
                            "TJLterm<T> TJLterm<T>::operator*( TJLterm<T> const& y ) ",
                            "Inconsistent number of coordinates." ) );
 }
#endif


 z.weight_   = this->weight_ + y.weight_;
 z.index_    = this->index_  + y.index_;  // overloaded + from IntArray
 z.value_    = this->value_  * y.value_;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJLterm<T>::operator+( TJLterm<T> const& y ) 
{

#if  0
 if( this->index_ != y.index_ ) {
   throw( typename TJLterm<T>::BadDimension( this->index_.Dim(), y.index_.Dim(),
                            __FILE__, __LINE__, 
                            "TJLterm<T> TJLterm<T>::operator*( TJLterm<T> const& y ) ",
                            "Inconsistent number of coordinates." ) );
 }
#endif

 TJLterm<T> z(*this);
 z.value_ += y.value_;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( TJLterm<T> const& a, TJLterm<T> const& b ) 
{
 if( a.weight_ != b.weight_ ) return false;
 if( a.value_  != b.value_  ) return false;
 if( a.index_  != b.index_  ) return false;
 return true;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( TJLterm<T> const& a, TJLterm<T> const& b ) 
{
 return !( a == b );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator<=( TJLterm<T> const& a, TJLterm<T> const& b ) 
{

 if( a.index_.Dim() != b.index_.Dim() ) {
   throw( typename TJLterm<T>::BadDimension( a.index_.Dim(), b.index_.Dim(), 
          __FILE__, __LINE__, 
          "char operator<=( TJLterm<T> const&, TJLterm<T> const& )",
          "Dimensions don't match.") );
 }

 if( a.weight_ != b.weight_ ) { return ( a.weight_ < b.weight_ ); }
 
 return (a.index_ <= b.index_); // overloaded operator: exponents lexicographical ordering 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator%=( TJLterm<T> const& a, TJLterm<T> const& b ) 
{
 if( a.weight_ != b.weight_ ) return false;
 return a.index_ == b.index_;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T> 
std::ostream& operator<<(std::ostream& os, TJLterm<T> const& term) {

 os << " Index:  "  << term.index_  
    << " Weight: "  << term.weight_ 
    << " Value:  "  << term.value_;

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
