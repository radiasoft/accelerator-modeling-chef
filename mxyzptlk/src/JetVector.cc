/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      JetVector.cc
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

/*
 * Implementation of class JetVector
 * 
 * Version 1.0       April, 1995
 * 
 * Leo Michelotti
 * michelot@calvin.fnal.gov
 * 
 */



#include "JetVector.h"
#include "JL.h"

#include <iomanip>

using namespace std;

// ================================================================
//      Global variables
//

#ifdef OBJECT_DEBUG
int JetVector::objectCount = 0;
#endif

// ================================================================
//      Constructors and the destructor ...
//

#define CHECKOUT(test,fcn,message)    \
  if( test ) {                        \
    throw( JL::GenericException       \
           ( __FILE__, __LINE__,      \
             fcn, message        ) ); \
  }

JetVector::JetVector( const int& n, 
                      const Jet* x, 
                      const Jet__environment* pje )
{
  int i;

  CHECKOUT(n <= 0, "JetVector::JetVector", "Dimension must be positive.")
  CHECKOUT(pje == 0, "JetVector::JetVector", "Environment is required.")

  myEnv = (Jet__environment*) pje;
  dim = n;
  comp = new Jet [ dim ];

  if( x ) for ( i = 0; i < dim; i++ ) comp[i] = x[i];
  else    for ( i = 0; i < dim; i++ ) {
    comp[i].setEnvTo( pje );
    comp[i] = 0.0;    // ??? Unnecessary?
  }

#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}

JetVector::JetVector( const JetVector& x )
{
  static int i;

  myEnv = x.myEnv;
  dim = x.dim;
  comp = new Jet [ dim ];

  for ( i = 0; i < dim; i++ ) {
    comp[i] = x.comp[i];    // Shallow copy is automatic because
                            // of lazy evaluation.
    #ifndef NOCHECKS
    if( comp[i].Env() != myEnv ) {
      cerr << "\n" 
           << "*** ERROR ***                                           \n" 
              "*** ERROR *** JetVector::JetVector( JetVector& )        \n"          
              "*** ERROR *** Inconsistent environment at component "
           << i << "\n"
              "*** ERROR *** "
           << (int) comp[i].Env() << " != " << (int) myEnv << "\n"
              "*** ERROR ***                                           \n" 
           << endl;
      exit(0);
    }
    #endif
  }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JetVector::~JetVector()
{
  delete [] comp;

#ifdef OBJECT_DEBUG
  objectCount--;
#endif
}



// Assignment ...

void JetVector::Set( const Jet* x )
{
  Jet__environment* pje;
  pje = x[0].Env();
#ifndef NOCHECKS
    CHECKOUT(pje != myEnv, "JetVector::Set", "Wrong environment.")
#endif
  for ( int i = 0; i < dim; i++ ) {
#ifndef NOCHECKS
    CHECKOUT(x[i].Env() != pje, "JetVector::Set", "Inequivalent environments.")
#endif
    comp[i] = x[i];
  }
}

void JetVector::SetComponent( const int& i, const Jet& x )
{
#ifndef NOCHECKS
    CHECKOUT(x.Env() != myEnv, "JetVector::Set", "Wrong environment.")
#endif
    comp[i] = x;
}

Jet& JetVector::operator() ( const int& i ) const
{
#ifndef NOCHECKS
    CHECKOUT(!(( 0 <= i ) && ( i < dim )), "JetVector::operator()", "Argument out of range")
#endif
    return comp[i];
}


// Algebraic functions ...

JetVector& JetVector::operator= ( const JetVector& x )
{
// #ifndef NOCHECKS
//   CHECKOUT(dim   != x.dim, "JetVector::operator=", "Incompatible dimensions.")
//   CHECKOUT(myEnv != x.myEnv, "JetVector::operator=", "Incompatible environments.")
// #endif

  Reconstruct( x.dim, x.myEnv );

  for ( int i = 0; i < dim; i++ ) comp[i] = x.comp[i];
  return *this;
}


JetVector JetVector::operator+ ( const JetVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "JetVector::operator+", "Incompatible dimensions.")
  CHECKOUT(myEnv != x.myEnv, "JetVector::operator+", "Incompatible environments.")
#endif

  JetVector z( dim, 0, myEnv );
  for ( int i = 0; i < dim; i++ ) 
    z.comp[i] = comp[i] + x.comp[i];
  return z;
}


JetVector operator+ ( const JetVector& x, const Vector& y )
{
#ifndef NOCHECKS
  CHECKOUT(x.dim != y.Dim(), "JetVector::operator+", "Incompatible dimensions.")
#endif

  JetVector z( x );
  for ( int i = 0; i < x.dim; i++ ) 
    z.comp[i] += y(i);
  return z;
}


JetVector operator+ ( const Vector& y, const JetVector& x )
{
#ifndef NOCHECKS
  CHECKOUT(x.dim != y.Dim(), "JetVector::operator+", "Incompatible dimensions.")
#endif

  JetVector z( x );
  for ( int i = 0; i < x.dim; i++ ) 
    z.comp[i] += y(i);
  return z;
}


JetVector JetVector::operator+= ( const JetVector& x )
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "JetVector::operator+=", "Incompatible dimensions.")
  CHECKOUT(myEnv != x.myEnv, "JetVector::operator+=", "Incompatible environments.")
