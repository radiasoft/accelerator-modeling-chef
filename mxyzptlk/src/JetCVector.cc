/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      JetCVector.cc
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
 * Implementation of class JetCVector
 * 
 * Version 1.0       April, 1996
 * 
 * Leo Michelotti
 * michelot@calvin.fnal.gov
 * 
 */

#include "JetCVector.h"
#include "JLC.h"

// ================================================================
//      Global variables
//

#ifdef OBJECT_DEBUG
int JetCVector::objectCount = 0;
#endif

using namespace std;
using FNAL::Complex;

// ================================================================
//      Constructors and the destructor ...
//

#define CHECKOUT(test,fcn,message)    \
  if( test ) {                        \
    throw( JLC::GenericException       \
           ( __FILE__, __LINE__,      \
             fcn, message        ) ); \
  }

JetCVector::JetCVector( const int& n, 
                        const JetC* x, 
                        const JetC__environment* pje )
{
  int i;

  CHECKOUT(n <= 0, "JetCVector::JetCVector", "Dimension must be positive.")
  CHECKOUT(pje == 0, "JetCVector::JetCVector", "Environment is required.")

  myEnv = (JetC__environment*) pje;
  dim = n;
  comp = new JetC [ dim ];

  if( x ) for ( i = 0; i < dim; i++ ) comp[i] = x[i];
  else    for ( i = 0; i < dim; i++ ) {
    comp[i].setEnvTo( pje );
    comp[i] = complex_0;    // ??? Unnecessary?
  }

#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}

JetCVector::JetCVector( const JetCVector& x )
{
  static int i;

  myEnv = x.myEnv;
  dim = x.dim;
  comp = new JetC [ dim ];

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

JetCVector::~JetCVector()
{
  delete [] comp;

#ifdef OBJECT_DEBUG
  objectCount--;
#endif
}



// Assignment ...

void JetCVector::Set( const JetC* x )
{
  JetC__environment* pje;
  pje = x[0].Env();
#ifndef NOCHECKS
    CHECKOUT(pje != myEnv, "JetCVector::Set", "Wrong environment.")
#endif
  for ( int i = 0; i < 3; i++ ) {
#ifndef NOCHECKS
    CHECKOUT(x[i].Env() != pje, "JetCVector::Set", "Inequivalent environments.")
#endif
    comp[i] = x[i];
  }
}

void JetCVector::SetComponent( const int& i, const JetC& x )
{
#ifndef NOCHECKS
    CHECKOUT(x.Env() != myEnv, "JetCVector::Set", "Wrong environment.")
#endif
    comp[i] = x;
}

JetC& JetCVector::operator() ( const int& i ) const
{
#ifndef NOCHECKS
    CHECKOUT(!(( 0 <= i ) && ( i < dim )), "JetCVector::operator()", "Argument out of range")
#endif
    return comp[i];
}


// Algebraic functions ...

JetCVector& JetCVector::operator= ( const JetCVector& x )
{
// #ifndef NOCHECKS
//   CHECKOUT(dim   != x.dim, "JetCVector::operator=", "Incompatible dimensions.")
//   CHECKOUT(myEnv != x.myEnv, "JetCVector::operator=", "Incompatible environments.")
// #endif

  Reconstruct( x.dim, x.myEnv );

  for ( int i = 0; i < dim; i++ ) comp[i] = x.comp[i];
  return *this;
}


JetCVector JetCVector::operator+ ( const JetCVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "JetCVector::operator+", "Incompatible dimensions.")
  CHECKOUT(myEnv != x.myEnv, "JetCVector::operator+", "Incompatible environments.")
#endif

  JetCVector z( dim, 0, myEnv );
  for ( int i = 0; i < dim; i++ ) 
    z.comp[i] = comp[i] + x.comp[i];
  return z;
}


JetCVector JetCVector::operator+= ( const JetCVector& x )
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "JetCVector::operator+=", "Incompatible dimensions.")
  CHECKOUT(myEnv != x.myEnv, "JetCVector::operator+=", "Incompatible environments.")
#endif

  for ( int i = 0; i < dim; i++ ) comp[i] += x.comp[i];
  return *this;
}


JetCVector JetCVector::operator- ( const JetCVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "JetCVector::operator-", "Incompatible dimensions.")
  CHECKOUT(myEnv != x.myEnv, "JetCVector::operator-", "Incompatible environments.")
#endif

  JetCVector z( dim, 0, myEnv );
  for ( int i = 0; i < dim; i++ ) {
    z.comp[i] = comp[i] - x.comp[i];
  }
  return z;
}


JetCVector operator- ( const JetCVector& x )
{
  JetCVector z( x );
  for ( int i = 0; i < x.dim; i++ ) 
    z.comp[i].Negate();
  return z;
}


