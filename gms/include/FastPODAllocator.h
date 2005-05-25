#ifndef GMS_FASTPODALLOCATOR_H_
#define GMS_FASTPODALLOCATOR_H_

#include <boost/pool/pool.hpp>
#include <cassert>

// For debugging purposes, define this

/* #define PODALLOCATOR_DEBUG */

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
// This version, modifications to handle arrays authored by
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
			// assert(size == sizeof(T));
#ifdef PODALLOCATOR_DEBUG 
			return malloc(sizeof(T));
#else
			return s_memPool.malloc();
#endif
		}

		static void* operator new[] (size_t size) {

#ifdef PODALLOCATOR_DEBUG 
		return malloc( size );
#else
                        // NOTE: size%sizeof(T) will not be 0 if the class T
                        // has an explicitly defined destructor. This is because in that case, 
                        // the compiler needs to remember how many instances of T have 
                        // to have their (individaul) detructor called. The extra requested bytes 
                        // are used **by delete[] ** when it need to retrieve the array size. 
                        // Note that  "delete [](void * p) " calls "operator delete[]" when it needs 
                        // to deallocate the memory. However," delete [](void * p)" does additional 
                        // work before calling "operator[] delete": it calls destructors for
                        // the individual elements. Unfortunately, this last behavior *cannot*
                        // be overriden.  
                      
                        // Since we assume here that for effciency that memory is allocated 
                        // only in chunks of size(T), some "hokus pokus" is needed ;- )
                       
		        int m = 1;
                        while ( (m*sizeof(T)) <= sizeof(size_t) ) ++m;   
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

                        return (  p + m*sizeof(T) - sizeof(size_t)  );  //  p + n*sizeof(T) is the offset of the first usable block 
 #endif
		}

		static void operator delete(void* deletable) {
 			//don't delete null pointers
#ifdef PODALLOCATOR_DEBUG 
  		        free(deletable);

#else
			if (deletable)
				s_memPool.free(deletable);                 
#endif
		}

		static void operator delete[] (void* deletable) {

#ifdef PODALLOCATOR_DEBUG 
                  free(deletable);
#else
		  char* p = (char*) deletable;

                  int m =1;
                  while (  (m * sizeof(T)) <= sizeof(size_t)) ++m;
                  ++m;

                  p = p + sizeof(size_t) - m * sizeof(T); 

                  int n = *((int*) p);

                  s_ordered_memPool.ordered_free( p, n); 

#endif
		}

	protected:
		~FastPODAllocator() {}

	private:
		// each FastAllocator specialization has it's own memory pool
                // arrays are allocated from an ordered pool 
		static boost::pool<> s_memPool;
		static boost::pool<> s_ordered_memPool;
	};

	//the static variable s_memPool is defined here. It's constructor is passed the object size.
	template <class T>
	boost::pool<> FastPODAllocator<T>::s_memPool(sizeof(T));

	template <class T>
	boost::pool<> FastPODAllocator<T>::s_ordered_memPool( sizeof(T) );
}

#endif
