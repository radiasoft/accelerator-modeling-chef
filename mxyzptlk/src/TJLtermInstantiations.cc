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
// If explicit template instantiations are not desired, *do not* compile this file !
// =================================================================================

#ifndef MXYZPTLK_IMPLICIT_TEMPLATES 

#include <complex>
#include <ReferenceCounter.h>
#include <TJetEnvironment.h>
#include <TJLterm.h>
#include <TJLterm.tcc>
#include <FastPODAllocator.h>
#include <FastAllocator.h>
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


template
class __gnu_cxx::hash_map<TJLterm<double >*, unsigned int, boost::hash<TJLterm<double >* > >;

template
class __gnu_cxx::hash_map<TJLterm<std::complex<double>  >*, unsigned int, boost::hash<TJLterm<std::complex<double>  >* > >;

template
class __gnu_cxx::hashtable<std::pair<TJLterm<std::complex<double> >* const, unsigned int>, TJLterm<std::complex<double> >*, boost::hash<TJLterm<std::complex<double> >*>, std::_Select1st<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >, std::equal_to<TJLterm<std::complex<double> >*>, std::allocator<unsigned int> >;

template
class __gnu_cxx::hashtable<std::pair<TJLterm<double >* const, unsigned int>, TJLterm<double >*, boost::hash<TJLterm<double >*>, std::_Select1st<std::pair<TJLterm<double >* const, unsigned int> >, std::equal_to<TJLterm<double >*>, std::allocator<unsigned int> >;

template 
__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >** 
std::fill_n<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*> 
(__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >* const&);

template 
__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >** 
std::fill_n<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*> 
(__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >* const&);

template 
class std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*> >;

template 
class std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double>  >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double>  >* const, unsigned int> >*> >;

template
void std::fill<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*> > >, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*>(__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*> > >, __gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*> > >, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >* const&);


template 
void std::fill<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*> > >, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*>(__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*> > >, __gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*> > >, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >* const&);

template
unsigned int boost::hash_value<TJLterm<double > >(TJLterm<double >* const&);

template
unsigned int boost::hash_value<TJLterm<std::complex<double>  > >(TJLterm<std::complex<double>  >* const&);

template
__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*> > > std::fill_n<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*>(__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >* const&);

template
__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*> > > std::fill_n<__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*>(__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<double >* const, unsigned int> >* const&);

template
unsigned long const* std::lower_bound<unsigned long const*, unsigned long>(unsigned long const*, unsigned long const*, unsigned long const&);

#endif // MXYZPTLK_IMPLICIT_TEMPLATES 



