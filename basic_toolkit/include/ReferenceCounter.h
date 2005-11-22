/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra. 
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
******  This is a modified version of code suggested in
******  B. Karlsonn, "Beyond the C++ STL: An Introduction to Boost",
******  Addisson-Wesley 2005.
******                                                                
**************************************************************************
*************************************************************************/
#ifndef  REFERENCECOUNTER_H
#define  REFERENCECOUNTER_H

#include <iosetup.h>
#include <iostream>

class ReferenceCounter {

  unsigned int _refcount; 

 public:

  ReferenceCounter( ): _refcount(0) {}
  
  // NOTE: a locking mechanism needs to be provided in
  //       order to make the ref count manipulation
  //       thread-safe. 

  virtual void dispose() = 0; 

  friend void intrusive_ptr_add_ref(ReferenceCounter* p) {
    ++(p->_refcount);
  }


  friend void intrusive_ptr_release(ReferenceCounter* p) {
    if ( (--p->_refcount) == 0 ) p->dispose(); 
  }

  void add_ref() {
    ++ _refcount;
  }

  int release() {
    return --_refcount;;
  }

  int count() { 
    return _refcount; 
  }
  
 protected:

  // The intent of the definitions below is to avoid
  // make sure that copying of the reference count
  // is only done in an explicit manner.

  ReferenceCounter& operator=(const ReferenceCounter&) {
    return*this;
  }

  // Explicit destruction forbidden; 
  virtual ~ReferenceCounter(){ }        


 private:

  // Copy contruction forbidden
  ReferenceCounter(const  ReferenceCounter&); 
 
}; 
 

#endif  // REFERENCECOUNTER_H
