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
#include <basic_toolkit/globaldefs.h>

template<typename T>
class TJL1;

template<typename T>
class TJL;

template<typename T>
class JLPtr;

//------------------------------------------------
// class JLPtr  
//------------------------------------------------

template<typename T>
class DLLEXPORT JLPtr: public boost::intrusive_ptr<TJL<T> >
{

 public:

  JLPtr():                                          boost::intrusive_ptr<TJL<T> >() {}
  explicit JLPtr(TJL<T>* p, bool add_ref=true):     boost::intrusive_ptr<TJL<T> >(p,add_ref) {}

  template<class OtherType> 
  JLPtr (  OtherType const & r):                    boost::intrusive_ptr<OtherType>(r) {} 

  int count()  const                                { return this->get()->count(); }
};

//------------------------------------------------
// class ConstJLPtr  
//------------------------------------------------

template<typename T>
class DLLEXPORT ConstJLPtr: public boost::intrusive_ptr<TJL<T> const>
{

 public:

  ConstJLPtr():                                             boost::intrusive_ptr<TJL<T> const>() {}
  explicit ConstJLPtr(TJL<T> const* p, bool add_ref=true):  boost::intrusive_ptr<TJL<T> const>(p,add_ref) {}

  template<class OtherType> 
  ConstJLPtr (  OtherType const & r):                       boost::intrusive_ptr<OtherType>(r) {} 

  int count() const                                         { return this->get()->count(); }
};



//------------------------------------------------
// class JL1Ptr  
//------------------------------------------------

template<typename T>
class DLLEXPORT JL1Ptr: public boost::intrusive_ptr<TJL1<T> >
{

 public:

  JL1Ptr():                                          boost::intrusive_ptr<TJL1<T> >() {}
  explicit JL1Ptr(TJL1<T>* p, bool add_ref=true):    boost::intrusive_ptr<TJL1<T> >(p,add_ref) {}

  template<class OtherType> 
  JL1Ptr ( OtherType const & r):                     boost::intrusive_ptr<OtherType>(r) {} 

  int count() const                                 { return this->get()->count(); }

};

//------------------------------------------------
// class JL1Ptr  
//------------------------------------------------

template<typename T>
class DLLEXPORT ConstJL1Ptr: public boost::intrusive_ptr<TJL1<T> const>
{

 public:

  ConstJL1Ptr():                                              boost::intrusive_ptr<TJL1<T> const>() {}
  explicit ConstJL1Ptr(TJL1<T> const* p, bool add_ref=true):  boost::intrusive_ptr<TJL1<T> const>(p,add_ref) {}

  template<class OtherType> 
  ConstJL1Ptr ( OtherType const & r):                         boost::intrusive_ptr<OtherType>(r) {} 

  int count() const                                           { return this->get()->count(); }


};


#endif


