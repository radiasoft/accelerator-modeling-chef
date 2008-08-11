/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TMapping.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
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
******   implicit conversions.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - implementation details completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES
******
******  Dec 2006 ostiguy@fnal
******
******  - New TJetVector base class implementation. See  TJetVector.h for details. 
******                                                                
******  Mar 2008 ostiguy@fnal
******  - Map composition and evaluation code refactored. 
******  - Support for evaluation of complex maps                                                                   
******  - added (missing) implementation for in-place Map composition
******  Aug 2008 ostiguy@fnal.gov
****** - added templated constructor for initialization from a vector type
******
**************************************************************************
*************************************************************************/
#ifndef TMAPPING_H
#define TMAPPING_H

#include <basic_toolkit/globaldefs.h>
#include <mxyzptlk/TJetVector.h>


template <typename T>
class DLLEXPORT TMapping: public TJetVector<T> {

 template <typename U>
 friend class TMapping;

 public: 

  TMapping(        EnvPtr<T> const&  env        = TJetEnvironment<T>::topEnv() );

  TMapping( int n, EnvPtr<T> const&  env        = TJetEnvironment<T>::topEnv() );

  TMapping( TMapping const& );

  template<typename U>
  TMapping( TMapping<U> const& );

  template<typename iterator_t>
  TMapping( iterator_t itstart, iterator_t itend,  EnvPtr<T> const& env = TJetEnvironment<T>::topEnv() ); 

  TMapping( TJetVector<T> const& );

  explicit TMapping( const char* id, EnvPtr<T> const& env = TJetEnvironment<T>::topEnv() ); // Produces the identity.

 ~TMapping();

  TMapping& operator= ( TMapping const& );

  TVector<T> operator()( TVector<T> const& )   const;
  TMapping   operator()( TMapping   const& ) const;  // TMapping composition.

  TVector<T> operator* ( TVector<T>   const& ) const;  //  an alias for operator()
  TMapping   operator* ( TMapping     const& ) const;  //  an alias for operator()

  TMapping& operator*=( TMapping const& );

  TMatrix<T> jacobian() const;

  TMapping inverse() const;

 private:

  TMapping<T> epsInverse(  EnvPtr<T> const&) const;


};

//-------------------------------------------------------------------------------
// specializations
//-------------------------------------------------------------------------------

 template<>
 template<>
 TMapping<std::complex<double> >::TMapping( TMapping<double> const& );


#ifndef MXYZPTLK_EXPLICIT_TEMPLATES 
#include <mxyzptlk/TMapping.tcc>
#endif

#endif // TMAPPING_H
