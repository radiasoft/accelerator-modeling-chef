#ifndef INTARRAY_H
#define INTARRAY_H

#include <iostream.h>

class IntArray {
protected:
  int  dim;    // Number of componenents
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
  int&     operator()       ( const int& ) const;    // return or set
                                                     // component
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

  // Utilities ..
  friend ostream& operator<<( ostream&, const IntArray& );
  friend istream& operator>>( istream&, IntArray& );

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

inline IntArray::~IntArray()
{
  delete [] comp;
#ifdef OBJECT_DEBUG
  objectCount--;
#endif
}

inline int IntArray::Dim() const
{
  return dim;
}

#endif // INTARRAY_H
