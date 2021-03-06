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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <boost/intrusive_ptr.hpp>
#include <basic_toolkit/globaldefs.h>

#include <iostream>

template<typename T>
class TJetEnvironment;


template<typename T>
class DLLEXPORT EnvPtr: public boost::intrusive_ptr<TJetEnvironment<T> > {
 
 public: 

  EnvPtr():     boost::intrusive_ptr<TJetEnvironment<T> >() {}

  explicit EnvPtr(TJetEnvironment<T>* p, bool add_ref=true)
    : boost::intrusive_ptr<TJetEnvironment<T> >(p,add_ref) {}
 
  template<class OtherType> 
  EnvPtr (  EnvPtr<OtherType> const& r): boost::intrusive_ptr<TJetEnvironment<T> >(r) {} 

};

//--------------------------------
// specializations
//--------------------------------

template<>
template<>
EnvPtr<std::complex<double> >::EnvPtr( EnvPtr<double> const&);  


#endif // ENVPTR_H
