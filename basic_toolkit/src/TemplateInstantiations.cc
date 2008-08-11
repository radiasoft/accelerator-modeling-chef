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
#include <basic_toolkit/Barnacle.h>
#include <basic_toolkit/IntArray.h>
#include <basic_toolkit/IntArray.tcc>
#include <basic_toolkit/TMatrix.tcc>
#include <basic_toolkit/TML.tcc>
#include <basic_toolkit/TVector.tcc>
#include <basic_toolkit/FFTWAllocator.h>
#include <basic_toolkit/FFTFunctor.h>
#include <basic_toolkit/FFTFunctor.tcc>
#include <basic_toolkit/ConvolutionFunctor.h>
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
// Instantiations related to Barnacle
// ----------------------------------------------------------------------------

template class std::list<Barnacle>;

template void std::_List_base<Barnacle, std::allocator<Barnacle> >::_M_clear();

template BarnacleList::iterator
         std::remove_if(BarnacleList::iterator, BarnacleList::iterator, 
		        std::binder2nd<std::const_mem_fun1_ref_t<bool, Barnacle, std::string> >);

// ----------------------------------------------------------------------------
// Instantiations related to IntArray
// ----------------------------------------------------------------------------

template
IntArray::IntArray( int* it1, int* it2 );

template
IntArray::IntArray( int const* it1, int const* it2 );

template
void  IntArray::Set( int* it1, int* it2);

template
void  IntArray::Set( int const* it1, int const* it2);

template 
class std::vector<IntArray::exponent_t>;

namespace {
 
 IntArray::iterator               itdummy1;
 IntArray::const_iterator         itdummy2;  
 IntArray::reverse_iterator       itdummy3;
 IntArray::const_reverse_iterator itdummy4;
}
    
template
void std::fill( IntArray::iterator, IntArray::iterator, int const&); 


template
int std::accumulate( IntArray::iterator, IntArray::iterator,  int);

template
int std::accumulate( IntArray::const_iterator, IntArray::const_iterator,  int);

template 
class std::plus<IntArray::exponent_t>;

template 
class std::less_equal<IntArray::exponent_t>;

template
class std::greater<IntArray::exponent_t>;

template
class std::greater_equal<IntArray::exponent_t>;

template
IntArray::iterator std::transform( IntArray::iterator, IntArray::iterator, IntArray::iterator, IntArray::iterator,  std::plus<IntArray::exponent_t> );

template
IntArray::iterator std::transform( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator, IntArray::iterator,  std::plus<IntArray::exponent_t> );

template
IntArray::iterator std::transform( IntArray::iterator, IntArray::iterator, IntArray::iterator, IntArray::iterator,  std::multiplies<IntArray::exponent_t> );

template
IntArray::iterator std::transform( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator, IntArray::iterator,  std::multiplies<IntArray::exponent_t> );


template
bool std::equal( IntArray::iterator, IntArray::iterator, IntArray::iterator);

template
bool std::lexicographical_compare( IntArray::iterator, IntArray::iterator, IntArray::iterator,  IntArray::iterator, std::less<IntArray::exponent_t> );

template
bool std::lexicographical_compare( IntArray::iterator, IntArray::iterator, IntArray::iterator,  IntArray::iterator, std::less_equal<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::iterator, IntArray::iterator, IntArray::iterator,  IntArray::iterator, std::greater<IntArray::exponent_t>);

template
bool std::equal( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::less<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::less_equal<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::greater<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::greater_equal<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::less<IntArray::exponent_t> );

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::less_equal<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::greater<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::greater_equal<IntArray::exponent_t>);

template
void std::fill<int*, int>(int*, int*, int const&);

template
double* std::fill_n<double*, int, int>(double*, int, int const&);


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
// Instantiations related to Matrix Class
// ----------------------------------------------------------------------------

template class TMatrix<double>;
template class TMatrix<std::complex<double> >;
//template class TMatrix<int>;  // This should not be used at all, 
                                // but apparently it is ... 

template bool operator==( const TMatrix<double>&, TMatrix<double> const& );
template bool operator==( const TMatrix<double>&,          double const& );
template bool operator==( double const&,          TMatrix<double> const& );
template std::ostream& operator<<(std::ostream&,  TMatrix<double> const& );

template TMatrix<double> operator+(TMatrix<double> const&, TMatrix<double> const&);
template TMatrix<double> operator+(TMatrix<double> const&, double const&); 
template TMatrix<double> operator+(         double const&, TMatrix<double> const&); 
template TMatrix<double> operator-(TMatrix<double> const&); 
template TMatrix<double> operator-(TMatrix<double> const&, TMatrix<double> const&); 
template TMatrix<double> operator-(TMatrix<double> const&, double          const&); 
template TMatrix<double> operator-(         double const&, TMatrix<double> const&); 
template TMatrix<double> operator*(TMatrix<double> const&, TMatrix<double> const&); 
template TMatrix<double> operator*(TMatrix<double> const&,          double const&);
template TMatrix<double> operator*(         double const&, TMatrix<double> const&);
template TVector<double> operator*(TMatrix<double> const&, TVector<double> const&);


