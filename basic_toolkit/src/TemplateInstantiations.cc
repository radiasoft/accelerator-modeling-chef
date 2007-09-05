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
*************************************************************************/
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

#include <basic_toolkit/complexAddon.h>
#include <basic_toolkit/TMatrix.h>
#include <basic_toolkit/MathConstants.h>
#ifdef WIN32
#include <basic_toolkit/Distribution.h> // for drand48
#endif

#include <gms/FastPODAllocator.h>
#include <basic_toolkit/Barnacle.h>
#include <basic_toolkit/IntArray.h>
#include <basic_toolkit/Cascade.h>

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


template BarnacleList::iterator
         std::remove_if(BarnacleList::iterator, BarnacleList::iterator, 
		        std::binder2nd<std::const_mem_fun1_ref_t<bool, Barnacle, std::string> >);

// ----------------------------------------------------------------------------
// Instantiations related to IntArray
// ----------------------------------------------------------------------------

template 
class std::vector<exponent_t>;

namespace {
 
 IntArray::iterator               itdummy1;
 IntArray::const_iterator         itdummy2;  
 IntArray::reverse_iterator       itdummy3;
 IntArray::const_reverse_iterator itdummy4;
}
    

template
void std::fill( IntArray::iterator, IntArray::iterator, int const&); 

//template
//void std::fill (IntArray::iterator, IntArray::iterator, exponent_t const&);

template
int std::accumulate( IntArray::iterator, IntArray::iterator,  int);

template
int std::accumulate( IntArray::const_iterator, IntArray::const_iterator,  int);

template 
class std::plus<exponent_t>;

template 
class std::less_equal<exponent_t>;

template
class std::greater<exponent_t>;

template
class std::greater_equal<exponent_t>;

template
IntArray::iterator std::transform( IntArray::iterator, IntArray::iterator, IntArray::iterator, IntArray::iterator,  std::plus<exponent_t> );

template
bool std::equal( IntArray::iterator, IntArray::iterator, IntArray::iterator);

template
bool std::lexicographical_compare( IntArray::iterator, IntArray::iterator, IntArray::iterator,  IntArray::iterator, std::less<exponent_t> );

template
bool std::lexicographical_compare( IntArray::iterator, IntArray::iterator, IntArray::iterator,  IntArray::iterator, std::less_equal<exponent_t>);

template
bool std::lexicographical_compare( IntArray::iterator, IntArray::iterator, IntArray::iterator,  IntArray::iterator, std::greater<exponent_t>);

template
IntArray::iterator std::transform( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator, IntArray::iterator,  std::plus<exponent_t> );

template
bool std::equal( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::less<exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::less_equal<exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::greater<exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::greater_equal<exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::less<exponent_t> );

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::less_equal<exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::greater<exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::greater_equal<exponent_t>);

template
void std::fill<int*, int>(int*, int*, int const&);

template
double* std::fill_n<double*, int, int>(double*, int, int const&);

// ----------------------------------------------------------------------------
// Instantiations related to Cascade 
// ----------------------------------------------------------------------------

template
class std::vector<Switch>;

template
class std::list<Switch*>;

template  
void std::list<Switch*>::sort(Switch::PartialLessThan);


// ----------------------------------------------------------------------------
// Instantiations related to Vector Class
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

template double std::inner_product(std::vector<double>::const_iterator, std::vector<double>::const_iterator, 
                                   std::vector<double>::const_iterator, double);
template complex<double> std::inner_product(std::vector<complex<double> >::const_iterator, std::vector<complex<double> >::const_iterator, 
                                            std::vector<complex<double> >::const_iterator, 
                                            complex<double>, 
                                            std::plus<std::complex<double> >, TVector<complex<double> >::op_mult ); 

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
template TMatrix<double> operator*(TMatrix<double> const&, TVector<double> const&);


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
template TMatrix<std::complex<double> > operator*(TMatrix<std::complex<double> > const&,       TVector<std::complex<double> > const&);


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
MLPtr<double> 
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
MLPtr<std::complex<double> > 
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

template
FFTFunctor<double, std::complex<double>, fft_forward>*  boost::addressof(FFTFunctor<double, std::complex<double>, fft_forward>&);

template
FFTFunctor<std::complex<double>, double, fft_backward>* boost::addressof(FFTFunctor<std::complex<double>, double, fft_backward>&);

template
FFTFunctor<std::complex<double>, std::complex<double>, fft_forward>*  boost::addressof(FFTFunctor<std::complex<double>, std::complex<double>, fft_forward>&);

template
FFTFunctor<std::complex<double>, std::complex<double>, fft_backward>* boost::addressof(FFTFunctor<std::complex<double>, std::complex<double>, fft_backward>&);


template class ConvolutionFunctor<double>;
template class ConvolutionFunctor<std::complex<double> >;

template class std::vector<double,               FFTWAllocator<double> >;
template class std::vector<std::complex<double>, FFTWAllocator<std::complex<double> > >;


#endif //BASICTOOLKIT_EXPLICIT_TEMPLATES
