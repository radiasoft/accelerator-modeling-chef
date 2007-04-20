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
****** Mar 2007 ostiguy@fnal.gov 
****** - Introduced new compact monomial indexing scheme based on monomial ordering
******   to replace previous scheme based explicitly on monomial exponents tuples.
****** - monomial multiplication handled via a lookup-table.
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
boost::pool<>&  TJLterm<T>::ordered_memPool_ = *( new boost::pool<>( sizeof(TJLterm<T>), 2048 ));

template <typename T>
__gnu_cxx::hash_map< TJLterm<T>*, unsigned int, boost::hash<TJLterm<T>*> >& TJLterm<T>::array_sizes_ = 
* (new __gnu_cxx::hash_map< TJLterm<T>*, unsigned int, boost::hash<TJLterm<T>*> >()  );  




// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Implementation of Class TJLterm<T>

template<typename T>
TJLterm<T>::TJLterm() 
:value_(T()),  weight_(0), offset_(0)
{}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm( IntArray const& l, T const& x, EnvPtr<T> const& pje ) :   
value_(x), weight_(0) , offset_(0)
{
  
   offset_  = pje->offsetIndex(l);  
   weight_  = pje->weight(offset_);

} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm(  T const& x , int const& weight, int const& offset)
: value_(x), weight_(weight), offset_(offset) 
{ }


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


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
     static_cast<TJLterm<T>*>(ordered_memPool_.ordered_malloc( n+1 ));
     int * psize = (int *) p;
     *psize = n+1;
     ++p;
//-------------------------------------------------------------------------------------
// Alternately, one could save the array size in a hash table indexed with the pointer
// allocated block.
//------------------------------------------------------------------------------------

/***********

       TJLterm<T>* p = 
       static_cast<TJLterm<T>*>(ordered_memPool_.ordered_malloc( n ));
       array_sizes_[ p ] = n;

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
      ordered_memPool_.ordered_free( p, *psize ); 

//-----------------------------------------------------------------------------------------------------
// Alternately, one could retrieve the array size from a hash table indexed with the pointer to the
// allocated block
//------------------------------------------------------------------------------------------------------

/***********

     ordered_memPool_.ordered_free( p, array_sizes_[p] ); 
     array_sizes_.erase(p );

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
 offset_  = x.offset_;

 return *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJLterm<T>::operator<( TJLterm<T> const& rhs ) const 
{

 return ( offset_ < rhs.offset_); 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( TJLterm<T> const& a, TJLterm<T> const& b ) 
{

 if( a.offset_ != b.offset_  ) return false;
 if( a.value_  != b.value_   ) return false;
 
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

 return (a.offset_ <= b.offset_); 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator%=( TJLterm<T> const& a, TJLterm<T> const& b ) 
{

   return a.offset_ == b.offset_;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T> 
std::ostream& operator<<(std::ostream& os, TJLterm<T> const& term) {

 os << " Offset: "  << term.offset_  
    << " Index:  "  << "FIX ME ! " 
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