JetCVector JetCVector::operator-= ( const JetCVector& x )
{
#ifndef NOCHECKS
  CHECKOUT(dim != x.dim, "JetCVector::operator-=", "Incompatible dimensions.")
  CHECKOUT(myEnv != x.myEnv, "JetCVector::operator-=", "Incompatible environments.")
#endif

  for ( int i = 0; i < dim; i++ ) 
    comp[i] -= x.comp[i];
  return *this;
}


JetCVector JetCVector::operator* ( const JetC& c ) const
{
  JetCVector z( dim, 0, myEnv );
  for ( int i = 0; i < dim; i++ ) z.comp[i] = c * comp[i];
  return z;
}

JetCVector JetCVector::operator* ( const Complex& c ) const
{
  JetCVector z( dim, 0, myEnv );
  for ( int i = 0; i < dim; i++ ) z.comp[i] = c * comp[i];
  return z;
}

JetCVector JetCVector::operator* ( const double& c ) const
{
  JetCVector z( dim, 0, myEnv );
  for ( int i = 0; i < dim; i++ ) z.comp[i] = c * comp[i];
  return z;
}

JetCVector operator* ( const JetC& c, const JetCVector& x )
{
  JetCVector z( x.dim, 0, x.myEnv );
  for ( int i = 0; i < x.dim; i++ ) z.comp[i] = c * x.comp[i];
  return z;
}

JetCVector operator* ( const Complex& c, const JetCVector& x )
{
  JetCVector z( x.dim, 0, x.myEnv );
  for ( int i = 0; i < x.dim; i++ ) z.comp[i] = c * x.comp[i];
  return z;
}

JetCVector operator* ( const double& c, const JetCVector& x )
{
  JetCVector z( x.dim, 0, x.myEnv );
  for ( int i = 0; i < x.dim; i++ ) z.comp[i] = c * x.comp[i];
  return z;
}

JetCVector JetCVector::operator*=( const JetC& c ) {
 for( int i = 0; i < dim; i++ ) comp[i] *= c;
 return *this;
}


JetCVector JetCVector::operator*=( Complex c ) {
 for( int i = 0; i < dim; i++ ) comp[i] *= c;
 return *this;
}


JetCVector JetCVector::operator/ ( const JetC& c ) const
{
 JetCVector z( *this );  // ??? Can be speeded up by negate function.
 for( int i = 0; i < dim; i++ ) z.comp[i] /= c;
 return z;
}

JetCVector JetCVector::operator/ ( Complex c ) const
{
 JetCVector z( *this );
 for( int i = 0; i < dim; i++ ) z.comp[i] /= c;
 return z;
}

JetCVector JetCVector::operator/=( const JetC& c ) {
 for( int i = 0; i < dim; i++ ) comp[i] /= c;
 return *this;
}

JetCVector JetCVector::operator/=( Complex c ) {
 for( int i = 0; i < dim; i++ ) comp[i] /= c;
 return *this;
}


JetC JetCVector::operator* ( const JetCVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(myEnv != x.myEnv, "JetCVector::operator*", "Incompatible environments.")
  CHECKOUT(dim != x.dim, "JetCVector::operator*", "Incompatible dimensions.")
#endif

  JetC u( myEnv );
  u = complex_0;
  int i;
  for ( i = 0; i < dim; i++ ) u += comp[i] * x.comp[i];
  return u;
}


JetCVector JetCVector::operator^ ( const JetCVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT((dim != 3) || ( x.dim != 3 ),
           "JetCVector::operator^",
           "Dimension must be 3." )
#endif

  JetCVector z( 3, 0, myEnv );
  z.comp[ 0 ] = comp[ 1 ] * x.comp[ 2 ] - comp[ 2 ] * x.comp[ 1 ];
  z.comp[ 1 ] = comp[ 2 ] * x.comp[ 0 ] - comp[ 0 ] * x.comp[ 2 ];
  z.comp[ 2 ] = comp[ 0 ] * x.comp[ 1 ] - comp[ 1 ] * x.comp[ 0 ];
  return z;
}


JetCVector JetCVector::operator^ ( Vector x ) const
{
#ifndef NOCHECKS
  CHECKOUT((dim != 3) || ( x.Dim() != 3 ),
           "JetCVector::operator^",
           "Dimension must be 3." )
#endif

  JetCVector z( 3, 0, myEnv );
  z.comp[ 0 ] = comp[ 1 ] * x( 2 ) - comp[ 2 ] * x( 1 );
  z.comp[ 1 ] = comp[ 2 ] * x( 0 ) - comp[ 0 ] * x( 2 );
  z.comp[ 2 ] = comp[ 0 ] * x( 1 ) - comp[ 1 ] * x( 0 );
  return z;
}

