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

#include <mxyzptlk/TJL1.h>
#include <mxyzptlk/EnvPtr.h>
#include <basic_toolkit/GenericException.h>


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<>
TJL1<std::complex<double> >::TJL1( TJL1<double> const& x ):
 count_(x.count_),       
 weight_(x.weight_),     
 accuWgt_(x.accuWgt_),
 myEnv_(x.myEnv_) // implicit conversion

{

    terms_[0] = std::complex<double>(x.terms_[0], 0.0);
     jcb_ = new std::complex<double> [ count_-1 ];

     for (int i=0; i<count_-1; ++i) 
       jcb_[i] =  std::complex<double>(x.jcb_[i], 0.0);



 if ( !myEnv_ ) {
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
 
  if (thePool_.empty() ) 
     return JL1Ptr<std::complex<double> >(new TJL1<std::complex<double> >(x) );
 
  TJL1<std::complex<double> >* p = thePool_.back(); thePool_.pop_back(); 
  
  if ( p->count_  != x.myEnv_->numVar()+1) {
      delete [] p->jcb_; 
      p->jcb_   = new std::complex<double>[ x.myEnv_->numVar() ];
      p->count_ =  x.myEnv_->numVar()+1;
  }
 
  p->weight_   = x.weight_;  
  p->accuWgt_  = x.accuWgt_;
  p->myEnv_    = x.myEnv_;
 
  p->terms_[0]      = std::complex<double>(x.terms_[0], 0.0);

  for (int i=0; i < (p->count_-1); ++i) 
       p->jcb_[i] = std::complex<double>( x.jcb_[i], 0.0);


 if ( !p->myEnv_ ) {
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

  if( terms_[0] <= 0.0 ) {

    throw( GenericException( __FILE__, __LINE__, 
           "Jet sqrt( const Jet& )",
           "Non-positive standard part.") );
  }

  JL1Ptr<double> z( makeTJL(myEnv_) ); 

  z->terms_[0] = std::sqrt(terms_[0] );

  for( int i=0; i < count_-1; ++i ) { 
    z->jcb_[i] = ( 1.0 / ( 2.0*z->terms_[0] )) * jcb_[i]; 
  }
  return z;


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
JL1Ptr<std::complex<double> > TJL1<std::complex<double> >::sqrt() const 
{

  JL1Ptr<std::complex<double> > z( makeTJL(myEnv_) ); 

  z->terms_[0] = std::sqrt(terms_[0] );

  for( int i=0; i < count_-1; ++i ) { 
    z->jcb_[i] = ( 1.0 / ( 2.0*z->terms_[0] )) * jcb_[i]; 
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

  EnvPtr<double> env = TJetEnvironment<std::complex<double> >::makeRealJetEnvironment(z->myEnv_);            


  JL1Ptr<double> x( TJL1<double>::makeTJL(env));

  x->terms_[0] = std::real(z->terms_[0]);
  for (int i=0; i< x->count_-1; ++i) {
     x->jcb_[i] = std::real(z->jcb_[i]);
  }

 if ( !x->myEnv_ ) {
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

  EnvPtr<double> env = TJetEnvironment<std::complex<double> >::makeRealJetEnvironment(z->myEnv_);            

  JL1Ptr<double> x( TJL1<double>::makeTJL(env));

  x->terms_[0] = std::imag(z->terms_[0]);
  for (int i=0; i<x->count_-1 ; ++i) {
     x->jcb_[i] = std::imag(z->jcb_[i]);
  }

  if ( !x->myEnv_ ) {
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

