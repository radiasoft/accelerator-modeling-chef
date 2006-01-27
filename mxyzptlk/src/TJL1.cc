/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJL1.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
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
******  Authors: Jean-Francois Ostiguy   ostiguy@fnal.gov                                    
******           Leo Michelotti          michelotti@fnal.gov
******
******           Fermilab, Batavia IL 60510                                           
******  
******  NOTE:
******
******  The code in this file is a specialized first order implementation 
******  of the general class TJL, meant to integrate and interoperate as 
******  transparently as possible with it. It borrows liberally from 
******  an earlier specialized first order implementation authored by 
******  Leo Michelotti.  
******
******
**************************************************************************
*************************************************************************/
#ifdef  FIRST_ORDER_JETS 

#include <TJL1.h>
#include <EnvPtr.h>
#include <GenericException.h>




//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLPtr<double>::Type TJL1<double>::sqrt() const 
{

  if( _std.value <= 0.0 ) {

    std::cout << " _std.value = " << _std.value << std::endl;

    throw( GenericException( __FILE__, __LINE__, 
           "Jet sqrt( const Jet& )",
           "Non-positive standard part.") );
  }

  JLPtr<double>::Type z( makeTJL(_myEnv) ); 

  z->_std.value = std::sqrt(_std.value );

  for( int i=0; i < _count-1; ++i ) { 
    z->_jcb[i].value = ( 1.0 / ( 2.0*z->_std.value )) * _jcb[i].value; 
  }
  return z;


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLPtr<std::complex<double> >::Type TJL1<std::complex<double> >::sqrt() const 
{

  JLPtr<std::complex<double> >::Type z( makeTJL(_myEnv) ); 

  z->_std.value = std::sqrt(_std.value );

  for( int i=0; i < _count-1; ++i ) { 
    z->_jcb[i].value = ( 1.0 / ( 2.0*z->_std.value )) * _jcb[i].value; 
  }
  return z;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJL1<double>::operator JLPtr<std::complex<double> >::Type () const {

 EnvPtr<std::complex<double> >::Type env(*this->_myEnv );                 // implicit conversion
 JLPtr<std::complex<double> >::Type z( TJL1<std::complex<double> >::makeTJL( env ) ); 

 z->_std.value = std::complex<double>( _std.value, 0.0);
 
 for( int i=0; i < _myEnv->numVar(); ++i ) { 
   z->_jcb[i].value = std::complex<double>(_jcb[i].value, 0.0 );
 } 

 return z;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJL1<std::complex<double> >::operator JLPtr<double>::Type () const {

 EnvPtr<double>::Type env( *(this->_myEnv));           // implicit conversion
 JLPtr<double>::Type x( TJL1<double>::makeTJL( env ) ); 

 x->_std.value = std::real( _std.value);
 
 for( int i=0; i < _myEnv->numVar(); ++i ) { 
   x->_jcb[i].value = std::real(_jcb[i].value);
 } 

 return x;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJL1<std::complex<double> >::operator JLPtr<std::complex<double> >::Type () const {

 // *** this operator should never get called
 
 JLPtr<std::complex<double> >::Type z; // Null ptr 
 return z;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJL1<double>::operator JLPtr<double>::Type () const {

 // *** this operator should never get called

 JLPtr<double>::Type x; // Null ptr 
 return x;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLPtr<double>::Type  real(JLPtr<std::complex<double> >::Type const& z) {

 EnvPtr<double>::Type env( *(z->_myEnv));           // implicit conversion
  JLPtr<double>::Type x( TJL1<double>::makeTJL(env));
  x->_std.value = std::real(z->_std.value);
  for (int i=0; i< x->_count-1; ++i) {
     x->_jcb[i].value = std::real(z->_jcb[i].value);
  }
  return x;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JLPtr<double>::Type    imag(JLPtr<std::complex<double> >::Type const& z){


 EnvPtr<double>::Type env( *(z->_myEnv));           // implicit conversion
  JLPtr<double>::Type x( TJL1<double>::makeTJL(env));
  x->_std.value = std::imag(z->_std.value);
  for (int i=0; i<x->_count-1 ; ++i) {
     x->_jcb[i].value = std::imag(z->_jcb[i].value);
  }
  return x;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


#endif //FIRST_ORDER_JETS 





#if 0
template<typename T>
typename JLPtr<T>::Type TJL1<T>::fabs( ) const 
{
 
 T u = _jltermStore[0]._value;
 if( u ==  T() ) 
   throw( GenericException( __FILE__, __LINE__, 
	 "Jet::fabs( const Jet& x )", 
	 "Attempt to use infinitesimal argument."));

 typename JLPtr<T>::Type z(*this); 

 if( u > 0.0 )  
     return z;
 else           
     return z->Negate();
 
}
#endif

