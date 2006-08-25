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
******   for implicit instantiations, define MXYZPTLK_IMPLICIT_TEMPLATES 
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
#include <EnvPtr.h>
#include <IntArray.h>

 
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
class TJLterm
{

 public:
  // Data
  IntArray _index;      //  An integer array giving the derivatives associated
                        //  with the JLterm.  For example, ( 1, 1, 0, 2 )
                        //  would correspond to D_1^1 D_2^1 D_4^2 .
  int      _weight;     //  The sum of the values in index.  For the above example, 
                        //  this would be 4.
  T        _value;      //  The value associated with the JLterm.

  bool     _deleted;    //  true when the term has been deleted (but not yet de-allocated)

  // Constructors and destructors
  TJLterm();

  void Reconstruct( const IntArray&, const T& );

  //   NOTE: The void constructor is to be used ONLY in
  //   conjunction with Reconstruct. By default,
  //   _index is made 6 dimensional until Reconstruct
  //   is invoked.

  TJLterm( EnvPtr<T> const& pje );
  TJLterm( const IntArray&, const T&,  EnvPtr<T> const& pje);

  //   If 0 is used for the environment pointer,
  //   it is assumed that the that the variable
  //   represents a constant function.

  TJLterm( const IntArray&, const T& );
  TJLterm( const TJLterm& );

  template<typename U>
  TJLterm( const TJLterm<U>& );

  TJLterm& operator=( const TJLterm& );
  TJLterm  operator*( const TJLterm& );
  TJLterm  operator+( const TJLterm& );
  
  // Accessors
  IntArray& exponents()         { return _index; }
  IntArray  exponents()   const { return _index; }
  T&        coefficient()       { return _value; }
  T         coefficient() const { return _value; }
  T         coeff()       const { return _value; }  // old

  // JLterm array allocation functions

  static TJLterm<T>*       array_allocate(int n);
  static void              array_deallocate(TJLterm<T>* p);

  // ~TJLterm();           // MUST **NOT** BE DEFINED ! 

  private:


  static boost::pool<>                                                       
                         _ordered_memPool;  // an ordered pool of TJLterms
  
  static __gnu_cxx::hash_map< TJLterm<T>*, unsigned int, boost::hash<TJLterm<T>*> > 
                         _array_sizes;      // the size of the allocated arrays, 
                                            // indexed by their pointers   

  // these declations are meant to prevent use of all forms of operator new[];

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


// specializations

template<>
template<>
TJLterm<std::complex<double> >::TJLterm( TJLterm<double> const& );

#ifdef MXYZPTLK_IMPLICIT_TEMPLATES
#include <TJLterm.tcc>
#endif

#endif // TJLTERM_H