template TMatrix<double> operator/<double>(const TMatrix<double>&, const double&);
template TMatrix<double> operator/<double>(const double&,          TMatrix<double> const &);
template TMatrix<double> operator/<double>(TMatrix<double> const&, TMatrix<double> const&);


#if 0
==================integer form of Matrix is disabled ============================================
                    

template bool operator==( const TMatrix<int>&, const TMatrix<int>& );
template bool operator==( const TMatrix<int>&, const int& );
template bool operator==( const int&, const TMatrix<int>& );
template std::ostream& operator<<(std::ostream&, const TMatrix<int>&);

template TMatrix<int> operator+(const TMatrix<int>&, const TMatrix<int>&);
template TMatrix<int> operator+(const TMatrix<int>&, const int&); 
template TMatrix<int> operator+(const int&,          const TMatrix<int>&); 
template TMatrix<int> operator-(const TMatrix<int>&); 
template TMatrix<int> operator-(const TMatrix<int>&, const TMatrix<int>&); 
template TMatrix<int> operator-(const TMatrix<int>&, const int&); 
template TMatrix<int> operator-(const int&,          const TMatrix<int>&); 
template TMatrix<int> operator*(const TMatrix<int>&, const TMatrix<int>&); 
template TMatrix<int> operator*(const TMatrix<int>&, const int);
template TMatrix<int> operator*(const int&,          const TMatrix<int>&);
template TMatrix<int> operator/(TMatrix<int> const&, const int&);
template TMatrix<int> operator/(const int&,          TMatrix<int> const&);
template TMatrix<int> operator/(TMatrix<int> const&, TMatrix<int> const&);

===================================================================================================
#endif

template bool operator==( const TMatrix<std::complex<double> >&, const TMatrix<std::complex<double> >& );
template bool operator==( const TMatrix<std::complex<double> >&, const std::complex<double> & );
template bool operator==( const std::complex<double> &, const TMatrix<std::complex<double> >& );
template std::ostream& operator<< <std::complex<double> >(std::ostream&, TMatrix<std::complex<double> >const&);

template TMatrix<std::complex<double> > operator+(TMatrix<std::complex<double> > const&,       TMatrix<std::complex<double> > const& );
template TMatrix<std::complex<double> > operator+(TMatrix<std::complex<double> > const&,       std::complex<double>           const& ); 
template TMatrix<std::complex<double> > operator+(std::complex<double>  const&,                TMatrix<std::complex<double> > const& ); 
template TMatrix<std::complex<double> > operator-(TMatrix<std::complex<double> > const&); 
template TMatrix<std::complex<double> > operator-(TMatrix<std::complex<double> > const&,       TMatrix<std::complex<double> > const& ); 
template TMatrix<std::complex<double> > operator-(TMatrix<std::complex<double> > const&,       std::complex<double> const&           ); 
template TMatrix<std::complex<double> > operator-(std::complex<double>  const&,                TMatrix<std::complex<double> > const& ); 
template TMatrix<std::complex<double> > operator*(TMatrix<std::complex<double> > const&,       TMatrix<std::complex<double> > const& ); 
template TMatrix<std::complex<double> > operator*(TMatrix<std::complex<double> > const&,       std::complex<double> const&           );
template TMatrix<std::complex<double> > operator*(std::complex<double> const& ,                TMatrix<std::complex<double> > const& );
template TVector<std::complex<double> > operator*(TMatrix<std::complex<double> > const&,       TVector<std::complex<double> > const&);


template TMatrix<std::complex<double> > operator/(TMatrix<std::complex<double> > const&,       std::complex<double> const&           );
template TMatrix<std::complex<double> > operator/(std::complex<double> const& ,                TMatrix<std::complex<double> > const& );
template TMatrix<std::complex<double> > operator/(TMatrix<std::complex<double> > const&,       TMatrix<std::complex<double> > const& );

template bool operator!=<std::complex<double> >(TMatrix<std::complex<double> > const&, TMatrix<std::complex<double> > const&);


//----------------------------------------------------------------------------------------------------------------
// Instantiations for class TML<T>
//----------------------------------------------------------------------------------------------------------------

template class TML<double>;
template class TML<std::complex<double> >;

//template class TML<int>;

template MLPtr<std::complex<double> > Negate<std::complex<double> >(MLPtr<std::complex<double> > const&);
template MLPtr<double> Negate<double>(MLPtr<double> const&);


//template std::ostream& operator<< (std::ostream&, TML<int> const&);
template std::ostream& operator<< (std::ostream&, TML<double> const&);
template std::ostream& operator<< (std::ostream&, TML<std::complex<double> > const&);


template bool operator!=<double>(TMatrix<double> const&, TMatrix<double> const&);

