/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      EnvPtr.h
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
******  Author:    Jean-Francois Ostiguy
******             Email: ostiguy@fnal.gov                         
******
******                                                                
**************************************************************************
*************************************************************************/
#ifndef ENVPTR_H
#define ENVPTR_H



#include <boost/intrusive_ptr.hpp>
#include <basic_toolkit/globaldefs.h>

#include <iostream>

template<typename T>
class TJetEnvironment;

//-------------------------------------------------------------------------------------

template<typename T>
class DLLEXPORT EnvPtr: public boost::intrusive_ptr<TJetEnvironment<T> > {
 
 public: 

  EnvPtr():     boost::intrusive_ptr<TJetEnvironment<T> >() {}

  explicit EnvPtr(TJetEnvironment<T>* p, bool add_ref=true)
    : boost::intrusive_ptr<TJetEnvironment<T> >(p,add_ref) {}
 
  template<typename U>
  operator EnvPtr<U>() const;

};

//-------------------------------------------------------------------------------------

template<typename T>
class DLLEXPORT ConstEnvPtr: public boost::intrusive_ptr<TJetEnvironment<T> const> { 
 
 public: 

  ConstEnvPtr():boost::intrusive_ptr<TJetEnvironment<T> const>() {}

  explicit ConstEnvPtr(TJetEnvironment<T> const* p, bool add_ref=true)
    : boost::intrusive_ptr<TJetEnvironment<T> const >(p,add_ref) {}

  ConstEnvPtr(EnvPtr<T> const & env)
    : boost::intrusive_ptr<TJetEnvironment<T> const >(env) {}

  template<typename U>
  operator ConstEnvPtr<U>(); 
};


//-------------------------------------------------------------------------------------

//--------------------------------
// specializations
//--------------------------------

template<>
template<>
EnvPtr<double>::operator EnvPtr<std::complex<double> >() const;

template<>
template<>
ConstEnvPtr<double>::operator ConstEnvPtr<std::complex<double> >(); 


#endif // ENVPTR_H
