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
**************************************************************************
*************************************************************************/

#include <TVector.h>


#define CHECKOUT(type, test, fcn, message)                       \
  if( test ) {                                           \
    throw( TVector<type >::GenericException( fcn, message ) ); \
  }



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TVector<double>::operator* ( const TVector<double>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(double, _dim != x._dim, "TVector<double>::operator*", "dimensions incompatible.")
#endif

  double u = 0.0;

  for ( int i=0; i < _dim; ++i)  u += _comp[i] * x._comp[i];

  return u;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
std::complex<double> TVector<std::complex<double> >::operator* ( const TVector<std::complex<double> >& x ) const {
#ifndef NOCHECKS
  CHECKOUT(std::complex<double>, _dim != x._dim, "TVector<double>::operator*", "dimensions incompatible.")
#endif

  std::complex<double> u = 0.0;

  for ( int i=0; i < _dim; ++i)  
      u += _comp[i] * (std::real(x._comp[i]) - std::complex<double>(0.0,1.0) * std::imag(x._comp[i])) ;

  return u;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
bool TVector<double>::operator> ( const TVector<double>& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(double, _dim != x._dim, "TVector<T>::operator>", "Dimensions incompatible.")
#endif
  for( int i=0; i < _dim; ++i) if( _comp[i] <=  x._comp[i] ) return false;
  return true;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TVector<double>::Norm () const
{
  double x = 0.0;
  for ( int i = 0; i < _dim; ++i) x += _comp[i]*_comp[i];
  return sqrt(x);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TVector<std::complex<double> >::Norm () const
{
  double x = 0.0;
  for ( int i = 0; i < _dim; ++i) x += ( std::real(_comp[i])*std::real(_comp[i]) ) + ( std::imag(_comp[i])*std::imag(_comp[i]) ); 
  return sqrt(x);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
bool TVector<double>::IsUnit() const
{
  double x = 0.0;
  for( int i = 0; i < _dim; ++i ) x += _comp[i]*_comp[i];
  return ( fabs(x - 1.0) < 1.0e-10 );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
bool TVector<std::complex<double> >::IsUnit() const
{
  double x = 0.0;
  for( int i = 0; i < _dim; ++i ) x += (std::real(_comp[i])*std::real(_comp[i]) +  std::imag(_comp[i])*std::imag(_comp[i]) );
  return ( fabs(x - 1.0) < 1.0e-10 );
}

