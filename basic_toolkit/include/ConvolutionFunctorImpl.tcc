/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                   Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      ConvolutionFunctorImpl.tcc
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

#include <complex> // note: this header *must* be included *before* fftw3.h
#include <fftw3.h>
#include <basic_toolkit/FFTFunctor.h>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
ConvolutionFunctor<T>::ConvolutionFunctorImpl::~ConvolutionFunctorImpl() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
ConvolutionFunctorFFTImpl<T>::~ConvolutionFunctorFFTImpl() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
std::vector<T> const& ConvolutionFunctorFFTImpl<T>::operator()( std::vector<T> const& lhs, std::vector<T> const& rhs )  
{
  //-------------------------------------------------------------------------------------------
  // compute lhs transform ... If unary=true, lhs is assumed to already contain the transform 
  //-------------------------------------------------------------------------------------------

   std::copy( (FFT_Input_t*) &lhs[0],  (FFT_Input_t*) &lhs[0] + fft_input_array_size_,   (FFT_Input_t*)  &lhsdata_[0] );   

   forward_transform_( (FFT_Input_t*) &lhsdata_[0] );


  //--------------------------------------------------------------
  // compute rhs transform ....
  //--------------------------------------------------------------

   std::copy(  (FFT_Input_t*) &rhs[0],  (FFT_Input_t*) &rhs[0] + fft_input_array_size_ ,   (FFT_Input_t*) &rhsdata_[0] );

   forward_transform_( (FFT_Input_t*) &rhsdata_[0] );

  
  //--------------------------------------------------------------
  // compute product in Fourier space and apply normalization ...
  //--------------------------------------------------------------
 
  FFT_Output_t*  it_lhs     =  ( FFT_Output_t* ) &lhsdata_[0];
  FFT_Output_t*  it_lhs_end =  ( FFT_Output_t* ) &lhsdata_[0] + fft_output_array_size_;

  FFT_Output_t*  it_rhs     =  ( FFT_Output_t* ) &rhsdata_[0];
  FFT_Output_t*  it_rhs_end =  ( FFT_Output_t* ) &rhsdata_[0] + fft_output_array_size_;


  for ( ; it_lhs != it_lhs_end;  ++it_lhs, ++it_rhs )
  {
    (*it_lhs) *= (*it_rhs);
  }
 
  //------------------------------------
  // invert and return ...
  //-------------------------------------
  
  inverse_transform_( (FFT_Output_t*) &lhsdata_[0] );
  
  return reinterpret_cast<std::vector<T> const&> ( lhsdata_ );
} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
std::vector<T> const& ConvolutionFunctorFFTImpl<T>::operator()( std::vector<T> const& rhs )  
{
  //-------------------------------------------------------------------------------------------
  // lhsdata_ already contain the transform 
  //-------------------------------------------------------------------------------------------

  //--------------------------------------------------------------
  // compute rhs transform ....
  //--------------------------------------------------------------

   std::copy(  (FFT_Input_t*) &rhs[0],  (FFT_Input_t*) &rhs[0] + fft_input_array_size_ ,   (FFT_Input_t*) &rhsdata_[0] );

   forward_transform_( (FFT_Input_t*) &rhsdata_[0] );

  //--------------------------------------------------------------
  // compute product in Fourier space and apply normalization ...
  //--------------------------------------------------------------
 
  FFT_Output_t*  it_lhs     =  ( FFT_Output_t* ) &lhsdata_[0];
  FFT_Output_t*  it_lhs_end =  ( FFT_Output_t* ) &lhsdata_[0] + fft_output_array_size_;

  FFT_Output_t*  it_rhs     =  ( FFT_Output_t* ) &rhsdata_[0];
  FFT_Output_t*  it_rhs_end =  ( FFT_Output_t* ) &rhsdata_[0] + fft_output_array_size_;

  FFT_Output_t*  it_result     =  ( FFT_Output_t* ) &result_[0];
  FFT_Output_t*  it_result_end =  ( FFT_Output_t* ) &result_[0] + fft_output_array_size_;

  std::copy( it_lhs, it_lhs_end, it_result);

  for ( ; it_result != it_result_end;  ++it_result, ++it_rhs )
  {
    (*it_result) *= (*it_rhs);
  }
 
  //------------------------------------
  // invert and return ...
  //-------------------------------------
  
  inverse_transform_( (FFT_Output_t*) &result_[0] );
  return reinterpret_cast<std::vector<T> const&> (result_ );
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
  
template <typename T>
void ConvolutionFunctorFFTImpl<T>::resetLHS( std::vector<T>  const& lhs) 
{
  //---------------------------------------------------
  // recompute and store new lhs operand transform ...  
  //---------------------------------------------------

   std::copy( (FFT_Input_t*) &lhs[0],  (FFT_Input_t*) &lhs[0] + fft_input_array_size_,   (FFT_Input_t*)  &lhsdata_[0] );   

   forward_transform_( (FFT_Input_t*) &lhsdata_[0] );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<typename Fnct>
ConvolutionFunctorFFTImpl<double>::ConvolutionFunctorFFTImpl( int nsamples, Fnct lhs, bool measure )
  : nsamples_(nsamples), fft_input_array_size_(nsamples),  fft_output_array_size_(nsamples/2+1)   
{
  
  lhsdata_.resize(  2 * (nsamples/2+ 1) ); 
  rhsdata_.resize(  2 * (nsamples/2+ 1) ); 
   result_.resize(  2 * nsamples ); 

  forward_transform_ =  FFTFunctor<double, std::complex<double>, fft_forward >( nsamples_, measure);  
  inverse_transform_ =  FFTFunctor<std::complex<double>, double, fft_backward>( nsamples_, measure);

  int i = 0;
  for (  double* it  = &lhsdata_[0];  
                 it != &lhsdata_[0] + fft_input_array_size_;  ++it, ++i ) {
   (*it) = lhs(i);
  }

  forward_transform_(   (FFT_Input_t*) &lhsdata_[0] );
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<typename Fnct>
ConvolutionFunctorFFTImpl<std::complex<double> >::ConvolutionFunctorFFTImpl( int nsamples, Fnct lhs, bool measure)
  : nsamples_(nsamples),  fft_input_array_size_(nsamples),  fft_output_array_size_(nsamples)  
{
  
  lhsdata_.resize(  2 * nsamples );
  rhsdata_.resize(  2 * nsamples );
   result_.resize(  2 * nsamples ); 

  forward_transform_ =  FFTFunctor<std::complex<double>, std::complex<double>, fft_forward >( nsamples, measure);  
  inverse_transform_ =  FFTFunctor<std::complex<double>, std::complex<double>, fft_backward>( nsamples, measure);

  int i = 0;
  for ( std::complex<double>* it  = ( std::complex<double>* ) &lhsdata_[0];  
                              it != ( std::complex<double>* ) &lhsdata_[0]+ fft_input_array_size_ ;  ++it, ++i) {
    (*it) = lhs(i);
  };

  forward_transform_(   (FFT_Input_t*) &lhsdata_[0] );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
