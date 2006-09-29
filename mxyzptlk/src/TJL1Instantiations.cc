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

#ifdef MXYZPTLK_EXPLICIT_TEMPLATES 

#include <complex>
#include <basic_toolkit/ReferenceCounter.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <mxyzptlk/TJLterm.h>
#include <mxyzptlk/TJLterm.tcc>
#include <mxyzptlk/TJL1.h>
#include <mxyzptlk/TJL1.tcc>
#include <mxyzptlk/TJL1Iterator.h>
#include <mxyzptlk/TJL1Iterator.tcc>


// ============================================================================================
// Friend functions 
//=============================================================================================

template class JL1Ptr<double>;
template class JL1Ptr<std::complex<double> >;


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


template JL1Ptr<double>                operator-(JL1Ptr<double> const&);
template JL1Ptr<std::complex<double> > operator-(JL1Ptr<std::complex<double> > const&);

template JL1Ptr<double>                operator-(JL1Ptr<double> const&, JL1Ptr<double>  const&);
template JL1Ptr<std::complex<double> > operator-(JL1Ptr<std::complex<double> > const&, JL1Ptr<std::complex<double> >  const&);

template JL1Ptr<double>                operator+(JL1Ptr<double> const&, JL1Ptr<double> const&);
template JL1Ptr<std::complex<double> > operator+(JL1Ptr<std::complex<double> > const&, JL1Ptr<std::complex<double> > const&);

template JL1Ptr<double>&                operator+=(JL1Ptr<double> &x,                JL1Ptr<double>  const& y  );
template JL1Ptr<std::complex<double> >& operator+=(JL1Ptr<std::complex<double> > &x, JL1Ptr<std::complex<double> >  const& y  );

template JL1Ptr<double>                operator/(JL1Ptr<double> const&,                JL1Ptr<double> const&);
template JL1Ptr<std::complex<double> > operator/(JL1Ptr<std::complex<double> > const&, JL1Ptr<std::complex<double> > const&);

template JL1Ptr<double>                operator/(double const&, JL1Ptr<double> const&);
template JL1Ptr<std::complex<double> > operator/(std::complex<double> const&, JL1Ptr<std::complex<double> > const&);

template JL1Ptr<double>                operator*(JL1Ptr<double> const&, JL1Ptr<double> const&);
template JL1Ptr<std::complex<double> > operator*(JL1Ptr<std::complex<double> > const&, JL1Ptr<std::complex<double> > const&);
template JL1Ptr<double>                 operator*(JL1Ptr<double> const& x, double const& y );
template JL1Ptr<std::complex<double> >  operator*(JL1Ptr<std::complex<double> > const& x, std::complex<double> const& y );


//============================================================================================
// The class(es)
//=============================================================================================

template class TJL1<double>;
template class TJL1<std::complex<double> >;
template JL1Ptr<double> TJL1<double>::makeTJL( TJL1<double> const& x );
template JL1Ptr<std::complex<double> > TJL1<std::complex<double> >::makeTJL( TJL1<std::complex<double> >const& x );

template class TJL1Iterator<double>;
template class TJL1Iterator<std::complex<double> >;

//============================================================================================
// misc
//=============================================================================================



template void std::vector<TJL1<double>*, std::allocator<TJL1<double>*> >::_M_insert_aux(__gnu_cxx::__normal_iterator<TJL1<double>**, std::vector<TJL1<double>*, std::allocator<TJL1<double>*> > >, TJL1<double>* const&);

template void std::vector<TJL1<std::complex<double> >*, std::allocator<TJL1<std::complex<double> >*> >::_M_insert_aux(__gnu_cxx::__normal_iterator<TJL1<std::complex<double> >**, std::vector<TJL1<std::complex<double> >*, std::allocator<TJL1<std::complex<double> >*> > >, TJL1<std::complex<double> >* const&);


template 
class std::vector<boost::shared_ptr<TJLterm<double> > >;

template 
class std::vector<boost::shared_ptr<TJLterm<std::complex<double> > > >;


template
class boost::shared_ptr<TJLterm<double> >;
template
class boost::shared_ptr<TJLterm<std::complex<double> > >;


template 
class boost::detail::sp_counted_impl_p<TJLterm<double> >;

template 
class boost::detail::sp_counted_impl_p<TJLterm<std::complex<double> > >;

template 
class __gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<double> >*, std::vector<boost::shared_ptr<TJLterm<double> >, std::allocator<boost::shared_ptr<TJLterm<double> > > > > std::fill_n<__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<double> >*, std::vector<boost::shared_ptr<TJLterm<double> >, std::allocator<boost::shared_ptr<TJLterm<double> > > > >, unsigned int, boost::shared_ptr<TJLterm<double> > >(__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<double> >*, std::vector<boost::shared_ptr<TJLterm<double> >, std::allocator<boost::shared_ptr<TJLterm<double> > > > >, unsigned int, boost::shared_ptr<TJLterm<double> > const&);

template class
__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<std::complex<double> > >*, std::vector<boost::shared_ptr<TJLterm<std::complex<double> > >, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > > > > std::fill_n<__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<std::complex<double> > >*, std::vector<boost::shared_ptr<TJLterm<std::complex<double> > >, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > > > >, unsigned int, boost::shared_ptr<TJLterm<std::complex<double> > > >(__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<std::complex<double> > >*, std::vector<boost::shared_ptr<TJLterm<std::complex<double> > >, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > > > >, unsigned int, boost::shared_ptr<TJLterm<std::complex<double> > > const&);


