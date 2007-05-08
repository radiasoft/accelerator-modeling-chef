/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                   Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      FFTFunctor.cc
******                                                                
******  Copyright (c) Fermi Research Alliance LLC 
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
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

#include <basic_toolkit/FFTWAllocator.h>
#include <basic_toolkit/FFTFunctor.h>

using namespace std;
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
void FFTFunctorImpl<double, std::complex<double>, fft_forward>::ctor()
{
  
  input_array_size_   =  nsamples_;
  output_array_size_  =  nsamples_/2+ 1;

  transform_  =  &fftw_execute_dft_r2c;
  data_       =  std::vector<double,FFTWAllocator<double> > ( 2*(nsamples_/2+ 1) ); 

  FFTW_Input_t*  fft_data_in      = (FFTW_Input_t*)  &data_[0];
  FFTW_Output_t* fft_data_out     = (FFTW_Output_t*) &data_[0];
 
  plan_ = fftw_plan_dft_r2c_1d( nsamples_, fft_data_in, fft_data_out, (measure_ ? FFTW_MEASURE: FFTW_ESTIMATE) );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
void FFTFunctorImpl<std::complex<double>, double, fft_backward>::ctor()
{
  
  input_array_size_  =  nsamples_/2+ 1;
  output_array_size_ =  nsamples_;

  transform_  =  &fftw_execute_dft_c2r;
  data_       =  std::vector<double,FFTWAllocator<double> > ( 2*(nsamples_/2+ 1) );

  FFTW_Input_t*  fft_data_in      =  (FFTW_Input_t* )  &data_[0];
  FFTW_Output_t* fft_data_out     =  (FFTW_Output_t* ) &data_[0];

  plan_   = fftw_plan_dft_c2r_1d( nsamples_, fft_data_in,  fft_data_out, (measure_ ? FFTW_MEASURE: FFTW_ESTIMATE) );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