JetCVector operator^ ( Vector y, const JetCVector& x )
{
#ifndef NOCHECKS
  CHECKOUT(( y.Dim() != 3 ) || ( x.dim != 3 ),
           "JetCVector::operator^",
           "Dimension must be 3." )
#endif

  JetCVector z( 3, 0, x.Env() );
  z.comp[ 0 ] = y( 1 ) * x.comp[ 2 ] - y( 2 ) * x.comp[ 1 ];
  z.comp[ 1 ] = y( 2 ) * x.comp[ 0 ] - y( 0 ) * x.comp[ 2 ];
  z.comp[ 2 ] = y( 0 ) * x.comp[ 1 ] - y( 1 ) * x.comp[ 0 ];
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetCVector operator*(  /* const */ MatrixC& M, 
                          const JetCVector& x ) {
 static JetCVector z;
 static int i, j, r, c;
 static JetC__environment* pje;
 static Complex temp;    // ??? I don't like this A LOT!!!  It's needed because 
                        // M(i,j) is not a const operation
                        // for the matrix element.

 // Paranoid initializations
 i = j = 0;
 pje = x.myEnv;
 z.Reconstruct( M.rows(), pje );
 r = M.rows();
 c = M.cols();

 if( M.cols() != x.dim ) {
  throw( JLC::GenericException( __FILE__, __LINE__, 
         "JetCVector operator*(  /* const */ MatrixC&, const JetCVector& )",
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

char JetCVector::operator== ( const JetCVector& x ) const
{
  if( ( dim != x.dim ) || myEnv != x.myEnv ) return 0;
  for( int i = 0; i < dim; i++ ) 
    if( comp[i] != x.comp[i] ) return 0;
  return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetCVector::operator==( const Complex& x ) const 
{
 for( int i = 0; i < dim; i++ )
   if( comp[i] != x ) return 0;     // ??? WHAT???
 return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetCVector::operator!= ( const JetCVector& x ) const
{
  return !( operator==( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetCVector::operator!=( const Complex& x ) const 
{
 return !( this->operator==( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetCVector::operator< ( const JetCVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(myEnv != x.myEnv, "JetCVector::operator<", "Incompatible environments.")
  CHECKOUT(dim != x.dim, "JetCVector::operator<", "Incompatible dimensions.")
#endif

  for( int i = 0; i < dim; i++ ) 
    if( comp[i].standardPart() >= x.comp[i].standardPart() ) return 0;
  return 1;
}

char JetCVector::operator<= ( const JetCVector& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(myEnv != x.myEnv, "JetCVector::operator<", "Incompatible environments.")
  CHECKOUT(dim != x.dim, "JetCVector::operator<", "Incompatible dimensions.")
#endif

  for( int i = 0; i < dim; i++ ) 
    if( comp[i].standardPart() > x.comp[i].standardPart() ) return 0;
  return 1;
}

char JetCVector::operator> ( const JetCVector& x ) const
{
  return !( operator<=( x ) );
}

char JetCVector::operator>= ( const JetCVector& x ) const
{
  return !( operator<( x ) );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetCVector::IsNull() const
{
  int i = -1;
  while ( ++i < dim ) if( comp[i] != complex_0 ) return 0;
  return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetCVector::IsUnit() const
{
  Complex x = 0.0;
  for( int i = 0; i < dim; i++ ) 
    x += comp[i].standardPart()*comp[i].standardPart();
  return x == 1.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JetCVector::IsNilpotent() const {
 for( int i = 0; i <  myEnv->SpaceDim; i++ ) 
  if( !(comp[i].isNilpotent()) ) return 0;
 return 1;
}



// Utilities ..
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 
JetCVector::Reconstruct()
{
  delete [] comp;
  comp = new JetC [ dim ];
  for ( int i = 0; i < dim; i++ ) {
    comp[i].setEnvTo( myEnv );
    comp[i].Reconstruct();
  }
}


void 
JetCVector::Reconstruct( const int& n, JetC__environment* pje )
{
  delete [] comp;
  dim = n;
  myEnv = pje;
  comp = new JetC [ dim ];
  for ( int i = 0; i < dim; i++ ) {
    comp[i].Reconstruct( pje );
  }
}


void JetCVector::peekAt() const
{
  cout << "\n\nBegin JetCVector::peekAt() ......\n";
  for ( int i = 0; i < dim; i++ ) {
    cout << "JetCVector::peekAt(): Component " << i << endl;
    comp[i].peekAt();
  }
  cout << "End JetCVector::peekAt() ......\n" << endl;
}


void JetCVector::printCoeffs() const
{
  int i;
  cout << "\n\nBegin JetCVector::printCoeffs() ......\n"
       << "Dimension: " << dim 
       << ", Weight = " << Weight()
       << ", Max accurate weight = " << AccuWgt() 
       << endl;
  cout << "JetCVector reference point: " 
       << endl;
  for( i = 0; i < myEnv->NumVar; i++ ) 
    cout << setw(20) << setprecision(12) 
         << real( myEnv->refPoint[i] )
         << " + i"
         << setw(20) << setprecision(12) 
         << imag( myEnv->refPoint[i] )
         << "\n" << endl;

  for ( i = 0; i < dim; i++ ) {
    cout << "JetCVector::printCoeffs(): Component " << i << endl;
    comp[i].printCoeffs();
  }
  cout << "End JetCVector::printCoeffs() ......\n" << endl;
}


JetC JetCVector::Norm () const
{
  JetC x( myEnv );
  x = complex_0;
  for ( int i = 0; i < dim; i++ ) x += comp[i]*comp[i];
  return sqrt(x);
}

JetCVector JetCVector::Unit () const
{
  JetC x( myEnv );
  x = Norm();
  JetCVector z( *this );
  for ( int i = 0; i < dim; i++ ) z.comp[i] /= x;
  return z;
}


void JetCVector::Rotate ( JetCVector& v, double theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((dim != 3) || ( v.dim != 3 ),
           "JetCVector::Rotate",
           "Dimension must be 3." )
#endif

  double c, s;
  JetCVector e( 3, 0, myEnv ), u( 3, 0, myEnv);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v.comp[i] = u.comp[i];
}


void JetCVector::Rotate ( JetCVector& v, const JetC& theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((dim != 3) || ( v.dim != 3 ),
           "JetCVector::Rotate",
           "Dimension must be 3." )
  CHECKOUT((myEnv != v.myEnv)||(myEnv != theta.Env()), 
           "JetCVector::Rotate", 
           "Incompatible environments.")
#endif

  JetC c( myEnv ), s( myEnv );
  JetCVector e( 3, 0, myEnv), u( 3, 0, myEnv);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v.comp[i] = u.comp[i];
}


ostream& operator<<( ostream& os, const JetCVector& v )
{
  os << "Begin JetCVector:" << endl;
  os << v.dim << endl;
  os << *(v.myEnv);
  for( int i = 0; i < v.dim; i++ ) {
    os << "\nJetCVector component " << i << ":" << endl;
    os << v.comp[i];
  }
  os << "\nEnd JetCVector." << endl;
  return os;
}


istream& operator>>( istream& is, JetCVector& v )
{
  static char buf[100];
  is >> buf;
  is >> buf;
  is >> v.dim;
  CHECKOUT(v.dim <= 0, "JetCVector::JetCVector", "Dimension must be positive.")

  streamIn( is, &(v.myEnv) );

  if( v.comp ) delete [] v.comp;
  v.comp = new JetC [ v.dim ];

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

JetCVector JetCVector::filter( MX_R_FUNCCPTR f [] ) {
 JetCVector z( dim, 0, myEnv );
 int i;

 for( i = 0; i < dim; i++ )
  z.comp[i] = comp[i].filter( f[i] );

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetCVector JetCVector::filter( int lower, int upper ) {
 JetCVector z( dim, 0, myEnv );
 int i;        

 for( i = 0; i < dim; i++ )
  z.comp[i] = comp[i].filter( lower, upper );

 return z;
}



// Operations related to differentiation |||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetCVector::weightedDerivative( int* m, Complex* x ) {
 int i;
 for( i = 0; i < dim; i++ ) x[i] = comp[i].weightedDerivative( m );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetCVector::derivative( int* m, Complex* x ) {
 int i;
 for( i = 0; i < dim; i++ ) x[i] = comp[i].derivative( m );
}



// Query functions |||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int 
JetCVector::AccuWgt() const
{
  int accuWgt = myEnv->MaxWeight;
  for( int i = 0; i < dim; i++ ) {
   if( myEnv != comp[i].Env() ) {
     cerr << "\n\n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** JetCVector::AccuWgt()             \n"
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
JetCVector::Weight()  const
{
  int weight = -1;
  for( int i = 0; i < dim; i++ ) {
   if( myEnv != comp[i].Env() ) {
     cerr << "\n\n"
 	  << "*** WARNING ***                                   \n"
 	  << "*** WARNING *** JetCVector::Weight()               \n"
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

void JetCVector::standardPart( Complex* x ) const {
 int i;
 for( i = 0; i < dim; i++ ) 
   x[i] = comp[i].standardPart();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetCVector::getReference( Complex* r ) const {
 int i;
 for( i = 0; i < myEnv->NumVar; i++ ) r[i] = myEnv->refPoint[i];
}

