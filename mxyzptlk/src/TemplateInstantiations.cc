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

#define  EXPLICIT_TEMPLATE_INSTANTIATIONS 

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
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <vector>
#include <algorithm>
#include <complex>

// ============================================================================================
// The standard classes
//=============================================================================================

template class TJetEnvironment<double, std::complex<double> >;
template class TJetEnvironment<std::complex<double>, double >;

template class TJLterm<double, std::complex<double> >;
template class TJLterm<std::complex<double>, double >;

template class TJL<double, std::complex<double> >;
template class TJL<std::complex<double>, double >;

template class TJet<double, std::complex<double> >;
template class TJet<std::complex<double>, double >;

template class TJetVector<double, std::complex<double> >;
template class TJetVector<std::complex<double>, double >;

template class TMapping<double, std::complex<double> >;
template class TMapping<std::complex<double>, double >;

template class TLieOperator<double, std::complex<double> >;
template class TLieOperator<std::complex<double>, double >;

template class Tcoord<double, std::complex<double> >;
template class Tcoord<std::complex<double>, double >;


// ============================================================================================
// The friend function operators
//=============================================================================================

template 
TJet<double, std::complex<double> > operator+<double, std::complex<double> >(double const&, TJet<double, std::complex<double> > const&);

template 
TJet<std::complex<double>, double> operator+<std::complex<double>, double>(TJet<std::complex<double>, double> const&, TJet<std::complex<double>, double> const&);

template 
TJet<std::complex<double>, double> operator+<std::complex<double>, double>(std::complex<double> const&, TJet<std::complex<double>, double> const&);

template 
TJet<std::complex<double>, double> operator+<std::complex<double>, double>(TJet<std::complex<double>, double> const&, std::complex<double> const&);

template 
TJet<double, std::complex<double> > operator+<double, std::complex<double> >(TJet<double, std::complex<double> > const&, double const&);

template 
TJet<double, std::complex<double> > operator+<double, std::complex<double> >(TJet<double, std::complex<double> > const&, TJet<double, std::complex<double> > const&);

template
TJet<std::complex<double>, double> operator-<std::complex<double>, double>(std::complex<double> const&, TJet<std::complex<double>, double> const&);

template 
TJet<std::complex<double>, double> operator-<std::complex<double>, double>(TJet<std::complex<double>, double> const&, std::complex<double> const&);

template 
TJet<double, std::complex<double> > operator-<double, std::complex<double> >(TJet<double, std::complex<double> > const&);

template 
TJetVector<double, std::complex<double> > operator-<double, std::complex<double> >(TJetVector<double, std::complex<double> > const&);

template 
TJet<std::complex<double>, double> operator-<std::complex<double>, double>(TJet<std::complex<double>, double> const&, TJet<std::complex<double>, double> const&);

template 
TJet<double, std::complex<double> > operator-<double, std::complex<double> >(TJet<double, std::complex<double> > const&, TJet<double, std::complex<double> > const&);

template 
TJet<double, std::complex<double> > operator-<double, std::complex<double> >(double const&, TJet<double, std::complex<double> > const&);

template 
TJet<double, std::complex<double> > operator-<double, std::complex<double> >(TJet<double, std::complex<double> > const&, double const&);

template 
TJet<std::complex<double>, double> operator-<std::complex<double>, double>(TJet<std::complex<double>, double> const&);

template
TJetVector<double, std::complex<double> > operator*<double, std::complex<double> >(double const&, TJetVector<double, std::complex<double> > const&);

template
 TJet<std::complex<double>, double> operator*<std::complex<double>, double>(TJet<std::complex<double>, double> const&, std::complex<double> const&);

template 
TJet<std::complex<double>, double> operator*<std::complex<double>, double>(std::complex<double> const&, TJet<std::complex<double>, double> const&);

template 
TJet<double, std::complex<double> > operator*<double, std::complex<double> >(double const&, TJet<double, std::complex<double> > const&);

template 
TJetVector<double, std::complex<double> > operator*<double, std::complex<double> >(TJet<double, std::complex<double> > const&, Vector const&);

template 
TJetVector<std::complex<double>, double> operator*<std::complex<double>, double>(TMatrix<std::complex<double> > const&, TJetVector<std::complex<double>, double> const&);

template 
TJetVector<double, std::complex<double> > operator*<double, std::complex<double> >(TMatrix<double> const&, TJetVector<double, std::complex<double> > const&);

template 
TJetVector<double, std::complex<double> > operator*<double, std::complex<double> >(TJet<double, std::complex<double> > const&, TJetVector<double, std::complex<double> > const&);

template 
TJet<std::complex<double>, double> operator*<std::complex<double>, double>(TJet<std::complex<double>, double> const&, TJet<std::complex<double>, double> const&);

