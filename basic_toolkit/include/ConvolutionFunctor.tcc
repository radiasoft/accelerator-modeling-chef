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


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename T>
ConvolutionFunctor<T>::ConvolutionFunctor( int nsamples, bool measure) 
{ 
   pimpl_ = boost::shared_ptr<ConvolutionFunctorImpl >( new ConvolutionFunctorFFTImpl<T>( nsamples, measure) ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
template<typename Fnct>
ConvolutionFunctor<T>::ConvolutionFunctor(  int nsamples, Fnct lhs, bool measure ) { 
  pimpl_ = boost::shared_ptr<ConvolutionFunctorImpl >( new ConvolutionFunctorFFTImpl<T>( nsamples, lhs, measure) ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
ConvolutionFunctor<T>::~ConvolutionFunctor() 
{ }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
void ConvolutionFunctor<T>::resetLHS( std::vector<T>  const& lhs)
{ 
  return pimpl_->resetLHS( lhs); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
std::vector<T>   ConvolutionFunctor<T>::operator()( std::vector<T>  const& lhs, std::vector<T>  const& rhs )
{ 
  return pimpl_->operator()( lhs, rhs); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
std::vector<T>   ConvolutionFunctor<T>::operator()(  std::vector<T> const& rhs ) 
{ 
  return pimpl_->operator()( rhs); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

