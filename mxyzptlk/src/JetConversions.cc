/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      JetConversions.cc
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

#include "mxyzptlk.h"

using FNAL::Complex;

// ================================================================
//      External routines
//

extern char nexcom( int, int, int* );  
                                // Computes the next composition
                                //  of an integer into a number of parts.
                                //  Algorithm devised by Herbert Wilf.

extern "C" {
 int bcfRec( int, int );        // Recursive evaluation of binomial
                                //  coefficient.
 int nearestInteger( double );  // Returns the integer nearest to its
                                //  double argument.
}

using namespace std;

// ================================================================

char EnvEquivalent( const Jet__environment*  x, 
                    const JetC__environment* y ) {
  static int i;
  if( x->NumVar    != y->NumVar    ) return 0;
  if( x->SpaceDim  != y->SpaceDim  ) return 0;
  if( x->MaxWeight != y->MaxWeight ) return 0;
  for( i = 0; i < x->NumVar; i++ ) {
    if( ( x->refPoint[i] != real( y->refPoint[i] ) ) ||
        ( imag( y->refPoint[i] ) != 0.0  )
      )                            return 0;
    if( x->scale[i] != y->scale[i] ) return 0;
  }
  return 1;  
}

char EnvEquivalent( const JetC__environment* x, 
                    const Jet__environment* y ) {
  return EnvEquivalent( y, x );
}

// ================================================================


JetC__environment::JetC__environment( const Jet__environment& x )
: MaxWeight( x.MaxWeight ), 
  NumVar( x.NumVar ), 
  SpaceDim( x.SpaceDim )
{
  if( JetC::workEnv != 0 ) {
    throw( JL::GenericException( __FILE__, __LINE__, 
           "JetC__environment::JetC__environment( const Jet__environment& )",
           "Close open JetC environment before copying." ) );
  }

  if( NumVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** JetC__environment::JetC__environment     \n"
         << "*** WARNING *** ( const Jet__environment& )              \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    monomial           = 0;
    JLCmonomial        = 0;
    exponent           = 0;
    expCode            = 0;
    PBOK               = 0;
    numPaths           = 0;
    NumVar             = 0;
    MaxWeight          = 0;
    SpaceDim           = -1;
    dof                = 0;            
    refPoint           = 0;       
    scale              = 0;

    return;
  }

  int w = MaxWeight;
  int n = NumVar;
  int i, j, bcfr;

  exponent   = new int[ n ];
  expCode    = new char[ w + n ];
  PBOK       = x.PBOK;

  numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  dof                = x.dof;

  refPoint = new Complex[ n ];
  scale = new double[ n ];
  AllZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   refPoint[i] = Complex( x.refPoint[i], 0.0 );
   scale[i] = x.scale[i];
   AllZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }

  slist_iterator getNext( x.myCoords );
  Jet* pj;
  while((  pj = (Jet*) getNext()  )) {
    myCoords.append( new JetC( *pj, this ) );
  }

  bcfr = bcfRec( w + n, n );
  monomial   = new Complex[ bcfr ];
  JLCmonomial = new JetC  [ bcfr ];
  for( i = 0; i < bcfr; i++ ) JLCmonomial[i].Reconstruct( this );

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


JetC__environment& JetC__environment::operator=( const Jet__environment& x )
{
  if( JetC::workEnv != 0 ) {
    throw( JL::GenericException( __FILE__, __LINE__, 
           "JetC__environment& JetC__environment::operator=( const Jet__environment& )",
           "Close the open JetC environment first." ) );
  }

  if( NumVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** JetC__environment::JetC__environment     \n"
         << "*** WARNING *** ( const Jet__environment& )              \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    monomial           = 0;
    JLCmonomial        = 0;
    exponent           = 0;
    expCode            = 0;
    PBOK               = 0;
    numPaths           = 0;
    NumVar             = 0;
    MaxWeight          = 0;
    SpaceDim           = -1;
    dof                = 0;            
    refPoint           = 0;       
    scale              = 0;

    return *this;
  }

  // ----------------------------------
  MaxWeight = x.MaxWeight;
  NumVar    = x.NumVar;
  SpaceDim  = x.SpaceDim;

  int w = MaxWeight;
  int n = NumVar;
  int i, j, bcfr;

  delete [] exponent;
  delete [] expCode;
  exponent   = new int[ n ];
  expCode    = new char[ w + n ];
  PBOK       = x.PBOK;

  delete numPaths;
  numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  dof                = x.dof;

  delete [] refPoint;
  delete [] scale;
  refPoint = new Complex[ n ];
  scale = new double[ n ];
  AllZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   refPoint[i] = Complex( x.refPoint[i], 0.0 );
   scale[i] = x.scale[i];
   AllZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }


  // Clear and reload myCoords ...
  slist_iterator getNext( myCoords );
  Jet*  pj;
  JetC* pjc;

  if( myCoords.Owns() ) 
    while((  pjc = (JetC*) getNext()  )) delete pjc;

  getNext.Reset( x.myCoords );
  while((  pj = (Jet*) getNext()  )) {
    myCoords.append( new JetC( *pj, this ) );
  }

  myCoords.DoesOwn();


  // Final constructions ...
  bcfr = bcfRec( w + n, n );
  monomial   = new Complex[ bcfr ];
  JLCmonomial = new JetC  [ bcfr ];
  for( i = 0; i < bcfr; i++ ) JLCmonomial[i].Reconstruct( this );

  return *this;
}


