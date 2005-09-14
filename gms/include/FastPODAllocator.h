////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Make the allocation/deallocation faster for small NON-POLYMORPHIC objects 
//  .
// Usage:
//
// class A { };
// =>
// class A : public gms::FastPODAllocator<A> { };
//
// Original version for individual objects authored by 
// Hannu Kankaanp‰‰ (hkankaan@cc.hut.fi)
//
// This version, modified by
// Jean-Francois Ostiguy (ostiguy@fnal.gov)
// Fermi National Accelerator Laboratory, Batavia, IL, USA 
//
// Note (1): FastPODAllocator<> is not thread safe.
// Note (2): Array allocation is *not* overloaded, nor should it be
//           because there is no way to do this in a general, portable way
// 
// Use as you wish.
// @see http://www.boost.org/
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef GMS_FASTPODALLOCATOR_H_
#define GMS_FASTPODALLOCATOR_H_

#include <boost/pool/pool.hpp>
#include <cassert>

//
// For debugging purposes, this can be defined. In that case, all allocations are done using the std malloc call.
// 

// #define PODALLOCATOR_MALLOC


namespace gms {

template <class T>
class FastPODAllocator {
	
public:

    static const unsigned int pool_initial_size = 32; 
		
    static void* operator new(size_t size, void* p) throw() {
                    return ::operator new(size,p);
    }

    static void* operator new(size_t size) {
      //just in case someone tries to misuse this class, at least a run-time error is shown
      assert(size == sizeof(T));
#ifdef PODALLOCATOR_MALLOC 
      return malloc( size );
#else
      return s_memPool.malloc();
#endif
    }



     static void operator delete(void* deletable) {
       //don't delete null pointers
#ifdef PODALLOCATOR_MALLOC 
       if (deletable) {
    	  free(deletable);
       }

#else
       if (deletable) {
	 s_memPool.free(deletable);                 
       }
#endif
     }


protected:

      ~FastPODAllocator() {}

private:
     
     // Each FastAllocator specialization has its own memory pool
     // Arrays are allocated from an ordered pool 

      static boost::pool<> s_memPool;
      
}; // class FastPODAllocator


// The static variable s_memPool is defined here. Its constructor is passed the object size.
// We use the virtually undocumented third argument to set the initial size of the pool
// boost::pool<> uses a doubling algorithm to resize the pool. The default value for initial size is 
// 32.  

template <class T>
boost::pool<> FastPODAllocator<T>::s_memPool(sizeof(T), FastPODAllocator<T>::pool_initial_size);


} // gms namespace

#endif
