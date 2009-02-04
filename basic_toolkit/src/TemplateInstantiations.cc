/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TemplateInstantiations.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Leo Michelotti                                     
******             Email: michelotti@fnal.gov                         
****** 
******  Revision (Sep 2005):
******
******             Jean-Francois Ostiguy
******             ostiguy@fnal.gov
******             
******             - segregated explicit template instantiations
******             - new template Vector class
******           
****** Mar 2007 ostiguy@fnal.gov
******
******  - eliminated need for instantiating dependant classes, in particular
******    private classes used by the STL implementation.
******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/

// =================================================================================
// If implicit template instantiations are desired, *do not* compile this file !
// =================================================================================

#ifdef BASICTOOLKIT_EXPLICIT_TEMPLATES 

#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <complex>
#include <iostream>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <numeric>
#include <functional>
#include <algorithm>
#include <list>
#include <boost/typeof/typeof.hpp>
#include <boost/function.hpp>

#include <basic_toolkit/TMatrix.h>
#include <basic_toolkit/MathConstants.h>
#ifdef WIN32
#include <basic_toolkit/Distribution.h> // for drand48
#endif

#include <gms/FastPODAllocator.h>
#include <basic_toolkit/TMatrix.tcc>
#include <basic_toolkit/TML.tcc>
#include <basic_toolkit/TVector.tcc>
#include <basic_toolkit/FFTWAllocator.h>
#include <basic_toolkit/FFTFunctor.h>
#include <basic_toolkit/FFTFunctorImpl.h>
#include <basic_toolkit/FFTFunctor.tcc>
#include <basic_toolkit/FFTFunctorImpl.tcc>
#include <basic_toolkit/ConvolutionFunctor.h>
#include <basic_toolkit/ConvolutionFunctorImpl.h>
#include <basic_toolkit/ConvolutionFunctorImpl.tcc>
#include <basic_toolkit/ConvolutionFunctor.tcc>


using namespace std;
using std::ostringstream;

const double limit = double(1e-14);
#define WID setw(8)
#define PREC setprecision(8)

// ----------------------------------------------------------------------------
// Instantiations related to specialized allocator
// ----------------------------------------------------------------------------
template 
class boost::pool<>;

template
class boost::simple_segregated_storage<unsigned int>;

template
unsigned int boost::details::pool::lcm<unsigned int>(unsigned int const&, unsigned int const&);

template
unsigned int boost::details::pool::gcd<unsigned int>(unsigned int, unsigned int);

// ----------------------------------------------------------------------------
// Instantiations related to TVector 
// ----------------------------------------------------------------------------

template class TVector<double>;
template class TVector<std::complex<double> >;

template double TVector<double>::Norm() const;
template double TVector<std::complex<double> >::Norm() const;

template TVector<double> operator-(TVector<double> const&);
template TVector<std::complex<double> > operator-(TVector<std::complex<double> > const&);

template TVector<double> operator*<double>(TVector<double> const&,          double const&);
template TVector<double> operator*<double>(         double const&, TVector<double> const&);

template TVector<std::complex<double> > operator*(          std::complex<double> const&, TVector<std::complex<double> > const&);
template TVector<std::complex<double> > operator*(TVector<std::complex<double> > const&,           std::complex<double> const&);

template std::ostream& operator<<(std::ostream&, TVector<double> const&);
template std::ostream& operator<<(std::ostream&, TVector<std::complex<double> > const&);

template double std::inner_product( TVector<double>::const_iterator, TVector<double>::const_iterator, 
                                    TVector<double>::const_iterator, double);
template complex<double> std::inner_product(TVector<complex<double> >::const_iterator,   TVector<complex<double> >::const_iterator, 
                                            TVector<complex<double> >::const_iterator, 
                                            complex<double>, 
                                            std::plus<std::complex<double> >, TVector<complex<double> >::op_mult ); 


template TVector<std::complex<double> >::TVector( std::vector<std::complex<double> >::iterator,  
                                                  std::vector<std::complex<double> >::iterator);

