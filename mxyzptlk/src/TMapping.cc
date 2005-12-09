/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TMapping.cc
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
******             Fermilab                                           
******             Email: michelotti@fnal.gov                         
******
******  Revision History
******   
******  Feb 2005       Jean-Francois Ostiguy
******                 ostiguy@fnal.gov 
****** 
****** - new memory management scheme 
******                                                            
******  Sep-Dec 2005  ostiguy@fnal.gov
******  
****** - refactored code to usea single class template parameter
******   instead of two. Mixed mode operations now handled using 
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - implementation details completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for implicit instantiations, define MXYZPTLK_IMPLICIT_TEMPLATES
******
**************************************************************************
*************************************************************************/

#include <GenericException.h>
#include <iosetup.h>
#include <TMapping.h>
#include <complex>

using FNAL::pcerr;
using FNAL::pcout;

using std::complex;


Vector TMapping<double>::operator()( const Vector& x ) const
{
 int i = x.Dim();
 if( ( i != _myEnv->numVar() ) || ( i != _dim ) ) {
   throw( GenericException(__FILE__, __LINE__, 
          "Vector TMapping<double>::operator()( const Vector& ) const",
          "Incompatible dimensions." ) );
 }

 Vector z( _dim );

 for( i = 0; i < _myEnv->spaceDim(); i++) {
  z(i) = _comp[i]( x );
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector TMapping<complex<double> >::operator()( const Vector& x ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "Vector TMapping<complex<double> >::operator()( const Vector& x ) const", 
         "This specialization is meaningless. It should not be invoked." ) );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMapping<double>::operator TMapping<std::complex<double> > () const {

  TMapping<std::complex<double> > z;

  z._dim   = _dim;
  z._myEnv = *_myEnv; // implicit conversion operator

  for (int i =0; i< _dim; ++i ) {

    z._comp[i] = TJet<complex<double> >( _comp[i] ); // the environment is converted by the conversion operator
    
  }

  return z;

}