#endif

  for ( int i = 0; i < dim; i++ ) comp[i] += x.comp[i];
  return *this;
}


JetVector JetVector::operator+= ( const Vector& x )
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.Dim(), "JetVector::operator-=", "Incompatible dimensions.")
#endif

  for ( int i = 0; i < dim; i++ ) 
    comp[i] += x(i);
  return *this;
}


JetVector JetVector::operator- ( const JetVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "JetVector::operator-", "Incompatible dimensions.")
  CHECKOUT(myEnv != x.myEnv, "JetVector::operator-", "Incompatible environments.")
#endif

  JetVector z( dim, 0, myEnv );
  for ( int i = 0; i < dim; i++ ) 
    z.comp[i] = comp[i] - x.comp[i];
  return z;
}


JetVector operator- ( const JetVector& x, const Vector& y )
{
#ifndef NOCHECKS
  CHECKOUT(x.dim != y.Dim(), "JetVector::operator-", "Incompatible dimensions.")
#endif

  JetVector z( x );
  for ( int i = 0; i < x.dim; i++ ) 
    z.comp[i] -= y(i);
  return z;
}


JetVector operator- ( const Vector& y, const JetVector& x )
{
#ifndef NOCHECKS
  CHECKOUT(x.dim != y.Dim(), "JetVector::operator-", "Incompatible dimensions.")
#endif

  JetVector z( x );
  for ( int i = 0; i < x.dim; i++ ) {
    z.comp[i] -= y(i);
    z.comp[i].Negate();
  }
  return z;
}


JetVector operator- ( const JetVector& x )
{
  JetVector z( x );
  for ( int i = 0; i < x.dim; i++ ) 
    z.comp[i].Negate();
  return z;
}


JetVector JetVector::operator-= ( const JetVector& x )
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "JetVector::operator-=", "Incompatible dimensions.")
  CHECKOUT(myEnv != x.myEnv, "JetVector::operator-=", "Incompatible environments.")
#endif

  for ( int i = 0; i < dim; i++ ) 
    comp[i] -= x.comp[i];
  return *this;
}


JetVector JetVector::operator-= ( const Vector& x )
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.Dim(), "JetVector::operator-=", "Incompatible dimensions.")
#endif

  for ( int i = 0; i < dim; i++ ) 
    comp[i] -= x(i);
  return *this;
}


JetVector JetVector::operator* ( const Jet& c ) const
{
  JetVector z( dim, 0, myEnv );
  for ( int i = 0; i < dim; i++ ) z.comp[i] = c * comp[i];
  return z;
}

JetVector JetVector::operator* ( double c ) const
{
  JetVector z( dim, 0, myEnv );
  for ( int i = 0; i < dim; i++ ) z.comp[i] = c * comp[i];
  return z;
}

JetVector operator* ( const Jet& c, const JetVector& x )
{
  JetVector z( x.dim, 0, x.myEnv );
  for ( int i = 0; i < x.dim; i++ ) z.comp[i] = c * x.comp[i];
  return z;
}

JetVector operator* ( double c, const JetVector& x )
{
  JetVector z( x.dim, 0, x.myEnv );
  for ( int i = 0; i < x.dim; i++ ) z.comp[i] = c * x.comp[i];
  return z;
}

JetVector operator* ( const Jet& c, const Vector& x )
{
  static int d, i;
  d = x.Dim();
  JetVector z( d, 0, c.Env() );
  for ( i = 0; i < d; i++ ) z.SetComponent( i, c * x(i) );
  return z;
}

JetVector JetVector::operator*=( const Jet& c ) {
 for( int i = 0; i < dim; i++ ) comp[i] *= c;
 return *this;
}


JetVector JetVector::operator*=( double c ) {
 for( int i = 0; i < dim; i++ ) comp[i] *= c;
 return *this;
}


