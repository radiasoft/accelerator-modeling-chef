/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      IntArray.h
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


// Private allocators
// J.F. Ostiguy Beam Physics - Feb 11 1999
//#define __PRIVATE_ALLOCATOR__

#ifndef INTARRAY_H
#define INTARRAY_H

#include <iostream>
#include <exception>
#include <string>

using std::istream;
using std::ostream;

#ifdef __PRIVATE_ALLOCATOR__
#include <vmalloc.h>
#endif

class IntArray {
#ifdef __PRIVATE_ALLOCATOR__
private:
  static int         _init;  // private pool memory manager initialization status flag
  static Vmalloc_t*  _vmem;  // private pool memory manager heap
  static             void meminit();
#endif
protected:
  int  dim;    // Number of components
  int* comp;   // Data
public:

  // Constructors and the destructor ...
  IntArray( const int&   /* dim */        = 3,
            const int*   /* components */ = 0 );
  IntArray( const IntArray& );
  ~IntArray();

  // Assignment ...
  void     Set              ( const int* );
  void     Set              ( const int& );
  int      operator()       ( const int& ) const;    // return component
  int&     operator()       ( const int& );          // set    component
  void     Reconstruct( const int& = 0, const int* = 0 );  
           // Just like constructor

  // Functions ...
  IntArray&     operator=      ( const IntArray& );

  char          operator==     ( const IntArray& ) const;
  char          operator!=     ( const IntArray& ) const;
  char          operator<      ( const IntArray& ) const;
  char          operator<=     ( const IntArray& ) const;
  char          operator>      ( const IntArray& ) const;
  char          operator>=     ( const IntArray& ) const;

  char          operator==     ( const int& ) const;
  char          operator!=     ( const int& ) const;

  char          IsNull         () const;

  // Queries ...
  int  Dim() const;
  int  Sum() const;


  //  void* operator new(size_t bytes_to_allocate);
  //  void  operator delete(void* obj, size_t size);

  // Utilities ..
  friend ostream& operator<<( ostream&, const IntArray& );
  friend istream& operator>>( istream&, IntArray& );

  // Exceptions ...
  struct GenericException : public std::exception
  {
    GenericException( const char*, const char* );
    // 1st argument: identifies function containing throw
    // 2nd         : identifies type of error
    ~GenericException() throw() {}
    const char* what() const throw();
    std::string errorString;
  };

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

inline int IntArray::Dim() const
{
  return dim;
}

#endif // INTARRAY_H
