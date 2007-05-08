/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                        Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      FFTWAllocator.h
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
****** 
******  
******  A specialized allocator for vectors intended to be used with FFTW.
******  FFTWAllocator allocates contiguous memory aligned for
******  optimized performance.
******   
******  From the FTWD doc :
******  ------------------- 
******  SIMD floating-point instructions are available on several popular 
******  CPUs: SSE/SSE2 (single/double precision) on Pentium III/IV and higher, 
******  3DNow! (single precision) on the AMD K7 and higher, and AltiVec 
******  (single precision) on the PowerPC G4 and higher. 
******  FFTW can be compiled to support the SIMD instructions on any of 
******  these systems. 
******  A program linking to an FFTW library compiled with SIMD support 
******  can obtain a nonnegligible speedup for most complex and r2c/c2r 
******  transforms. In order to obtain this speedup, however, the arrays 
******  of complex (or real) data passed to FFTW must be specially aligned 
******  in memory (typically 16-byte aligned), and often this alignment 
******  is more stringent than that provided by the usual malloc (etc.) 
******  allocation routines.
******
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef FFTWALLOCATOR_H
#define FFTWALLOCATOR_H

#include <complex>
#include <fftw3.h>
#include <limits>
#include <cstddef> // for size_t


template <typename T>
class FFTWAllocator {

 public:
 
  typedef std::size_t          size_type;  
  typedef std::ptrdiff_t    ptrdiff_type;
  typedef T*                     pointer;
  typedef const T*         const_pointer;
  typedef T&                   reference;
  typedef T const&       const_reference;
  typedef T                   value_type;


  template<class U> 
  struct rebind {
    typedef FFTWAllocator<U> other;
  };


  pointer address (reference value) const {
    return &value;
  }

  const_pointer address (const_reference value) const {
    return &value;
  }

  FFTWAllocator() throw() {
  }

  FFTWAllocator( FFTWAllocator const&)  throw() {
  }

  ~FFTWAllocator() throw() {
  }

  size_type max_size() const throw() {
    return std::numeric_limits<size_t>::max() /sizeof(T);
  }
  
  pointer allocate (size_type num,  void const* hint=0) {
    return (pointer) fftw_malloc( ((num==0) ? 1 : num )* sizeof(value_type) );
  }

  void construct(pointer p, T const& value) {
     new ( (void*)p ) T(value) ;
  }

  void destroy(pointer p) {
    p->~T();
  }

  void deallocate(pointer p, size_type num ) {
      fftw_free(p);
  }

};

//-----------------------------------------------------------
// ALL Specializations of  FFTWAllocator are interchangeable
//----------------------------------------------------------

 template< typename T1, typename T2>
 bool operator == ( FFTWAllocator<T1> const&,   FFTWAllocator<T2> const& ) throw() {

   return true;
 }


 template< typename T1, typename T2>
 bool operator != ( FFTWAllocator<T1> const&,   FFTWAllocator<T2> const& ) throw() {

   return false;
 }


#endif // FFTWALLOCATOR_H


  
  
