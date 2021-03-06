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
// If implicit template instantiations are desired, *do not* compile this file !
// =================================================================================

#ifdef MXYZPTLK_EXPLICIT_TEMPLATES 

#include <complex>
#include <basic_toolkit/ReferenceCounter.h>
#include <mxyzptlk/TJLterm.h>
#include <mxyzptlk/TJet.h>
#include <mxyzptlk/TJet.tcc>  
#include <mxyzptlk/TJetEnvironment.h>
#include <mxyzptlk/TJetVector.h>
#include <mxyzptlk/TJetVector.tcc>
#include <mxyzptlk/TMapping.h>
#include <mxyzptlk/TMapping.tcc>
#include <mxyzptlk/TLieOperator.h>
#include <mxyzptlk/TLieOperator.tcc>

#include <gms/FastPODAllocator.h>
#include <gms/FastAllocator.h>
#include <boost/functional/hash/hash.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>
#include <istream>

// ============================================================================================
// The standard classes
//=============================================================================================

template class TJet<double>;
template class TJet<std::complex<double> >;

template TJet<std::complex<double> >::TJet( const TJet<double>& );


template class TJetVector<double>;
template class TJetVector<std::complex<double> >;

template class TMapping<double>;
template class TMapping<std::complex<double> >;

template 
TMapping<double>::TMapping( std::vector<TJet<double> >::iterator, 
			    std::vector<TJet<double> >::iterator, EnvPtr<double> const&);


template class TLieOperator<double>;
template class TLieOperator<std::complex<double> >;
template TJet<double>                               TLieOperator<double>::expMap(               double const&,                TJet<double> const & );
template TJet<std::complex<double> > TLieOperator<std::complex<double> >::expMap( std::complex<double> const&, TJet<std::complex<double> > const & );

template TJet<double>                               TLieOperator<double>::expMap( TJet<double> const&,                               TJet<double> const & );
template TJet<std::complex<double> > TLieOperator<std::complex<double> >::expMap( TJet<std::complex<double> > const&, TJet<std::complex<double> > const & );

template TJetVector<double>                               TLieOperator<double>::expMap(               double const&,                TJetVector<double> const & );
template TJetVector<std::complex<double> > TLieOperator<std::complex<double> >::expMap( std::complex<double> const&, TJetVector<std::complex<double> > const & );

template TJetVector<double>                               TLieOperator<double>::expMap( TJet<double> const&,                               TJetVector<double> const & );
template TJetVector<std::complex<double> > TLieOperator<std::complex<double> >::expMap( TJet<std::complex<double> > const&, TJetVector<std::complex<double> > const & );

template class Tcoord<double>;
template class Tcoord<std::complex<double> >;

template class Tparam<double>;
template class Tparam<std::complex<double> >;

// ============================================================================================
// Friend functions
//=============================================================================================
//  ****** Class TJet *******

//template
//JLPtr<double> makeJL<double>(IntArray const&, double const&, EnvPtr<double> const&);

//template
//JLPtr<std::complex<double> > makeJL<std::complex<double> >(IntArray const&, std::complex<double> const&, EnvPtr<std::complex<double> > const&);

//template
//JLPtr<double> makeJL<double>(EnvPtr<double> const&, double);

//template
//JLPtr<std::complex<double> > makeJL<std::complex<double> >( EnvPtr<std::complex<double> > const&, std::complex<double>);

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


// **** Math Functions

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
TJet<double > acos(TJet<double > const&);

template 
TJet<std::complex<double> > acos(TJet<std::complex<double> > const&);

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
TJet<double > tan(TJet<double > const&);

template 
TJet<std::complex<double> > tan(TJet<std::complex<double> > const&);

template 
TJet<double > pow(TJet<double > const&, int);

template 
TJet<std::complex<double> > pow(TJet<std::complex<double> > const&, int);

template 
bool operator!=<double >(TJet<double > const&, double const&);

template 
TJet<std::complex<double> > operator/<std::complex<double> >(TJet<std::complex<double> > const&, TJet<std::complex<double> > const&);

// Lie operators. *Note that the complex form is not instantiated*.

template 
TJetVector<double > operator^<double >(Vector const&, TJetVector<double > const&);

template 
TJet<double> operator^( TJet<double> const&, TJet<double> const& );

template
double sobolev_norm<double>(TJet<double> const&);

template
double sobolev_norm<std::complex<double> >(TJet<std::complex<double> > const&);


// ** stream operators
// ----------------------
template 
std::istream& operator>>(std::istream&, TJetVector<double >&);

template
std::ostream& operator<< <double >(std::ostream&, TJetVector<double > const&);

template
std::istream& operator>>(std::istream&, TJet<double >&);

//=======================================================================================================
// BOOST & STL
//=======================================================================================================

template
class std::vector<TJet<double> >;

template
class std::vector<TJet<std::complex<double> > >;

static std::vector<TJet<double> >::iterator                  dummy_iter_1;

static std::vector<TJet<std::complex<double> > >::iterator   dummy_iter_2;

static std::vector<TJet<double> >::const_iterator            dummy_iter_3;

std::vector<TJet<std::complex<double> > >::const_iterator    dummy_iter_4;


template
void std::fill( vector<TJet<double> >::iterator, vector<TJet<double> >::iterator, TJet<double> const&);

template
void std::fill( vector<TJet<std::complex<double> > >::iterator, vector<TJet<std::complex<double> > >::iterator,  TJet<complex<double> > const&);

template
vector<TJet<double> >::iterator std::fill_n(vector<TJet<double> >::iterator, unsigned int, TJet<double> const&);

template
vector<TJet<std::complex<double> > >::iterator std::fill_n(vector<TJet<std::complex<double> > >::iterator, unsigned int, TJet<std::complex<double> > const&);

template
void std::fill<Tcoord<double >**, Tcoord<double >*>(Tcoord<double >**, Tcoord<double >**, Tcoord<double >* const&);

template
void std::fill<Tcoord<std::complex<double> >**, Tcoord<std::complex<double> >*>(Tcoord<std::complex<double> >**, Tcoord<std::complex<double> >**, Tcoord<std::complex<double> >* const&);

template
void std::fill<Tparam<double >**, Tparam<double >*>(Tparam<double >**, Tparam<double >**, Tparam<double >* const&);

template
void std::fill<Tparam<std::complex<double> >**, Tparam<std::complex<double> >*>(Tparam<std::complex<double> >**, Tparam<std::complex<double> >**, Tparam<std::complex<double> >* const&);

template
class std::deque<Tcoord<double>* >;

template
class std::deque<Tcoord<std::complex<double> >* >;

template
class std::deque<Tparam<double>* >;

template
class std::deque<Tparam<std::complex<double> >* >;


#endif // MXYZPTLK_EXPLICIT_TEMPLATES 



