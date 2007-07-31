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
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES
******
**************************************************************************
*************************************************************************/

#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/iosetup.h>
#include <mxyzptlk/TMapping.h>
#include <complex>

using FNAL::pcerr;
using FNAL::pcout;

using std::complex;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
template<>
TMapping<std::complex<double> >::TMapping( TMapping<double> const& x)
: TJetVector<std::complex<double> >( x ) {}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
Vector TMapping<double>::operator()( Vector const& x ) const
{
 int i = x.Dim();
 if( ( i != myEnv_->numVar() ) || ( i != comp_.size() ) ) {
   throw( GenericException(__FILE__, __LINE__, 
          "Vector TMapping<double>::operator()( const Vector& ) const",
          "Incompatible dimensions." ) );
 }

 Vector z( comp_.size() );

 for( i=0;  i <myEnv_->spaceDim(); ++i) {
  z(i) = comp_[i]( x );
 }

 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
Vector TMapping<double>::operator*( Vector const& x ) const
{
 return this->operator()(x);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||





