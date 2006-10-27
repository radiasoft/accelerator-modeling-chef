/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TVector.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******
******  Revision History:
******
******  Dec 2005   Jean-Francois Ostiguy
******             ostiguy@fnal.gov
******
******  - templated version
****** 
******  October 2006
******   
******  - new implementation based on std::vector<> 
******                                                              
**************************************************************************
*************************************************************************/

#include <basic_toolkit/TVector.h>
#include <numeric>
#include <functional>
#include <complex>

#ifdef CHECKOUT
#undef CHECKOUT
#endif

#define CHECKOUT(type, test, fcn, message)                       \
  if( test ) {                                           \
    throw( TVector<type >::GenericException( fcn, message ) ); \
  }



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TVector<double>::operator* ( TVector <double> const& x ) const
{

#ifndef NOCHECKS
  CHECKOUT(double, Dim() != x.Dim(), "TVector<double>::operator*", "dimensions incompatible.")
#endif

  return std::inner_product(m_theVector.begin(), m_theVector.end(), x.m_theVector.begin(), 0.0);

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<> 
std::complex<double> 
TVector<std::complex<double> >::operator* ( TVector<std::complex<double> > const& x ) const {

#ifndef NOCHECKS
  CHECKOUT(std::complex<double>, Dim() != x.Dim(), "TVector<double>::operator*", "dimensions incompatible.")
#endif

   

  return std::real( std::inner_product( m_theVector.begin(),  m_theVector.end(), 
                                      x.m_theVector.begin(),  std::complex<double>(), 
                                      std::plus<std::complex<double> >(),
				      op_mult() )); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
template<>
bool 
TVector<double>::operator > ( TVector<double> const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(double, Dim() != x.Dim(), "TVector<T>::operator>", "Dimensions incompatible.")
#endif

  for( int i=0; i < Dim(); ++i) if( m_theVector[i] <=  x.m_theVector[i] ) return false;
  return true;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TVector<double>::Norm () const
{
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TVector<std::complex<double> >::Norm () const
{

  return std::sqrt( std::real( (*this) * (*this) ) );  

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
bool TVector<double>::IsUnit() const
{
  return (std::abs( Norm() - 1.0 ) < 1.0e-10 );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
bool TVector<std::complex<double> >::IsUnit() const
{
  return (std::abs( Norm() - 1.0 ) < 1.0e-10 );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

