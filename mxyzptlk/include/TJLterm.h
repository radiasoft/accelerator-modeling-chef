/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJLterm.h
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
******  Sep 2006 
******  
****** - eliminated archaic "Reconstruct" members. Use placement new syntax instead.
******
**************************************************************************
*************************************************************************/
#ifndef TJLTERM_H
#define TJLTERM_H

#include <complex>
#include <ext/hash_map>
#include <ostream>
#include <boost/functional/hash/hash.hpp>
#include <boost/pool/pool.hpp>
#include <mxyzptlk/EnvPtr.h>
#include <basic_toolkit/IntArray.h>

 
template<typename T>
class TJetEnvironment;

template<typename T>
class TJLterm;

template<typename T> 
bool operator<=(TJLterm<T> const&, TJLterm<T> const&);

template<typename T> 
bool operator%=(TJLterm<T> const&, TJLterm<T> const&);

template<typename T> 
bool operator==(TJLterm<T> const&, TJLterm<T> const&);

template<typename T> 
std::ostream& operator<<(std::ostream& os, TJLterm<T> const&);


// ********************************************************************************************************************



template<typename T>
class DLLEXPORT TJLterm
{

 public:

  T        value_;      //  The value associated with the JLterm.
  int      weight_;     //  The sum of the values in index.  For the above example, 
                        //  this would be 4.
  int      offset_;     //  The offset of this term in the scratchpad  
  IntArray index_;      //  An integer array giving the derivatives associated
                        //  with the JLterm.  For example, ( 1, 1, 0, 2 )
                        //  would correspond to D_1^1 D_2^1 D_4^2 .




  // Constructors and destructors

  TJLterm( int nvar=6);

  TJLterm( EnvPtr<T> const&  pje );     // pje = null implies a constant term 
  TJLterm( IntArray  const&, const T&,  EnvPtr<T> const& pje );

  TJLterm( IntArray const&, T const& );
  TJLterm( TJLterm  const& );

  template<typename U>
  TJLterm( TJLterm<U> const& );

  TJLterm& operator=( TJLterm const& );
  TJLterm  operator*( TJLterm const& );
  TJLterm  operator+( TJLterm const& );
  
  // Accessors
  IntArray& exponents()         { return index_; }
  IntArray  exponents()   const { return index_; }
  T&        coefficient()       { return value_; }
  T         coefficient() const { return value_; }
  T         coeff()       const { return value_; }  // old

  // JLterm array allocation functions

  static TJLterm<T>*       array_allocate(int n);
  static void              array_deallocate(TJLterm<T>* p);

  // ~TJLterm();           // MUST **NOT** BE DEFINED ! 

  private:

  static boost::pool<>& _ordered_memPool;  // an ordered pool of TJLterms
  
   // the sizes of the allocated arrays, indexed by their pointers (used for deallocation).    
  static __gnu_cxx::hash_map< TJLterm<T>*, unsigned int, boost::hash<TJLterm<T>*> >& _array_sizes; 
 
  // the declarations below are meant to prevent use of all forms of operator new[];

  static void* operator new[]( std::size_t size) throw (std::bad_alloc); 
  static void* operator new[]( std::size_t size, const std::nothrow_t&) throw();
  static void* operator new[]( std::size_t size, void*) throw();

  static void operator delete[](void* p) throw();
  static void operator delete[](void* p, const std::nothrow_t&) throw();
  // this form *cannot* be overloaded: static void operator delete[](void* p, void*) throw();


  public:

 // Exception subclasses____________________________________________

  struct BadDimension: public std::exception
  {
    BadDimension( int, int, std::string, int, const char* = "", const char* = "" );
    // A binary operator has detected mismatch between its two arguments 
    // in number of Jet coordinates.
    // 1st argument: IntArray dimension of operator's first argument
    // 2nd         : IntArray dimension of operator's second argument
    // 3rd         : name of file in which exception is thrown
    // 4th         : line from which exception is thrown
    // 5th         : identifies function containing throw
    // 6th         : identifies type of error
    ~BadDimension() throw() {}
    const char* what() const throw();
    std::string errorString;
    int xdim, ydim;
  };


} ;

//-----------------------------------------------------------------------------------
// specializations
//-----------------------------------------------------------------------------------

template<>
template<>
TJLterm<std::complex<double> >::TJLterm( TJLterm<double> const& );


//-----------------------------------------------------------------------------------
// Inline members
//-----------------------------------------------------------------------------------


template<typename T>
inline TJLterm<T>::TJLterm( TJLterm<T> const& x ) 
{

 // this copy constructor is called **furiously**. 
 // It must be as efficient as possible !!! 

 memcpy((void *) this, (const void *) &x, sizeof( TJLterm<T>) );

}

#ifndef MXYZPTLK_EXPLICIT_TEMPLATES
#include <mxyzptlk/TJLterm.tcc>
#endif

#endif // TJLTERM_H

