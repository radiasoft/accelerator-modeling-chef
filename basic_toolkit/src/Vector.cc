/*
 * Implementation of class Vector
 * 
 * Version 1.0       Sep, 1991
 *         2.0       Mar, 1995
 *         2.1       Apr, 1995
 * 
 * Leo Michelotti
 * michelot@calvin.fnal.gov
 * 
 */


#include <string.h>   // for memcpy()
#include "VectorD.h"

#ifdef __VISUAL_CPP__
using std::cout;
#include <iomanip>
using std::setw;
using std::setprecision;
using std::resetiosflags;
using std::setiosflags;
#else
#include <stdlib.h>
#include <iomanip.h>
#endif


// ================================================================
//      Globals
//

#ifdef OBJECT_DEBUG
int Vector::objectCount = 0;
#endif

OutputFormat* Vector::defOFPtr = 0;

void Vector::setDefaultFormat( const OutputFormat& x )
{
  if( ( (OutputFormat*) 0 ) != Vector::defOFPtr ) {
    delete Vector::defOFPtr;
  }
  defOFPtr = new OutputFormat( x );
}

// ================================================================
//      Constructors and the destructor ...
//

#define CHECKOUT(test,fcn,message)          \
  if( test ) {                              \
    cout << "\n\n"                          \
            "*** ERROR ***\n"               \
            "*** ERROR ***" fcn "\n"        \
            "*** ERROR ***" message "\n"    \
            "*** ERROR ***\n"               \
            "*** ERROR ***"                 \
         << endl;                           \
         exit(1);                           \
  }

Vector::Vector( int n, const double* x, OutputFormat* q )
{
  int i;

  CHECKOUT(n <= 0, "Vector::Vector", "Dimension must be positive.")

  dim = n;
  comp = new double [ dim ];
  if( x ) for ( i = 0; i < dim; i++ ) comp[i] = x[i];
  else    for ( i = 0; i < dim; i++ ) comp[i] = 0.0;

  ofPtr = q;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

Vector::Vector( const Vector& x )
{
  dim = x.dim;
  comp = new double [ dim ];
  for ( int i = 0; i < dim; i++ ) comp[i] = x.comp[i];

  ofPtr = x.ofPtr;  // Note: shallow copy of output format

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

Vector::~Vector()
{
  delete [] comp;

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}



// Assignment ...

void Vector::Set( const double* x )
{
  for ( int i = 0; i < dim; i++ ) comp[i] = x[i];
}

void Vector::set( double x, double y, double z )
{
  // ??? REMOVE: static char firstTime = 1;
  // ??? REMOVE: if( firstTime ) {
  // ??? REMOVE:   firstTime = 0;
  // ??? REMOVE:   cerr << "\n"
  // ??? REMOVE:           "*** WARNING ***                                 \n"
  // ??? REMOVE:           "*** WARNING *** Vector::set( double, double, double ) \n"
  // ??? REMOVE:           "*** WARNING *** This function is obsolete.      \n"
  // ??? REMOVE:           "*** WARNING ***                                 \n"
  // ??? REMOVE:        << endl;
  // ??? REMOVE: }
  if( dim >= 3 ) {
    comp[0] = x;
    comp[1] = y;
    comp[2] = z;
  }
  else for( int i = 0; i < dim; i++ ) comp[i] = 0.0;
}

double& Vector::operator() ( int i ) const
{
  static double dummy = - 1. / 137.;
  if ( ( 0 <= i ) && ( i < dim ) ) return comp[i];
  else                             return dummy;
}


// Algebraic functions ...

Vector& Vector::operator= ( const Vector& x )
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "Vector::operator=", "Incompatible dimensions.")
#endif
  memcpy((void *)comp, (const void *)x.comp, dim * sizeof(double));
  return *this;
}


Vector Vector::operator+ ( const Vector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "Vector::operator+", "Incompatible dimensions.")
#endif

  Vector z( dim );
  for ( int i = 0; i < dim; i++ ) 
    z.comp[i] = comp[i] + x.comp[i];
  return z;
}


Vector Vector::operator+= ( const Vector& x )
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "Vector::operator+=", "Incompatible dimensions.")
#endif

  for ( int i = 0; i < dim; i++ ) comp[i] += x.comp[i];
  return *this;
}


Vector Vector::operator- ( const Vector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "Vector::operator-", "Incompatible dimensions.")
#endif

  Vector z( dim );
  for ( int i = 0; i < dim; i++ ) 
    z.comp[i] = comp[i] - x.comp[i];
  return z;
}


Vector operator- ( const Vector& x )
{
  Vector z( x.dim );
  for ( int i = 0; i < x.dim; i++ ) 
    z.comp[i] = - x.comp[i];
  return z;
}


Vector Vector::operator-= ( const Vector& x )
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "Vector::operator-=", "Incompatible dimensions.")
#endif

  for ( int i = 0; i < dim; i++ ) comp[i] -= x.comp[i];
  return *this;
}


Vector Vector::operator* ( double c ) const
{
  Vector z( dim );
  for ( int i = 0; i < dim; i++ ) z.comp[i] = c * comp[i];
  return z;
}

Vector operator* ( double c, const Vector& x )
{
  Vector z( x.dim );
  for ( int i = 0; i < x.dim; i++ ) z.comp[i] = c * x.comp[i];
  return z;
}

Vector Vector::operator*=( double c ) {
 for( int i = 0; i < dim; i++ ) comp[i] *= c;
 return *this;
}


