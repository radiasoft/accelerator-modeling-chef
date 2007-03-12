/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJLtermInstantiations.cc
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
#include <boost/shared_ptr.hpp>
#include <basic_toolkit/ReferenceCounter.h>
#include <mxyzptlk/TJetEnvironment.h>
#include <mxyzptlk/TJLterm.h>
#include <mxyzptlk/TJLterm.tcc>
#include <gms/FastPODAllocator.h>
#include <gms/FastAllocator.h>
#include <istream>


// ============================================================================================
// The standard classes
//=============================================================================================

template class TJLterm<double>;
template class TJLterm<std::complex<double> >;



// ============================================================================================
// Friend functions
//=============================================================================================

template 
bool operator<=(TJLterm<double> const&, TJLterm<double> const&);
template 
bool operator<=(TJLterm<std::complex<double> > const&, TJLterm<std::complex<double> > const&);

template
bool operator%=<double>(TJLterm<double> const&, TJLterm<double> const&);

template 
bool operator%=<std::complex<double> >(TJLterm<std::complex<double> > const&, TJLterm<std::complex<double> > const&);

template
bool operator==<double >(TJLterm<double > const&, TJLterm<double > const&);
template
bool operator==<std::complex<double> >(TJLterm<std::complex<double> > const&, TJLterm<std::complex<double> > const&);

template 
std::ostream& operator<<(std::ostream&, TJLterm<double > const&);
template 
std::ostream& operator<<(std::ostream&, TJLterm<std::complex<double> > const&);



//=============================
// Memory allocation templates
//=============================

template 
class gms::FastPODAllocator<TJLterm<std::complex<double> > >;

template 
class gms::FastPODAllocator<TJLterm<double> >;


//=======================================================================================================
// BOOST & STL
//=======================================================================================================

template 
class boost::pool<>;

template
class boost::simple_segregated_storage<unsigned int>;

template
unsigned int boost::details::pool::lcm<unsigned int>(unsigned int const&, unsigned int const&);

template
unsigned boost::details::pool::gcd<unsigned>(unsigned, unsigned);

template 
class boost::hash<TJLterm<double >* >;

template 
class boost::hash<TJLterm<std::complex<double>  >* >; 


#endif // MXYZPTLK_EXPLICIT_TEMPLATES 



