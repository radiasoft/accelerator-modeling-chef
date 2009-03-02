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


template <typename T,  template<typename U> class AlgorithmPolicy >
ConvolutionFunctor<T, AlgorithmPolicy>::ConvolutionFunctor( int nsamples, bool measure) 
:  AlgorithmPolicy<T>(nsamples,measure)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T,  template<typename U> class AlgorithmPolicy >
ConvolutionFunctor<T, AlgorithmPolicy >::~ConvolutionFunctor() 
{ }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T,  template<typename U> class AlgorithmPolicy >
void ConvolutionFunctor<T, AlgorithmPolicy >::resetLHS( std::vector<T>  const& lhs)
{ 
   AlgorithmPolicy<T>::resetLHS( lhs); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T,  template<typename U> class AlgorithmPolicy >
std::vector<T>   ConvolutionFunctor<T, AlgorithmPolicy >::operator()( std::vector<T>  const& lhs, std::vector<T>  const& rhs )
{ 
  return  AlgorithmPolicy<T>::operator()( lhs, rhs); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T,  template<typename U> class AlgorithmPolicy >
std::vector<T>   ConvolutionFunctor<T, AlgorithmPolicy >::operator()(  std::vector<T> const& rhs ) 
{ 
  return  AlgorithmPolicy<T>::operator()( rhs); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

