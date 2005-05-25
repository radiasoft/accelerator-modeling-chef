/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.1                    
******                                    
******  File:      IntArray.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
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
****** Revision History:
******
******  Mar 2005  
******
******  Jean-Francois Ostiguy, ostiguy@fnal.gov
******
******  Efficiency improvements. 
******  - Data is no longer dynamically allocated, 
******    resulting in fixed size TJLterm<>.  
******  - exponents are represented by exponent_t type variables. 
******    exponent_t is defined by default as signed char (8 bits) 
******  - no explicit destructor 
******  - added iterator classes
******  - eliminated inefficient array style 
******    dereferencing 
******   
*************************************************************************/
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <IntArray.h>

#include <iostream>
#include <cstdlib>
#include <string>

using std::cerr;
using std::endl;

#ifdef OBJECT_DEBUG
int IntArray::objectCount = 0;
#endif


IntArray::GenericException::GenericException( const char* fcn, const char* msg )
: errorString( msg )
{
  static bool firstTime = true;
  if( firstTime ) {
    cerr << "\n*** ERROR *** "
            "\n*** ERROR *** " << fcn
         << "\n*** ERROR *** " << msg
         << "\n*** ERROR *** This message is printed only once." 
         << endl;
    firstTime = false;
  }
}


const char* IntArray::GenericException::what() const throw()
{
  return strcat( "IntArray::GenericException: ", errorString.c_str() );
}


#define CHECKOUT(test,fcn,message)                       \
  if( test ) {                                           \
    throw( IntArray::GenericException( fcn, message ) ); \
  }

