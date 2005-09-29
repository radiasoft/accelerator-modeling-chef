/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TemplateInstantiations.cc
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
#include <TJL.h>
#include <TJL.tcc>
#include <TJet.h>
#include <TJet.tcc>  
#include <TJetEnvironment.h>
#include <TJetEnvironment.tcc>
#include <TJetVector.h>
#include <TJetVector.tcc>
#include <TMapping.h>
#include <TMapping.tcc>
#include <TLieOperator.h>
#include <TLieOperator.tcc>

#include <FastPODAllocator.h>
#include <FastAllocator.h>
#include <boost/functional/hash/hash.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <vector>
#include <ext/hash_map>
#include <algorithm>
#include <complex>

// ============================================================================================
// The standard classes
//=============================================================================================

template class TJetEnvironment<double>;
template class TJetEnvironment<std::complex<double> >;

template class TJLterm<double>;
template class TJLterm<std::complex<double> >;

template class TJL<double>;
template class TJL<std::complex<double> >;

template class TJet<double>;
template class TJet<std::complex<double> >;

template class TJetVector<double>;
template class TJetVector<std::complex<double> >;

template class TMapping<double>;
template class TMapping<std::complex<double> >;

template class TLieOperator<double>;
template class TLieOperator<std::complex<double> >;

template class Tcoord<double>;
template class Tcoord<std::complex<double> >;


// ============================================================================================
// The friend function operators
//=============================================================================================

template 
TJet<double> operator+(double const&, TJet<double> const&);

template 
TJet<std::complex<double> > operator+(TJet<std::complex<double> > const&, TJet<std::complex<double> > const&);

template 
TJet<std::complex<double> > operator+(std::complex<double> const&, TJet<std::complex<double> > const&);

template 
TJet<std::complex<double> > operator+(TJet<std::complex<double> > const&, std::complex<double> const&);

template 
TJet<double> operator+(TJet<double> const&, double const&);

template 
TJet<double> operator+(TJet<double> const&, TJet<double> const&);

template
TJet<std::complex<double> > operator-(std::complex<double> const&, TJet<std::complex<double> > const&);

template 
TJet<std::complex<double> > operator-(TJet<std::complex<double> > const&, std::complex<double> const&);

template 
TJet<double> operator-(TJet<double> const&);

template 
TJetVector<double> operator-(TJetVector<double> const&);

template 
TJet<std::complex<double> > operator-(TJet<std::complex<double> > const&, TJet<std::complex<double> > const&);

template 
TJet<double > operator-(TJet<double > const&, TJet<double > const&);

template 
TJet<double > operator-(double const&, TJet<double > const&);

template 
TJet<double > operator-(TJet<double > const&, double const&);

template 
TJet<std::complex<double> > operator-(TJet<std::complex<double> > const&);

template
TJetVector<double > operator*(double const&, TJetVector<double > const&);

template
 TJet<std::complex<double> > operator*(TJet<std::complex<double> > const&, std::complex<double> const&);

template 
TJet<std::complex<double> > operator*(std::complex<double> const&, TJet<std::complex<double> > const&);

template 
TJet<double > operator*(double const&, TJet<double > const&);

template 
TJetVector<double > operator*(TJet<double > const&, Vector const&);

template 
TJetVector<std::complex<double> > operator*(TMatrix<std::complex<double> > const&, TJetVector<std::complex<double> > const&);

template 
TJetVector<double > operator*(TMatrix<double> const&, TJetVector<double > const&);

template 
TJetVector<double > operator*<double >(TJet<double > const&, TJetVector<double > const&);

template 
TJet<std::complex<double> > operator*<std::complex<double> >(TJet<std::complex<double> > const&, TJet<std::complex<double> > const&);

template 
TJet<double > operator*<double >(TJet<double > const&, double const&);

template
TJet<double > operator*<double >(TJet<double > const&, TJet<double > const&);

template 
TJet<double > operator/<double >(TJet<double > const&, double const&);

template
 TJet<double > operator/<double >(TJet<double > const&, TJet<double > const&);

template 
TJet<std::complex<double> > operator/<std::complex<double> >(std::complex<double> const&, TJet<std::complex<double> > const&);

template 
TJet<std::complex<double> > operator/<std::complex<double> >(TJet<std::complex<double> > const&, std::complex<double> const&);


template
std::basic_ostream<char, std::char_traits<char> >& operator<< <double >(std::basic_ostream<char, std::char_traits<char> >&, TJet<double > const&);

template 
bool operator<=<std::complex<double> >(TJLterm<std::complex<double> > const&, TJLterm<std::complex<double> > const&);

template 
TJet<double > cosh<double >(TJet<double > const&);


template 
bool operator!=<double >(TJet<double > const&, TJet<double > const&);

template 
bool operator!=<std::complex<double> >(TJet<std::complex<double> > const&, std::complex<double> const&);

template 
bool operator==<std::complex<double> >(TJL<std::complex<double> > const&, TJL<std::complex<double> > const&);

template 
std::basic_ostream<char, std::char_traits<char> >& operator<< <std::complex<double> >(std::basic_ostream<char, std::char_traits<char> >&, TJet<std::complex<double> > const&);

template 
bool operator==<double >(TJL<double > const&, TJL<double > const&);

template 
TJet<double > asin<double >(TJet<double > const&);

template 
bool operator<=<double >(TJLterm<double > const&, TJLterm<double > const&);

template 
TJet<double > exp<double >(TJet<double > const&);

template 
TJet<std::complex<double> > sqrt<std::complex<double> >(TJet<std::complex<double> > const&);

