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
// If implicit template instantiations are desired, *do not* compile this file !
// =================================================================================

#ifdef MXYZPTLK_EXPLICIT_TEMPLATES 

#include <complex>
#include <basic_toolkit/ReferenceCounter.h>
#include <mxyzptlk/TJLterm.h>
#include <mxyzptlk/TJL.h>
#include <mxyzptlk/TJL.tcc>

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

template class TJL<double>;
template JLPtr<double> TJL<double>::makeTJL(TJL<double> const&);

template class TJL<std::complex<double> >;
template TJL<std::complex<double> >::TJL(TJL<double> const&);
template JLPtr<std::complex<double> > TJL<std::complex<double> >::makeTJL(TJL<std::complex<double> > const&);

// ============================================================================================
// Friend function operators
//=============================================================================================

template
JLPtr<double>   operator+(JLPtr<double> const & x, JLPtr<double> const& y  );  
template
JLPtr<std::complex<double> >   operator+(JLPtr<std::complex<double> > const & x, JLPtr<std::complex<double> > const& y  );  

template
JLPtr<double>&  operator+=(JLPtr<double>& x,      JLPtr<double> const& y  );  
template
JLPtr<std::complex<double> >&  operator+=(JLPtr<std::complex<double> >& x,      JLPtr<std::complex<double> > const& y  );  

template
JLPtr<double>   operator-(JLPtr<double> const &x);  
template
JLPtr<std::complex<double> >   operator-(JLPtr<std::complex<double> > const &x);  

template
JLPtr<double>   operator-(JLPtr<double> const & x,  JLPtr<double> const& y  );  
template
JLPtr<std::complex<double> >   operator-(JLPtr<std::complex<double> > const & x,  JLPtr<std::complex<double> > const& y  );  

template
JLPtr<double>&  operator-=(JLPtr<double>& x,      JLPtr<double> const& y  );  
template
JLPtr<std::complex<double> >&  operator-=(JLPtr<std::complex<double> >& x,      JLPtr<std::complex<double> > const& y  );  


template
JLPtr<double>   operator*(JLPtr<double> const & x,  JLPtr<double> const& y  );  
template
JLPtr<std::complex<double> >   operator*(JLPtr<std::complex<double> > const & x,  JLPtr<std::complex<double> > const& y  );  

template
JLPtr<double>   operator*(JLPtr<double> const & x,   double const& y  );  
template
JLPtr<std::complex<double> >   operator*(JLPtr<std::complex<double> > const & x,  std::complex<double> const& y  );  

template
JLPtr<double>   operator*(double const & x,  JLPtr<double> const& y  );  
template
JLPtr<std::complex<double> >   operator*(std::complex<double> const & x,  JLPtr<std::complex<double> > const& y  );  

template
JLPtr<double>&  operator*=(JLPtr<double> & x,     JLPtr<double> const& y  );  
template
JLPtr<std::complex<double> >&  operator*=(JLPtr<std::complex<double> > & x,     JLPtr<std::complex<double> > const& y  );  

template
JLPtr<double>   operator/( JLPtr<double> const& x,  JLPtr<double> const& y  );  
template
JLPtr<std::complex<double> >   operator/(  JLPtr<std::complex<double> > const& x,   JLPtr<std::complex<double> > const& y  );  

template
JLPtr<double>   operator/(  JLPtr<double> const & x,  double const& y  );  
template
JLPtr<std::complex<double> >   operator/( JLPtr<std::complex<double> > const & x,  std::complex<double> const& y  );  


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

//operator*(TJet<std::complex<double> > const&, TJet<double> const&)'
//operator+(TJet<double> const&, TJet<std::complex<double> > const&)'

//template 
//void intrusive_ptr_release<TML<std::complex<double> > >(ReferenceCounter<TML<std::complex<double> > >*);

//template
//void intrusive_ptr_release<TML<double> >(ReferenceCounter<TML<double> >*);

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
void std::fill( std::vector< TJL< std::complex<double> >* >::iterator,  
                std::vector< TJL< std::complex<double> >* >::iterator,  
                TJL<std::complex<double> >* const& );

template
void std::fill( std::vector< TJL<double>* >::iterator, 
                std::vector< TJL<double>* >::iterator,
                TJL<double>* const& );
template
std::vector< TJL< std::complex<double> >* >::iterator
std::fill_n( std::vector< TJL< std::complex<double> >* >::iterator, 
             unsigned int,  
             TJL< std::complex<double> >* const& );

template
std::vector< TJL< double>* >::iterator
std::fill_n( std::vector< TJL<double>* >::iterator, 
             unsigned int,
             TJL<double>* const& );

template
TJL<double>**
std::fill_n(     TJL<double>**, 
                 unsigned int,
                 TJL<double> * const& );
template
TJL< std::complex<double> >**
std::fill_n ( TJL<std::complex<double> >**, 
              unsigned int,
              TJL<std::complex<double> > * const& );


template
double* std::fill_n(double*, unsigned int, double const&);


//-------------------------------------------------------------------------------------------------------------------------------------------------------

#if (__GNUC__ > 3 )

template 
void std::__uninitialized_fill_n_a(__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double>* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double>* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double>* const, unsigned int> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<double>* const, unsigned int> >* const&, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double>* const, unsigned int> >*>);

template 
void  std::__uninitialized_fill_n_a(__gnu_cxx::__normal_iterator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >**, std::vector<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*> > >, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >* const&, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*>);


template
void std::__uninitialized_fill_n_a(TJL<double>**, unsigned int, TJL<double>* const&, std::allocator<TJL<double>*>);

template
void std::__uninitialized_fill_n_a(__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double>* const, unsigned int> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<double>* const, unsigned int> >* const&, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<double>* const, unsigned int> >*>);

template 
void std::__uninitialized_fill_n_a(TJL<std::complex<double> >**, unsigned int, TJL<std::complex<double> >* const&, std::allocator<TJL<std::complex<double> >*>);

template 
void std::__uninitialized_fill_n_a(__gnu_cxx::__normal_iterator<TJL<double>**, std::vector<TJL<double>*, std::allocator<TJL<double>*> > >, unsigned int, TJL<double>* const&, std::allocator<TJL<double>*>);

template
void std::__uninitialized_fill_n_a(__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >**, unsigned int, __gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >* const&, std::allocator<__gnu_cxx::_Hashtable_node<std::pair<TJLterm<std::complex<double> >* const, unsigned int> >*>);

template 
void std::__uninitialized_fill_n_a(__gnu_cxx::__normal_iterator<TJL<std::complex<double> >**, std::vector<TJL<std::complex<double> >*, std::allocator<TJL<std::complex<double> >*> > >, unsigned int, TJL<std::complex<double> >* const&, std::allocator<TJL<std::complex<double> >*>);

#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------

template
std::complex<double> std::pow(std::complex<double> const&, double const&);

template
std::complex<double>  std::sqrt<double>( const std::complex<double>& ); 


#endif // MXYZPTLK_EXPLICIT_TEMPLATES 