JetVector JetVector::operator/ ( const Jet& c ) const
{
 JetVector z( *this );  // ??? Can be speeded up by negate function.
 for( int i = 0; i < dim; i++ ) z.comp[i] /= c;
 return z;
}

JetVector JetVector::operator/ ( double c ) const
{
 JetVector z( *this );
 for( int i = 0; i < dim; i++ ) z.comp[i] /= c;
 return z;
}

JetVector JetVector::operator/=( const Jet& c ) {
 for( int i = 0; i < dim; i++ ) comp[i] /= c;
 return *this;
}

JetVector JetVector::operator/=( double c ) {
 for( int i = 0; i < dim; i++ ) comp[i] /= c;
 return *this;
}


Jet JetVector::operator* ( const JetVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(myEnv != x.myEnv, "JetVector::operator*", "Incompatible environments.")
  CHECKOUT(dim != x.dim, "JetVector::operator*", "Incompatible dimensions.")
#endif

  Jet u( myEnv );
  u = 0.0;
  int i;
  for ( i = 0; i < dim; i++ ) u += comp[i] * x.comp[i];
  return u;
}


Jet operator* ( const JetVector& x, const Vector& y )
{
#ifndef NOCHECKS
  CHECKOUT(x.dim != y.Dim(), "JetVector::operator*", "Incompatible dimensions.")
#endif

  static int i;
  Jet u( x.myEnv );
  u = 0.0;
  for ( i = 0; i < x.dim; i++ ) u += x.comp[i] * y(i);
  return u;
}


Jet operator* ( const Vector& y, const JetVector& x )
{
#ifndef NOCHECKS
  CHECKOUT(x.Dim() != y.Dim(), "JetVector::operator*", "Incompatible dimensions.")
#endif

  static int i;
  Jet u( x.myEnv );
  u = 0.0;
  for ( i = 0; i < x.dim; i++ ) u += x.comp[i] * y(i);
  return u;
}


JetVector JetVector::operator^ ( const JetVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT((dim != 3) || ( x.dim != 3 ),
           "JetVector::operator^",
           "Dimension must be 3." )
#endif

  JetVector z( 3, 0, myEnv );
  z.comp[ 0 ] = comp[ 1 ] * x.comp[ 2 ] - comp[ 2 ] * x.comp[ 1 ];
  z.comp[ 1 ] = comp[ 2 ] * x.comp[ 0 ] - comp[ 0 ] * x.comp[ 2 ];
  z.comp[ 2 ] = comp[ 0 ] * x.comp[ 1 ] - comp[ 1 ] * x.comp[ 0 ];
  return z;
}


JetVector JetVector::operator^ ( Vector x ) const
{
#ifndef NOCHECKS
  CHECKOUT((dim != 3) || ( x.Dim() != 3 ),
           "JetVector::operator^",
           "Dimension must be 3." )
#endif

  JetVector z( 3, 0, myEnv );
  z.comp[ 0 ] = comp[ 1 ] * x( 2 ) - comp[ 2 ] * x( 1 );
  z.comp[ 1 ] = comp[ 2 ] * x( 0 ) - comp[ 0 ] * x( 2 );
  z.comp[ 2 ] = comp[ 0 ] * x( 1 ) - comp[ 1 ] * x( 0 );
  return z;
}

