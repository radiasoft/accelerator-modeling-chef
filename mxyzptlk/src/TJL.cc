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
_count(x._count ),          
_weight(x._weight),     
_accuWgt(x._accuWgt),
_lowWgt(x._lowWgt),
_jltermStoreCapacity(0), 
_jltermStore(0), 
_jltermStoreCurrentPtr(0)
{

  _myEnv = x._myEnv; // implicit conversion        

  initStore( x._jltermStoreCapacity ); //  ***NOTE *** this resets the count !               

  //--------------------------------------------------------------------------
  // copy all the terms of the TJL<double> to the new TJL<complex> ...
  // --------------------------------------------------------------------------


  for ( TJLterm<double> const *p = x._jltermStore; p < x._jltermStoreCurrentPtr; ++p) { 

      new  (storePtr()) TJLterm<std::complex<double> >(*p); //implicit conversion 

  }
  
  return; 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<>
JLPtr<std::complex<double> >  TJL<std::complex<double> >::makeTJL(  TJL<double> const& x )
{

  if (_thePool.empty() ) return JLPtr<std::complex<double> >( new TJL<std::complex<double> >(x) );

  TJL<std::complex<double> >* p = _thePool.back(); _thePool.pop_back(); 
  
  if (p->_jltermStoreCapacity < x._jltermStoreCapacity)  
  { 
       TJLterm<std::complex<double> >::array_deallocate( p->_jltermStore );
       p->initStore(x._jltermStoreCapacity);   
  
  }

  p->_count    = x._count;   
  p->_weight   = x._weight;  
  p->_accuWgt  = x._accuWgt;
  p->_lowWgt   = x._lowWgt;
  p->_myEnv    = x._myEnv;  // implicit conversion
 

  //--------------------------------------------------------------------------
  // copy all the terms of the TJL<double> to the new TJL<complex> 
  // note: no direct memcopy here because of conversion  
  // --------------------------------------------------------------------------


  for ( TJLterm<double> const* r = x._jltermStore; r < x._jltermStoreCurrentPtr; ++r) { 

    new ( p->storePtr() )  TJLterm<std::complex<double> >(*r); // implicit conversion 
  }
 

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

  x->clear(); // clears **all** the terms (including the linear ones that are present by default); 

  for ( TJLterm<std::complex<double> >* p = z->_jltermStore; p < z->_jltermStoreCurrentPtr; ++p) {

     new ( x->storePtr() )  TJLterm<double>( p->_index, std::real( p->_value ), x->_myEnv );
  }
 
  // Set the maximum accurate _weight.

  x->_weight  = z->_weight;
  x->_count   = z->_count;
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
 
  x->clear(); // clears **all** the terms (including the linear ones that are present by default); 

  for ( TJLterm<std::complex<double> >* p = z->_jltermStore; p < z->_jltermStoreCurrentPtr; ++p) {

    new (x->storePtr() )  TJLterm<double>( p->_index, std::imag( p->_value ), x->_myEnv );
  }
 
  // Set the maximum accurate _weight.

  x->_weight  = z->_weight;
  x->_count   = z->_count;
  x->_accuWgt = z->_accuWgt;
  x->_lowWgt  = z->_lowWgt;
  
  return x;

}