template 
TJet<double, std::complex<double> > operator*<double, std::complex<double> >(TJet<double, std::complex<double> > const&, double const&);

template
TJet<double, std::complex<double> > operator*<double, std::complex<double> >(TJet<double, std::complex<double> > const&, TJet<double, std::complex<double> > const&);

template 
TJet<double, std::complex<double> > operator/<double, std::complex<double> >(TJet<double, std::complex<double> > const&, double const&);

template
 TJet<double, std::complex<double> > operator/<double, std::complex<double> >(TJet<double, std::complex<double> > const&, TJet<double, std::complex<double> > const&);

template 
TJet<std::complex<double>, double> operator/<std::complex<double>, double>(std::complex<double> const&, TJet<std::complex<double>, double> const&);

template 
TJet<std::complex<double>, double> operator/<std::complex<double>, double>(TJet<std::complex<double>, double> const&, std::complex<double> const&);


template
std::basic_ostream<char, std::char_traits<char> >& operator<< <double, std::complex<double> >(std::basic_ostream<char, std::char_traits<char> >&, TJet<double, std::complex<double> > const&);

template 
bool operator<=<std::complex<double>, double>(TJLterm<std::complex<double>, double> const&, TJLterm<std::complex<double>, double> const&);

template 
TJet<double, std::complex<double> > cosh<double, std::complex<double> >(TJet<double, std::complex<double> > const&);


template 
bool operator!=<double, std::complex<double> >(TJet<double, std::complex<double> > const&, TJet<double, std::complex<double> > const&);

template 
bool operator!=<std::complex<double>, double>(TJet<std::complex<double>, double> const&, std::complex<double> const&);

template 
bool operator==<std::complex<double>, double>(TJL<std::complex<double>, double> const&, TJL<std::complex<double>, double> const&);

template 
std::basic_ostream<char, std::char_traits<char> >& operator<< <std::complex<double>, double>(std::basic_ostream<char, std::char_traits<char> >&, TJet<std::complex<double>, double> const&);

template 
bool operator==<double, std::complex<double> >(TJL<double, std::complex<double> > const&, TJL<double, std::complex<double> > const&);

template 
TJet<double, std::complex<double> > asin<double, std::complex<double> >(TJet<double, std::complex<double> > const&);

template 
bool operator<=<double, std::complex<double> >(TJLterm<double, std::complex<double> > const&, TJLterm<double, std::complex<double> > const&);

template 
TJet<double, std::complex<double> > exp<double, std::complex<double> >(TJet<double, std::complex<double> > const&);

template 
TJet<std::complex<double>, double> sqrt<std::complex<double>, double>(TJet<std::complex<double>, double> const&);

template 
TJet<double, std::complex<double> > pow<double, std::complex<double> >(TJet<double, std::complex<double> > const&, double const&);


template 
TJet<double, std::complex<double> > operator/<double, std::complex<double> >(double const&, TJet<double, std::complex<double> > const&);

template 
bool operator==<std::complex<double>, double>(TJL<std::complex<double>, double> const&, std::complex<double> const&);

template 
bool operator==<double, std::complex<double> >(TJL<double, std::complex<double> > const&, double const&);

template 
bool operator%=<double, std::complex<double> >(TJLterm<double, std::complex<double> > const&, TJLterm<double, std::complex<double> > const&);

template 
bool operator!=<std::complex<double>, double>(TJet<std::complex<double>, double> const&, TJet<std::complex<double>, double> const&);

template 
TJet<double, std::complex<double> > sqrt<double, std::complex<double> >(TJet<double, std::complex<double> > const&);

template 
TJet<double, std::complex<double> > sinh<double, std::complex<double> >(TJet<double, std::complex<double> > const&);

template 
TJetVector<double, std::complex<double> > operator^<double, std::complex<double> >(Vector const&, TJetVector<double, std::complex<double> > const&);

template 
bool operator%=<std::complex<double>, double>(TJLterm<std::complex<double>, double> const&, TJLterm<std::complex<double>, double> const&);


template 
TJet<double, std::complex<double> > sin<double, std::complex<double> >(TJet<double, std::complex<double> > const&);

template 
TJet<double, std::complex<double> > cos<double, std::complex<double> >(TJet<double, std::complex<double> > const&);

template 
TJet<double, std::complex<double> > pow<double, std::complex<double> >(TJet<double, std::complex<double> > const&, int);


template 
Tcoord<std::complex<double>, double>::Tcoord(Tcoord<std::complex<double>, double> const&);


template 
bool operator!=<double, std::complex<double> >(TJet<double, std::complex<double> > const&, double const&);



template 
std::basic_istream<char, std::char_traits<char> >& operator>><double, std::complex<double> >(std::basic_istream<char, std::char_traits<char> >&, TJetVector<double, std::complex<double> >&);

