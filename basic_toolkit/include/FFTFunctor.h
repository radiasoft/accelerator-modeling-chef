/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                        Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      FFTFunctor.h
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

#ifndef FFTFUNCTOR_H
#define FFTFUNCTOR_H

#include <complex> 
#include <fftw3.h>
#include <vector>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <basic_toolkit/FFTWAllocator.h>

enum transform_type { fft_forward=FFTW_FORWARD, fft_backward=FFTW_BACKWARD };

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


//-----------------------------------------------

 
template <typename Input_t, typename Output_t, transform_type direction>
class FFTFunctorImpl {

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

  bool                                                                   measure_;
  int                                                                    nsamples_;
  int                                                                    input_array_size_;
  int                                                                    output_array_size_;
  fftw_plan                                                              plan_; 

  std::vector<double, FFTWAllocator<double> >                            data_;

  boost::function<void ( fftw_plan, FFTW_Input_t*,   FFTW_Output_t*) >   transform_; 

};

//--------------------------------------------------------------------------------------------------

template <typename Input_t, typename Output_t, transform_type direction>
class FFTFunctor {

  typedef FFTFunctorImpl<Input_t,Output_t,direction> ImplType;

 public:
 

  FFTFunctor( int array_size, bool measure=false)
  { 
     pimpl_ = boost::shared_ptr<ImplType>( new ImplType( array_size, measure ) ); 
  }

  ~FFTFunctor() {}


  std::vector<Output_t>  operator()( std::vector<Input_t> const& lhs )
    { return pimpl_->operator()(lhs); }

  Output_t*              operator()( Input_t* lhs )                  // in place FFT 
    { return pimpl_->operator()(lhs); }

 private:

   boost::shared_ptr<ImplType>   pimpl_;

};

//-------------------
// Specializations
//-------------------

template<>
class FFTFunctorImpl<double, double, fft_forward>;                 // not implemented !

template<>
class FFTFunctorImpl<double, double, fft_backward>;                // not implemented !

template<>
class FFTFunctorImpl<double, std::complex<double>, fft_backward>;  // not implemented !

template<>
class FFTFunctorImpl<std::complex<double>, double, fft_forward>;   // not implemented !


template<>
void FFTFunctorImpl<double, std::complex<double>, fft_forward>::ctor();

template<>
void FFTFunctorImpl<std::complex<double>, double, fft_backward>::ctor();



#ifndef BASICTOOLKIT_EXPLICIT_TEMPLATES
#include <basic_toolkit/FFTFunctor.tcc>
#endif


#endif  // FFTFUNCTOR_H

