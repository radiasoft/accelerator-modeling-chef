#ifndef COMPLEXADDON_H
#define COMPLEXADDON_H

// This is written only to avoid constant warning messages
// about ambiguous operators.  It arises because of constructors
// which can change double and complex to matrices or jets.
// 
// Leo Michelotti
// Dec 16, 1994    (Battle of the Bulge + 50 years)

#ifdef __GNUG__
  typedef complex<double> Complex;
#endif

#ifdef __sparc
  typedef complex Complex;
#endif

#ifdef __BORLAND_CPP__
  typedef complex Complex;
#endif

#ifdef __VISUAL_CPP__
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
#endif  // __VISUAL_CPP__

#ifdef __sparc 

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

#endif // __sparc 


#ifdef __BORLAND_CPP__

inline Complex pow ( const Complex& x, int n )
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

#endif // __BORLAND_CPP__

#ifdef __VISUAL_CPP__

inline double abs(const Complex& x) {
	return sqrt(real(x)*real(x) + imag(x)*imag(x));
}

ostream& operator<<(ostream& os, const Complex& x) {
	os << "( " << real(x) << "," << imag(x) << " )" << std::endl;
	return os;
}
inline char operator>= ( const Complex& x, const Complex& y )
{
 return real(x) >= real(y)  &&  imag(x) >= imag(y);
}
inline char operator> ( const Complex& x, const Complex& y )
{
 return real(x) > real(y)  &&  imag(x) > imag(y);
}

#endif // __BORLAND_CPP__

const Complex complex_1( 1.0, 0.0 );
const Complex complex_0( 0.0, 0.0 );
const Complex complex_i( 0.0, 1.0 );

#endif // COMPLEXADDON_H
