/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                        Basic TOOLKIT  
******   Low level utility C++ classes and functions.
******                                    
******  File:      FFTFunctor.h
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
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef FFTFUNCTOR_H
#define FFTFUNCTOR_H

#include <complex> 
#include <vector>
#include <boost/shared_ptr.hpp>

enum transform_type { fft_forward=1, fft_backward=-1 };

template <typename Input_t, typename Output_t, transform_type direction>
class FFTFunctor {

 public:
 
  class FFTFunctorImpl;

  FFTFunctor( int array_size, bool measure=false);
 ~FFTFunctor();

  std::vector<Output_t>  operator()( std::vector<Input_t> const& lhs );

  Output_t*              operator()( Input_t* lhs );

 private:

  boost::shared_ptr<FFTFunctorImpl> pimpl_; 

};

//-------------------
// Specializations
//-------------------

template<>
class FFTFunctor<double, double, fft_forward>;                 // not implemented !

template<>
class FFTFunctor<double, double, fft_backward>;                // not implemented !

template<>
class FFTFunctor<double, std::complex<double>, fft_backward>;  // not implemented !

template<>
class FFTFunctor<std::complex<double>, double, fft_forward>;   // not implemented !


#ifndef BASICTOOLKIT_EXPLICIT_TEMPLATES
#include <basic_toolkit/FFTFunctor.tcc>
#endif


#endif  // FFTFUNCTOR_H