template 
TJet<double > pow<double >(TJet<double > const&, double const&);


template 
TJet<double > operator/<double >(double const&, TJet<double > const&);

template 
bool operator==<std::complex<double> >(TJL<std::complex<double> > const&, std::complex<double> const&);

template 
bool operator==<double >(TJL<double > const&, double const&);

template 
bool operator%=<double >(TJLterm<double > const&, TJLterm<double > const&);

template 
bool operator!=<std::complex<double> >(TJet<std::complex<double> > const&, TJet<std::complex<double> > const&);

template 
TJet<double > sqrt<double >(TJet<double > const&);

template 
TJet<double > sinh<double >(TJet<double > const&);

template 
TJetVector<double > operator^<double >(Vector const&, TJetVector<double > const&);

template 
bool operator%=<std::complex<double> >(TJLterm<std::complex<double> > const&, TJLterm<std::complex<double> > const&);


template 
TJet<double > sin<double >(TJet<double > const&);

template 
TJet<double > cos<double >(TJet<double > const&);

template 
TJet<double > pow<double >(TJet<double > const&, int);


template 
Tcoord<std::complex<double> >::Tcoord(Tcoord<std::complex<double> > const&);


template 
bool operator!=<double >(TJet<double > const&, double const&);



template 
std::basic_istream<char, std::char_traits<char> >& operator>><double >(std::basic_istream<char, std::char_traits<char> >&, TJetVector<double >&);

template 
TJet<std::complex<double> > exp<std::complex<double> >(TJet<std::complex<double> > const&);

template 
TJet<std::complex<double> > operator/<std::complex<double> >(TJet<std::complex<double> > const&, TJet<std::complex<double> > const&);


template
std::basic_ostream<char, std::char_traits<char> >& operator<< <double >(std::basic_ostream<char, std::char_traits<char> >&, TJetVector<double > const&);

template
std::basic_istream<char, std::char_traits<char> >& operator>><double >(std::basic_istream<char, std::char_traits<char> >&, TJet<double >&);

template
std::basic_ostream<char, std::char_traits<char> >& operator<< <double >(std::basic_ostream<char, std::char_traits<char> >&, TJL<double > const&);

template
bool operator==<std::complex<double> >(TJLterm<std::complex<double> > const&, TJLterm<std::complex<double> > const&);

template
class boost::simple_segregated_storage<unsigned int>;

template
std::basic_istream<char, std::char_traits<char> >& streamIn<double >(std::basic_istream<char, std::char_traits<char> >&, TJetEnvironment<double >**);

template
std::basic_ostream<char, std::char_traits<char> >& operator<< <std::complex<double> >(std::basic_ostream<char, std::char_traits<char> >&, TJL<std::complex<double> > const&);

template
std::basic_ostream<char, std::char_traits<char> >& operator<< <double >(std::basic_ostream<char, std::char_traits<char> >&, TJetEnvironment<double > const&);

template
std::complex<double> std::sqrt<double>(std::complex<double> const&);

template
bool operator==<double >(TJLterm<double > const&, TJLterm<double > const&);

template 
std::basic_ostream<char, std::char_traits<char> >& operator<< <std::complex<double> >(std::basic_ostream<char, std::char_traits<char> >&, TJetEnvironment<std::complex<double> > const&);

template
class std::vector< TJL<std::complex<double> > * >;

template
class std::vector< TJL<double > *>;

template
std::basic_istream<char, std::char_traits<char> >& operator>><double >(std::basic_istream<char, std::char_traits<char> >&, TJL<double >&);


//=============================
// Memory allocation templates
//=============================

template 
class boost::pool<>;

template 
class gms::FastPODAllocator<TJLterm<std::complex<double> > >;

template 
class gms::FastPODAllocator<TJLterm<double> >;

template
unsigned int boost::details::pool::lcm<unsigned int>(unsigned int const&, unsigned int const&);

template
unsigned boost::details::pool::gcd<unsigned>(unsigned, unsigned);

//=======================================================================================================
// I am not sure why this is needed, but apparently it is. Probably originates from the bowels of BOOST ;-) 
//=======================================================================================================


template
void std::fill< std::vector< TJL< complex<double> >* >::iterator, TJL< complex<double> >* > 
               (std::vector< TJL< complex<double> >* >::iterator,  
                std::vector< TJL< complex<double> >* >::iterator,  TJL<complex<double> >* const& );

template
void std::fill< std::vector< TJL< double>* >::iterator, TJL< double>* > 
               (std::vector< TJL<double>* >::iterator, 
               std::vector< TJL<double>* >::iterator,
               TJL<double>* const& );
template
std::vector< TJL< complex<double> >* >::iterator
std::fill_n< std::vector< TJL< complex<double> >* >::iterator, unsigned int,  TJL< complex<double> >* > 
                 (std::vector< TJL< complex<double> >* >::iterator, 
                  unsigned int,  
                  TJL< complex<double> >* const& );

template
std::vector< TJL< double>* >::iterator
std::fill_n< std::vector< TJL<double>* >::iterator, unsigned int, TJL<double>* > 
                (std::vector< TJL<double>* >::iterator, 
                 unsigned int,
                 TJL<double>* const& );
template
TJL<double>**
std::fill_n< TJL<double>**, unsigned int, TJL<double>* > 
                (TJL<double>**, 
                 unsigned int,
                 TJL<double> * const& );
template
TJL< complex<double> >**
std::fill_n< TJL< complex<double> >**, unsigned int, TJL< complex<double> >* > 
                (TJL<complex<double> >**, 
                 unsigned int,
                 TJL<complex<double> > * const& );



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