template 
void std::fill<__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<std::complex<double> > >*, std::vector<boost::shared_ptr<TJLterm<std::complex<double> > >, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > > > >, boost::shared_ptr<TJLterm<std::complex<double> > > >(__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<std::complex<double> > >*, std::vector<boost::shared_ptr<TJLterm<std::complex<double> > >, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > > > >, __gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<std::complex<double> > >*, std::vector<boost::shared_ptr<TJLterm<std::complex<double> > >, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > > > >, boost::shared_ptr<TJLterm<std::complex<double> > > const&);

template
void std::fill<__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<double> >*, std::vector<boost::shared_ptr<TJLterm<double> >, std::allocator<boost::shared_ptr<TJLterm<double> > > > >, boost::shared_ptr<TJLterm<double> > >(__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<double> >*, std::vector<boost::shared_ptr<TJLterm<double> >, std::allocator<boost::shared_ptr<TJLterm<double> > > > >, __gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<double> >*, std::vector<boost::shared_ptr<TJLterm<double> >, std::allocator<boost::shared_ptr<TJLterm<double> > > > >, boost::shared_ptr<TJLterm<double> > const&);

//-------------------------------------------------------------------------------------------------------------------------------------------------------

#if (__GNUC__ > 3 )

template 
void std::__uninitialized_fill_n_a<__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<double> >*, std::vector<boost::shared_ptr<TJLterm<double> >, std::allocator<boost::shared_ptr<TJLterm<double> > > > >, unsigned int, boost::shared_ptr<TJLterm<double> >, boost::shared_ptr<TJLterm<double> > >(__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<double> >*, std::vector<boost::shared_ptr<TJLterm<double> >, std::allocator<boost::shared_ptr<TJLterm<double> > > > >, unsigned int, boost::shared_ptr<TJLterm<double> > const&, std::allocator<boost::shared_ptr<TJLterm<double> > >);

template
void std::__uninitialized_fill_n_a<__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<std::complex<double> > >*, std::vector<boost::shared_ptr<TJLterm<std::complex<double> > >, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > > > >, unsigned int, boost::shared_ptr<TJLterm<std::complex<double> > >, boost::shared_ptr<TJLterm<std::complex<double> > > >(__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<std::complex<double> > >*, std::vector<boost::shared_ptr<TJLterm<std::complex<double> > >, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > > > >, unsigned int, boost::shared_ptr<TJLterm<std::complex<double> > > const&, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > >);

template 
void std::__uninitialized_fill_n_a<boost::shared_ptr<TJLterm<double> >*, unsigned int, boost::shared_ptr<TJLterm<double> >, boost::shared_ptr<TJLterm<double> > >(boost::shared_ptr<TJLterm<double> >*, unsigned int, boost::shared_ptr<TJLterm<double> > const&, std::allocator<boost::shared_ptr<TJLterm<double> > >);

template 
void std::__uninitialized_fill_n_a<boost::shared_ptr<TJLterm<std::complex<double> > >*, unsigned int, boost::shared_ptr<TJLterm<std::complex<double> > >, boost::shared_ptr<TJLterm<std::complex<double> > > >(boost::shared_ptr<TJLterm<std::complex<double> > >*, unsigned int, boost::shared_ptr<TJLterm<std::complex<double> > > const&, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > >);

template 
void std::__uninitialized_fill_n_aux<__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<double> >*, std::vector<boost::shared_ptr<TJLterm<double> >, std::allocator<boost::shared_ptr<TJLterm<double> > > > >, unsigned int, boost::shared_ptr<TJLterm<double> > >(__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<double> >*, std::vector<boost::shared_ptr<TJLterm<double> >, std::allocator<boost::shared_ptr<TJLterm<double> > > > >, unsigned int, boost::shared_ptr<TJLterm<double> > const&, __false_type);

template
void std::__uninitialized_fill_n_aux<__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<std::complex<double> > >*, std::vector<boost::shared_ptr<TJLterm<std::complex<double> > >, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > > > >, unsigned int, boost::shared_ptr<TJLterm<std::complex<double> > > >(__gnu_cxx::__normal_iterator<boost::shared_ptr<TJLterm<std::complex<double> > >*, std::vector<boost::shared_ptr<TJLterm<std::complex<double> > >, std::allocator<boost::shared_ptr<TJLterm<std::complex<double> > > > > >, unsigned int, boost::shared_ptr<TJLterm<std::complex<double> > > const&, __false_type);

template
void std::__uninitialized_fill_n_aux<boost::shared_ptr<TJLterm<double> >*, unsigned int, boost::shared_ptr<TJLterm<double> > >(boost::shared_ptr<TJLterm<double> >*, unsigned int, boost::shared_ptr<TJLterm<double> > const&, __false_type);

template
void std::__uninitialized_fill_n_aux<boost::shared_ptr<TJLterm<std::complex<double> > >*, unsigned int, boost::shared_ptr<TJLterm<std::complex<double> > > >(boost::shared_ptr<TJLterm<std::complex<double> > >*, unsigned int, boost::shared_ptr<TJLterm<std::complex<double> > > const&, __false_type);

#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------


#endif // MXYZPTLK_EXPLICIT_TEMPLATES 

