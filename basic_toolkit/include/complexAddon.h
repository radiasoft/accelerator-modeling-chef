/**************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      complexAddon.h
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


#ifndef COMPLEXADDON_H
#define COMPLEXADDON_H

// This is written only to avoid constant warning messages
// about ambiguous operators.  It arises because of constructors
// which can change double and complex to matrices or jets.
// 
// Leo Michelotti
// Dec 16, 1994    (Battle of the Bulge + 50 years)

#include <complex>
#include <cmath>
#if !(defined(__SUNPRO_CC) && defined(IOSTREAMH))
#include <iostream>
#endif

namespace FNAL {
typedef std::complex<double> Complex;
}
using std::real;
using std::imag;
using std::exp;
using std::conj;
using std::cos;
using std::sin;
using std::pow;
using std::sqrt;
using std::ostream;
using std::cerr;
using std::endl;

//#if defined(__SUNPRO_CC) 
//#ifdef __BORLANDC__
//#ifdef _MSC_VER


// -----------------------------------------------------------
// The following operators are used by the mxyzptlk library
// They define a partial ordering that is used to sort 
// DA variables. DO NOT modify this unless you understand
// what you are doing !

inline char operator>= ( const FNAL::Complex& x, const FNAL::Complex& y )
{
 return real(x) >= real(y)  &&  imag(x) >= imag(y);
}

inline char operator< ( const FNAL::Complex& x, const FNAL::Complex& y )
{
 return real(x) < real(y)  &&  imag(x) < imag(y);
}

inline char operator> ( const FNAL::Complex& x, const FNAL::Complex& y )
{
 return real(x) > real(y)  &&  imag(x) > imag(y);
}

// -----------------------------------------------------------

#if defined(__SUNPRO_CC) && defined(IOSTREAMH)
#include <iostream.h>
// this is a kludge to allow the std::complex type to be used in conjunction 
// with the old style AT&T iostream library
 

inline ostream& operator<<(ostream& os, const FNAL::Complex& x) {
	os << "( " << std::real(x) << "," << std::imag(x) << " )";
	return os;
}

inline istream& operator>>(istream& is, FNAL::Complex& x) {
 double u,v;
	is  >> u; 
        is  >> v;
        std::complex<double> z(u,v);
        x = z;
	return is;
}
#endif 


#ifdef __BORLANDC__

inline FNAL::Complex pow ( const FNAL::Complex& x, int n )
{
  return pow( x, (double) n );
}

inline double pow( const double& x, int n )
{
  return pow( x, (double) n );
}

inline bool operator==( const FNAL::Complex& x, const double& y )
{
  if( ( imag(x) != 0.0 ) || ( real(x) != y ) )   return 0;
  else                                           return 1;
}

inline bool operator==( const double& y, const FNAL::Complex& x )
{
  if( ( imag(x) != 0.0 ) || ( real(x) != y ) )   return 0;
  else                                           return 1;
}

inline bool operator!=( const FNAL::Complex& x, const double& y )
{
  return !( operator==( x, y ) );
}

inline bool operator!=( const double& y, const FNAL::Complex& x )
{
  return !( operator==( x, y ) );
}

#endif // __BORLANDC__

#ifdef _MSC_VER

inline double abs(const FNAL::Complex& x) {
	return sqrt(real(x)*real(x) + imag(x)*imag(x));
}

//ostream& operator<<(ostream& os, const FNAL::Complex& x) {
//	os << "( " << real(x) << "," << imag(x) << " )" << std::endl;
//	return os;
//}

#endif // _MSC_VER

const FNAL::Complex complex_1( 1.0, 0.0 );
const FNAL::Complex complex_0( 0.0, 0.0 );
const FNAL::Complex complex_i( 0.0, 1.0 );

#endif // COMPLEXADDON_H
