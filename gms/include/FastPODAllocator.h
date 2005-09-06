#ifndef GMS_FASTPODALLOCATOR_H_
#define GMS_FASTPODALLOCATOR_H_

#include <boost/pool/pool.hpp>
#include <cassert>


// 

#ifdef __sparc 
#define ARRAY_SIZE_OFFSET 8 
#else
#define ARRAY_SIZE_OFFSET 4 
#endif

//
// For debugging purposes, this can be defined. In that case, all allocations are done using the std malloc call.
// 
// #define PODALLOCATOR_DEBUG 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Make the allocation/deallocation faster for small NON-POLYMORPHIC objects or arrays of such objects
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
// This version, modified to handle array allocation by
// Jean-Francois Ostiguy (ostiguy@fnal.gov)
// Fermi National Accelerator Laboratory, Batavia, IL, USA 
//
// Note that FastPODAllocator<> is not thread safe.
// 
// Use as you wish.
// @see http://www.boost.org/
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////



namespace gms {

	template <class T>
	class FastPODAllocator {
	public:
		static void* operator new(size_t size, void* p) throw() {
                    return ::operator new(size,p);
		}

		static void* operator new(size_t size) {
			//just in case someone tries to misuse this class, at least a run-time error is shown
			 assert(size == sizeof(T));
#ifdef PODALLOCATOR_DEBUG 
			return malloc( size );
#else
			return s_memPool.malloc();
#endif
		}


		static void* operator new[] (size_t size) {

#ifdef PODALLOCATOR_DEBUG 
		return malloc( size );
#else
                        // NOTE: In general, size%sizeof(T) will not be 0 if the class T
                        // has an explicitly defined destructor. This is because 
                        // the compiler needs to know how many instances of T have 
                        // to have their (individual) destructor called. The extra requested bytes 
                        // are used **by delete[] ** when it need to retrieve the array size. 
                        // Note that  "delete [](void * p) " calls "operator delete[]" when it needs 
                        // to deallocate the memory. However," delete [](void * p)" does additional 
                        // work before calling "operator[] delete": it calls destructors for
                        // the individual elements. Unfortunately, this behavior *cannot*
                        // be overriden.  
                      
                        // We assume here that, for efficiency, memory can only be allocated 
                        // in chunks of size(T). The ugly code below preserves the storage normally
                        // used by the compiler to store the array size. In addition, an extra block of memory
                        // is allocated to store our own copy of that same size.  
                   
                       
		        int m = 1;
                        while ( (m*sizeof(T)) <= ARRAY_SIZE_OFFSET ) ++m;   
                        ++m; 

                        // In case T has an explicit destructor, we need to add m extra blocks to allow the compiler
                        // to store the no of blocks requested. 
                        // We also add one block to store our own copy of the total number of blocks allocated.  
                        // When T has an  explicit destructor, this is redundant, but we cannot rely on 
                        // the existence of an explicit destructor ! 

		        int n = (size/sizeof(T) + m);
   			
                        char* p = (char*) s_ordered_memPool.ordered_malloc( n );
                        
                        int* nsave =  (int*) p;
                        *nsave = n; 

                        return (  p + m*sizeof(T) - ARRAY_SIZE_OFFSET  );  //  p + n*sizeof(T) is the offset of the first usable block 
 #endif
		}

		static void operator delete(void* deletable) {
 			//don't delete null pointers
#ifdef PODALLOCATOR_DEBUG 
		  if (deletable) {
    		        free(deletable);
		  }

#else
			if (deletable)
				s_memPool.free(deletable);                 
#endif
		}


		static void operator delete[] (void* deletable) {

#ifdef PODALLOCATOR_DEBUG 

		 if (deletable)
                     free(deletable);
#else
		  char* p = (char*) deletable;

                  int m =1;
                  while (  (m * sizeof(T)) <= ARRAY_SIZE_OFFSET ) ++m;
                  ++m;

                  p = p + ARRAY_SIZE_OFFSET - m * sizeof(T); 

                  int n = *((int*) p);
                  s_ordered_memPool.ordered_free( p, n); 

#endif
		}

	protected:
		~FastPODAllocator() {}

	private:
		// Each FastAllocator specialization has its own memory pool
                // Arrays are allocated from an ordered pool 

		static boost::pool<> s_memPool;
		static boost::pool<> s_ordered_memPool;
	};

	// The static variable s_memPool is defined here. Its constructor is passed the object size.
        // We use the virtually undocumented third argument to set the initial size of the pool
        // boost::pool<> uses a doubling algorithm to resize the pool. The initial size is 
        // 32, unless it is overriden.  

	template <class T>
	boost::pool<> FastPODAllocator<T>::s_memPool(sizeof(T), 2048);

	template <class T>
	boost::pool<> FastPODAllocator<T>::s_ordered_memPool( sizeof(T), 2048 );
}

#endif
