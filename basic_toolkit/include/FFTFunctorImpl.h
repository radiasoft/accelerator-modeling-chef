/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                        Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      FFTFunctorImpl.h
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
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef FFTFUNCTORIMPL_H
#define FFTFUNCTORIMPL_H

#include <fftw3.h>
#include <boost/function.hpp>
#include <algorithm>
#include <functional>

template <typename U>
     struct FFTWType {
};

template<>
struct FFTWType<double> {
   typedef double                Type; 
}; 

template<>
struct FFTWType<std::complex<double> > {
   typedef fftw_complex          Type; 
}; 

// enum transform_type { fft_forward=FFTW_FORWARD, fft_backward=FFTW_BACKWARD };

template <typename Input_t, typename Output_t, transform_type direction>
class FFTFunctor<Input_t, Output_t,direction>::FFTFunctorImpl {

 public:

  typedef typename  FFTWType<Input_t>::Type   FFTW_Input_t;
  typedef typename  FFTWType<Output_t>::Type  FFTW_Output_t;

  FFTFunctorImpl( int array_size, bool measure=false);
 ~FFTFunctorImpl();


  std::vector<Output_t>  operator()( std::vector<Input_t> const& lhs );
  Output_t*              operator()( Input_t* lhs );                     // in place FFT 

 private:
  
  FFTFunctorImpl( FFTFunctorImpl const& other); // 

  void ctor();

  bool                                         measure_;
  int                                          nsamples_;
  int                                          input_array_size_;
  int                                          output_array_size_;
  fftw_plan                                    plan_; 

  std::vector<double, FFTWAllocator<double> >  data_;

  boost::function<void ( fftw_plan, FFTW_Input_t*,   FFTW_Output_t*) >   transform_; 

};

template<>
void FFTFunctor<double, std::complex<double>, fft_forward>::FFTFunctorImpl::ctor();

template<>
void FFTFunctor<std::complex<double>, double, fft_backward>::FFTFunctorImpl::ctor();



#ifndef BASICTOOLKIT_EXPLICIT_TEMPLATES
#include <basic_toolkit/FFTFunctorImpl.tcc>
#endif

#endif  // FFTFUNCTORIMPL_H
