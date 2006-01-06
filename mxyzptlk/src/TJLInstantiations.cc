/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJLInstantiations.cc
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
#include <TJL.h>
#include <TJL.tcc>

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

template class TJL<double>;
template class TJL<std::complex<double> >;

// ============================================================================================
// Friend function operators
//=============================================================================================

template
JLPtr<double>::Type   operator+(JLPtr<double>::Type const & x, JLPtr<double>::Type const& y  );  
template
JLPtr<std::complex<double> >::Type   operator+(JLPtr<std::complex<double> >::Type const & x, JLPtr<std::complex<double> >::Type const& y  );  

template
JLPtr<double>::Type&  operator+=(JLPtr<double>::Type& x,      JLPtr<double>::Type const& y  );  
template
JLPtr<std::complex<double> >::Type&  operator+=(JLPtr<std::complex<double> >::Type& x,      JLPtr<std::complex<double> >::Type const& y  );  

template
JLPtr<double>::Type   operator-(JLPtr<double>::Type const &x);  
template
JLPtr<std::complex<double> >::Type   operator-(JLPtr<std::complex<double> >::Type const &x);  

template
JLPtr<double>::Type   operator-(JLPtr<double>::Type const & x,  JLPtr<double>::Type const& y  );  
template
JLPtr<std::complex<double> >::Type   operator-(JLPtr<std::complex<double> >::Type const & x,  JLPtr<std::complex<double> >::Type const& y  );  

template
JLPtr<double>::Type&  operator-=(JLPtr<double>::Type& x,      JLPtr<double>::Type const& y  );  
template
JLPtr<std::complex<double> >::Type&  operator-=(JLPtr<std::complex<double> >::Type& x,      JLPtr<std::complex<double> >::Type const& y  );  


template
JLPtr<double>::Type   operator*(JLPtr<double>::Type const & x,  JLPtr<double>::Type const& y  );  
template
JLPtr<std::complex<double> >::Type   operator*(JLPtr<std::complex<double> >::Type const & x,  JLPtr<std::complex<double> >::Type const& y  );  

template
JLPtr<double>::Type   operator*(JLPtr<double>::Type const & x,   double const& y  );  
template
JLPtr<std::complex<double> >::Type   operator*(JLPtr<std::complex<double> >::Type const & x,  std::complex<double> const& y  );  

template
JLPtr<double>::Type   operator*(double const & x,  JLPtr<double>::Type const& y  );  
template
JLPtr<std::complex<double> >::Type   operator*(std::complex<double> const & x,  JLPtr<std::complex<double> >::Type const& y  );  

template
JLPtr<double>::Type&  operator*=(JLPtr<double>::Type & x,     JLPtr<double>::Type const& y  );  
template
JLPtr<std::complex<double> >::Type&  operator*=(JLPtr<std::complex<double> >::Type & x,     JLPtr<std::complex<double> >::Type const& y  );  

template
JLPtr<double>::Type   operator/(JLPtr<double>::Type const & x,  JLPtr<double>::Type const& y  );  
template
JLPtr<std::complex<double> >::Type   operator/(JLPtr<std::complex<double> >::Type const & x,  JLPtr<std::complex<double> >::Type const& y  );  

template
JLPtr<double>::Type   operator/(JLPtr<double>::Type const & x,  double const& y  );  
template
JLPtr<std::complex<double> >::Type   operator/(JLPtr<std::complex<double> >::Type const & x,  std::complex<double> const& y  );  


template 
bool operator==(TJL<double > const&, TJL<double > const&);

template 
bool operator==<std::complex<double> >(TJL<std::complex<double> > const&, TJL<std::complex<double> > const&);

template 
bool operator==(TJL<std::complex<double> > const&, std::complex<double> const&);

template 
bool operator==(TJL<double > const&, double const&);


template
std::ostream& operator<<(std::ostream&, TJL<double > const&);
template
std::ostream& operator<<(std::ostream&, TJL<std::complex<double> > const&);

template
std::istream& operator>>(std::istream&, TJL<double >&);
template
std::istream& operator>>(std::istream&, TJL<std::complex<double> >&);

template
class boost::simple_segregated_storage<unsigned int>;


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
// BOOST & STL
//=======================================================================================================

template
class std::vector< TJL<std::complex<double> > * >;

template
class std::vector< TJL<double > *>;


template
void std::fill< std::vector< TJL< std::complex<double> >* >::iterator, TJL< std::complex<double> >* > 
               (std::vector< TJL< std::complex<double> >* >::iterator,  
                std::vector< TJL< std::complex<double> >* >::iterator,  TJL<std::complex<double> >* const& );

template
void std::fill< std::vector< TJL< double>* >::iterator, TJL< double>* > 
               (std::vector< TJL<double>* >::iterator, 
               std::vector< TJL<double>* >::iterator,
               TJL<double>* const& );
template
std::vector< TJL< std::complex<double> >* >::iterator
std::fill_n< std::vector< TJL< std::complex<double> >* >::iterator, unsigned int,  TJL< std::complex<double> >* > 
                 (std::vector< TJL< std::complex<double> >* >::iterator, 
                  unsigned int,  
                  TJL< std::complex<double> >* const& );

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
TJL< std::complex<double> >**
std::fill_n< TJL< std::complex<double> >**, unsigned int, TJL< std::complex<double> >* > 
                (TJL<std::complex<double> >**, 
                 unsigned int,
                 TJL<std::complex<double> > * const& );



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
std::complex<double> std::pow(std::complex<double> const&, double const&);

template
std::complex<double>  std::sqrt<double>( const std::complex<double>& ); 


#endif // MXYZPTLK_IMPLICIT_TEMPLATES 



