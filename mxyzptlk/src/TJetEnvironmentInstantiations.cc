/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetEnvironmentInstantiations.cc
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
// If implicit template instantiations are desired, *do not* compile this file !
// =================================================================================

#ifdef MXYZPTLK_EXPLICIT_TEMPLATES 

#include <complex>
#include <basic_toolkit/ReferenceCounter.h>
#include <mxyzptlk/TJLterm.h>
#include <mxyzptlk/TJL.h>
#include <mxyzptlk/TJet.h>
#include <mxyzptlk/TJetEnvironment.h>
#include <mxyzptlk/TJetEnvironment.tcc>

#include <gms/FastPODAllocator.h>
#include <gms/FastAllocator.h>
#include <boost/functional/hash/hash.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <vector>
#include <deque>
#include <list>
#include <ext/hash_map>
#include <algorithm>
#include <istream>


// ============================================================================================
// The standard classes
//=============================================================================================

template class TJetEnvironment<double>;
template class TJetEnvironment<std::complex<double> >;

template class TJetEnvironment<double>::ScratchArea<double>;
template class TJetEnvironment<std::complex<double> >::ScratchArea<std::complex<double> >;

template class EnvPtr<double>;
template class EnvPtr<std::complex<double> >;

template class EnvList<double>;
template class EnvList<std::complex<double> >;

// ============================================================================================
// The friend function operators
//=============================================================================================


template
std::ostream& operator<<(std::ostream&, TJetEnvironment<double > const&);

template 
std::ostream& operator<<(std::ostream&, TJetEnvironment<std::complex<double> > const&);

template
std::istream& streamIn( std::istream&, EnvPtr<double>& pje );

template
std::istream& streamIn( std::istream&, EnvPtr<std::complex<double> >& pje);


//=======================================================================================================
// Boost and STL 
//=======================================================================================================


template
class std::list<TJetEnvironment<std::complex<double> >::ScratchArea<std::complex<double> >*>;

template
class std::list<TJetEnvironment<double>::ScratchArea<double> *>;

template
class std::_List_base<TJetEnvironment<std::complex<double> >::ScratchArea<std::complex<double> >*, std::allocator<TJetEnvironment<std::complex<double> >::ScratchArea<std::complex<double> >*> >;

template
class std::_List_base<TJetEnvironment<double>::ScratchArea<double> *, std::allocator<TJetEnvironment<double>::ScratchArea<double>*> >;


template
std::complex<double> std::pow(std::complex<double> const&, double const&);

//template
//class boost::detail::sp_counted_impl_p<TJetEnvironment<std::complex<double> > >;

//template
//class boost::detail::sp_counted_impl_p<TJetEnvironment<double> >;

template 
class std::list<EnvPtr<double> >;

template 
class std::list<EnvPtr<std::complex<double> > >;


template
void std::_List_base<EnvPtr<double>, std::allocator<EnvPtr<double> > > ::_M_clear();

template
void std::_List_base<EnvPtr<std::complex<double> >,  std::allocator<EnvPtr<std::complex<double> > > >::_M_clear();

template
std::complex<double>  std::sqrt<double>( const std::complex<double>& );


template
std::_List_iterator<EnvPtr<double> > 
std::remove(std::_List_iterator<EnvPtr<double> >, 
            std::_List_iterator<EnvPtr<double> >, 
            EnvPtr<double> const&);


template
std::_List_iterator<EnvPtr<std::complex<double> > > 
std::remove(std::_List_iterator<EnvPtr<std::complex<double> > >, 
            std::_List_iterator<EnvPtr<std::complex<double> > >, 
            EnvPtr<std::complex<double> > const& );


template
std::_List_iterator<EnvPtr<double> > 
std::remove_copy<std::_List_iterator<EnvPtr<double> >, 
                 std::_List_iterator<EnvPtr<double> >, 
                 EnvPtr<double> >
     (std::_List_iterator<EnvPtr<double> >, 
      std::_List_iterator<EnvPtr<double> >, 
      std::_List_iterator<EnvPtr<double> >, 
      EnvPtr<double> const&);

template
std::_List_iterator<EnvPtr<std::complex<double> > > 
std::remove_copy<std::_List_iterator<EnvPtr<std::complex<double> > >, 
                 std::_List_iterator<EnvPtr<std::complex<double> > >, 
                 EnvPtr<std::complex<double> > >
     (std::_List_iterator<EnvPtr<std::complex<double> > >, 
      std::_List_iterator<EnvPtr<std::complex<double> > >, 
      std::_List_iterator<EnvPtr<std::complex<double> > >, 
      EnvPtr<std::complex<double>  > const&);

#endif // MXYZPTLK_EXPLICIT_TEMPLATES 



