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

#ifndef MXYZPTLK_IMPLICIT_TEMPLATES 

#include <complex>
#include <ReferenceCounter.h>
#include <TJLterm.h>
#include <TJLterm.tcc>
#include <TJL1.h>
#include <TJL1.tcc>
//#include <TJetEnvironment.h>
//#include <TJetEnvironment.tcc>
//#include <TJetVector.h>
//#include <TJetVector.tcc>
//#include <TMapping.h>
//#include <TMapping.tcc>
//#include <TLieOperator.h>
//#include <TLieOperator.tcc>

//#include <FastPODAllocator.h>
//#include <FastAllocator.h>
//#include <boost/functional/hash/hash.hpp>
//#include <boost/pool/pool.hpp>
//#include <boost/pool/pool_alloc.hpp>
//#include <vector>
//#include <deque>
//#include <list>
//#include <ext/hash_map>
//#include <algorithm>
//#include <istream>

// ============================================================================================
// Friend functions 
//=============================================================================================

template class JL1Ptr<double>;
template class JL1Ptr<std::complex<double> >;

template JL1Ptr<double>::Type                operator-(JL1Ptr<double>::Type const&);
template JL1Ptr<std::complex<double> >::Type operator-(JL1Ptr<std::complex<double> >::Type const&);

template JL1Ptr<double>::Type                operator-(JL1Ptr<double>::Type const&, JL1Ptr<double>::Type  const&);
template JL1Ptr<std::complex<double> >::Type operator-(JL1Ptr<std::complex<double> >::Type const&, JL1Ptr<std::complex<double> >::Type  const&);

template JL1Ptr<double>::Type                operator+(JL1Ptr<double>::Type const&, JL1Ptr<double>::Type const&);
template JL1Ptr<std::complex<double> >::Type operator+(JL1Ptr<std::complex<double> >::Type const&, JL1Ptr<std::complex<double> >::Type const&);

template JL1Ptr<double>::Type                operator/(JL1Ptr<double>::Type const&,                JL1Ptr<double>::Type const&);
template JL1Ptr<std::complex<double> >::Type operator/(JL1Ptr<std::complex<double> >::Type const&, JL1Ptr<std::complex<double> >::Type const&);

template JL1Ptr<double>::Type                operator/(double const&, JL1Ptr<double>::Type const&);
template JL1Ptr<std::complex<double> >::Type operator/(std::complex<double> const&, JL1Ptr<std::complex<double> >::Type const&);

template JL1Ptr<double>::Type                operator*(JL1Ptr<double>::Type const&, JL1Ptr<double>::Type const&);
template JL1Ptr<std::complex<double> >::Type operator*(JL1Ptr<std::complex<double> >::Type const&, JL1Ptr<std::complex<double> >::Type const&);


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

