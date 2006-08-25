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
******
******                                                                
**************************************************************************
*************************************************************************/
#ifdef BASICTOOLKIT_EXPLICIT_TEMPLATES

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <TMatrix.h>
#include <complex>
#include <ostream>
#include <complexAddon.h>
#include <MathConstants.h>
#ifdef WIN32
#include <Distribution.h> // for drand48
#endif

#include <iostream>
#include <iomanip>
#include <sstream>


#include <TMatrix.tcc>
#include <TML.tcc>
#include <TVector.tcc>
#include <FastPODAllocator.h>
#include <dlist.h>
#include <slist.h>

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
class gms::FastPODAllocator<dlink>;
template 
class gms::FastPODAllocator<slink>;

template
class boost::simple_segregated_storage<unsigned int>;

template
unsigned int boost::details::pool::lcm<unsigned int>(unsigned int const&, unsigned int const&);
template
unsigned int boost::details::pool::gcd<unsigned int>(unsigned int, unsigned int);

// ----------------------------------------------------------------------------
// Instantiations related to Vector Class
// ----------------------------------------------------------------------------

template class TVector<double>;
template class TVector<std::complex<double> >;

template double TVector<double>::Norm() const;
template double TVector<std::complex<double> >::Norm() const;

template TVector<double> operator-(TVector<double> const&);
template TVector<std::complex<double> > operator-(TVector<std::complex<double> > const&);

template TVector<double> operator*<double>(TVector<double> const&, double);
template TVector<double> operator*<double>(double, TVector<double> const&);

template TVector<std::complex<double> > operator*(std::complex<double>, TVector<std::complex<double> > const&);
template TVector<std::complex<double> > operator*(TVector<std::complex<double> > const&, std::complex<double>);

template bool TVector<double>::IsUnit() const;
template bool TVector<std::complex<double> >::IsUnit() const;

template TVector<double> operator*(TMatrix<double> const&, TVector<double> const&);
template TVector<std::complex<double> > operator*(TMatrix<std::complex<double> > const&, TVector<std::complex<double> > const&);

template std::ostream& operator<<(std::ostream&, TVector<double> const&);
template std::ostream& operator<<(std::ostream&, TVector<std::complex<double> > const&);



// ----------------------------------------------------------------------------
// Instantiations related to Matrix Class
// ----------------------------------------------------------------------------

template class TMatrix<double>;
template class TMatrix<std::complex<double> >;
//template class TMatrix<int>;  // This should not be used at all, 
                                // but apparently it is ... 

template bool operator==( const TMatrix<double>&, const TMatrix<double>& );
template bool operator==( const TMatrix<double>&, const double& );
template bool operator==( const double&, const TMatrix<double>& );
template std::ostream& operator<<(std::ostream&, const TMatrix<double>&);

template TMatrix<double> operator+(const TMatrix<double>&, const TMatrix<double>&);
template TMatrix<double> operator+(const TMatrix<double>&, const double&); 
template TMatrix<double> operator+(const double&,          const TMatrix<double>&); 
template TMatrix<double> operator-(const TMatrix<double>&); 
template TMatrix<double> operator-(const TMatrix<double>&, const TMatrix<double>&); 
template TMatrix<double> operator-(const TMatrix<double>&, const double&); 
template TMatrix<double> operator-(const double&,          const TMatrix<double>&); 
template TMatrix<double> operator*(const TMatrix<double>&, const TMatrix<double>&); 
template TMatrix<double> operator*(const TMatrix<double>&, const double&);
template TMatrix<double> operator*(const double&,          const TMatrix<double>&);

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

#if  0
==============================================================================================
template 
MLPtr<int> TML<int>::Negate (  MLPtr<int> const& );

template 
MLPtr<int> TML<int>::add     (  MLPtr<int> const&,  MLPtr<int> const& );  
template 
MLPtr<int> TML<int>::add     (  MLPtr<int> const&,  int const& );  

template 
MLPtr<int> TML<int>::subtract(  MLPtr<int> const&,  MLPtr<int> const& );  

template 
MLPtr<int> TML<int>::multiply(  MLPtr<int> const&,  MLPtr<int> const& );  

template 
MLPtr<int> TML<int>::multiply(  MLPtr<int> const&,  int const& );  


template 
MLPtr<int> TML<int>::divide  (  MLPtr<int> const&,  MLPtr<int> const& );  

template 
MLPtr<int> TML<int>::divide  (  MLPtr<int> const&,  int const& );  

template 
MLPtr<int> TML<int>::divide  (  int const&,                MLPtr<int> const& );  
===============================================================================================
#endif

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


template MLPtr<std::complex<double> > 
divide<std::complex<double> >  (  MLPtr<std::complex<double> > const&,  MLPtr<std::complex<double> > const& );  

template MLPtr<std::complex<double> > 
divide<std::complex<double> >  (  MLPtr<std::complex<double> > const&,  std::complex<double> const& );  

template MLPtr<std::complex<double> > 
divide<std::complex<double> >  (  std::complex<double> const&,                MLPtr<std::complex<double> > const& );  


template class MLPtr<double>;
template class MLPtr<std::complex<double> >;

template MLPtr<double>::operator MLPtr<std::complex<double> >() const;


#endif //BASICTOOLKIT_EXPLICIT_TEMPLATES