// ----------------------------------------------------------

Jet__environment::Jet__environment( const JetC__environment& x )
: MaxWeight( x.MaxWeight ), 
  NumVar( x.NumVar ), 
  SpaceDim( x.SpaceDim )
{
  for( int ii = 0; ii < NumVar; ii++ ) {
    if( imag( x.refPoint[ii] ) != 0.0 ) {
      throw( JL::BadReference ( ii, imag( x.refPoint[ii] ), 
                                __FILE__, __LINE__, 
                                "Jet__environment::Jet__environment( const JetC__environment& )", 
                                "Cannot copy complex environment to real one." ) );
    }
  }

  if( Jet::workEnv != 0 ) {
    throw( JL::GenericException( __FILE__, __LINE__, 
           "Jet__environment::Jet__environment( const JetC__environment& )",
           "Close open Jet environment before copying." ) );
  }

  if( NumVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** Jet__environment::Jet__environment       \n"
         << "*** WARNING *** ( const JetC__environment& )             \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    monomial           = 0;
    JLmonomial         = 0;
    exponent           = 0;
    expCode            = 0;
    PBOK               = 0;
    numPaths           = 0;
    NumVar             = 0;
    MaxWeight          = 0;
    SpaceDim           = -1;
    dof                = 0;            
    refPoint           = 0;       
    scale              = 0;

    return;
  }

  int w = MaxWeight;
  int n = NumVar;
  int i, j, bcfr;

  exponent   = new int[ n ];
  expCode    = new char[ w + n ];
  PBOK       = x.PBOK;

  numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  dof                = x.dof;

  refPoint = new double[ n ];
  scale = new double[ n ];
  AllZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   refPoint[i] = real( x.refPoint[i] );
   scale[i] = x.scale[i];
   AllZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }

  slist_iterator getNext( x.myCoords );
  JetC* pj;
  while((  pj = (JetC*) getNext()  )) {
    myCoords.append( new Jet( *pj, this ) );
  }

  bcfr = bcfRec( w + n, n );
  monomial   = new double[ bcfr ];
  JLmonomial = new Jet   [ bcfr ];
  for( i = 0; i < bcfr; i++ ) JLmonomial[i].Reconstruct( this );

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


Jet__environment& Jet__environment::operator=( const JetC__environment& x )
{
  for( int ii = 0; ii < NumVar; ii++ ) {
    if( imag( x.refPoint[ii] ) != 0.0 ) {
      throw( JL::BadReference ( ii, imag( x.refPoint[ii] ), 
                                __FILE__, __LINE__, 
                                "Jet__environment& Jet__environment::operator=( const JetC__environment& )", 
                                "Cannot assign complex environment to real one." ) );
    }
  }

  if( Jet::workEnv != 0 ) {
    throw( JL::GenericException( __FILE__, __LINE__, 
           "Jet__environment& Jet__environment::operator=( const JetC__environment& )",
           "Close the open Jet environment first." ) );
  }

  if( NumVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** Jet__environment::operator=              \n"
         << "*** WARNING *** ( const JetC__environment& )             \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    monomial           = 0;
    JLmonomial         = 0;
    exponent           = 0;
    expCode            = 0;
    PBOK               = 0;
    numPaths           = 0;
    NumVar             = 0;
    MaxWeight          = 0;
    SpaceDim           = -1;
    dof                = 0;            
    refPoint           = 0;       
    scale              = 0;

    return *this;
  }

  // ----------------------------------
  MaxWeight = x.MaxWeight;
  NumVar    = x.NumVar;
  SpaceDim  = x.SpaceDim;

  int w = MaxWeight;
  int n = NumVar;
  int i, j, bcfr;

  delete [] exponent;
  delete [] expCode;
  exponent   = new int[ n ];
  expCode    = new char[ w + n ];
  PBOK       = x.PBOK;

  delete numPaths;
  numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  dof                = x.dof;

  delete [] refPoint;
  delete [] scale;
  refPoint = new double[ n ];
  scale = new double[ n ];
  AllZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   refPoint[i] = real( x.refPoint[i] );
   scale[i] = x.scale[i];
   AllZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }


  // Clear and reload myCoords ...
  slist_iterator getNext( myCoords );
  Jet*  pj;
  JetC* pjc;

  if( myCoords.Owns() ) 
    while((  pj = (Jet*) getNext()  )) delete pj;

  getNext.Reset( x.myCoords );
  while((  pjc = (JetC*) getNext()  )) {
    myCoords.append( new Jet( *pjc, this ) );
  }

  myCoords.DoesOwn();


  // Final constructions ...
  bcfr = bcfRec( w + n, n );
  monomial   = new double[ bcfr ];
  JLmonomial = new Jet   [ bcfr ];
  for( i = 0; i < bcfr; i++ ) JLmonomial[i].Reconstruct( this );

  return *this;
}

