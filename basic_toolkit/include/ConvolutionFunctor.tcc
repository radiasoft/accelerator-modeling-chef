/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                   Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      ConvolutionFunctor.tcc
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
#include <iostream>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
ConvolutionFunctorImpl<T>::~ConvolutionFunctorImpl() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
std::vector<T> const& ConvolutionFunctorImpl<T>::operator()( std::vector<T> const& lhs, std::vector<T> const& rhs )  
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
  
  std::copy(  (T*) &lhsdata_[0], (T*) &lhsdata_[0]+ fft_input_array_size_, (T*) &result_[0] );
  
  return result_;
} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
std::vector<T> const& ConvolutionFunctorImpl<T>::operator()( std::vector<T> const& rhs )  
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


  for ( ; it_lhs != it_lhs_end;  ++it_lhs, ++it_rhs )
  {
    (*it_lhs) *= (*it_rhs);
  }
 
  //------------------------------------
  // invert and return ...
  //-------------------------------------
  
  inverse_transform_( (FFT_Output_t*) &lhsdata_[0] );
  
  std::copy(  (T*) &lhsdata_[0], (T*) &lhsdata_[0]+ fft_input_array_size_, (T*) &result_[0] );
  
  return result_;
} 