template 
TJet<std::complex<double>, double> exp<std::complex<double>, double>(TJet<std::complex<double>, double> const&);

template 
TJet<std::complex<double>, double> operator/<std::complex<double>, double>(TJet<std::complex<double>, double> const&, TJet<std::complex<double>, double> const&);


template
std::basic_ostream<char, std::char_traits<char> >& operator<< <double, std::complex<double> >(std::basic_ostream<char, std::char_traits<char> >&, TJetVector<double, std::complex<double> > const&);

template
std::basic_istream<char, std::char_traits<char> >& operator>><double, std::complex<double> >(std::basic_istream<char, std::char_traits<char> >&, TJet<double, std::complex<double> >&);

template
std::basic_ostream<char, std::char_traits<char> >& operator<< <double, std::complex<double> >(std::basic_ostream<char, std::char_traits<char> >&, TJL<double, std::complex<double> > const&);

template
bool operator==<std::complex<double>, double>(TJLterm<std::complex<double>, double> const&, TJLterm<std::complex<double>, double> const&);

template
class boost::simple_segregated_storage<unsigned int>;

template
std::basic_istream<char, std::char_traits<char> >& streamIn<double, std::complex<double> >(std::basic_istream<char, std::char_traits<char> >&, TJetEnvironment<double, std::complex<double> >**);

template
std::basic_ostream<char, std::char_traits<char> >& operator<< <std::complex<double>, double>(std::basic_ostream<char, std::char_traits<char> >&, TJL<std::complex<double>, double> const&);

template
std::basic_ostream<char, std::char_traits<char> >& operator<< <double, std::complex<double> >(std::basic_ostream<char, std::char_traits<char> >&, TJetEnvironment<double, std::complex<double> > const&);

template
std::complex<double> std::sqrt<double>(std::complex<double> const&);

template
bool operator==<double, std::complex<double> >(TJLterm<double, std::complex<double> > const&, TJLterm<double, std::complex<double> > const&);

template 
std::basic_ostream<char, std::char_traits<char> >& operator<< <std::complex<double>, double>(std::basic_ostream<char, std::char_traits<char> >&, TJetEnvironment<std::complex<double>, double> const&);

template
class std::vector< TJL<std::complex<double>, double> * >;

template
class std::vector< TJL<double, std::complex<double> > *>;

template
std::basic_istream<char, std::char_traits<char> >& operator>><double, std::complex<double> >(std::basic_istream<char, std::char_traits<char> >&, TJL<double, std::complex<double> >&);


//=============================
// Memory allocation templates
//=============================

template 
class boost::pool<>;

template 
class gms::FastPODAllocator<TJLterm<std::complex<double>, double               > >;

template 
class gms::FastPODAllocator<TJLterm<double,               std::complex<double> > >;

//=======================================================================================================
// I am not sure why this is needed, but apparently it is. Probably originates from the bowels of BOOST ;-) 
//=======================================================================================================

template
void std::fill< std::vector< TJL< complex<double>,double >* >::iterator, TJL< complex<double>, double >* > 
               (std::vector< TJL< complex<double>,double >* >::iterator,  
                std::vector< TJL< complex<double>,double >* >::iterator,  TJL<complex<double>, double>* const& );

template
void std::fill< std::vector< TJL< double, complex<double> >* >::iterator, TJL< double, complex<double> >* > 
              (std::vector< TJL<double, complex<double> >* >::iterator, 
               std::vector< TJL<double, complex<double> >* >::iterator,
               TJL<double, complex<double> >* const& );
template
std::vector< TJL< complex<double>,double >* >::iterator
std::fill_n< std::vector< TJL< complex<double>,double >* >::iterator, unsigned int,  TJL< complex<double>, double >* > 
                 (std::vector< TJL< complex<double>,double >* >::iterator, 
                  unsigned int,  
                  TJL< complex<double>, double>* const& );

template
std::vector< TJL< double, complex<double> >* >::iterator
std::fill_n< std::vector< TJL< double, complex<double> >* >::iterator, unsigned int, TJL< double, complex<double> >* > 
                (std::vector< TJL<double, complex<double> >* >::iterator, 
                 unsigned int,
                 TJL<double, complex<double> >* const& );
template
TJL< double, complex<double> >**
std::fill_n< TJL< double, complex<double> >**, unsigned int, TJL< double, complex<double> >* > 
                (TJL<double, complex<double> >**, 
                 unsigned int,
                 TJL<double, complex<double> > * const& );
template
TJL< complex<double>, double >**
std::fill_n< TJL< complex<double>, double >**, unsigned int, TJL< complex<double>,double >* > 
                (TJL<complex<double>, double >**, 
                 unsigned int,
                 TJL<complex<double>, double > * const& );




