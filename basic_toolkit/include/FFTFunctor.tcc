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

#include <basic_toolkit/FFTFunctorImpl.h>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Input_t, typename Output_t, transform_type fft_direction>
FFTFunctor<Input_t, Output_t,  fft_direction>::FFTFunctor( int array_size, bool measure)
{ 
     pimpl_ = boost::shared_ptr<FFTFunctorImpl>( new FFTFunctorImpl( array_size, measure ) ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Input_t, typename Output_t, transform_type fft_direction>
FFTFunctor<Input_t, Output_t,  fft_direction>::~FFTFunctor() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Input_t, typename Output_t, transform_type fft_direction>
std::vector<Output_t>  FFTFunctor<Input_t, Output_t,  fft_direction>::operator()( std::vector<Input_t> const& lhs )
{ 
  return pimpl_->operator()(lhs); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Input_t, typename Output_t, transform_type fft_direction>
Output_t*  FFTFunctor<Input_t, Output_t,  fft_direction>::operator()( Input_t* lhs )                  // in place FFT 
{ 
  return pimpl_->operator()(lhs); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

