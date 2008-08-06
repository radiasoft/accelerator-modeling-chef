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
******  Oct 2006 ostiguy@fnal.gov
******  - new implementation based on std::vector<> 
******  Aug 2008 ostiguy@fnal.gov
******  - added real() and imag()
******  
******                                                                
**************************************************************************
*************************************************************************/

#include <basic_toolkit/TVector.h>
#include <basic_toolkit/GenericException.h>
#include <boost/function.hpp>
#include <complex>
#include <numeric>
#include <functional>
#include <complex>


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TVector<double>::operator* ( TVector <double> const& x ) const
{
  return std::inner_product(theVector_.begin(), theVector_.end(), x.theVector_.begin(), 0.0);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<> 
std::complex<double> 
TVector<std::complex<double> >::operator* ( TVector<std::complex<double> > const& x ) const 
{
  return std::real( std::inner_product( theVector_.begin(),  theVector_.end(), 
                                      x.theVector_.begin(),  std::complex<double>(), 
                                      std::plus<std::complex<double> >(),
				      op_mult() )); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TVector<double>::Norm () const
{
  return std::sqrt( (*this) * (*this) );  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TVector<std::complex<double> >::Norm () const
{
  return std::sqrt( std::real( (*this) * (*this) ) );  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

static double Real( std::complex<double> const& value) { return value.real(); }
static double Imag( std::complex<double> const& value) { return value.imag(); }

TVector<double> real( TVector<std::complex<double> > const& v)
{
  TVector<double> result( v.Dim() );
  boost::function< double( std::complex<double> const& ) > fReal = &Real;
  std::transform( v.begin(), v.end(),  result.begin(),  fReal);
  return result;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TVector<double> imag( TVector<std::complex<double> > const& v)
{
  TVector<double> result( v.Dim() );
  boost::function< double( std::complex<double> const& ) > fImag = &Imag;
  std::transform( v.begin(), v.end(),  result.begin(), fImag );
  return result; 
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