template
double* 
std::transform( double const*,  double const*,  
                double const*,  double*,  std::plus<double> );

template
std::complex<double> * 
std::transform( std::complex<double>  const*, std::complex<double>  const*, 
                std::complex<double> const*,  std::complex<double> *,  std::plus<std::complex<double> > );

template
double* 
std::transform(  double const*, double const*, 
                 double const*,  double*, std::minus<double> );

template
std::complex<double> * 
std::transform(  std::complex<double>  const*, std::complex<double>  const*, 
                 std::complex<double>  const*, std::complex<double> *, std::minus<std::complex<double> > );


//template std::ostream& operator<< <std::complex<double> >(ostream&, MLPtr<std::complex<double> >const&);

// ---------------------------------------------------------------------------------------

template 
MLPtr<double> add<double>(  MLPtr<double> const&,  MLPtr<double> const& );  
template 
MLPtr<double> add<double>(  MLPtr<double> const&,  double const& );  

template 
MLPtr<double> subtract<double>(  MLPtr<double> const&,  MLPtr<double> const& );  


template 
MLPtr<double> multiply<double>(  MLPtr<double> const&,  MLPtr<double> const& );  

template 
MLPtr<double> multiply<double>(  MLPtr<double> const&,  double const& );  

template
TVector<double> 
multiply<double>(MLPtr<double> const&, TVector<double> const&);


template 
MLPtr<double> divide<double>  (  MLPtr<double> const&,  MLPtr<double> const& );  

template 
MLPtr<double> divide<double>  (  MLPtr<double> const&,  double const& );

template 
MLPtr<double> divide<double>  (  double const&,                MLPtr<double> const& );  

// ---------------------------------------------------------------------------------------

template MLPtr<std::complex<double> > 
add<std::complex<double> >(  MLPtr<std::complex<double> > const&,  MLPtr<std::complex<double> > const& );  

template MLPtr<std::complex<double> > 
add<std::complex<double> >(  MLPtr<std::complex<double> > const&,  std::complex<double> const& );  

template MLPtr<std::complex<double> > 
subtract<std::complex<double> >(  MLPtr<std::complex<double> > const&,  MLPtr<std::complex<double> > const& );  

template MLPtr<std::complex<double> > 
multiply<std::complex<double> >(  MLPtr<std::complex<double> > const&,  MLPtr<std::complex<double> > const& );  

template MLPtr<std::complex<double> > 
multiply<std::complex<double> >(  MLPtr<std::complex<double> > const&,  std::complex<double> const& );  

template
TVector<std::complex<double> > 
multiply<std::complex<double> >(MLPtr<std::complex<double> > const&, TVector<std::complex<double> > const&);


template MLPtr<std::complex<double> > 
divide<std::complex<double> >  (  MLPtr<std::complex<double> > const&,  MLPtr<std::complex<double> > const& );  

template MLPtr<std::complex<double> > 
divide<std::complex<double> >  (  MLPtr<std::complex<double> > const&,  std::complex<double> const& );  

template MLPtr<std::complex<double> > 
divide<std::complex<double> >  (  std::complex<double> const&,                MLPtr<std::complex<double> > const& );  


//----------------------------------------------------------------------------------------------------------------
// Instantiations for class MLPtr<T>
//----------------------------------------------------------------------------------------------------------------

template class MLPtr<double>;
template class MLPtr<std::complex<double> >;

template MLPtr<double>::operator MLPtr<std::complex<double> >() const;

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

template class FFTFunctorImpl<std::complex<double>, std::complex<double>, (transform_type)-1>;
template class FFTFunctorImpl<double,               std::complex<double>, (transform_type)-1>;
template class FFTFunctorImpl<std::complex<double>, double,               (transform_type) 1>;
template class FFTFunctorImpl<std::complex<double>, std::complex<double>, (transform_type) 1>;


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

template class ConvolutionFunctorImpl<double>;
template class ConvolutionFunctorImpl<std::complex<double> >;

template 
class FFTWAllocator<double>;

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

template class
std::list<int>;


template
double*
std::copy( double const*, double const*, double* );

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
IntArray::iterator
std::copy( IntArray::const_iterator, 
           IntArray::const_iterator, IntArray::iterator ); 

template
IntArray::iterator
std::copy( IntArray::iterator, 
           IntArray::iterator, IntArray::iterator ); 

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




// FIX ME !!!!!!!!!!!!!!!!                                                   

template void std::__uninitialized_fill_n_aux(std::complex<double>*, unsigned int, std::complex<double> const&, std::__false_type);

template class std::_List_base<int, std::allocator<int> >;

template int* std::fill_n<int*, unsigned int, int>(int*, unsigned int, int const&);

template void std::_Destroy<double*, FFTWAllocator<double> >(double*, double*, FFTWAllocator<double>);

template 
void std::__uninitialized_fill_n_a<double*, unsigned int, double, FFTWAllocator<double> >(double*, unsigned int, double const&, FFTWAllocator<double>); 





