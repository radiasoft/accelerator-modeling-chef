/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TMatrixInstantiations.cc
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
******                                      
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
#include <numeric>
#include <functional>
#include <algorithm>

#include <basic_toolkit/TMatrix.h>
#include <basic_toolkit/TMatrix.tcc>
#include <basic_toolkit/TML.tcc>

using namespace std;
using std::ostringstream;

// ----------------------------------------------------------------------------
// Instantiations related to Matrix Class
// ----------------------------------------------------------------------------

template class TMatrix<double>;
template class TMatrix<std::complex<double> >;

template bool operator==( TMatrix<double> const&, TMatrix<double> const& );
template bool operator==( TMatrix<double> const&,          double const& );
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


template TMatrix<double> operator/<double>( TMatrix<double> const&, double          const&);
template TMatrix<double> operator/<double>(  double         const&, TMatrix<double> const&);
template TMatrix<double> operator/<double>(TMatrix<double>  const&, TMatrix<double> const&);

template bool operator==( TMatrix<std::complex<double> > const&, TMatrix<std::complex<double> > const& );
template bool operator==( TMatrix<std::complex<double> > const&, std::complex<double>           const& );
template bool operator==( std::complex<double>           const&, TMatrix<std::complex<double> > const& );
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

#endif
