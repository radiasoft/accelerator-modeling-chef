/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Basic Toolkit:  Low Level utility classes 
******                                    
******  File:      ReferenceCounter.h
******                                                                
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab     
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
******                                                                
******  Author:    Jean-Francois Ostiguy
******             ostiguy@fnal.gov                         
******                                                                
******  Inspired by the code suggested in
******  B. Karlsonn, "Beyond the C++ STL: An Introduction to Boost",
******  Addisson-Wesley 2005.
******
******
****** Note:
****** -----
******
****** This version of the ReferenceCounter class uses the Curiously 
****** Recurring Template idiom to provide compile-time polymorphism. 
****** (needed for the dispose() function).
******
****** A derived class DerivedClass should be declared as follows:
******
******   class DerivedClass : public  ReferenceCounter<DerivedClass> {
******   public:
******   void dispose(); // a class-specific implementation of dispose() 
******                   // must be provided 
******   ... 
******   ...
******                                   
******   };                          
*****
**************************************************************************
*************************************************************************/
#ifndef  REFERENCECOUNTER_H
#define  REFERENCECOUNTER_H

#include <iostream>

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/iosetup.h>

template<typename T>
class TJL;

template<typename T>
class ReferenceCounter;

template<typename T>
void intrusive_ptr_add_ref(ReferenceCounter<T>* p);

template<typename T>
void intrusive_ptr_release(ReferenceCounter<T>* p);



template<typename T>
class DLLEXPORT ReferenceCounter {

  unsigned int refcount_; 

  T& toDerivedClass() { return static_cast<T&>(*this); }

 public:

  ReferenceCounter( ): refcount_(0) {}
  
  // NOTE: a locking mechanism needs to be provided in
  //       order to make the ref count manipulation
  //       thread-safe. 

  void dispose(){ return toDerivedClass().dispose(); }

  friend void intrusive_ptr_add_ref<>(ReferenceCounter* p);

  friend void intrusive_ptr_release<>(ReferenceCounter* p);


  void add_ref() {
    ++ refcount_;
  }

  int release() {
    return --refcount_;;
  }

  int count() { 
    return refcount_; 
  }
  
 protected:

  // The intent of the definitions below is to
  // make sure that copying of the reference count
  // is only done in an explicit manner.

  ReferenceCounter& operator=( ReferenceCounter const&) {
    return *this;
  }

  // Explicit destruction forbidden; 
  ~ReferenceCounter(){ }        
  

 private:

  // Copy contruction forbidden
  ReferenceCounter(ReferenceCounter const&); 
 
}; 

template<typename T>
inline void intrusive_ptr_add_ref(ReferenceCounter<T>* p)
{
    ++(p->refcount_);
}

template<typename T>
inline void intrusive_ptr_release(ReferenceCounter<T>* p)
{
    if ( --(p->refcount_) == 0 ) p->dispose(); 
} 

 

#endif  // REFERENCECOUNTER_H
