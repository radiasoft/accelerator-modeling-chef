/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                        Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      ConvolutionFunctor.h
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
****** SYNOPSIS:
****** 
******* A function object to efficiently perform a convolution between  
******  two functions using FFTs. 
******
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef CONVOLUTIONFUNCTOR_H
#define CONVOLUTIONFUNCTOR_H

#include <complex> // note: this header *must* be included *before* fftw3.h
#include <vector>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <basic_toolkit/ConvolutionPolicies.h>

template <typename T, template<typename U> class AlgorithmPolicy > 
class ConvolutionFunctor : public AlgorithmPolicy<T> {

 public:

  ConvolutionFunctor( int nsamples=0, bool test =true ); 

  // Fnct is a function object. lhs(i) returns the value at sample i
  // The samples i are assumed to be equally spaced.  

  template<typename Fnct>
  ConvolutionFunctor(  int nsamples, Fnct lhs, bool test );

 ~ConvolutionFunctor();

  void resetLHS( std::vector<T>  const& lhs);

  std::vector<T>   operator()( std::vector<T>  const& lhs, std::vector<T>  const& rhs );
  std::vector<T>   operator()( std::vector<T> const& rhs );

};


template <typename T,  template<typename U> class AlgorithmPolicy >
template <typename Fnct>
ConvolutionFunctor<T, AlgorithmPolicy >::ConvolutionFunctor(  int nsamples, Fnct lhs, bool measure ) 
: AlgorithmPolicy<T>(nsamples, lhs, measure)
{}


#ifndef BASICTOOLKIT_EXPLICIT_TEMPLATES
#include <basic_toolkit/ConvolutionFunctor.tcc>
#endif


#endif  // CONVOLUTIONFUNCTOR_H

