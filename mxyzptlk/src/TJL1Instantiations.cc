/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJL1TemplateInstantiations.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab   
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
******  Author:    Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******  
**************************************************************************
*************************************************************************/

// =================================================================================
// If explicit template instantiations are not desired, *do not* compile this file !
// =================================================================================

#ifdef  FIRST_ORDER_JETS
#ifndef MXYZPTLK_IMPLICIT_TEMPLATES 

#include <complex>
#include <ReferenceCounter.h>
#include <TJLterm.h>
#include <TJLterm.tcc>
#include <TJL1.h>
#include <TJL1.tcc>


// ============================================================================================
// Friend functions 
//=============================================================================================

template class JLPtr<double>;
template class JLPtr<std::complex<double> >;


template bool operator==( const TJL1<double>& x,                const TJL1<double>& y ); 
template bool operator==( const TJL1<std::complex<double> >& x, const TJL1<std::complex<double> >& y ); 

template bool operator==( const TJL1<double>& x,                const double& y ); 
template bool operator==( const TJL1<std::complex<double> >& x, const std::complex<double> & y ); 

template bool operator==( const double& y,                const TJL1<double>& x );
template bool operator==( const std::complex<double> & y, const TJL1<std::complex<double> >& x );

template bool operator!=( const TJL1<double>& x,                const TJL1<double>& y ); 
template bool operator!=( const TJL1<std::complex<double> >& x, const TJL1<std::complex<double> >& y ); 

template bool operator!=( const TJL1<double>& x,                const double& y ); 
template bool operator!=( const TJL1<std::complex<double> >& x, const std::complex<double> & y ); 

template bool operator!=( const double& x,                const TJL1<double>& y ); 
template bool operator!=( const std::complex<double> & x, const TJL1<std::complex<double> >& y ); 

template std::ostream& operator<<( std::ostream& os, const TJL1<double>& x); 
template std::ostream& operator<<( std::ostream& os, const TJL1<std::complex<double> >& x); 

template std::istream& operator>>( std::istream& is,  TJL1<double>&  x); 
template std::istream& operator>>( std::istream& is,  TJL1<std::complex<double> >&  x); 


template JLPtr<double>::Type                operator-(JLPtr<double>::Type const&);
template JLPtr<std::complex<double> >::Type operator-(JLPtr<std::complex<double> >::Type const&);

template JLPtr<double>::Type                operator-(JLPtr<double>::Type const&, JLPtr<double>::Type  const&);
template JLPtr<std::complex<double> >::Type operator-(JLPtr<std::complex<double> >::Type const&, JLPtr<std::complex<double> >::Type  const&);

template JLPtr<double>::Type                operator+(JLPtr<double>::Type const&, JLPtr<double>::Type const&);
template JLPtr<std::complex<double> >::Type operator+(JLPtr<std::complex<double> >::Type const&, JLPtr<std::complex<double> >::Type const&);

template JLPtr<double>::Type&                operator+=(JLPtr<double>::Type &x,                JLPtr<double>::Type  const& y  );
template JLPtr<std::complex<double> >::Type& operator+=(JLPtr<std::complex<double> >::Type &x, JLPtr<std::complex<double> >::Type  const& y  );

template JLPtr<double>::Type                operator/(JLPtr<double>::Type const&,                JLPtr<double>::Type const&);
template JLPtr<std::complex<double> >::Type operator/(JLPtr<std::complex<double> >::Type const&, JLPtr<std::complex<double> >::Type const&);

template JLPtr<double>::Type                operator/(double const&, JLPtr<double>::Type const&);
template JLPtr<std::complex<double> >::Type operator/(std::complex<double> const&, JLPtr<std::complex<double> >::Type const&);

template JLPtr<double>::Type                operator*(JLPtr<double>::Type const&, JLPtr<double>::Type const&);
template JLPtr<std::complex<double> >::Type operator*(JLPtr<std::complex<double> >::Type const&, JLPtr<std::complex<double> >::Type const&);
template JLPtr<double>::Type                 operator*(JLPtr<double>::Type const& x, double const& y );
template JLPtr<std::complex<double> >::Type  operator*(JLPtr<std::complex<double> >::Type const& x, std::complex<double> const& y );


//============================================================================================
// The class(es)
//=============================================================================================

template class TJL1<double>;
template class TJL1<std::complex<double> >;

//============================================================================================
// misc
//=============================================================================================


//TJL1<double>::printCoeffs() const'
//TJL1<std::complex<double> >::printCoeffs() const'

template void std::vector<TJL1<double>*, std::allocator<TJL1<double>*> >::_M_insert_aux(__gnu_cxx::__normal_iterator<TJL1<double>**, std::vector<TJL1<double>*, std::allocator<TJL1<double>*> > >, TJL1<double>* const&);

template void std::vector<TJL1<std::complex<double> >*, std::allocator<TJL1<std::complex<double> >*> >::_M_insert_aux(__gnu_cxx::__normal_iterator<TJL1<std::complex<double> >**, std::vector<TJL1<std::complex<double> >*, std::allocator<TJL1<std::complex<double> >*> > >, TJL1<std::complex<double> >* const&);

#endif // MXYZPTLK_IMPLICIT_TEMPLATES 
#endif // FIRST_ORDER_JETS
