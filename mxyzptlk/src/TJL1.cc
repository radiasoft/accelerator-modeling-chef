#include <TJL1.h>
#include <EnvPtr.h>
#include <GenericException.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JL1Ptr<double>::Type TJL1<double>::sqrt() const 
{

  if( _std <= 0.0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "Jet sqrt( const Jet& )",
           "Non-positive standard part xxxx.") );
  }

  JL1Ptr<double>::Type z( makeTJL(_myEnv) ); 

  z->_std = std::sqrt(_std );

  for( int i=0; i < _myEnv->numVar(); ++i ) { 
    z->_jcb[i] = ( 1.0 / ( 2.0*z->_std ) * _jcb[i] ); 
  }
  return z;


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJL1<double>::operator JL1Ptr<std::complex<double> >::Type () const {

 EnvPtr<std::complex<double> >::Type env(*this->_myEnv );                 // implicit conversion
 JL1Ptr<std::complex<double> >::Type z( TJL1<std::complex<double> >::makeTJL( env ) ); 

 z->_std = std::complex<double>( _std, 0.0);
 
 for( int i=0; i < _myEnv->numVar(); ++i ) { 
   z->_jcb[i] = std::complex<double>(_jcb[i], 0.0 );
 } 

 return z;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJL1<std::complex<double> >::operator JL1Ptr<double>::Type () const {

 EnvPtr<double>::Type env( *(this->_myEnv));           // implicit conversion
 JL1Ptr<double>::Type x( TJL1<double>::makeTJL( env ) ); 

 x->_std = std::real( _std);
 
 for( int i=0; i < _myEnv->numVar(); ++i ) { 
   x->_jcb[i] = std::real(_jcb[i]);
 } 

 return x;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJL1<std::complex<double> >::operator JL1Ptr<std::complex<double> >::Type () const {

 // *** this operator should never get called
 
 JL1Ptr<std::complex<double> >::Type z; // Null ptr 
 return z;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJL1<double>::operator JL1Ptr<double>::Type () const {

 // *** this operator should never get called

 JL1Ptr<double>::Type x; // Null ptr 
 return x;
}




#if 0
template<typename T>
typename JL1Ptr<T>::Type TJL1<T>::fabs( ) const 
{
 
 T u = _jltermStore[0]._value;
 if( u ==  T() ) 
   throw( GenericException( __FILE__, __LINE__, 
	 "Jet::fabs( const Jet& x )", 
	 "Attempt to use infinitesimal argument."));

 typename JL1Ptr<T>::Type z(*this); 

 if( u > 0.0 )  
     return z;
 else           
     return z->Negate();
 
}
#endif

