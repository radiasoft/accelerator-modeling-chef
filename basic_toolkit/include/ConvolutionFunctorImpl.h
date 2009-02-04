/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                        Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      ConvolutionFunctorImpl.h
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
******* A function object to efficiently perform a convolution between  
******  two functions using FFTs. 
******
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef CONVOLUTIONFUNCTORIMPL_H
#define CONVOLUTIONFUNCTORIMPL_H

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


template <typename T>
class ConvolutionFunctor<T>::ConvolutionFunctorImpl {

 public:

  virtual ~ConvolutionFunctorImpl() = 0 ;

  virtual std::vector<T> const&   operator()( std::vector<T>  const& lhs, std::vector<T>  const& rhs ) = 0;
  virtual std::vector<T> const&   operator()( std::vector<T>  const& rhs )                             = 0;

  virtual void resetLHS( std::vector<T>  const& lhs) = 0;

};

//---------------------------------------------------------------------------------------------------------

template <typename T>
class ConvolutionFunctorFFTImpl: public ConvolutionFunctor<T>::ConvolutionFunctorImpl {

 public:

  typedef typename ConvolutionFunctorTraits<T>::FFT_Input_t   FFT_Input_t;
  typedef typename ConvolutionFunctorTraits<T>::FFT_Output_t  FFT_Output_t;

  ConvolutionFunctorFFTImpl( int sample, bool measure);

  template<typename Fnct>
  ConvolutionFunctorFFTImpl(  int nsamples, Fnct lhs, bool measure); 

 ~ConvolutionFunctorFFTImpl();

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
ConvolutionFunctorFFTImpl<double>::ConvolutionFunctorFFTImpl( int array_size, bool fftw_measure);

template<>
ConvolutionFunctorFFTImpl<std::complex<double> >::ConvolutionFunctorFFTImpl( int array_size, bool fftw_measure);


#ifndef BASICTOOLKIT_EXPLICIT_TEMPLATES
#include <basic_toolkit/ConvolutionFunctorImpl.tcc>
#endif


#endif  // CONVOLUTIONFUNCTORIMPL_H

