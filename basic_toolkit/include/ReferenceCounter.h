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
******  Inspired by code suggested in
******  B. Karlsonn, 
****** "Beyond the C++ STL: An Introduction to Boost",
******  Addison-Wesley 2005.
******
****** Note:
****** -----
******
****** This ReferenceCounter class uses the 
****** Curiously Recurring Template idiom to provide compile-time 
****** polymorphism for destruction (through the dispose() function).
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
class ReferenceCounter;

template<typename T>
void intrusive_ptr_add_ref(ReferenceCounter<T>* p);

template<typename T>
void intrusive_ptr_release(ReferenceCounter<T>* p);

template<typename T>
void intrusive_ptr_add_ref(ReferenceCounter<T> const* p) 
{ intrusive_ptr_add_ref(const_cast<ReferenceCounter<T>*>(p) ); }

template<typename T>
void intrusive_ptr_release(ReferenceCounter<T> const* p)
{ intrusive_ptr_release( const_cast<ReferenceCounter<T>*>(p) ); }


template<typename T>
class DLLEXPORT ReferenceCounter {

  unsigned int refcount_; 

  T& toDerivedClass() { return static_cast<T&>(*this); }

 public:

  //--------------------------------------------------------
  // NOTE: ReferenceCount is not thread safe. 
  //       Thread safety would require a locking mechanism 
  //       for refcount_
  //--------------------------------------------------------

  ReferenceCounter( ): refcount_(0) {}
 
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

  //---------------------------------------------------
  // Assignment operator is a no-op operator. 
  // This necessary for operations such as  
  // std::swap(o1, o2) to work as expected.
  //---------------------------------------------------

  ReferenceCounter& operator=( ReferenceCounter const&) {
    return *this;
  }

  // Explicit destruction is forbidden; 

  ~ReferenceCounter(){ }        
  
  //-------------------------------------------------------- 
  // Copy contruction of the reference count is forbidden. 
  // since copying an object managed by a smart pointer 
  // must produce a new object with a reference count of 1.
  //---------------------------------------------------------

 private:

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
