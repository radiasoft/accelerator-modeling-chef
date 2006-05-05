/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetEnvironment.cc
******                                                                
******  Copyright Universities Research Association, Inc. / Fermilab   
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
*****   and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******  Author: Jean-Francois Ostiguy
*****           ostiguy@fnal.gov
******  
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <complex>
#include <EnvPtr.h>
#include <TJetEnvironment.h>
#include <GenericException.h>
#include <iosetup.h>

using FNAL::pcout;
using FNAL::pcerr;


template<>
TJetEnvironment<double>::operator EnvPtr<double>::Type () const {

// This should never be called !

(*pcerr) << "ERROR:  TJetEnvironment<double>::operator TJetEnvironment<double>* () const has been called." << std::endl; 

 return EnvPtr<double>::Type(); // null pointer

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
TJetEnvironment<double>::operator EnvPtr<std::complex<double> >::Type () const {

  // NOTE: Using the TJetEnvironment copy constructor is forbidden;  

  std::complex<double>* zrefPoint = new std::complex<double>[this->_numVar ];

  for( int i = 0; i < _numVar ; i++ ) {
    zrefPoint[i]  = std::complex<double> ( _refPoint[i], 0.0 );
  }

  EnvPtr<std::complex<double> >::Type  zp(TJetEnvironment<std::complex<double> >::makeJetEnvironment(this->_maxWeight, this->_numVar,  this->_spaceDim,  zrefPoint, this->_scale));
  
  
  delete [] zrefPoint;


  return zp; 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
TJetEnvironment<std::complex<double> >::operator EnvPtr<double>::Type () const {
 
  // NOTE: Using the TJetEnvironment copy constructor is forbidden;  

  double* refPoint = new double[this->_numVar ];

  for( int i = 0; i < _numVar ; i++ ) {
    if( imag( _refPoint[i] ) != 0.0 ) {
    throw GenericException( __FILE__, __LINE__, 
      "TJetEnvironment<complex<double> >::operator TJetEnvironment<double>* ()",
      "Cannot copy complex environment with non-zero imaginary part to real one." );
    }
    refPoint[i]  = std::real( _refPoint[i] );
  }

  EnvPtr<double>::Type xp ( TJetEnvironment<double>::makeJetEnvironment(this->_maxWeight, this->_numVar,  this->_spaceDim, refPoint, this->_scale));

  delete[]  refPoint;  

  return xp;

}

