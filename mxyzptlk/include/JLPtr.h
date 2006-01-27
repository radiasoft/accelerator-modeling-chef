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
#ifndef JLPTR_H
#define JLPTR_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <boost/intrusive_ptr.hpp>

template<typename T>
class TJL1;

template<typename T>
class TJL;

template<typename T>
struct JLPtr
{
#ifdef  FIRST_ORDER_JETS 
  typedef boost::intrusive_ptr<TJL1<T> > Type;
#else
  typedef boost::intrusive_ptr<TJL<T> > Type;
#endif

};



#endif


