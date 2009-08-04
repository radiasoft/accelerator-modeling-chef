/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                   Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      ConvolutionPolicies.tcc
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
#include <algorithm>
#include <numeric>


template <typename T>
ConvolutionFFTPolicy<T>::~ConvolutionFFTPolicy() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
std::vector<T> const& ConvolutionFFTPolicy<T>::operator()( std::vector<T> const& lhs, std::vector<T> const& rhs )  
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
std::vector<T> const& ConvolutionFFTPolicy<T>::operator()( std::vector<T> const& rhs )  
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
void ConvolutionFFTPolicy<T>::resetLHS( std::vector<T>  const& lhs) 
{
  //---------------------------------------------------
  // recompute and store new lhs operand transform ...  
  //---------------------------------------------------

   std::copy( (FFT_Input_t*) &lhs[0],  (FFT_Input_t*) &lhs[0] + fft_input_array_size_,   (FFT_Input_t*)  &lhsdata_[0] );   

   forward_transform_( (FFT_Input_t*) &lhsdata_[0] );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename T>
ConvolutionInnerProductPolicy<T>::ConvolutionInnerProductPolicy( int nsamples, bool measure )
: lhsdata_(nsamples), result_()
{}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
ConvolutionInnerProductPolicy<T>:: ~ConvolutionInnerProductPolicy()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
std::vector<T> const&  ConvolutionInnerProductPolicy<T>::operator()( std::vector<T>  const& lhs, std::vector<T>  const& rhs )
{ 

  //---------------------------------------------------------------
  // Performs the discrete Convolution   h[n] = Sum_m f[m] g[n-m] 
  // f and g are assumed to be sampled equally with e same interval 
  // value. The number of samples in for function need not be equal.  
  //----------------------------------------------------------------

   result_.clear();

   typename std::vector<T>::const_iterator         itlhs1   =   lhs.begin();
   typename std::vector<T>::const_iterator         itlhs2   = ++(lhs.begin());
   typename std::vector<T>::const_reverse_iterator itrhs1   = --(rhs.rend());

   typename std::vector<T>::iterator it  = result_.begin(); 

   for (  ; itlhs2 != lhs.end(); ++itlhs2, --itrhs1 ) {
        if ( std::distance( itlhs1, itlhs2 ) <= std::distance( itrhs1, rhs.rend()) ) {
            result_.push_back( std::inner_product( itlhs1, itlhs2, itrhs1, T() ) );
        }
        else {
            result_.push_back( std::inner_product( itrhs1, rhs.rend(), itlhs1, T() ) );
        }
   }
   
   itrhs1 = rhs.rbegin();  
   ++itlhs1;
 
   for (  ; itlhs1 != lhs.end(); ++itlhs1 ) {
       if (std::distance( itlhs1, itlhs2 ) <= std::distance( itrhs1, rhs.rend()) ) { 
            result_.push_back( std::inner_product( itlhs1, itlhs2, itrhs1, T() ));
       }
       else {
            result_.push_back( std::inner_product( itrhs1, rhs.rend(), itlhs1, T() ) );
       }
   }

  return result_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
std::vector<T> const&   ConvolutionInnerProductPolicy<T>::operator()( std::vector<T>  const& rhs )
{
  
  //------------------------------------------------
  // Discrete Convolution   h[n] = Sum_m f[m] g[n-m] 
  //-------------------------------------------------

   result_.clear();

   typename std::vector<T>::const_iterator         itlhs1   =   lhsdata_.begin();
   typename std::vector<T>::const_iterator         itlhs2   = ++(lhsdata_.begin());
   typename std::vector<T>::const_reverse_iterator itrhs1   = --(rhs.rend());

   typename std::vector<T>::iterator it  = result_.begin(); 

   for (  ; itlhs2 != lhsdata_.end(); ++it, ++itlhs2, --itrhs1 ) {
        if (std::distance( itlhs1,itlhs2 ) <= std::distance(itrhs1, rhs.rend()) ) {
             result_.push_back(std::inner_product( itlhs1, itlhs2, itrhs1, T() ) );
        }
        else {
             result_.push_back(std::inner_product( itrhs1, rhs.rend(), itlhs1, T() ) );
        }
   }
   
   itrhs1 = rhs.rbegin();  
   ++itlhs1;
 
   for (  ; itlhs1 != lhsdata_.end(); ++it, ++itlhs1 ) {
       if (std::distance( itlhs1, itlhs2 ) <= std::distance( itrhs1, rhs.rend()) ){ 
            result_.push_back( std::inner_product( itlhs1, itlhs2, itrhs1, T() ) );
       }
       else {
            result_.push_back( std::inner_product( itrhs1, rhs.rend(), itlhs1, T() ) );
       }
   }

  return result_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename T>
void ConvolutionInnerProductPolicy<T>::resetLHS( std::vector<T>  const& lhs)
{
   lhsdata_ = lhs;  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||