template TVector<double>::TVector( TVector<double>::iterator,   
                                   TVector<double>::iterator);

template TVector<std::complex<double> >::TVector(  TVector<std::complex<double> >::const_iterator,  
                                                   TVector<std::complex<double> >::const_iterator);

template TVector<double>::TVector( double*, double* );

template TVector<double>::TVector( double const*, double const* );

template TVector<std::complex<double> >::TVector( std::complex<double>*,  
                                                  std::complex<double>* );

template TVector<std::complex<double> >::TVector( std::complex<double> const*,  
                                                  std::complex<double> const* );


template 
TVector<double>::iterator
std::transform( TVector<std::complex<double> >::const_iterator, 
                TVector<std::complex<double> >::const_iterator, 
                TVector<double>::iterator, 
                boost::function<double(std::complex<double> const&)> ); 


template TMatrix<double> operator%( TVector<double> const&,       TVector<double> const&); // outer product 

template TVector<double>::iterator                std::copy( double const*, double const*, TVector<double>::iterator );   
template TVector<std::complex<double> >::iterator std::copy( std::complex<double> const*, 
                                                             std::complex<double> const*, TVector<std::complex<double> >::iterator );   

// ----------------------------------------------------------------------------
// Instantiations related to class ConvolutionFunctor
// ----------------------------------------------------------------------------

template class
std::vector<double>;

template class
std::vector<std::complex<double> >;

template class FFTFunctor<double,                     std::complex<double>, fft_forward>;
template class FFTFunctor<std::complex<double>,       double,               fft_backward>;

template class FFTFunctor<std::complex<double>, std::complex<double>, fft_forward >;
template class FFTFunctor<std::complex<double>, std::complex<double>, fft_backward >;

template
FFTFunctor<double, std::complex<double>, fft_forward>*  boost::addressof(FFTFunctor<double, std::complex<double>, fft_forward>&);

template
FFTFunctor<std::complex<double>, double, fft_backward>* boost::addressof(FFTFunctor<std::complex<double>, double, fft_backward>&);

template
FFTFunctor<std::complex<double>, std::complex<double>, fft_forward>*  boost::addressof(FFTFunctor<std::complex<double>, std::complex<double>, fft_forward>&);

template
FFTFunctor<std::complex<double>, std::complex<double>, fft_backward>* boost::addressof(FFTFunctor<std::complex<double>, std::complex<double>, fft_backward>& );

template
FFTFunctor<std::complex<double>, std::complex<double>, fft_forward> const* boost::addressof(FFTFunctor<std::complex<double>, std::complex<double>,  fft_forward> const& );

template 
FFTFunctor<std::complex<double>, std::complex<double>, fft_backward> const* boost::addressof(FFTFunctor<std::complex<double>, std::complex<double>, fft_backward> const& );

template
FFTFunctor<std::complex<double>, double, (transform_type)1> const* 
boost::addressof(FFTFunctor<std::complex<double>, double, (transform_type)1> const&);

template
FFTFunctor<double, std::complex<double>, (transform_type)-1> const* 
boost::addressof(FFTFunctor<double, std::complex<double>, (transform_type)-1> const&);


template class ConvolutionFunctor<double>;
template class ConvolutionFunctor<std::complex<double> >;

template class ConvolutionFunctorFFTImpl<double>;
template class ConvolutionFunctorFFTImpl<std::complex<double> >;

template 
class FFTWAllocator<double>;
template 
class FFTWAllocator<std::complex<double> >;

template class std::vector<double,               FFTWAllocator<double> >;
template class std::vector<std::complex<double>, FFTWAllocator<std::complex<double> > >;

template
bool operator == ( FFTWAllocator<double> const&,   FFTWAllocator<std::complex<double> > const& ) throw();

template
bool operator == ( FFTWAllocator<std::complex<double> > const&,  FFTWAllocator<double> const& ) throw();

// ----------------------------------------------------------------------------
// Instantiations related to NewtonSolver, QuasiNewtonSolver 
// ----------------------------------------------------------------------------

template 
class boost::function<Vector  ( Vector const&) >;

