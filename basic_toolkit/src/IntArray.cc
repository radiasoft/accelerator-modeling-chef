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
****** Sep 2006 ostiguy@fnal.gov
******  
****** - eliminated archaic "Reconstruct" members. 
******   Use placement new syntax instead.  
****** - added MonomialOrderPredicate member. 
******   (useful for debugging)
******
******  Feb 2007 ostiguy@fnal.gov
******  
****** - refactored implementation based on STL iterators/algorithms
******
******   
*************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <iostream>
#include <cstdlib>
#include <string>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/IntArray.h>

using FNAL::pcerr;
using FNAL::pcout;

using namespace std;


#define CHECKOUT(test,fcn,message)                       \
  if( test ) {                                           \
    throw( IntArray::GenericException( fcn, message ) ); \
  }


IntArray::GenericException::GenericException( const char* fcn, const char* msg )
: errorString( msg )
{
  static bool firstTime = true;
  if( firstTime ) {
(*pcerr) << "\n*** ERROR *** "
         << "\n*** ERROR *** " << fcn
         << "\n*** ERROR *** " << msg
         << "\n*** ERROR *** This message is printed only once." 
         << std::endl;
    firstTime = false;
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* IntArray::GenericException::what() const throw()
{
  return ("IntArray::GenericException: " + errorString).c_str();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

IntArray::IntArray( int n, int const* const x )
  : weight_(0), weight_is_valid_(false)
 {

  if ( n <=0 ) return;
  
  comp_.resize(n);

  if (x) 
      { std::copy( x, x+n, begin() ); }
  else
      { std::fill(begin(), end(), exponent_t() ); }

  Sum(); // this sets weight_ and   weight_is_valid to true;     
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

IntArray::IntArray( IntArray::const_iterator it1, IntArray::const_iterator it2 )
  : weight_(0), weight_is_valid_(false)
{
  comp_.resize( it2 - it1 );
  std::copy( it1, it2, begin() ); 
  Sum(); // this sets weight_ and   weight_is_valid to true;     

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Assignment ...

void IntArray::Set( int const* x, int n)
{

  comp_.resize( n );
  std::copy( x, x + n, begin() ); 

  Sum(); // this sets weight_ and   weight_is_valid to true;     

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void IntArray::Set( int x )
{

  std::fill( begin(), end(), x); 
  Sum(); // this sets weight_ and   weight_is_valid to true;     

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Algebraic functions ...

IntArray& IntArray::operator= ( IntArray const& x )
{
  if ( &x == this ) return *this;

  comp_            = x.comp_;
  weight_          = x.weight_;
  weight_is_valid_ = x.weight_is_valid_;

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Boolean functions ...

bool IntArray::operator== ( IntArray const& rhs ) const
{

  return std::equal( begin(), end(), rhs.begin() );

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool IntArray::PartialEqual( IntArray const& lhs, IntArray const& rhs, int idx1, int idx2 )
{
  
  //compares only entries between IntArray(idx1) and IntArray(idx2) (inclusively).
 
  return std::equal( lhs.begin()+idx1, lhs.begin()+idx2+1,  rhs.begin()+idx1 );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool IntArray::PartialLessThan( IntArray const& lhs, IntArray const& rhs, int idx1, int idx2 )
{
  
  //compares only entries between IntArray(idx1) and IntArray(idx2) (inclusively).
 
   return std::lexicographical_compare( lhs.begin()+idx1, lhs.begin()+idx2+1, rhs.begin()+idx1,  rhs.begin()+idx2+1, std::less<exponent_t>() );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool IntArray::operator!= ( IntArray const& x ) const
{
  return !( operator==( x ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool IntArray::operator< ( IntArray const& x ) const
{

  return std::lexicographical_compare( rbegin(), rend(), x.rbegin(),  x.rend(), std::less<exponent_t>()   );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool IntArray::operator<= ( IntArray const& x ) const
{

  return std::lexicographical_compare( rbegin(), rend(), x.rbegin(),  x.rend(), std::less_equal<exponent_t>() );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool IntArray::operator> ( IntArray const& x ) const
{

  return std::lexicographical_compare( rbegin(), rend(), x.rbegin(),  x.rend(), std::greater<exponent_t>() );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool IntArray::operator>= ( IntArray const& x ) const
{

  return std::lexicographical_compare( rbegin(), rend(), x.rbegin(), x.rend(), std::greater_equal<exponent_t>() );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool IntArray::operator== ( int x ) const
{

  for ( const_iterator it = begin();  it != end(); ++it ) 
  {
    if ( *it == x ) return false;
  }

  return true;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool IntArray::operator!= ( int x ) const
{
  return !( operator==( x ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

IntArray IntArray::operator+( IntArray const& y) const 
{
     
     IntArray ret( *this );

     std::transform( begin(), end(), y.begin(), ret.begin(), std::plus<exponent_t>());

     ret.weight_is_valid_ = false;  
     ret.Sum();  // forces recomputation of the weight
   
     return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool IntArray::IsNull() const
{

  for ( IntArray::const_iterator it  = begin();
	                         it != end(); ++it ) 
  {
    if ( *it != 0 ) return false;
  }   

  return true;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Streams

std::ostream& operator<<( std::ostream& os, const IntArray& x ) 
{
  os << "( ";
  for( int i = 0; i < x.Dim() - 1; i++ )
    os << (int) x.comp_[i] << ", ";
  os << (int) x.comp_[ x.Dim() - 1 ] << " )";
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::istream& operator>>( std::istream& is, IntArray& x )
{
  char buf[80];
  buf[79] = '\0';
  int i;

  is >> buf;
  if( buf[0] != '(' ) {
    is >> buf;
    throw( IntArray::GenericException(
                         "istream& operator>>( istream& is, IntArray& x )",
                         "Incorrect first character in line."  ) );
  }
  
  i = 0;
  is >> buf;

  while( buf[ strlen(buf) - 1 ] != ')' ) {
    x.comp_[i++] = atoi( buf );
    is >> buf;
  }

  if( x.Dim() != i ) {
    throw( IntArray::GenericException(
                                "istream& operator>>( istream& is, IntArray& x )",
                                "Incorrect number of components were read.") );
  }

  return is;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// The destructor should ***never be defined nor called*** !!!

#if  0
IntArray::~IntArray() { }
#endif