JetVector operator^ ( Vector y, const JetVector& x )
{
#ifndef NOCHECKS
  CHECKOUT(( y.Dim() != 3 ) || ( x.dim != 3 ),
           "JetVector::operator^",
           "Dimension must be 3." )
#endif

  JetVector z( 3, 0, x.Env() );
  z.comp[ 0 ] = y( 1 ) * x.comp[ 2 ] - y( 2 ) * x.comp[ 1 ];
  z.comp[ 1 ] = y( 2 ) * x.comp[ 0 ] - y( 0 ) * x.comp[ 2 ];
  z.comp[ 2 ] = y( 0 ) * x.comp[ 1 ] - y( 1 ) * x.comp[ 0 ];
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetVector operator*(  /* const */ MatrixD& M, 
                      const JetVector& x ) {
 static JetVector z;
 static int i, j, r, c;
 static Jet__environment* pje;
 static double temp;    // ??? I don't like this A LOT!!!  It's needed because 
                        // M(i,j) is not a const operation
                        // for the matrix element.

 // Paranoid initializations
 i = j = 0;
 pje = x.myEnv;
 z.Reconstruct( M.rows(), pje );
 r = M.rows();
 c = M.cols();

 if( M.cols() != x.dim ) {
   throw( JL::GenericException( __FILE__, __LINE__, 
          "JetVector operator*(  /* const */ MatrixD&, const JetVector& )",
          "Rows and/or columns of the matrix are not correct." ) );
 }

 for( i = 0; i < r; i++ ) {
  temp = M( i, 0 );
  z.comp[i] = temp * x.comp[0];
  j = 1;
  while( j < c )   { 
    temp = M( i, j );
    z.comp[i] += temp * ( x.comp[j++] ) ;
  }
 }

 return z;
}


// Boolean functions |||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetVector::operator== ( const JetVector& x ) const
{
  if( ( dim != x.dim ) || myEnv != x.myEnv ) return 0;
  for( int i = 0; i < dim; i++ ) 
    if( comp[i] != x.comp[i] ) return 0;
  return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetVector::operator==( const double& x ) const 
{
 for( int i = 0; i < dim; i++ )
   if( comp[i] != x ) return 0;     // ??? WHAT???
 return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetVector::operator!= ( const JetVector& x ) const
{
  return !( operator==( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetVector::operator!=( const double& x ) const 
{
 return !( this->operator==( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetVector::operator< ( const JetVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(myEnv != x.myEnv, "JetVector::operator<", "Incompatible environments.")
  CHECKOUT(dim != x.dim, "JetVector::operator<", "Incompatible dimensions.")
#endif

  for( int i = 0; i < dim; i++ ) 
    if( comp[i].standardPart() >= x.comp[i].standardPart() ) return 0;
  return 1;
}

char JetVector::operator<= ( const JetVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(myEnv != x.myEnv, "JetVector::operator<", "Incompatible environments.")
  CHECKOUT(dim != x.dim, "JetVector::operator<", "Incompatible dimensions.")
#endif

  for( int i = 0; i < dim; i++ ) 
    if( comp[i].standardPart() > x.comp[i].standardPart() ) return 0;
  return 1;
}

char JetVector::operator> ( const JetVector& x ) const
{
  return !( operator<=( x ) );
}

char JetVector::operator>= ( const JetVector& x ) const
{
  return !( operator<( x ) );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetVector::IsNull() const
{
  int i = -1;
  while ( ++i < dim ) if( comp[i] != 0.0 ) return 0;
  return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetVector::IsUnit() const
{
  double x = 0.0;
  for( int i = 0; i < dim; i++ ) 
    x += comp[i].standardPart()*comp[i].standardPart();
  return x == 1.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetVector::IsNilpotent() const {
 for( int i = 0; i <  myEnv->SpaceDim; i++ ) 
  if( !(comp[i].isNilpotent()) ) return 0;
 return 1;
}



// Utilities ..
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 
JetVector::Reconstruct()
{
  delete [] comp;
  comp = new Jet [ dim ];
  for ( int i = 0; i < dim; i++ ) {
    comp[i].setEnvTo( myEnv );
    comp[i].Reconstruct();
  }
}


void 
JetVector::Reconstruct( const int& n, Jet__environment* pje )
{
  delete [] comp;
  dim = n;
  myEnv = pje;
  comp = new Jet [ dim ];
  for ( int i = 0; i < dim; i++ ) {
    comp[i].Reconstruct( pje );
  }
}


void 
JetVector::CopyFrom( const JetVector& x )
{
  this->Reconstruct( x.Dim(), x.Env() );
  this->operator=( x );
}


void JetVector::peekAt() const
{
  cout << "\n\nBegin JetVector::peekAt() ......\n";
  for ( int i = 0; i < dim; i++ ) {
    cout << "JetVector::peekAt(): Component " << i << endl;
    comp[i].peekAt();
  }
  cout << "End JetVector::peekAt() ......\n" << endl;
}


void JetVector::printCoeffs() const
{
  int i;
  cout << "\n\nBegin JetVector::printCoeffs() ......\n"
       << "Dimension: " << dim 
       << ", Weight = " << Weight()
       << ", Max accurate weight = " << AccuWgt() 
       << endl;
  cout << "JetVector reference point: " 
       << endl;
  for( i = 0; i < myEnv->NumVar; i++ ) 
    cout << setw(20) << setprecision(12) 
         << myEnv->refPoint[i];
  cout << "\n" << endl;

  for ( i = 0; i < dim; i++ ) {
    cout << "JetVector::printCoeffs(): Component " << i << endl;
    comp[i].printCoeffs();
  }
  cout << "End JetVector::printCoeffs() ......\n" << endl;
}


Jet JetVector::Norm () const
{
  Jet x( myEnv );
  x = 0.0;
  for ( int i = 0; i < dim; i++ ) x += comp[i]*comp[i];
  return sqrt(x);
}

JetVector JetVector::Unit () const
{
  Jet x( myEnv );
  x = Norm();
  JetVector z( *this );
  for ( int i = 0; i < dim; i++ ) z.comp[i] /= x;
  return z;
}


void JetVector::Rotate ( JetVector& v, double theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((dim != 3) || ( v.dim != 3 ),
           "JetVector::Rotate",
           "Dimension must be 3." )
#endif

  double c, s;
  JetVector e( 3, 0, myEnv ), u( 3, 0, myEnv);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v.comp[i] = u.comp[i];
}


void JetVector::Rotate ( JetVector& v, const Jet& theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((dim != 3) || ( v.dim != 3 ),
           "JetVector::Rotate",
           "Dimension must be 3." )
  CHECKOUT((myEnv != v.myEnv)||(myEnv != theta.Env()), 
           "JetVector::Rotate", 
           "Incompatible environments.")
#endif

  Jet c( myEnv ), s( myEnv );
  JetVector e( 3, 0, myEnv), u( 3, 0, myEnv);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v.comp[i] = u.comp[i];
}


ostream& operator<<( ostream& os, const JetVector& v )
{
  os << "Begin JetVector:" << endl;
  os << v.dim << endl;
  os << *(v.myEnv);
  for( int i = 0; i < v.dim; i++ ) {
    os << "\nJetVector component " << i << ":" << endl;
    os << v.comp[i];
  }
  os << "\nEnd JetVector." << endl;
  return os;
}


istream& operator>>( istream& is, JetVector& v )
{
  static char buf[100];
  is >> buf;
  is >> buf;
  is >> v.dim;
  CHECKOUT(v.dim <= 0, "JetVector::JetVector", "Dimension must be positive.")

  streamIn( is, &(v.myEnv) );

  if( v.comp ) delete [] v.comp;
  v.comp = new Jet [ v.dim ];

  for ( int i = 0; i < v.dim; i++ ) {
    is >> buf;
    is >> buf;
    is >> buf;
    is >> v.comp[i];
  }

  is >> buf;
  is >> buf;

  return is;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetVector JetVector::filter( MX_R_FUNCPTR f [] ) {
 JetVector z( dim, 0, myEnv );
 int i;

 for( i = 0; i < dim; i++ )
  z.comp[i] = comp[i].filter( f[i] );

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetVector JetVector::filter( int lower, int upper ) {
 JetVector z( dim, 0, myEnv );
 int i;        

 for( i = 0; i < dim; i++ )
  z.comp[i] = comp[i].filter( lower, upper );

 return z;
}



// Operations related to differentiation |||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetVector::weightedDerivative( int* m, double* x ) {
 int i;
 for( i = 0; i < dim; i++ ) x[i] = comp[i].weightedDerivative( m );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetVector::derivative( int* m, double* x ) {
 int i;
 for( i = 0; i < dim; i++ ) x[i] = comp[i].derivative( m );
}



// Query functions |||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int 
JetVector::AccuWgt() const
{
  int accuWgt = myEnv->MaxWeight;
  for( int i = 0; i < dim; i++ ) {
   if( myEnv != comp[i].Env() ) {
     cerr << "\n\n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** JetVector::AccuWgt()              \n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** Inconsistent environments at      \n"
 	  << "*** WARNING *** index " << i << "                 \n"
 	  << "*** WARNING ***                                   \n"
 	  << endl;
   }
   if( accuWgt > comp[i].getAccuWgt() ) accuWgt = comp[i].getAccuWgt();
  }
  return accuWgt;
}

int 
JetVector::Weight()  const
{
  int weight = -1;
  for( int i = 0; i < dim; i++ ) {
   if( myEnv != comp[i].Env() ) {
     cerr << "\n\n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** JetVector::Weight()               \n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** Inconsistent environments at      \n"
 	  << "*** WARNING *** index " << i << "                 \n"
 	  << "*** WARNING ***                                   \n"
 	  << endl;
   }
   if( weight  < comp[i].getWeight()  ) weight  = comp[i].getWeight();
  }
  return weight;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetVector::standardPart( double* x ) const {
 int i;
 for( i = 0; i < dim; i++ ) 
   x[i] = comp[i].standardPart();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetVector::getReference( double* r ) const {
 int i;
 for( i = 0; i < myEnv->NumVar; i++ ) r[i] = myEnv->refPoint[i];
}

