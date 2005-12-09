/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
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
#include <ReferenceCounter.h>
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

template class Tparam<double>;
template class Tparam<std::complex<double> >;


// ============================================================================================
// The friend function operators
//=============================================================================================


//  ****** Class TJet *******
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
 TJet<std::complex<double> > operator*(TJet<std::complex<double> > const&, std::complex<double> const&);

template 
TJet<std::complex<double> > operator*(std::complex<double> const&, TJet<std::complex<double> > const&);

template 
TJet<double > operator*(double const&, TJet<double > const&);

template 
TJet<std::complex<double> > operator*<std::complex<double> >(TJet<std::complex<double> > const&, TJet<std::complex<double> > const&);

template 
TJet<double > operator*<double >(TJet<double > const&, double const&);

template
TJet<double > operator*<double >(TJet<double > const&, TJet<double > const&);

template 
TJet<double > operator/<double >(double const&, TJet<double > const&);

template 
TJet<double > operator/<double >(TJet<double > const&, double const&);

template
 TJet<double > operator/<double >(TJet<double > const&, TJet<double > const&);

template 
TJet<std::complex<double> > operator/(std::complex<double> const&, TJet<std::complex<double> > const&);

template 
TJet<std::complex<double> > operator/(TJet<std::complex<double> > const&, std::complex<double> const&);

template
std::ostream& operator<<(std::ostream&, TJet<double > const&);

template 
bool operator<=(TJLterm<std::complex<double> > const&, TJLterm<std::complex<double> > const&);


template 
bool operator!=(TJet<double > const&, TJet<double > const&);

template 
bool operator!=(TJet<std::complex<double> > const&, std::complex<double> const&);

template
bool operator==(TJet<std::complex<double> > const&, TJet<std::complex<double> > const&);

template 
bool operator!=(TJet<std::complex<double> > const&, TJet<std::complex<double> > const&);

template 
std::ostream& operator<<(std::ostream&, TJet<std::complex<double> > const&);

template
bool operator==(TJet<double> const&, TJet<double> const&);

template 
bool operator==(TJet<double> const&, double const&);

template
bool operator==(TJet<std::complex<double> > const&, std::complex<double> const&);

// class ***********TJetVector **************

template 
TJetVector<double> operator-(TJetVector<double> const&);

template 
TJetVector<double > operator*(TJet<double > const&, Vector const&);

template 
TJetVector<double > operator*(TMatrix<double> const&, TJetVector<double > const&);

template 
TJetVector<double > operator*<double >(TJet<double > const&, TJetVector<double > const&);

template
TJetVector<double > operator*(double const&, TJetVector<double > const&);

template 
TJetVector<std::complex<double> > operator*(TMatrix<std::complex<double> > const&, TJetVector<std::complex<double> > const&);

template 
TJetVector<double > operator*(TJetVector<double > const&, double const&);

template 
TJetVector<std::complex<double> > operator*(std::complex<double> const&, TJetVector<std::complex<double> > const&);

template 
TJetVector<std::complex<double> > operator*(TJetVector<std::complex<double> > const&, std::complex<double> const&);



// class ***********TJL **************



template 
bool operator==(TJL<double > const&, TJL<double > const&);

template 
bool operator==<std::complex<double> >(TJL<std::complex<double> > const&, TJL<std::complex<double> > const&);

template 
bool operator<=(TJLterm<double > const&, TJLterm<double > const&);

template 
bool operator==(TJL<std::complex<double> > const&, std::complex<double> const&);

template 
bool operator==(TJL<double > const&, double const&);

template 
bool operator%=(TJLterm<double > const&, TJLterm<double > const&);



//----------------------------------------------------------------------------------------------------------------------
// Math Functions
//---------------------------------------------------------------------------------------------------------------------

template 
TJet<double > log(TJet<double > const&);

template 
TJet<std::complex<double> > log(TJet<std::complex<double> > const&);

template 
TJet<double > log10(TJet<double > const&);

template 
TJet<std::complex<double> > log10(TJet<std::complex<double> > const&);

template 
TJet<double > asin(TJet<double > const&);

template 
TJet<std::complex<double> > asin(TJet<std::complex<double> > const&);

template 
TJet<double > atan(TJet<double > const&);

template 
TJet<std::complex<double> > atan(TJet<std::complex<double> > const&);

template 
TJet<double > exp(TJet<double > const&);

template 
TJet<std::complex<double> > exp<std::complex<double> >(TJet<std::complex<double> > const&);

template 
TJet<double > sqrt(TJet<double > const&);

template 
TJet<std::complex<double> > sqrt(TJet<std::complex<double> > const&);

template 
TJet<double > pow<double >(TJet<double > const&, double const&);

template 
TJet<double > sinh(TJet<double > const&);

template 
TJet<std::complex<double> > sinh(TJet<std::complex<double> > const&);

template 
TJet<double > cosh(TJet<double > const&);

template 
TJet<std::complex<double> > cosh(TJet<std::complex<double> > const&);

template 
TJet<double > tanh(TJet<double > const&);

