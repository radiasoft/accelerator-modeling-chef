/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      JLPtr.h
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

template<typename T>
class TJetEnvironment;

template<typename T>
struct EnvPtr
{

  typedef boost::intrusive_ptr<TJetEnvironment<T> > Type;

};

#endif // ENVPTR_H


