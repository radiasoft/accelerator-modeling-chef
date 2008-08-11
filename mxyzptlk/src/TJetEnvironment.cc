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
******          ostiguy@fnal.gov
******
****** - Introduced new compact monomial indexing scheme based 
******   on monomial ordering to replace previous scheme based 
******   explicitly on monomial exponents tuples.
****** - monomial multiplication handled via a lookup-table.
******
**************************************************************************
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <complex>
#include <limits>
#include <mxyzptlk/EnvPtr.h>
#include <mxyzptlk/TJetEnvironment.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/iosetup.h>
#include <mxyzptlk/TJet.h>

using FNAL::pcout;
using FNAL::pcerr;

void createStandardEnvironments( int deg )
{
   EnvPtr<double> env  = 
   TJetEnvironment<double>::makeJetEnvironment(deg, 6, 6);

   EnvPtr<std::complex<double> > envc = 
   TJetEnvironment<std::complex<double> >::makeJetEnvironment(deg, 6, 6);  

   TJetEnvironment<double>::pushEnv( env );
   TJetEnvironment<std::complex<double> >::pushEnv( envc );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<>
EnvPtr<std::complex<double> >  TJetEnvironment<std::complex<double> >::makeJetEnvironment( EnvPtr<double> const& env) {

 if  ( *TJetEnvironment<std::complex<double> >::topEnv() ==  *env ) { 
  return TJetEnvironment<std::complex<double> >::topEnv();
 }

 TVector<std::complex<double> >  tmp_refpoints(env->numVar() ); 
 TVector<std::complex<double> >::iterator itc  =  tmp_refpoints.begin();

 for ( TVector<double>::const_iterator it  = env->refPoint().begin(); 
	                               it != env->refPoint().end(); ++it,++itc  ) { 
   *itc = std::complex<double>(*it, 0.0) ;
 }

 TJetEnvironment<std::complex<double> >* pje 
   = new TJetEnvironment<std::complex<double> >( env->maxWeight(), env->numVar(), env->spaceDim(), 
                                                                                  tmp_refpoints );
  

 for(  std::list<EnvPtr<std::complex<double> > >::iterator it  =  TJetEnvironment<std::complex<double> >::environments_.begin(); 
                                                           it !=  TJetEnvironment<std::complex<double> >::environments_.end(); 
                                                          ++it ) { 
 
    if ( **it == *pje ) return *it; 
 }

 //-------------------------------------------------------------------------------------------------- 
 // NOTE: The reference count will go to 0 when the only instance of the smart ptr is the one left in 
 //       the environment list_. When that happens, the custom deleter (dispose()) removes the env from the list.   
 //-------------------------------------------------------------------------------------------------------------- 

 EnvPtr<std::complex<double> > newpje(pje);
 TJetEnvironment<std::complex<double> >::environments_.push_back( newpje );
 newpje->release(); // do not count the instance that is in the environments_ list.
 return newpje;
 
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
EnvPtr<double> TJetEnvironment<double>::makeJetEnvironment(int maxweight, Vector const& v ) 
{
 return EnvPtr<double>( TJetEnvironment<double>::makeJetEnvironment(maxweight, v.Dim(), v.Dim(), v ) );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
EnvPtr<double>  TJetEnvironment<double>::getApproxJetEnvironment(int maxweight, Vector const& refpoints)
{
  //--------------------------------------------------
  //   Determine if a Jet environment already exists
  //   whose reference point is sufficiently close. 
  //   Otherwise, return a NULL environment
  //-------------------------------------------------
 
  TJetEnvironment<double>* pje = new TJetEnvironment<double>( maxweight, refpoints.Dim(), refpoints.Dim(), refpoints );
  
  int nvar = refpoints.Dim();
  
  Vector tolerance( nvar );

  for( int i=0;  i<nvar; ++i ) {
    tolerance[i] =  std::abs(0.001 *refpoints[i]);
  }

  for (  std::list<EnvPtr<double> >::iterator env_iter  = TJetEnvironment<double>::environments_.begin();  
                                              env_iter != TJetEnvironment<double>::environments_.end(); 
                                                                   ++env_iter) { 
    if ( **env_iter == *pje ) return *env_iter;

  }
    
  return EnvPtr<double>(); 
    
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
template<>
template<>
EnvPtr<double> 
TJetEnvironment<std::complex<double> >::makeRealJetEnvironment(EnvPtr<std::complex<double> > const& env) {

  // NOTE: This conversion should probably fail if the reference point is complex FIXME !

 TVector<double> tmp_refpoints = real( env->refPoint() );
 
 TJetEnvironment<double>* pje = new TJetEnvironment<double>( env->maxWeight(), env->numVar(), env->spaceDim(), tmp_refpoints );

 for( std::list<EnvPtr<double> >::iterator env_iter  = TJetEnvironment<double>::environments_.begin(); 
                                           env_iter != TJetEnvironment<double>::environments_.end(); 
                                                                                          ++env_iter ) { 
  if ( **env_iter == *pje ) return *env_iter;
 }

 
 //-------------------------------------------------------------------------------------------------- 
 // NOTE: The reference count will go to 0 when the only instance of the smart ptr is the one left in 
 //       the environment list_. When that happens, the custom deleter (dispose()) removes the env from the list.   
 //-------------------------------------------------------------------------------------------------------------- 

 EnvPtr<double> newpje(pje);
 TJetEnvironment<double>::environments_.push_back( newpje );
 newpje->release(); // do not count the instance that is in the environments_ list.
 return newpje;

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
template <>
bool  TJetEnvironment<std::complex<double> >::operator==( TJetEnvironment<double> const& rhs ) const
{

  if( rhs.numVar_    !=  this->numVar_ )    return false;
  if( rhs.spaceDim_  !=  this->spaceDim_ )  return false;
  if( rhs.maxWeight_ !=  this->maxWeight_ ) return false;
 
  if ( imag( this->refPoint_ ).Norm() > 10.0*std::numeric_limits<double>::epsilon() ) return false; 
 
  Vector const tol( rhs.numVar(), 10.0*std::numeric_limits<double>::epsilon() );
  TVector<double>  eps     =  ( real(this->refPoint_) - rhs.refPoint_).Abs();
  return std::equal (eps.begin(), eps.end(), tol.begin(), (std::less<double>()) );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <>
template <>
bool  TJetEnvironment<double>::operator==( TJetEnvironment<std::complex<double> > const& rhs) const
{
  return (rhs == *this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