IntArray::IntArray( int n, const int* x ): _dim(n)
{

  // This function depends on endian-ness of the CPU !
  
  
        exponent_t* comp_ptr = &_comp[0] - 1;
                   const int* x_ptr    = x-1;

  int count = _dim+1;

  if( x ) 
    while (--count) *(++comp_ptr) = *(++x_ptr); // 
  else    
    while (--count) *(++comp_ptr) = 0;

#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}

IntArray::IntArray( const IntArray& x ) : _dim(x._dim)
{

  memcpy((void *)_comp, (const void *)x._comp, _dim * sizeof(exponent_t));

#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}

// Assignment ...

void IntArray::Set( const int* x )
{
      const int* x_ptr    = x-1;
  exponent_t* comp_ptr   = &_comp[0] -1 ;

  int count = _dim+1;
 
  while (--count) *(++comp_ptr) = *(++x_ptr);
}

void IntArray::Set( int x )
{
  exponent_t* comp_ptr = &_comp[0] -1 ;

  int count = _dim+1;
  while (--count) *(++comp_ptr) = x;
}


// These functions are in-lined

//  int IntArray::operator() (  int i ) const
// int& IntArray::operator() ( int i )



void IntArray::Reconstruct( int n, const int* x )
{
 
  exponent_t*       comp_ptr = &_comp[0] - 1;
          const int* x_ptr    =  x -1 ;
  
  _dim  = n;
  int count = _dim+1; 
  
  if (x) while (--count) *(++comp_ptr) = *(++x_ptr);
  else   while (--count) *(++comp_ptr) = 0;

}

void IntArray::Reconstruct( const IntArray& x )
{

  exponent_t*       comp_ptr = &_comp[0]  - 1;
  const exponent_t* x_ptr    = &(x._comp[0]) - 1;
  
  _dim = x.Dim();
  int count = _dim+1; 
  while ( --count ) *(++comp_ptr) = *(++x_ptr);

}

// Algebraic functions ...

IntArray& IntArray::operator= ( const IntArray& x )
{
  memcpy((void *)_comp, (const void *)x._comp, _dim * sizeof(exponent_t));
  return *this;
}


IntArray IntArray::operator+( const IntArray& y )
{
  IntArray ret( *this );
  const exponent_t* yPtr = &(y._comp[0])   - 1;  
        exponent_t* xPtr = &(ret._comp[0]) - 1; 

  const exponent_t* upper = yPtr + _dim;
  while( yPtr < upper ) { 
    *(++xPtr) += *(++yPtr);

  }
  return ret;
}


int IntArray::Sum() const
{
  int s = 0;
  const exponent_t* comp_ptr  = &_comp[0] - 1;  
  int count = _dim+1;

  while (--count) s += *(++comp_ptr);
  return s;
}

// Boolean functions ...

bool IntArray::operator== ( const IntArray& x ) const
{


  const exponent_t* comp_ptr  = &_comp[0] - 1;     
  const exponent_t* xcomp_ptr = &(x._comp[0]) - 1;   

  if( _dim != x._dim ) return 0;
  int count = _dim+1;

  while (--count) 
    if( *(++comp_ptr) != *(++xcomp_ptr) ) return 0;

  return 1;


}

bool IntArray::operator!= ( const IntArray& x ) const
{
  return !( operator==( x ) );
}

bool IntArray::operator< ( const IntArray& x ) const
{

  bool z = true;

  const exponent_t*        comp_ptr = &_comp[0] -1 ;     
  const exponent_t*  xcomp_ptr = &(x._comp[0]) -1;   
  
  int count = _dim+1;

  while ( (--count ) && z ) 
       z &= *(++comp_ptr) < *(++xcomp_ptr);

  return z;
}

bool IntArray::operator<= ( const IntArray& x ) const
{

  bool z = true;
  int  i = -1;

  const exponent_t*   comp_ptr = &_comp[0] -1 ;     
  const exponent_t*  xcomp_ptr = &(x._comp[0]) - 1; 
  
  int count = _dim+1;

  while ( (--count) && z ) 
       z &= *(++comp_ptr)  <=   *(++xcomp_ptr);

  return z;
}

bool IntArray::operator> ( const IntArray& x ) const
{

  bool z = true;
  const exponent_t*   comp_ptr = &_comp[0] - 1;
  const exponent_t*  xcomp_ptr = &(x._comp[0]) - 1;

  int  count  = _dim+1;
  while ( ( --count ) && z ) 
     z &=  *(++comp_ptr) > *(++xcomp_ptr);

  return z;
}

bool IntArray::operator>= ( const IntArray& x ) const
{

  bool z = true;

  const exponent_t*   comp_ptr = &_comp[0] - 1;   
  const exponent_t*  xcomp_ptr = &(x._comp[0]) -1; 

  int  count = _dim+1;
  while ( (--count) && z ) z  &=  ( *(++comp_ptr)) >= (*( ++xcomp_ptr ));
  return z;
}

bool IntArray::operator== ( const int x ) const
{

  const exponent_t*   comp_ptr = &_comp[0] -1 ; 

  int count = _dim+1;

  while (--count )
    if( *(++comp_ptr) != x ) return 0;

  return 1;
}

bool IntArray::operator!= ( int x ) const
{
  return !( operator==( x ) );
}

bool IntArray::IsNull() const
{
  const exponent_t*   comp_ptr = &_comp[0] - 1;

  int count = _dim+1;
  while ( --count ) if( *(++comp_ptr) != 0 ) return 0;
  return 1;
}

// Streams

std::ostream& operator<<( std::ostream& os, const IntArray& x ) 
{
  os << "( ";
  for( int i = 0; i < x._dim - 1; i++ )
    os << (int) x._comp[i] << ", ";
  os << (int) x._comp[ x._dim - 1 ] << " )";
  return os;
}

std::istream& operator>>( std::istream& is, IntArray& x )
{
  char buf[80];
  int i;

  is >> buf;
  if( buf[0] != '(' ) {
    throw( IntArray::GenericException(
                         "istream& operator>>( istream& is, IntArray& x )",
                         "Incorrect first character in line."  ) );
  }
  
  i = 0;
  is >> buf;

  while( buf[ strlen(buf) - 1 ] != ')' ) {
    x._comp[i++] = atoi( buf );
    is >> buf;
  }

  if( x._dim != i ) {
    throw( IntArray::GenericException(
                                "istream& operator>>( istream& is, IntArray& x )",
                                "Incorrect number of components were read.") );
  }

  return is;
}

// this destructor should never be called 
#if  0
IntArray::~IntArray()
{

#ifdef OBJECT_DEBUG
  objectCount--;
#endif
}
#endif




