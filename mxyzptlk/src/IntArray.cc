/*
 * Implementation of class IntArray
 * 
 * Version 1.0       Mar, 1996
 * 
 * Leo Michelotti
 * michelot@calvin.fnal.gov
 * 
 */


#include <stdlib.h>
#include <string.h>
#include "IntArray.h"

#ifdef OBJECT_DEBUG
int IntArray::objectCount = 0;
#endif

#define CHECKOUT(test,fcn,message)          \
  if( test ) {                              \
    cerr << "\n\n"                          \
            "*** ERROR ***\n"               \
            "*** ERROR ***" fcn "\n"        \
            "*** ERROR ***" message "\n"    \
            "*** ERROR ***\n"               \
            "*** ERROR ***"                 \
         << endl;                           \
         exit(1);                           \
  }

IntArray::IntArray( const int& n, const int* x )
{
  static int i;

  CHECKOUT(n <= 0, "IntArray::IntArray", "Dimension must be positive.")

  dim = n;
  comp = new int [ dim ];
  if( x ) for ( i = 0; i < dim; i++ ) comp[i] = x[i];
  else    for ( i = 0; i < dim; i++ ) comp[i] = 0;
#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}

IntArray::IntArray( const IntArray& x )
{
  static int i;
  dim = x.dim;
  comp = new int [ dim ];
  for ( i = 0; i < dim; i++ ) comp[i] = x.comp[i];
#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}



// Assignment ...

void IntArray::Set( const int* x )
{
  static int i;
  for ( i = 0; i < dim; i++ ) comp[i] = x[i];
}

void IntArray::Set( const int& x )
{
  static int i;
  for ( i = 0; i < dim; i++ ) comp[i] = x;
}

int& IntArray::operator() ( const int& i ) const
{
  static int dummy = - 12345678;  // ??? Better way???
  if ( ( 0 <= i ) && ( i < dim ) ) return comp[i];
  else {
    cerr << "\n\n"
         << "*** ERROR ***                                    \n"
         << "*** ERROR *** IntArray::operator()               \n"
         << "*** ERROR ***                                    \n"
         << "*** ERROR *** Index is out of range.             \n"
         << "*** ERROR ***                                    \n"
         << endl;
    if( 3+2==5 ) exit(1);
    return dummy;
  }
}


void IntArray::Reconstruct( const int& n, const int* x )
{
  static int i;

  CHECKOUT(n < 0, "IntArray::IntArray", "Dimension must be positive.")

  if( n > 0 ) { 
    dim = n;
    if( comp ) delete [] comp;
    comp = new int [ dim ];
  }

  if( x ) for ( i = 0; i < dim; i++ ) comp[i] = x[i];
  else    for ( i = 0; i < dim; i++ ) comp[i] = 0;
}

// Algebraic functions ...

IntArray& IntArray::operator= ( const IntArray& x )
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "IntArray::operator=", "Incompatible dimensions.")
#endif

  static int i;
  for ( i = 0; i < dim; i++ ) comp[i] = x.comp[i];
  return *this;
}


int IntArray::Sum() const
{
  static int i, s;
  s = 0;
  for( i = 0; i < dim; i++ ) s += comp[i];
  return s;
}

// Boolean functions ...

char IntArray::operator== ( const IntArray& x ) const
{
  if( dim != x.dim ) return 0;
  for( int i = 0; i < dim; i++ ) 
    if( comp[i] != x.comp[i] ) return 0;
  return 1;
}

char IntArray::operator!= ( const IntArray& x ) const
{
  return !( operator==( x ) );
}

char IntArray::operator< ( const IntArray& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "IntArray::operator<", "Dimensions incompatible.")
#endif

  char z = 1;
  int  i = -1;
  while ( ( ++i < dim ) && z ) z &= comp[i] < x.comp[i];
  return z;
}

char IntArray::operator<= ( const IntArray& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "IntArray::operator<=", "Dimensions incompatible.")
#endif

  char z = 1;
  int  i = -1;
  while ( ( ++i < dim ) && z ) z &= comp[i] <= x.comp[i];
  return z;
}

char IntArray::operator> ( const IntArray& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "IntArray::operator>", "Dimensions incompatible.")
#endif

  char z = 1;
  int  i = -1;
  while ( ( ++i < dim ) && z ) z &= comp[i] > x.comp[i];
  return z;
}

char IntArray::operator>= ( const IntArray& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "IntArray::operator>=", "Dimensions incompatible.")
#endif

  char z = 1;
  int  i = -1;
  while ( ( ++i < dim ) && z ) z &= comp[i] >= x.comp[i];
  return z;
}

char IntArray::operator== ( const int& x ) const
{
  for( int i = 0; i < dim; i++ ) 
    if( comp[i] != x ) return 0;
  return 1;
}

char IntArray::operator!= ( const int& x ) const
{
  return !( operator==( x ) );
}

char IntArray::IsNull() const
{
  int i = -1;
  while ( ++i < dim ) if( comp[i] != 0.0 ) return 0;
  return 1;
}



// Streams

ostream& operator<<( ostream& os, const IntArray& x )
{
  os << "( ";
  for( int i = 0; i < x.dim - 1; i++ )
    os << x.comp[i] << ", ";
  os << x.comp[ x.dim - 1 ] << " )";
  return os;
}

istream& operator>>( istream& is, IntArray& x )
{
  char buf[80];
  int i;

  is >> buf;
  if( buf[0] != '(' ) {
    cerr << "\n\n"
         << "*** ERROR ***                                \n"
         << "*** ERROR *** istream operator>>( IntArray ) \n"
         << "*** ERROR *** First character read was       \n"
         << "*** ERROR *** not correct.                   \n"
         << "*** ERROR ***                                \n"
         << "*** ERROR *** Expected ( and got " << buf << " \n"
         << "*** ERROR ***                                \n"
         << endl;
    exit(1);
  }
  
  i = 0;
  is >> buf;
  x.comp[i] = atoi( buf );

  while( buf[ strlen(buf) - 1 ] != ')' ) {
    is >> buf;
    x.comp[++i] = atoi( buf );
  }

  if( x.dim != ++i ) {
    cerr << "\n\n"
         << "*** ERROR ***                                \n"
         << "*** ERROR *** istream operator>>( IntArray ) \n"
         << "*** ERROR *** Incorrect number of components \n"
         << "*** ERROR *** were read. .                   \n"
         << "*** ERROR ***                                \n"
         << "*** ERROR *** Expected " << x.dim 
         <<              " and got "  << i <<           " \n"
         << "*** ERROR ***                                \n"
         << endl;
    exit(1);
  }

  return is;
}

