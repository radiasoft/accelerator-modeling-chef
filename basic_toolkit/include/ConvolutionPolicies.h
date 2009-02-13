/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                        Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      ConvolutionPolicies.h
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
****** SYNOPSIS:
****** 
******* Policy objects to perform a convolution between  
******  two functions. 
******
******  Policy 1:  use FFTs. 
******  Policy 2:  use inner product
******
******  Both policies assume equal spacing of the samples. 
******
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef CONVOLUTIONPOLICIES_H
#define CONVOLUTIONPOLICIES_H

#include <complex> // note: this header *must* be included *before* fftw3.h
#include <vector>

#include <basic_toolkit/FFTWAllocator.h>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

template <typename U>
     struct ConvolutionFunctorTraits {

};

template<>
struct ConvolutionFunctorTraits<double> {
   typedef double                FFT_Input_t; 
   typedef std::complex<double>  FFT_Output_t;

}; 

template<>
struct ConvolutionFunctorTraits<std::complex<double> > {
   typedef std::complex<double>  FFT_Input_t; 
   typedef std::complex<double>  FFT_Output_t;
}; 


template<typename T>
class ConvolutionFFTPolicy { 

 public:

  typedef typename ConvolutionFunctorTraits<T>::FFT_Input_t   FFT_Input_t;
  typedef typename ConvolutionFunctorTraits<T>::FFT_Output_t  FFT_Output_t;

  ConvolutionFFTPolicy( int sample, bool measure);

  template<typename Fnct>
  ConvolutionFFTPolicy(  int nsamples, Fnct lhs, bool measure); 

 ~ConvolutionFFTPolicy();

  std::vector<T> const&   operator()( std::vector<T>  const& lhs, std::vector<T>  const& rhs );
  std::vector<T> const&   operator()( std::vector<T>  const& rhs );

  void resetLHS( std::vector<T>  const& lhs);

 private:

  int                                                  nsamples_;
  int                                                  fft_input_array_size_;
  int                                                  fft_output_array_size_;

  boost::function< FFT_Output_t* ( FFT_Input_t*  ) >   forward_transform_; 
  boost::function< FFT_Input_t*  ( FFT_Output_t* ) >   inverse_transform_; 

  std::vector<double, FFTWAllocator<double> >          rhsdata_;
  std::vector<double, FFTWAllocator<double> >          lhsdata_;
  std::vector<double, FFTWAllocator<double> >          result_;

};

//-------------------------------------------------------------------------------------------------------
// Specializations
//-------------------------------------------------------------------------------------------------------

template<>
ConvolutionFFTPolicy<double>::ConvolutionFFTPolicy( int array_size, bool measure);

template<>
ConvolutionFFTPolicy<std::complex<double> >::ConvolutionFFTPolicy( int array_size, bool measure );


//------------------------------------------------------------------------------------------------------------

template<typename T>
class ConvolutionInnerProductPolicy  {
 public:

  ConvolutionInnerProductPolicy( int nsamples, bool );

  template<typename Fnct>
    ConvolutionInnerProductPolicy(  int nsamples, Fnct lhs, bool ); 

 ~ConvolutionInnerProductPolicy();

  std::vector<T> const&   operator()( std::vector<T>  const& lhs, std::vector<T>  const& rhs );
  std::vector<T> const&   operator()( std::vector<T>  const& rhs );

  void resetLHS( std::vector<T> const& lhs);

 private:

  std::vector<T>          lhsdata_;
  std::vector<T>          result_;


};

#ifndef BASICTOOLKIT_EXPLICIT_TEMPLATES
#include <basic_toolkit/ConvolutionPolicies.tcc>
#endif

#endif
