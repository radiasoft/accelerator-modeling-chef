#ifndef COMPLEXADDON_H
#define COMPLEXADDON_H

// This is written only to avoid constant warning messages
// about ambiguous operators.  It arises because of constructors
// which can change double and complex to matrices or jets.
// 
// Leo Michelotti
// Dec 16, 1994    (Battle of the Bulge + 50 years)

#include <math.h>
#include <complex>

#ifdef __GNUC__
  typedef complex<double> Complex;
#endif

// Check for Solaris.
#if defined(__SUNPRO_CC) 
#include <iostream.h>  
  typedef std::complex<double> Complex;
#endif

#ifdef __BORLANDC__
  typedef complex Complex;
#endif

#ifdef _MSC_VER
typedef std::complex<double> Complex;
using std::real;
using std::imag;
using std::exp;
using std::conj;
using std::cos;
using std::sin;
using std::pow;
using std::sqrt;
using  std::ostream;
using std::cerr;
using std::endl;
#endif 

#if defined(__SUNPRO_CC)

inline ostream& operator<<(ostream& os, const Complex& x) {
	os << "( " << real(x) << "," << imag(x) << " )" << endl;
	return os;
}

inline istream& operator>>(istream& is, const Complex& x) {
	is  >> real(x) >> imag(x);
	return is;
}

inline Complex operator+( const Complex& x, const double& y )
{
 return x + (Complex) y;
}

inline Complex operator+( const double& x,  const Complex& y )
{
 return (Complex) x + y;
}

inline Complex operator-( const Complex& x, const double&  y )
{
 return x - (Complex) y;
}

inline Complex operator-( const double& x,  const Complex& y )
{
 return (Complex) x - y;
}

inline Complex operator*( const Complex& x, const double& y )
{
 return x * (Complex) y;
}

inline Complex operator*( const double& x,  const Complex& y )
{
 return (Complex) x * y;
}

inline Complex operator/( const Complex& x, const double& y )
{
 return x / (Complex) y;
}

inline Complex operator/( const double& x,  const Complex& y )
{
 return (Complex) x / y;
}

inline double abs(const Complex& x) 
{
 return sqrt(real(x)*real(x) + imag(x)*imag(x));
}

inline double fabs( const Complex& x )
{
 return abs(x);
}

inline char operator>= ( const Complex& x, const Complex& y )
{
 return real(x) >= real(y)  &&  imag(x) >= imag(y);
}

inline char operator< ( const Complex& x, const Complex& y )
{
 return real(x) < real(y)  &&  imag(x) < imag(y);
}

inline char operator> ( const Complex& x, const Complex& y )
{
 return real(x) > real(y)  &&  imag(x) > imag(y);
}

#endif // __sparc and not __GNUG__

#ifdef __GNUC__
inline char operator>= ( const Complex& x, const Complex& y )
{
 return real(x) >= real(y)  &&  imag(x) >= imag(y);
}

inline char operator< ( const Complex& x, const Complex& y )
{
 return real(x) < real(y)  &&  imag(x) < imag(y);
}

inline char operator> ( const Complex& x, const Complex& y )
{
 return real(x) > real(y)  &&  imag(x) > imag(y);
}

#endif // __GNUC__

#ifdef __BORLANDC__

inline Complex pow ( const Complex& x, int n )
{
  return pow( x, (double) n );
}

inline double pow( const double& x, int n )
{
  return pow( x, (double) n );
}

inline bool operator==( const Complex& x, const double& y )
{
  if( ( imag(x) != 0.0 ) || ( real(x) != y ) )   return 0;
  else                                           return 1;
}

inline bool operator==( const double& y, const Complex& x )
{
  if( ( imag(x) != 0.0 ) || ( real(x) != y ) )   return 0;
  else                                           return 1;
}

inline bool operator!=( const Complex& x, const double& y )
{
  return !( operator==( x, y ) );
}

inline bool operator!=( const double& y, const Complex& x )
{
  return !( operator==( x, y ) );
}

inline char operator>= ( const Complex& x, const Complex& y )
{
 return real(x) >= real(y)  &&  imag(x) >= imag(y);
}

inline char operator< ( const Complex& x, const Complex& y )
{
 return real(x) < real(y)  &&  imag(x) < imag(y);
}

inline char operator> ( const Complex& x, const Complex& y )
{
 return real(x) > real(y)  &&  imag(x) > imag(y);
}

#endif // __BORLANDC__

#ifdef _MSC_VER

inline double abs(const Complex& x) {
	return sqrt(real(x)*real(x) + imag(x)*imag(x));
}

//ostream& operator<<(ostream& os, const Complex& x) {
//	os << "( " << real(x) << "," << imag(x) << " )" << std::endl;
//	return os;
//}
inline char operator>= ( const Complex& x, const Complex& y )
{
 return real(x) >= real(y)  &&  imag(x) >= imag(y);
}
inline char operator> ( const Complex& x, const Complex& y )
{
 return real(x) > real(y)  &&  imag(x) > imag(y);
}

#endif // __VISUAL_CPP__

const Complex complex_1( 1.0, 0.0 );
const Complex complex_0( 0.0, 0.0 );
const Complex complex_i( 0.0, 1.0 );

#endif // COMPLEXADDON_H
