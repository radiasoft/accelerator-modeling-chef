/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                   Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      ConvolutionFunctor.cc
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

#include <basic_toolkit/ConvolutionFunctor.h>

using namespace std;
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
ConvolutionFunctorImpl<double>::ConvolutionFunctorImpl( int nsamples, bool measure)
  : nsamples_(nsamples), fft_input_array_size_(nsamples),  fft_output_array_size_(nsamples/2+1)   
{
  
  if (nsamples == 0 ) return;

  lhsdata_.resize(  2 * (nsamples/2+ 1) ); 
  rhsdata_.resize(  2 * (nsamples/2+ 1) ); 
   result_.resize(  2 * (nsamples/2+ 1) ); 

  forward_transform_ =  FFTFunctor<double, std::complex<double>, fft_forward >( nsamples_, measure);  
  inverse_transform_ =  FFTFunctor<std::complex<double>, double, fft_backward>( nsamples_, measure);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
ConvolutionFunctorImpl<std::complex<double> >::ConvolutionFunctorImpl( int nsamples, bool measure)
  : nsamples_(nsamples),  fft_input_array_size_(nsamples),  fft_output_array_size_(nsamples)  
{
  
  if (nsamples == 0 ) return;

  lhsdata_.resize(  2 * nsamples );
  rhsdata_.resize(  2 * nsamples );
   result_.resize(  2 * nsamples ); 

  forward_transform_ =  FFTFunctor<std::complex<double>, std::complex<double>, fft_forward >( nsamples, measure);  
  inverse_transform_ =  FFTFunctor<std::complex<double>, std::complex<double>, fft_backward>( nsamples, measure);


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