template 
TJet<std::complex<double> > tanh(TJet<std::complex<double> > const&);

template 
TJet<double > sin(TJet<double > const&);

template 
TJet<std::complex<double> > sin(TJet<std::complex<double> > const&);

template 
TJet<double > cos(TJet<double > const&);

template 
TJet<std::complex<double> > cos(TJet<std::complex<double> > const&);

template 
TJet<double > pow(TJet<double > const&, int);

template 
TJet<std::complex<double> > pow(TJet<std::complex<double> > const&, int);


// ------------------------------------------------------------------------------------
// operators 
//---------------------------------------------------------------------------------------


template 
TJetVector<double > operator^<double >(Vector const&, TJetVector<double > const&);

template 
bool operator%=<std::complex<double> >(TJLterm<std::complex<double> > const&, TJLterm<std::complex<double> > const&);

template 
bool operator!=<double >(TJet<double > const&, double const&);


template 
std::istream& operator>>(std::istream&, TJetVector<double >&);


template 
TJet<std::complex<double> > operator/<std::complex<double> >(TJet<std::complex<double> > const&, TJet<std::complex<double> > const&);


template
std::ostream& operator<< <double >(std::ostream&, TJetVector<double > const&);

template
std::istream& operator>>(std::istream&, TJet<double >&);

template
std::ostream& operator<<(std::ostream&, TJL<double > const&);

template
bool operator==<std::complex<double> >(TJLterm<std::complex<double> > const&, TJLterm<std::complex<double> > const&);

template
class boost::simple_segregated_storage<unsigned int>;

//std::istream& streamIn<double>( std::istream&, EnvPtr<double>::Type* );

//template
//std::istream& streamIn( std::istream&, EnvPtr<std::complex<double> >::Type* );

template
std::ostream& operator<<(std::ostream&, TJL<std::complex<double> > const&);

template
std::ostream& operator<<(std::ostream&, TJetEnvironment<double > const&);

template
bool operator==<double >(TJLterm<double > const&, TJLterm<double > const&);

template 
std::ostream& operator<<(std::ostream&, TJetEnvironment<std::complex<double> > const&);

template
class std::vector< TJL<std::complex<double> > * >;

template
class std::vector< TJL<double > *>;

template
std::istream& operator>>(std::istream&, TJL<double >&);

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

template
double* std::fill_n<double*, unsigned int, double>(double*, unsigned int, double const&);

template
class std::deque<Tcoord<double>* >;

template
class std::deque<Tcoord<std::complex<double> >* >;

template class
std::_Deque_base<Tcoord<double>*, std::allocator<Tcoord<double>*> >;

template class
std::_Deque_base<Tcoord<std::complex<double> >*, std::allocator<Tcoord<std::complex<double> >*> >;


template
class std::deque<Tparam<double>* >;

template
class std::deque<Tparam<std::complex<double> >* >;

template class
std::_Deque_base<Tparam<double>*, std::allocator<Tparam<double>*> >;

template class
std::_Deque_base<Tparam<std::complex<double> >*, std::allocator<Tparam<std::complex<double> >*> >;

template
void std::fill(std::_Deque_iterator<Tcoord<double >*, Tcoord<double >*&, Tcoord<double >**>, std::_Deque_iterator<Tcoord<double >*, Tcoord<double >*&, Tcoord<double >**>, Tcoord<double >* const&);

template
void std::fill(std::_Deque_iterator<Tcoord<std::complex<double> >*, Tcoord<std::complex<double> >*&, Tcoord<std::complex<double> >**>, std::_Deque_iterator<Tcoord<std::complex<double> >*, Tcoord<std::complex<double> >*&, Tcoord<std::complex<double> >**>, Tcoord<std::complex<double> >* const&);

template
void std::deque<Tcoord<double >*>::_M_range_insert_aux(std::_Deque_iterator<Tcoord<double >*, Tcoord<double >*&, Tcoord<double >**>, std::_Deque_iterator<Tcoord<double >*, Tcoord<double >* const&, Tcoord<double >* const*>, std::_Deque_iterator<Tcoord<double >*, Tcoord<double >* const&, Tcoord<double >* const*>, std::forward_iterator_tag);

template
void std::deque<Tcoord<std::complex<double> >*>::_M_range_insert_aux(std::_Deque_iterator<Tcoord<std::complex<double> >*, Tcoord<std::complex<double> >*&, Tcoord<std::complex<double> >**>, std::_Deque_iterator<Tcoord<std::complex<double> >*, Tcoord<std::complex<double> >* const&, Tcoord<std::complex<double> >* const*>, std::_Deque_iterator<Tcoord<std::complex<double> >*, Tcoord<std::complex<double> >* const&, Tcoord<std::complex<double> >* const*>, std::forward_iterator_tag);

template
void std::fill(std::_Deque_iterator<Tparam<double >*, Tparam<double >*&, Tparam<double >**>, std::_Deque_iterator<Tparam<double >*, Tparam<double >*&, Tparam<double >**>, Tparam<double >* const&);