// ----------------------------------------------------------
// ----------------------------------------------------------

JetC__environment* JetC::CreateEnvFrom( const Jet__environment* x )
{
  // Check to see if an appropriate environment already exists
  slist_iterator g( JetC::environments );
  JetC__environment* pje;
  while( pje = (JetC__environment*) g() ) {
    if( EnvEquivalent( x, pje ) ) return pje;
  }
 
  // If not, then create a new JetC__environment .........................
  JetC__environment* pje_new = new JetC__environment( *x );
  JetC::environments.append( pje_new );
  return pje_new;
}

// ----------------------------------------------------------
// ----------------------------------------------------------

Jet__environment* Jet::CreateEnvFrom( const JetC__environment* x )
{
  // Check to see if an appropriate environment already exists
  slist_iterator g( Jet::environments );
  Jet__environment* pje;
  while( pje = (Jet__environment*) g() ) {
    if( EnvEquivalent( pje, x ) ) return pje;
  }
 
  // If not, then create a new Jet__environment .........................
  Jet__environment* pje_new = new Jet__environment( *x );
  Jet::environments.append( pje_new );
  return pje_new;
}

// ----------------------------------------------------------
// ----------------------------------------------------------


JLC* Jet::newJLC() const
{
  return new JLC( *(this->jl), JetC::CreateEnvFrom( this->jl->myEnv ) );
}


JL* JetC::newJL() const
{
  return new JL( *(this->jl), Jet::CreateEnvFrom( this->jl->myEnv ) );
}

JLC* Jet::newJLC( /* const */ JetC__environment* pje ) const
{
  return new JLC( *(this->jl), pje );
}


JL* JetC::newJL( /* const */ Jet__environment* pje ) const
{
  return new JL( *(this->jl), pje );
}

// ----------------------------------------------------------
// ----------------------------------------------------------

