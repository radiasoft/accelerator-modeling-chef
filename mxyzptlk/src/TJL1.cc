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

#include <TJL1.h>
#include <EnvPtr.h>
#include <GenericException.h>


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<>
TJL1<std::complex<double> >::TJL1( TJL1<double> const& x ):
 _count(x._count),       
 _weight(x._weight),     
 _accuWgt(x._accuWgt),
 _myEnv(x._myEnv) // implicit conversion

{

    _std.value = std::complex<double>(x._std.value, 0.0);
     _jcb = new term [ _count-1 ];

     for (int i=0; i<_count-1; ++i) 
       _jcb[i].value =  std::complex<double>(x._jcb[i].value, 0.0);



 if ( !_myEnv ) {
 throw( GenericException( __FILE__, __LINE__, 
          "TJL1<std::complex<double> >::TJL1( TJL1<double> const& x)" ,
          "Null Environment." ) );
 };



}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<>
JL1Ptr<std::complex<double> > TJL1<std::complex<double> >::makeTJL( const TJL1<double>& x )
{
 
  if (_thePool.empty() ) 
     return new TJL1<std::complex<double> >(x);
 
  TJL1<std::complex<double> >* p = _thePool.back(); _thePool.pop_back(); 
  
  if ( p->_count  != x._myEnv->numVar()+1) {
      delete [] p->_jcb; 
      p->_jcb   = new term[ x._myEnv->numVar() ];
      p->_count =  x._myEnv->numVar()+1;
  }
 
  p->_weight   = x._weight;  
  p->_accuWgt  = x._accuWgt;
  p->_myEnv    = x._myEnv;
 
  p->_std.value      = std::complex<double>(x._std.value, 0.0);

  for (int i=0; i < (p->_count-1); ++i) 
       p->_jcb[i].value = std::complex<double>( x._jcb[i].value, 0.0);


 if ( !p->_myEnv ) {
 throw( GenericException( __FILE__, __LINE__, 
          "TJL1<std::complex<double> >::makeTJL( const TJL1<double>& x )",
          "Null Environment." ) );
 };


 return JL1Ptr<std::complex<double> >(p);

}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
JL1Ptr<double> TJL1<double>::sqrt() const 
{

  if( _std.value <= 0.0 ) {

    throw( GenericException( __FILE__, __LINE__, 
           "Jet sqrt( const Jet& )",
           "Non-positive standard part.") );
  }

  JL1Ptr<double> z( makeTJL(_myEnv) ); 

  z->_std.value = std::sqrt(_std.value );

  for( int i=0; i < _count-1; ++i ) { 
    z->_jcb[i].value = ( 1.0 / ( 2.0*z->_std.value )) * _jcb[i].value; 
  }
  return z;


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
JL1Ptr<std::complex<double> > TJL1<std::complex<double> >::sqrt() const 
{

  JL1Ptr<std::complex<double> > z( makeTJL(_myEnv) ); 

  z->_std.value = std::sqrt(_std.value );

  for( int i=0; i < _count-1; ++i ) { 
    z->_jcb[i].value = ( 1.0 / ( 2.0*z->_std.value )) * _jcb[i].value; 
  }
  return z;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JL1Ptr<double>  real(JL1Ptr<std::complex<double> > const& z) {

  // ---------------------------------------------------------------------------------------------
  // *********************************** NOTE ***************************************************: 
  // Taking the real part of a Jet is only possible if the environment's reference point
  // DOES NOT HAVE imaginary components 
  // ---------------------------------------------------------------------------------------------- 

  EnvPtr<double> env = TJetEnvironment<std::complex<double> >::makeRealJetEnvironment(z->_myEnv);            


  JL1Ptr<double> x( TJL1<double>::makeTJL(env));

  x->_std.value = std::real(z->_std.value);
  for (int i=0; i< x->_count-1; ++i) {
     x->_jcb[i].value = std::real(z->_jcb[i].value);
  }

 if ( !x->_myEnv ) {
 throw( GenericException( __FILE__, __LINE__, 
          "JL1Ptr<double>  real(JL1Ptr<std::complex<double> > const& z)",
          "Null Environment." ) );
 };


  return x;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JL1Ptr<double>    imag(JL1Ptr<std::complex<double> > const& z){


 // ---------------------------------------------------------------------------------------------
 // *********************************** NOTE ***************************************************: 
 // Taking the imaginary part of a Jet is only possible if the environment's reference point
 // DOES NOT HAVE imaginary components 
 // ---------------------------------------------------------------------------------------------- 

  EnvPtr<double> env = TJetEnvironment<std::complex<double> >::makeRealJetEnvironment(z->_myEnv);            

  JL1Ptr<double> x( TJL1<double>::makeTJL(env));

  x->_std.value = std::imag(z->_std.value);
  for (int i=0; i<x->_count-1 ; ++i) {
     x->_jcb[i].value = std::imag(z->_jcb[i].value);
  }

  if ( !x->_myEnv ) {
  throw( GenericException( __FILE__, __LINE__, 
          "JL1Ptr<double>  image(JL1Ptr<std::complex<double> > const& z)",
          "Null Environment." ) );
  };

  return x;


}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


#if 0
template<typename T>
typename JL1Ptr<T> TJL1<T>::fabs( ) const 
{
 
 T u = _jltermStore[0]._value;
 if( u ==  T() ) 
   throw( GenericException( __FILE__, __LINE__, 
	 "Jet::fabs( const Jet& x )", 
	 "Attempt to use infinitesimal argument."));

 typename JL1Ptr<T> z(*this); 

 if( u > 0.0 )  
     return z;
 else           
     return z->Negate();
 
}
#endif

