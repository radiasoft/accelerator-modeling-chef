////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Make the allocation/deallocation faster for small, polymorphic objects. 
// Only the root of class hierarchy should be derived from gms::FastAllocator.
//  .
// Usage:
//
// class A { };
// =>
// class A : public gms::FastPODAllocator{ };
//
// Original version for individual objects authored by 
// Hannu Kankaanp‰‰ (hkankaan@cc.hut.fi)
//
// This version, modified by
// Jean-Francois Ostiguy (ostiguy@fnal.gov)
// Fermi National Accelerator Laboratory, Batavia, IL, USA 
//
// 
// Use as you wish.
// @see http://www.boost.org/
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef GMS_FASTALLOCATOR_H_
#define GMS_FASTALLOCATOR_H_

#include <boost/pool/pool.hpp>

namespace gms {

#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 64
#endif

/// Manages MAX_SMALL_OBJECT_SIZE amount of memory pools (no much cost for empty pools)
/// It selects the appropriate memory pool depending on how much memory is being allocated/deallocated.

class PoolManager {
	
public:

/// singleton access
    static PoolManager& getInstance() {
	                                static PoolManager instance;
			                return instance;
		                       }

     void* allocateMemory(size_t size) {
		                         if(   (size > MAX_SMALL_OBJECT_SIZE) ) { 
                                         return ::operator new(size);
		                         }		  
		                         return m_pools[size - 1]->malloc();
		                       }

     void releaseMemory(void* deletable, size_t size) {
	       		                                 if (size > MAX_SMALL_OBJECT_SIZE) {
				                            //use normal delete
                                                            // note: destructor is **not** called 
				                            ::operator delete(deletable);
			                                 } else {
				                           //use pool free. Don't delete null pointers
				                             if (deletable)
					                       m_pools[size - 1]->free(deletable);
			                                 }
		                                      }

    ~PoolManager() {
		      for (int i = 0; i < MAX_SMALL_OBJECT_SIZE; ++i) {
				  delete m_pools[i];
                      }


		    }

 private:
		

     PoolManager() {
			for (int i = 0; i < MAX_SMALL_OBJECT_SIZE; ++i)
				m_pools[i] = new boost::pool<>(i + 1);
		    }

    //no copying of a singleton:
     
    PoolManager(const PoolManager&);
		
    const PoolManager& operator=(const PoolManager&);

    /// memory pool array. m_pools[n] corresponds to pool with objectSize==n+1.
    
    boost::pool<>* m_pools[MAX_SMALL_OBJECT_SIZE];
	
}; //  class PoolManager


//=============================================================================================================


// NOTE: Under certain circumstances, there are problems with the boost::pool<> allocator under WIN32.
//       Until the issue is understood, FastAllocator merely uses the default system allocation 
//       on that platform. 

class FastAllocator {
public:

   static void* operator new(size_t size, void* p) throw()    {  // For placement new, continue using default behavior
     return ::operator new(size,p);                              // Usually, this is equivalent to simply returning p
   }


   static void* operator new(size_t size) {
         if (size==0) size=1 ;       // new must return a valid pointer when an object of zero size is allocated
#ifdef WIN32
        return ::operator new(size);
#else
        return PoolManager::getInstance().allocateMemory(size);
#endif

   }

   static void operator delete(void* deletable, size_t size) {
#ifdef WIN32
      ::operator delete(deletable);
#else
      PoolManager::getInstance().releaseMemory(deletable, size);
#endif

   }

   virtual ~FastAllocator() {}
   
}; // class FastAllocator


} // namespace gms

#endif