template 
class boost::function<double  ( Vector const&) >;

template 
class boost::function<Matrix  ( Vector const&) >; 


template 
boost::function<double( TVector<double> const&) > const* 
boost::addressof( boost::function<double(TVector<double> const& ) > const& );

template
boost::function<TVector<double>(TVector<double> const&) > const* 
boost::addressof( boost::function< TVector<double>(TVector<double> const& )> const& );

template TMatrix<double> boost::function1<TMatrix<double>, TVector<double> const&, std::allocator<void> >::operator()(TVector<double> const&) const;
template double          boost::function1<double, TVector<double> const&, std::allocator<void> >::operator()(TVector<double> const&) const;
template TVector<double> boost::function1<TVector<double>, TVector<double> const&, std::allocator<void> >::operator()(TVector<double> const&) const;



// ----------------------------------------------------------------------------
// Misc Instantiations related to STL
// ----------------------------------------------------------------------------

template class std::list<int>;

template
double* std::copy( double const*, double const*, double* );

//template
//class std::binder2nd<std::divides<std::complex<double> > >; 

template
TVector<double>::iterator 
std::transform(  TVector<double>::iterator, 
                 TVector<double>::iterator,  
                 TVector<double>::iterator, 
                 std::negate<double> ); 
template
TVector<std::complex<double> >::iterator 
std::transform(  TVector<std::complex<double> >::iterator, 
                 TVector<std::complex<double> >::iterator,  
                 TVector<std::complex<double> >::iterator, 
                 std::negate<std::complex<double> > ); 
template
TVector<double>::iterator 
std::transform(  TVector<double>::iterator, 
                 TVector<double>::iterator,  
                 TVector<double>::iterator, 
                 std::binder2nd< std::divides<double> > ); 
template
TVector<std::complex<double> >::iterator 
std::transform(  TVector<std::complex<double> >::iterator, 
                 TVector<std::complex<double> >::iterator,  
                 TVector<std::complex<double> >::iterator, 
                 std::binder2nd< std::divides<std::complex<double> > > ); 
template
TVector<double>::iterator 
std::transform(  TVector<double>::iterator, 
                 TVector<double>::iterator,  
                 TVector<double>::iterator, 
                 std::binder2nd< std::multiplies<double> > ); 
template
TVector<std::complex<double> >::iterator 
std::transform(  TVector<std::complex<double> >::iterator, 
                 TVector<std::complex<double> >::iterator,  
                 TVector<std::complex<double> >::iterator, 
                 std::binder2nd< std::multiplies<std::complex<double> > > ); 

template
TVector<double>::iterator
std::copy( TVector<double>::const_iterator,  
           TVector<double>::const_iterator,  
           TVector<double>::iterator);   

template
TVector<std::complex<double> >::iterator
std::copy( TVector<std::complex<double> >::const_iterator,  
           TVector<std::complex<double> >::const_iterator,  
           TVector<std::complex<double> >::iterator);   

template
TVector<double>::iterator
std::transform( TVector<double>::const_iterator,  
                TVector<double>::const_iterator, 
                TVector<double>::const_iterator, 
                TVector<double>::iterator, std::plus<double> ); 

template
TVector<double>::iterator
std::transform( TVector<double>::const_iterator,  
                TVector<double>::const_iterator, 
                TVector<double>::const_iterator, 
                TVector<double>::iterator, std::minus<double> ); 

template
TVector<std::complex<double> >::iterator
std::transform( TVector<std::complex<double> >::const_iterator,  
                TVector<std::complex<double> >::const_iterator, 
                TVector<std::complex<double> >::const_iterator, 
                TVector<std::complex<double> >::iterator, 
                std::plus<std::complex<double> > ); 
template
 TVector<std::complex<double> >::iterator
std::transform( TVector<std::complex<double> >::const_iterator,  
                TVector<std::complex<double> >::const_iterator, 
                TVector<std::complex<double> >::const_iterator, 
                TVector<std::complex<double> >::iterator, 
                std::minus<std::complex<double> > ); 

#endif