Vector Vector::operator/ ( double c ) const
{
 Vector z( dim );
 for( int i = 0; i < dim; i++ ) z.comp[i] = comp[i] / c;
 return z;
}

Vector Vector::operator/=( double c ) {
 for( int i = 0; i < dim; i++ ) comp[i] /= c;
 return *this;
}


double Vector::operator* ( const Vector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "Vector::operator*", "Dimensions incompatible.")
#endif

  double u;
  int i;
  u = 0.0;
  for ( i = 0; i < dim; i++ ) u += comp[i] * x.comp[i];
  return u;
}


Vector Vector::operator^ ( const Vector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT((dim != 3) || ( x.dim != 3 ),
           "Vector::operator^",
           "Dimension must be 3." )
#endif

  static Vector z( 3 );
  z.comp[ 0 ] = comp[ 1 ] * x.comp[ 2 ] - comp[ 2 ] * x.comp[ 1 ];
  z.comp[ 1 ] = comp[ 2 ] * x.comp[ 0 ] - comp[ 0 ] * x.comp[ 2 ];
  z.comp[ 2 ] = comp[ 0 ] * x.comp[ 1 ] - comp[ 1 ] * x.comp[ 0 ];
  return z;
}



// Boolean functions ...

char Vector::operator== ( const Vector& x ) const
{
  if( dim != x.dim ) return 0;
  for( int i = 0; i < dim; i++ ) 
    if( comp[i] != x.comp[i] ) return 0;
  return 1;
}

char Vector::operator!= ( const Vector& x ) const
{
  return !( operator==( x ) );
}

char Vector::operator< ( const Vector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "Vector::operator<", "Dimensions incompatible.")
#endif
  for( int i = 0; i < dim; i++ ) if( comp[i] >= x.comp[i] ) return 0;
  return 1;
}

char Vector::operator<= ( const Vector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "Vector::operator<=", "Dimensions incompatible.")
#endif
  for( int i = 0; i < dim; i++ ) if( comp[i] > x.comp[i] ) return 0;
  return 1;
}

char Vector::operator> ( const Vector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "Vector::operator>", "Dimensions incompatible.")
#endif
  for( int i = 0; i < dim; i++ ) if( comp[i] <= x.comp[i] ) return 0;
  return 1;
}

char Vector::operator>= ( const Vector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "Vector::operator>=", "Dimensions incompatible.")
#endif
  for( int i = 0; i < dim; i++ ) if( comp[i] < x.comp[i] ) return 0;
  return 1;
}

char Vector::IsNull() const
{
  int i = -1;
  while ( ++i < dim ) if( comp[i] != 0.0 ) return 0;
  return 1;
}

char Vector::IsUnit() const
{
  double x = 0.0;
  for( int i = 0; i < dim; i++ ) x += comp[i]*comp[i];
  return x == 1.0;
}



// Utilities ..

Vector Vector::Abs() const
{
  Vector z( dim );
  for( int i = 0; i < dim; i++ ) z.comp[i] = fabs( comp[i] );
  return z;
}

double Vector::Norm () const
{
  double x;
  x = 0.0;
  for ( int i = 0; i < dim; i++ ) x += comp[i]*comp[i];
  return sqrt(x);
}

Vector Vector::Unit () const
{
  double x = Norm();
  Vector z( dim );

  for ( int i = 0; i < dim; i++ ) z.comp[i] = comp[i] / x;
  return z;
}


void Vector::Rotate ( Vector& v, double theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((dim != 3) || ( v.dim != 3 ),
           "Vector::Rotate",
           "Dimension must be 3." )
#endif

  double c, s;
  Vector e, u;

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v.comp[i] = u.comp[i];
}


ostream& operator<<( ostream& os, const Vector& v )
{
  static OutputFormat* q;
  q = v.ofPtr;
  if( ( (OutputFormat*) 0 ) == q  ) {
    q = Vector::defOFPtr;
  }

  if( ( (OutputFormat*) 0 ) == q  ) {
    os << "( ";
    for( int i = 0; i < v.dim - 1; i++ )
      os << v.comp[i] << ", ";
    os << v.comp[ v.dim - 1 ] << " )";
    return os;
  }

  else {
    os << "( ";
    for( int i = 0; i < v.dim - 1; i++ ) {
      os << setw(q->width) 
         << setprecision(q->precision) 
         << resetiosflags(014002 | 04) 
         << setiosflags(q->flags) 
         << v.comp[i] << ", "
         << q->getPadding();
    }
    os << setw(q->width) 
       << setprecision(q->precision) 
       << resetiosflags(014002 | 04) 
       << setiosflags(q->flags) 
       << v.comp[ v.dim - 1 ] << " )";

    return os;
  }
}


// Utilities

Vector operator*( const MatrixD& A, const Vector& x )
{
  static int r; 
  static int c;
  static int i;
  static int j;

  r = A.rows();
  c = A.cols();
  if( c != x.Dim() ) {
    cout << "*** ERROR ***                                   \n" 
         << "*** ERROR *** Vector operator*( MatrixD, Vector ) \n" 
         << "*** ERROR *** Incompatible dimensions.          \n" 
         << "*** ERROR ***                                   \n" 
         << endl;
    exit(1);
  }
  Vector z( r );
  for( i = 0; i < r; i++ ) {
    z(i) = 0.0;
    for( j = 0; j < c; j++ ) {
      z(i) += A( i, j ) * x(j);
    }
  }
  return z;
}

