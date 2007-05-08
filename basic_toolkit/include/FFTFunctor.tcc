/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                   Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      FFTFunctor.tcc
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

#include <algorithm>
#include <functional>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename Input_t, typename Output_t, transform_type fft_direction>
FFTFunctorImpl<Input_t, Output_t, fft_direction>:: FFTFunctorImpl( int nsamples, bool measure)
 : nsamples_(nsamples), input_array_size_(0), output_array_size_(0),  measure_( measure)
{
  ctor();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Input_t, typename Output_t, transform_type fft_direction>
FFTFunctorImpl<Input_t, Output_t, fft_direction>::FFTFunctorImpl( FFTFunctorImpl const& other)
 : nsamples_(other.nsamples_), input_array_size_( other.input_array_size_), 
   output_array_size_(other.output_array_size_), measure_( other.measure_)
{

 ctor();

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename Input_t, typename Output_t, transform_type fft_direction>
void FFTFunctorImpl<Input_t, Output_t, fft_direction>::ctor()
{

  transform_  =  &fftw_execute_dft;

  data_.resize( nsamples_ * 2 ); 

  input_array_size_   =  nsamples_;
  output_array_size_  =  nsamples_;

  FFTW_Input_t*  fft_data_in      =  (FFTW_Input_t  *)   &data_[0];
  FFTW_Output_t* fft_data_out     =  (FFTW_Output_t *)   &data_[0];

  switch(fft_direction) { 

  case fft_forward:  
                plan_ = fftw_plan_dft_1d(nsamples_, fft_data_in, fft_data_out, FFTW_FORWARD,  (measure_ ? FFTW_MEASURE: FFTW_ESTIMATE) );
                break;

  case  fft_backward:
                plan_ = fftw_plan_dft_1d(nsamples_, fft_data_in, fft_data_out, FFTW_BACKWARD, (measure_ ? FFTW_MEASURE: FFTW_ESTIMATE) );
                break;
  default:
                break;

  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Input_t, typename Output_t, transform_type fft_direction>
FFTFunctorImpl<Input_t, Output_t, fft_direction>::~FFTFunctorImpl()
{
  fftw_destroy_plan( plan_ );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename Input_t, typename Output_t, transform_type fft_direction>
std::vector<Output_t > FFTFunctorImpl<Input_t, Output_t,fft_direction>::operator()( std::vector<Input_t> const& lhs )  
{
  
  //-------------------------------------------------------------------------------------------
  // compute transform ... 
  //-------------------------------------------------------------------------------------------

  std::copy( (Input_t*) &lhs[0], (Input_t*) &lhs[0]+ input_array_size_, (Input_t*) &data_[0]  );    

  FFTW_Input_t*  data_in  =  (FFTW_Input_t* ) &data_[0];
  FFTW_Output_t* data_out =  (FFTW_Output_t*) &data_[0];

  transform_( plan_, data_in, data_out );
  
  //--------------------------------------------------------------
  // apply normalization if necessary ... (inverse transform)
  //--------------------------------------------------------------
 
  if ( fft_direction == fft_backward ) 
     std::transform( (Output_t*) data_out, (Output_t*) data_out + output_array_size_, 
                     (Output_t*) data_out, std::bind2nd(std::divides<Output_t>(), nsamples_ ) );

  return std::vector<Output_t> ( (Output_t*) data_out, (Output_t*) data_out + output_array_size_ );    

} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Input_t, typename Output_t, transform_type fft_direction>
Output_t* FFTFunctorImpl<Input_t, Output_t, fft_direction >::operator() ( Input_t* lhs )  
{
    
  /**** in-place FFT ****/


  //-------------------------------------------------------------------------------------------
  // compute transform ... 
  //-------------------------------------------------------------------------------------------

  FFTW_Input_t*  data_in  =  (FFTW_Input_t*  ) lhs;
  FFTW_Output_t* data_out =  (FFTW_Output_t* ) lhs;

  transform_( plan_, data_in, data_out );
  
  //--------------------------------------------------------------
  // apply normalization if necessary ...
  //--------------------------------------------------------------
 
  if ( fft_direction == fft_backward ) 
     std::transform( (Output_t*) data_out, (Output_t*) data_out + output_array_size_, 
                     (Output_t*) data_out, std::bind2nd(std::divides<Output_t>(), nsamples_ ) );


  return reinterpret_cast<Output_t*>(lhs);
} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