JetC::JetC( const Jet& x )
{
  jl = x.newJLC();
  constIterPtr = 0;
  iterPtr      = 0;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


JetC& JetC::operator=( const Jet& x )
{
  if( --(jl->rc) == 0 ) delete jl;
  jl = x.newJLC();
  return *this; 
}


JetC::JetC( const Jet& x, /* const */ JetC__environment* pje )
{
  jl = x.newJLC( pje );
  constIterPtr = 0;
  iterPtr      = 0;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


// ----------------------------------------------------------

JLC::JLC( const JL& x, /* const */ JetC__environment* pje ) 
{
 dlist_iterator getNext( (dlist&) x );
 JLterm*  p;
 JLCterm* q;
 
 
 count = 0;   // Certain compilers need these initialized.
 weight = 0;
 while((  p = (JLterm*) getNext()  )) {
   q = new JLCterm( *p );
   append( q );
 }

 count    = x.count;
 weight   = x.weight;
 accuWgt  = x.accuWgt;
 myEnv    = pje;
 rc       = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JLCterm::JLCterm( const JLterm& x )
: index( x.index ), weight( x.weight ), value( x.value, 0.0 )
{
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JLCterm::JLCterm( const JLterm* x )
: index( x->index ), weight( x->weight ), value( x->value, 0.0 )
{
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// ----------------------------------------------------------

JetCVector& JetCVector::operator=( const JetVector& x ) 
{
// #ifndef NOCHECKS
//   CHECKOUT(dim   != x.dim, "JetCVector::operator=", "Incompatible dimensions.")
//   CHECKOUT(myEnv != x.myEnv, "JetCVector::operator=", "Incompatible environments.")
// #endif

  myEnv = JetC::CreateEnvFrom( x.Env() );
  Reconstruct( x.Dim(), myEnv );
  for ( int i = 0; i < dim; i++ ) comp[i] = x(i);
  return *this;
}


// ----------------------------------------------------------
// ----------------------------------------------------------

Jet::Jet( const JetC& x )
{
  jl = x.newJL();
  constIterPtr = 0;
  iterPtr      = 0;
 
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


Jet& Jet::operator=( const JetC& x )
{
  if( --(jl->rc) == 0 ) delete jl;
  jl = x.newJL();
  return *this; 
}


Jet::Jet( const JetC& x, /* const */ Jet__environment* pje )
{
  jl = x.newJL( pje );
  constIterPtr = 0;
  iterPtr      = 0;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


// ----------------------------------------------------------

JL::JL( const JLC& x, /* const */ Jet__environment* pje ) 
{
 dlist_iterator getNext( (dlist&) x );
 JLCterm*  p;
 JLterm* q;

 
 count = 0;   // Certain compilers need these initialized.
 weight = 0;
 while((  p = (JLCterm*) getNext()  )) {
   q = new JLterm( *p );
   append( q );
 }

 count    = x.count;
 weight   = x.weight;
 accuWgt  = x.accuWgt;
 myEnv    = pje;
 rc       = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JLterm::JLterm( const JLCterm& x )
: index( x.index ), weight( x.weight ), value( real( x.value ) )
{
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// ----------------------------------------------------------

JetVector& JetVector::operator=( const JetCVector& x ) 
{
// #ifndef NOCHECKS
//   CHECKOUT(dim   != x.dim, "JetVector::operator=", "Incompatible dimensions.")
//   CHECKOUT(myEnv != x.myEnv, "JetVector::operator=", "Incompatible environments.")
// #endif

  myEnv = Jet::CreateEnvFrom( x.Env() );
  Reconstruct( x.Dim(), myEnv );
  for ( int i = 0; i < dim; i++ ) comp[i] = x(i);
  return *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet real( const JetC& x ) {

  Jet       z;
  JLCterm*  p;
  JLterm*   q;
  JLC*      xPtr;
  JL*       zPtr;
 
  Jet__environment* pje_new = Jet::CreateEnvFrom( x->myEnv );

  // Proceed ...
  xPtr = x.jl;
  zPtr = z.jl;
 
  dlist_iterator gx( *(dlist*) xPtr );


  // If the argument is void, then return ...
  if( xPtr->count < 1 ) {
   z = 0.0;
   return z;
  }
 
  // .. otherwise, continue normal operations.
  while((  p = (JLCterm*) gx()  )) {
   q = new JLterm( p->index, real( p->value ), pje_new );
   zPtr->addTerm( q );
  }
 
  // Set the maximum accurate weight.
  zPtr->accuWgt = xPtr->accuWgt;
  
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet imag( const JetC& x ) {

  Jet       z;
  JLCterm*  p;
  JLterm*   q;
  JLC*      xPtr;
  JL*       zPtr;
 
  Jet__environment* pje_new = Jet::CreateEnvFrom( x->myEnv );

  // Proceed ...
  xPtr = x.jl;
  zPtr = z.jl;
 
  dlist_iterator gx( *(dlist*) xPtr );


  // If the argument is void, then return ...
  if( xPtr->count < 1 ) {
   z = 0.0;
   return z;
  }
 
  // .. otherwise, continue normal operations.
  while((  p = (JLCterm*) gx()  )) {
   q = new JLterm( p->index, imag( p->value ), pje_new );
   zPtr->addTerm( q );
  }
 
  // Set the maximum accurate weight.
  zPtr->accuWgt = xPtr->accuWgt;
  
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||| Mixed arithmetic
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator+( const Jet& x, const JetC& y ) {
  return JetC(x) + y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// JetC operator+( const Jet& x, const JetC& y ) { 
//                                         // ??? This should be modified so that
//  JetC z;                                // terms beyond the accurate weight of
//  JLCterm* p;                            // x or y are not computed and carried
//  JLterm*  q;                            // into the answer.
// 
//  p    = 0;
//  q    = 0;
// 
// 
//  // Possibility: constant argument
//  if( !(x->myEnv) ) {
//    if( x->count == 1 ) {
//      return ( Complex( x.standardPart(), 0.0 )  + y );
//    }
//    else {
//      cerr << "\n\n"
//           << "*** ERROR ***                                       \n"
//           << "*** ERROR *** JetC operator+( Jet, JetC )           \n"
//           << "*** ERROR ***                                       \n"
//           << "*** ERROR *** Null environment for the first        \n"
//           << "*** ERROR *** argument.                             \n"
//           << "*** ERROR ***                                       \n"
//           << endl;
//    }
//  }
// 
//  if( !(y->myEnv) ) {
//    if( y->count == 1 ) {
//      return ( JetC( x ) + y.standardPart() );
//    }
//    else {
//      cerr << "\n\n"
//           << "*** ERROR ***                                       \n"
//           << "*** ERROR *** JetC operator+( Jet, JetC )           \n"
//           << "*** ERROR ***                                       \n"
//           << "*** ERROR *** Null environment for the second       \n"
//           << "*** ERROR *** argument.                             \n"
//           << "*** ERROR ***                                       \n"
//           << endl;
//    }
//  }
// 
//  dlist_iterator gx( *(dlist*) x.jl );
//  dlist_iterator gy( *(dlist*) y.jl );
//  int i;
//  
//  // Check for consistency and set reference point of the sum.
//  if( !EnvEquivalent( x->myEnv, y->myEnv ) ) {
//      cerr << "\n\n*** operator+(Jet,JetC)  "
//              "ERROR: Inconsistent mixed environments." 
//           << endl;
//      exit(0);
//  }
//  
//  // If one of the arguments is void, then return the other ..
//  if( x->count < 1 ) {    // This is done in this way so that
//    z.DeepCopy( y );      // what is returned does not own
//    return z;
//  }
// 
//  if( y->count < 1 ) {
//    z = (JetC) x;
//    return z;
//  }
//                                 
//  // .. otherwise, continue normal operations.
//  if( x->count > y->count ) {
//    z = (JetC) x;
//    while((  p = (JLCterm*) gy()  )) 
//      z->addTerm( new JLCterm( p ) );
//  }
//  else {
//    z.DeepCopy( y );
//    while((  q = (JLterm*) gx()  )) 
//      z->addTerm( new JLCterm( q ) );
//  }
//  
//  // Determine the maximum weight computed accurately.
//  if( x->accuWgt < y->accuWgt ) z->accuWgt = x->accuWgt;
//  else                          z->accuWgt = y->accuWgt;
//  
//  return z;
// }
// 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator+( const JetC& x, const Jet& y ) {
  return x + JetC(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator-( const Jet& x, const JetC& y ) {
  return JetC(x) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator-( const JetC& x, const Jet& y ) {
  return x - JetC(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const Jet& x, const JetC& y ) {
  return JetC(x)*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const JetC& x, const Jet& y ) {
  return x*JetC(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator/( const Jet& x, const JetC& y ) {
  return JetC(x)/y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator/( const JetC& x, const Jet& y ) {
  return x/JetC(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const Complex& y, const Jet& x ) {  
 static JetC z;
 static JLterm* p;
 static JLCterm* q;
 static JL* xPtr;
 static JLC* zPtr;
 static int testWeight;
 static Complex CZero( 0.0, 0.0 );
 
 z.Reconstruct( JetC::CreateEnvFrom( x->myEnv ) );

 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;
 testWeight = z->accuWgt = x->accuWgt;

 if( y == CZero ) {
   return z;
 }
 
 // ??? REMOVE // If x is void, return it ..
 // ??? REMOVE if( xPtr->count < 1 ) {    // This is done in this way so that
 // ??? REMOVE   z.DeepCopy( x );         // what is returned does not own
 // ??? REMOVE   return z;                // the same data as x.
 // ??? REMOVE }

 // If x is void, return z, 
 // which is itself void at this point in the program.
 if( xPtr->count < 1 ) {
   return z;
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLCterm( p );
   q->value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const Jet& x, const Complex& y ) {  
  return y*x;
}



// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// JetC operator*( const Complex& x, const Jet& y ) {
//   return x*JetC(y);
// }
// 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// JetC operator*( const Jet& y, const Complex& x ) {
//   return operator*( x, y );
// }
// 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator/( const Complex& x, const Jet& y ) {
  return x/JetC(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator/( const Jet& y, const Complex& x ) {
  return JetC(y)/x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator-( const Complex& x, const Jet& y ) {
  return x - JetC(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator-( const Jet& y, const Complex& x ) {
  return JetC(y) - x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator+( const Complex& x, const Jet& y ) {
  return x + JetC(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator+( const Jet& y, const Complex& x ) {
  return operator+( x, y );
}