template
void std::fill(std::_Deque_iterator<Tparam<std::complex<double> >*, Tparam<std::complex<double> >*&, Tparam<std::complex<double> >**>, std::_Deque_iterator<Tparam<std::complex<double> >*, Tparam<std::complex<double> >*&, Tparam<std::complex<double> >**>, Tparam<std::complex<double> >* const&);

template
void std::deque<Tparam<double>* >::_M_range_insert_aux(std::_Deque_iterator<Tparam<double >*, Tparam<double >*&, Tparam<double >**>, std::_Deque_iterator<Tparam<double >*, Tparam<double >* const&, Tparam<double >* const*>, std::_Deque_iterator<Tparam<double >*, Tparam<double >* const&, Tparam<double >* const*>, std::forward_iterator_tag);

template
void std::deque<Tparam<std::complex<double> >*>::_M_range_insert_aux(std::_Deque_iterator<Tparam<std::complex<double> >*, Tparam<std::complex<double> >*&, Tparam<std::complex<double> >**>, std::_Deque_iterator<Tparam<std::complex<double> >*, Tparam<std::complex<double> >* const&, Tparam<std::complex<double> >* const*>, std::_Deque_iterator<Tparam<std::complex<double> >*, Tparam<std::complex<double> >* const&, Tparam<std::complex<double> >* const*>, std::forward_iterator_tag);


template
void std::deque<Tcoord<double>* >::_M_insert_aux(std::_Deque_iterator<Tcoord<double>*, Tcoord<double>*&, Tcoord<double>**>, std::_Deque_iterator<Tcoord<double>*, Tcoord<double>* const&, Tcoord<double>* const*>, std::_Deque_iterator<Tcoord<double>*, Tcoord<double>* const&, Tcoord<double>* const*>, unsigned int);

template
void std::deque<Tcoord<std::complex<double> >* >::_M_insert_aux(std::_Deque_iterator<Tcoord<std::complex<double> >*, Tcoord<std::complex<double> >*&, Tcoord<std::complex<double> >**>, std::_Deque_iterator<Tcoord<std::complex<double> >*, Tcoord<std::complex<double> >* const&, Tcoord<std::complex<double> >* const*>, std::_Deque_iterator<Tcoord<std::complex<double> >*, Tcoord<std::complex<double> >* const&, Tcoord<std::complex<double> >* const*>, unsigned int);

template
void std::deque<Tparam<double>* >::_M_insert_aux(std::_Deque_iterator<Tparam<double>*, Tparam<double>*&, Tparam<double>**>, std::_Deque_iterator<Tparam<double>*, Tparam<double>* const&, Tparam<double>* const*>, std::_Deque_iterator<Tparam<double>*, Tparam<double>* const&, Tparam<double>* const*>, unsigned int);

template
void std::deque<Tparam<std::complex<double> >* >::_M_insert_aux(std::_Deque_iterator<Tparam<std::complex<double> >*, Tparam<std::complex<double> >*&, Tparam<std::complex<double> >**>, std::_Deque_iterator<Tparam<std::complex<double> >*, Tparam<std::complex<double> >* const&, Tparam<std::complex<double> >* const*>, std::_Deque_iterator<Tparam<std::complex<double> >*, Tparam<std::complex<double> >* const&, Tparam<std::complex<double> >* const*>, unsigned int);

template
void std::fill<Tcoord<double >**, Tcoord<double >*>(Tcoord<double >**, Tcoord<double >**, Tcoord<double >* const&);

template
void std::fill<Tcoord<std::complex<double> >**, Tcoord<std::complex<double> >*>(Tcoord<std::complex<double> >**, Tcoord<std::complex<double> >**, Tcoord<std::complex<double> >* const&);

template
void std::fill<Tparam<double >**, Tparam<double >*>(Tparam<double >**, Tparam<double >**, Tparam<double >* const&);

template
void std::fill<Tparam<std::complex<double> >**, Tparam<std::complex<double> >*>(Tparam<std::complex<double> >**, Tparam<std::complex<double> >**, Tparam<std::complex<double> >* const&);


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

template 
class boost::shared_ptr<TJetEnvironment<double> >;

template 
class boost::shared_ptr<TJetEnvironment<std::complex<double> > >;

template
class boost::detail::sp_counted_impl_p<TJetEnvironment<std::complex<double> > >;

template
class boost::detail::sp_counted_impl_p<TJetEnvironment<double> >;

template 
class std::list<EnvPtr<double>::Type>;

template 
class std::list<EnvPtr<std::complex<double> >::Type>;


template
void std::_List_base<EnvPtr<double>::Type, std::allocator<EnvPtr<double>::Type> > ::_M_clear();

template
void std::_List_base<EnvPtr<std::complex<double> >::Type,  std::allocator<EnvPtr<std::complex<double> >::Type> >::_M_clear();

template
std::complex<double>  std::sqrt<double>( const std::complex<double>& );




#endif // MXYZPTLK_IMPLICIT_TEMPLATES 
