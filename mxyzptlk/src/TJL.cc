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
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES 
******
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/TJL.h>
#include <mxyzptlk/TJetEnvironment.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


template<>
template<>
TJL<std::complex<double> >::TJL( TJL<double> const& x):
_count(0),                     
_weight(x._weight),     
_accuWgt(x._accuWgt),
_lowWgt(x._lowWgt),
_jltermStoreCapacity(0), 
_jltermStore(0), 
_jltermStoreCurrentPtr(0)
{

  this->_myEnv = x._myEnv;        

  initStore( x._jltermStoreCapacity );

  //--------------------------------------------------------------------------
  // copy all the terms of the TJL<double> to the new TJL<complex> ...
  // --------------------------------------------------------------------------

  TJLterm<std::complex<double> >* q = 0;
  for ( TJLterm<double> const *p = x._jltermStore; p < x._jltermStoreCurrentPtr; ++p) { 

    append( TJLterm<complex<double> >( *p ) ); //implicit conversion 

  }
 
   _accuWgt  = x._accuWgt;  // accurate weight depends on previous operations, 
                             // so it must be preserved. 

   return; 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<>
JLPtr<std::complex<double> >  TJL<std::complex<double> >::makeTJL(  TJL<double> const& x )
{

  if (_thePool.empty() ) return new TJL<std::complex<double> >(x);

  TJL<std::complex<double> >* p = _thePool.back(); _thePool.pop_back(); 
  
  if (p->_jltermStoreCapacity < x._jltermStoreCapacity)  
  { 
       TJLterm<std::complex<double> >::array_deallocate( p->_jltermStore );
       p->initStore(x._jltermStoreCapacity);   
  
  }

  p->_count    = 0;          // needed by append function
  p->_weight   = x._weight;  // needed by append function
  p->_accuWgt  = x._accuWgt;
  p->_accuWgt  = x._lowWgt;
  p->_myEnv    = x._myEnv;  // implicit conversion
 

  //--------------------------------------------------------------------------
  // copy all the terms of the TJL<double> to the new TJL<complex> 
  // note: no direct memcopy here because of conversion  
  // --------------------------------------------------------------------------


  TJLterm<std::complex<double> >* q = 0;

  for ( TJLterm<double> const* r = x._jltermStore; r < x._jltermStoreCurrentPtr; ++r) { 
    p->append(TJLterm<complex<double> >(*r) );
  }
 
  p->_accuWgt  = x._accuWgt;  // accurate weight depends on previous operations, 
                             // so it must be preserved. 


 return JLPtr<std::complex<double> >(p);

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


JLPtr<double> real( JLPtr<std::complex<double> > const& z ) 
{

  // ---------------------------------------------------------------------------------------------
  // *********************************** NOTE ***************************************************: 
  // Taking the real part of a Jet is only possible if the environment's reference point
  // DOES NOT HAVE imaginary components 
  // ---------------------------------------------------------------------------------------------- 

  EnvPtr<double> pje = TJetEnvironment<std::complex<double> >::makeRealJetEnvironment(EnvPtr<std::complex<double> >(z->_myEnv) );            

  JLPtr<double>  x( TJL<double>::makeTJL( pje ) );

  x->setStandardPart( std::real( z->standardPart() ));

  for ( TJLterm<std::complex<double> >* p = z->_jltermStore+1; p < z->_jltermStoreCurrentPtr; ++p) {

    x->append( TJLterm<double>( p->_index, std::real( p->_value ), x->_myEnv ));
  }
 
  // Set the maximum accurate _weight.
  x->_accuWgt = z->_accuWgt;
  x->_lowWgt  = z->_lowWgt;

  
  return x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLPtr<double> imag( const JLPtr<std::complex<double> >& z ) 
{

  // ---------------------------------------------------------------------------------------------
  // *********************************** NOTE ***************************************************: 
  // Taking the imaginary part of a Jet is only possible if the environment's reference point
  // DOES NOT HAVE imaginary components 
  // ---------------------------------------------------------------------------------------------- 
  
  EnvPtr<double> pje = TJetEnvironment<std::complex<double> >::makeRealJetEnvironment( z->_myEnv);       

  JLPtr<double>  x( TJL<double>::makeTJL(pje) );

  // Proceed ...
 
  x->setStandardPart( std::imag( z->standardPart() ));

  for ( TJLterm<std::complex<double> >* p = z->_jltermStore+1; p < z->_jltermStoreCurrentPtr; ++p) {

    x->append( TJLterm<double>( p->_index, std::imag( p->_value ), x->_myEnv ) );
  }
 
  // Set the maximum accurate _weight.

  x->_accuWgt = z->_accuWgt;
  x->_lowWgt  = z->_lowWgt;
  
  return x;

}


