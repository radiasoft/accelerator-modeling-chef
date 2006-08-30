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
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - implementation details completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES
******
******                                                                
**************************************************************************
*************************************************************************/
#ifndef TMAPPING_H
#define TMAPPING_H

#include <basic_toolkit/globaldefs.h>
#include <mxyzptlk/TJet.h>
#include <mxyzptlk/TJetVector.h>

template <typename T>
class DLLEXPORT TMapping: public TJetVector<T> {

 template <typename U>
 friend class TMapping;

 private:

  TMapping<T> _epsInverse(  EnvPtr<T> const&) const;

 public: 

  TMapping( int               dim        = TJet<T>::_lastEnv->spaceDim(),
            const TJet<T>*    components = 0, 
            EnvPtr<T> const&  env        = TJet<T>::_lastEnv );

  template<typename U>
  TMapping( TMapping<U> const& );

  TMapping( TMapping const& );

  TMapping( const char* id, EnvPtr<T> const& env = TJet<T>::_lastEnv ); // Produces the identity.

  TMapping( TJetVector<T> const& );


  ~TMapping();

  TMapping& operator= ( TMapping const& );


  Vector     operator()( Vector const& ) const;
  TMapping   operator()( TMapping const& ) const;  // TMapping composition.
  TJet<T>    operator()( int ) const; 
  TJet<T>&   operator()( int ); 

  TMapping  operator* ( TMapping const& ) const;  // TMapping composition also; an alias.
  TMapping* operator*=( TMapping const& );

  TMatrix<T> Jacobian() const; // Retained for backwards compatability
  TMatrix<T> jacobian() const;

  TMapping Inverse() const;  // retained for backwards compatability
  TMapping inverse() const;
};


// specializations

 template<>
 Vector  TMapping<double>::operator()( const Vector& ) const;

 template<>
 template<>
 TMapping<std::complex<double> >::TMapping( TMapping<double> const& );

// Inline methods

template<typename T>
inline TJet<T> TMapping<T>::operator()( int i ) const
{ return this->TJetVector<T>::operator()( i ); }

template<typename T>
inline TJet<T>& TMapping<T>::operator()( int i )
{ return this->TJetVector<T>::operator()( i ); }

template<typename T>
inline TMapping<T>& TMapping<T>::operator=( const TMapping<T>& x )
{ static_cast< TJetVector<T>& >(*this) =  static_cast<TJetVector<T> const&>(x); 
  return *this;
}

#ifndef MXYZPTLK_EXPLICIT_TEMPLATES 
#include <mxyzptlk/TMapping.tcc>
#endif

#endif // TMAPPING_H
