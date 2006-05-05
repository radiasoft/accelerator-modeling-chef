/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJL.cc
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             Email: michelotti@fnal.gov                         
******
******  Revision History:
******
******  Feb-May 2005  Jean-Francois Ostiguy
******                ostiguy@fnal.gov
******
******  - Efficiency improvements.
******  - New memory management scheme.
******
******  Sept-Dec 2005  ostiguy@fnal.gov
******  
****** - refactored code to usea single class template parameter
******   instead of two. Mixed mode operations now handled using 
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - all implementation details now completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for implicit instantiations, define MXYZPTLK_IMPLICIT_TEMPLATES 
******
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef  FIRST_ORDER_JETS 

#include <iosetup.h>
#include <MathConstants.h>
#include <GenericException.h>
#include <VectorD.h>
#include <TJL.h>
#include <TJetEnvironment.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TJL<double>::operator JLPtr<std::complex<double> >::Type () const {

  EnvPtr<std::complex<double> >::Type pje (*_myEnv);    // implicit conversion;
  JLPtr<std::complex<double> >::Type z( TJL<std::complex<double> >::makeTJL(pje) ); // instantiates a complex JLPtr 
 
  //---------------------------------------------------------------------------
  // If necessary, adjust the capacity of the new JLterm<complex> store to match 
  // that of the current JLterm<double> 
  // --------------------------------------------------------------------------


  if ( z.get()->_jltermStoreCapacity < _jltermStoreCapacity) { 
   
      TJLterm<std::complex<double> >::array_deallocate( z->_jltermStore );
      z->initStore( _jltermStoreCapacity);   
  
  };
  
  //--------------------------------------------------------------------------
  // copy all the terms of the current TJL<double> to the new TJL<complex> ...
  // --------------------------------------------------------------------------

  dlist_iterator getNext( _theList );

  TJLterm<double>* p                = 0;
  TJLterm<std::complex<double> >* q = 0;

  while ( p = (TJLterm<double>*) getNext() ) {
    q = new( z->storePtr() )  TJLterm<complex<double> > ( *p ); // implicit conversion operator
    z->append(q);
  }
 
  z->_accuWgt  = _accuWgt;  // accurate weight depends on previous operations, 
                            // so it must be preserved. 


  return z;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TJL<std::complex<double> >::operator JLPtr<double>::Type () const {

  EnvPtr<double>::Type pje(*_myEnv); // implicit conversion;
  JLPtr<double>::Type  x( TJL<double>::makeTJL(pje) );
 
  //---------------------------------------------------------------------------
  // If necessary, adjust the capacity of the new JLterm<double> store to match 
  // that of the current JLterm<complex> 
  // --------------------------------------------------------------------------

  if (x->_jltermStoreCapacity < _jltermStoreCapacity) { 
   
      TJLterm<double>::array_deallocate( x->_jltermStore );
      x->initStore(_jltermStoreCapacity);   
  
  };

 
  //--------------------------------------------------------------------------
  // copy all the terms of the current TJL<complex> to the new TJL<double> ...
  // --------------------------------------------------------------------------

  dlist_iterator getNext( _theList );

  TJLterm<std::complex<double> >* p = 0; 
  TJLterm<double>*                q = 0;

  while ( p =  (TJLterm<std::complex<double> >*) getNext() ) {
    q    = new ( x->storePtr() )  TJLterm<double> ( *p ); // conversion operator
    x->append(q);
  }
 
  x->_accuWgt  = _accuWgt; // max accurate weight depends on previous operations, 
                           // so it must be preserved
  return x; 

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TJL<double>::operator boost::intrusive_ptr<TJL<double> >() const {

  // this should never be called 
  throw GenericException( __FILE__, __LINE__, "TJL<double>::operator boost::intrusive_ptr<TJL<double> >()", 
                         "This conversion operator should never be called." );
}



// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TJL<std::complex<double> >::operator boost::intrusive_ptr<TJL<std::complex<double> > >() const {

  // this should never be called 
  throw GenericException( __FILE__, __LINE__, "TJL<double>::operator boost::intrusive_ptr<TJL<double> >()", 
                         "This conversion operator should never be called." );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JLPtr<double>::Type real( const JLPtr<std::complex<double> >::Type & z ) 
{

  EnvPtr<double>::Type pje( *z->_myEnv );            // implicit conversion 

  // If the argument is void, then return a copy ...
  if( z->_count < 1 ) {
     return JLPtr<double>::Type(  TJL<double>::makeTJL(pje) );
  }

  JLPtr<double>::Type  x( TJL<double>::makeTJL( pje ) );

  // Proceed ...
 
  TJLterm<std::complex<double> >*  p;

  dlist_iterator gz( z->_theList );
   
  while((  p = (TJLterm<complex<double> >*) gz()  )) {
    x->addTerm( TJLterm<double>( p->_index, std::real( p->_value ), x->_myEnv ));
  }
 
  // Set the maximum accurate _weight.
  x->_accuWgt = z->_accuWgt;
  
  return x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLPtr<double>::Type imag( const JLPtr<std::complex<double> >::Type& z ) 
{

  EnvPtr<double>::Type pje( *z->_myEnv );        // implicit conversion 

  // If the argument is void, then return ...
  if( z->_count < 1 ) {
     return JLPtr<double>::Type(  TJL<double>::makeTJL( pje) );
  }

  JLPtr<double>::Type  x( TJL<double>::makeTJL(pje) );

  // Proceed ...
 
  TJLterm<complex<double> >*  p;
  dlist_iterator gz( z->_theList );
   
  while((  p = (TJLterm<complex<double> >*) gz()  )) {
    x->addTerm( TJLterm<double>( p->_index, std::imag( p->_value ), x->_myEnv ) );
  }
 
  // Set the maximum accurate _weight.
  x->_accuWgt = z->_accuWgt;
  
  return x;

}

#endif // FIRST_ORDER_JETS 

