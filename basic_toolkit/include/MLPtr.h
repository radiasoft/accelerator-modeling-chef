/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      MLPtr.cc
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
#ifndef MLPTR_H
#define MLPTR_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <complex>
#include <boost/intrusive_ptr.hpp>
#include <basic_toolkit/globaldefs.h>

template <typename T>
class TML;


template<typename T>
class DLLEXPORT MLPtr: public boost::intrusive_ptr<TML<T> > {
 
 public: 

 MLPtr():                             boost::intrusive_ptr<TML<T> >() {}
 MLPtr(TML<T>* p, bool add_ref=true): boost::intrusive_ptr<TML<T> >(p,add_ref) {}


 template<typename U>
 operator MLPtr<U>() const; 


};

// ---------------------------------------------------------------------------------
// specializations
// ---------------------------------------------------------------------------------


template<>
template<>
MLPtr<double>::operator MLPtr<std::complex<double> >() const;


#endif //  MLPTR_H
