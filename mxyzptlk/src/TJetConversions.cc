/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetConversions.cc
******  Version:   1.0
******                                                                
******  Copyright (c) 1990, 2004 Universities Research Association, Inc.    
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

#include "GenericException.h"
#include "complexAddon.h"

#include "TJL.h"
#include "TJetEnvironment.h"
#include "TJet.h"
#include "TJetVector.h"
#include "TLieOperator.h"
#include "TMapping.h"

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
using FNAL::Complex;

// ================================================================

bool EnvEquivalent( const TJetEnvironment<double,FNAL::Complex>* x, 
                    const TJetEnvironment<FNAL::Complex,double>* y ) 
{
  static int i;
  if( x->_numVar    != y->_numVar    ) return false;
  if( x->_spaceDim  != y->_spaceDim  ) return false;
  if( x->_maxWeight != y->_maxWeight ) return false;
  for( i = 0; i < x->_numVar; i++ ) {
    if( ( x->_refPoint[i] != real( y->_refPoint[i] ) ) ||
        ( imag( y->_refPoint[i] ) != 0.0  )
      )                                return false;
    if( x->_scale[i] != y->_scale[i] ) return false;
  }
  return true;
}

bool EnvEquivalent( const TJetEnvironment<Complex,double>* x, 
                    const TJetEnvironment<double,Complex>* y ) {
  return EnvEquivalent( y, x );
}

// ================================================================


TJetEnvironment<Complex,double>::TJetEnvironment( const TJetEnvironment<double,Complex>& x )
: _maxWeight( x._maxWeight ), 
  _numVar( x._numVar ), 
  _spaceDim( x._spaceDim )
{
  if( TJet<Complex,double>::_workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment<Complex,double>::TJetEnvironment<Complex,double>( const TJetEnvironment<double,Complex>& )",
           "Close open TJet<Complex,double> environment before copying." ) );
  }

  if( _numVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment<Complex,double>::TJetEnvironment<Complex,double>     \n"
         << "*** WARNING *** ( const TJetEnvironment<double,Complex>& )              \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    _monomial           = 0;
    _TJLmonomial        = 0;
    _exponent           = 0;
    _expCode            = 0;
    _pbok               = 0;
    _numPaths           = 0;
    _numVar             = 0;
    _maxWeight          = 0;
    _spaceDim           = -1;
    _dof                = 0;            
    _refPoint           = 0;       
    _scale              = 0;

    return;
  }

  int w = _maxWeight;
  int n = _numVar;
  int i, j, bcfr;

  _exponent   = new int[ n ];
  _expCode    = new char[ w + n ];
  _pbok       = x._pbok;

  _numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*_numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  _dof                = x._dof;

  _refPoint = new Complex[ n ];
  _scale = new double[ n ];
  _allZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   _refPoint[i] = Complex( x._refPoint[i], 0.0 );
   _scale[i] = x._scale[i];
   _allZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }

  slist_iterator getNext( x._myCoords );
  Jet* pj;
  while((  pj = (Jet*) getNext()  )) {
    _myCoords.append( new TJet<Complex,double>( *pj, this ) );
  }

  bcfr = bcfRec( w + n, n );
  _monomial   = new Complex[ bcfr ];
  _TJLmonomial = new TJet<Complex,double>  [ bcfr ];
  for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


TJetEnvironment<Complex,double>& TJetEnvironment<Complex,double>::operator=( const TJetEnvironment<double,Complex>& x )
{
  if( TJet<Complex,double>::_workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment<Complex,double>& TJetEnvironment<Complex,double>::operator=( const TJetEnvironment<double,Complex>& )",
           "Close the open TJet<Complex,double> environment first." ) );
  }

  if( _numVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment<Complex,double>::TJetEnvironment<Complex,double>     \n"
         << "*** WARNING *** ( const TJetEnvironment<double,Complex>& )              \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    _monomial           = 0;
    _TJLmonomial        = 0;
    _exponent           = 0;
    _expCode            = 0;
    _pbok               = 0;
    _numPaths           = 0;
    _numVar             = 0;
    _maxWeight          = 0;
    _spaceDim           = -1;
    _dof                = 0;            
    _refPoint           = 0;       
    _scale              = 0;

    return *this;
  }

  // ----------------------------------
  _maxWeight = x._maxWeight;
  _numVar    = x._numVar;
  _spaceDim  = x._spaceDim;

  int w = _maxWeight;
  int n = _numVar;
  int i, j, bcfr;

  delete [] _exponent;
  delete [] _expCode;
  _exponent   = new int[ n ];
  _expCode    = new char[ w + n ];
  _pbok       = x._pbok;

  delete _numPaths;
  _numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*_numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  _dof                = x._dof;

  delete [] _refPoint;
  delete [] _scale;
  _refPoint = new Complex[ n ];
  _scale = new double[ n ];
  _allZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   _refPoint[i] = Complex( x._refPoint[i], 0.0 );
   _scale[i] = x._scale[i];
   _allZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }


  // Clear and reload _myCoords ...
  slist_iterator getNext( _myCoords );
  Jet*  pj;
  TJet<Complex,double>* pjc;

  if( _myCoords.Owns() ) 
    while((  pjc = (TJet<Complex,double>*) getNext()  )) delete pjc;

  getNext.Reset( x._myCoords );
  while((  pj = (Jet*) getNext()  )) {
    _myCoords.append( new TJet<Complex,double>( *pj, this ) );
  }

  _myCoords.DoesOwn();


  // Final constructions ...
  bcfr = bcfRec( w + n, n );
  _monomial   = new Complex[ bcfr ];
  _TJLmonomial = new TJet<Complex,double>  [ bcfr ];
  for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

  return *this;
}


// ----------------------------------------------------------

TJetEnvironment<double,Complex>::TJetEnvironment<double,Complex>( const TJetEnvironment<Complex,double>& x )
: _maxWeight( x._maxWeight ), 
  _numVar( x._numVar ), 
  _spaceDim( x._spaceDim )
{
  for( int ii = 0; ii < _numVar; ii++ ) {
    if( imag( x._refPoint[ii] ) != 0.0 ) {
      throw( JL::BadReference ( ii, imag( x._refPoint[ii] ), 
                                __FILE__, __LINE__, 
                                "TJetEnvironment<double,Complex>::TJetEnvironment<double,Complex>( const TJetEnvironment<Complex,double>& )", 
                                "Cannot copy complex environment to real one." ) );
    }
  }

  if( Jet::_workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment<double,Complex>::TJetEnvironment<double,Complex>( const TJetEnvironment<Complex,double>& )",
           "Close open Jet environment before copying." ) );
  }

  if( _numVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment<double,Complex>::TJetEnvironment<double,Complex>       \n"
         << "*** WARNING *** ( const TJetEnvironment<Complex,double>& )             \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    _monomial           = 0;
    _TJLmonomial         = 0;
    _exponent           = 0;
    _expCode            = 0;
    _pbok               = 0;
    _numPaths           = 0;
    _numVar             = 0;
    _maxWeight          = 0;
    _spaceDim           = -1;
    _dof                = 0;            
    _refPoint           = 0;       
    _scale              = 0;

    return;
  }

  int w = _maxWeight;
  int n = _numVar;
  int i, j, bcfr;

  _exponent   = new int[ n ];
  _expCode    = new char[ w + n ];
  _pbok       = x._pbok;

  _numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*_numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  _dof                = x._dof;

  _refPoint = new double[ n ];
  _scale = new double[ n ];
  _allZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   _refPoint[i] = real( x._refPoint[i] );
   _scale[i] = x._scale[i];
   _allZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }

  slist_iterator getNext( x._myCoords );
  TJet<Complex,double>* pj;
  while((  pj = (TJet<Complex,double>*) getNext()  )) {
    _myCoords.append( new Jet( *pj, this ) );
  }

  bcfr = bcfRec( w + n, n );
  _monomial   = new double[ bcfr ];
  _TJLmonomial = new Jet   [ bcfr ];
  for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


TJetEnvironment<double,Complex>& TJetEnvironment<double,Complex>::operator=( const TJetEnvironment<Complex,double>& x )
{
  for( int ii = 0; ii < _numVar; ii++ ) {
    if( imag( x._refPoint[ii] ) != 0.0 ) {
      throw( JL::BadReference ( ii, imag( x._refPoint[ii] ), 
                                __FILE__, __LINE__, 
                                "TJetEnvironment<double,Complex>& TJetEnvironment<double,Complex>::operator=( const TJetEnvironment<Complex,double>& )", 
                                "Cannot assign complex environment to real one." ) );
    }
  }

  if( Jet::_workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment<double,Complex>& TJetEnvironment<double,Complex>::operator=( const TJetEnvironment<Complex,double>& )",
           "Close the open Jet environment first." ) );
  }

  if( _numVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment<double,Complex>::operator=              \n"
         << "*** WARNING *** ( const TJetEnvironment<Complex,double>& )             \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    _monomial           = 0;
    _TJLmonomial         = 0;
    _exponent           = 0;
    _expCode            = 0;
    _pbok               = 0;
    _numPaths           = 0;
    _numVar             = 0;
    _maxWeight          = 0;
    _spaceDim           = -1;
    _dof                = 0;            
    _refPoint           = 0;       
    _scale              = 0;

    return *this;
  }

  // ----------------------------------
  _maxWeight = x._maxWeight;
  _numVar    = x._numVar;
  _spaceDim  = x._spaceDim;

  int w = _maxWeight;
  int n = _numVar;
  int i, j, bcfr;

  delete [] _exponent;
  delete [] _expCode;
  _exponent   = new int[ n ];
  _expCode    = new char[ w + n ];
  _pbok       = x._pbok;

  delete _numPaths;
  _numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*_numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  _dof                = x._dof;

  delete [] _refPoint;
  delete [] _scale;
  _refPoint = new double[ n ];
  _scale = new double[ n ];
  _allZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   _refPoint[i] = real( x._refPoint[i] );
   _scale[i] = x._scale[i];
   _allZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }


  // Clear and reload _myCoords ...
  slist_iterator getNext( _myCoords );
  Jet*  pj;
  TJet<Complex,double>* pjc;

  if( _myCoords.Owns() ) 
    while((  pj = (Jet*) getNext()  )) delete pj;

  getNext.Reset( x._myCoords );
  while((  pjc = (TJet<Complex,double>*) getNext()  )) {
    _myCoords.append( new Jet( *pjc, this ) );
  }

  _myCoords.DoesOwn();


  // Final constructions ...
  bcfr = bcfRec( w + n, n );
  _monomial   = new double[ bcfr ];
  _TJLmonomial = new Jet   [ bcfr ];
  for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

  return *this;
}

// ----------------------------------------------------------
// ----------------------------------------------------------

TJetEnvironment<Complex,double>* TJet<Complex,double>::CreateEnvFrom( TJetEnvironment<double,Complex>* x )
{
  // Check to see if an appropriate environment already exists
  slist_iterator g( TJet<Complex,double>::_environments );
  TJetEnvironment<Complex,double>* pje;
  while( pje = (TJetEnvironment<Complex,double>*) g() ) {
    if( EnvEquivalent( x, pje ) ) return pje;
  }
 
  // If not, then create a new TJetEnvironment<Complex,double> .........................
  TJetEnvironment<Complex,double>* pje_new = new TJetEnvironment<Complex,double>( *x );
  TJet<Complex,double>::_environments.append( pje_new );
  return pje_new;
}

// ----------------------------------------------------------
// ----------------------------------------------------------

TJetEnvironment<double,Complex>* Jet::CreateEnvFrom( TJetEnvironment<Complex,double>* x )
{
  // Check to see if an appropriate environment already exists
  slist_iterator g( Jet::_environments );
  TJetEnvironment<double,Complex>* pje;
  while( pje = (TJetEnvironment<double,Complex>*) g() ) {
    if( EnvEquivalent( pje, x ) ) return pje;
  }
 
  // If not, then create a new TJetEnvironment<double,Complex> .........................
  TJetEnvironment<double,Complex>* pje_new = new TJetEnvironment<double,Complex>( *x );
  Jet::_environments.append( pje_new );
  return pje_new;
}

// ----------------------------------------------------------
// ----------------------------------------------------------


TJL<Complex,double>* TJet<double,Complex>::newJLOpp() const
{
  return new TJL<Complex,double>
    ( *(this->_jl), TJet<Complex,double>::CreateEnvFrom( this->_jl->_myEnv ) );
}


TJL<double,Complex>* TJet<Complex,double>::newJLOpp() const
{
  return new TJL<double,Complex>
    ( *(this->_jl), Jet::CreateEnvFrom( this->_jl->_myEnv ) );
}

TJL<Complex,double>* TJet<double,Complex>::newJLOpp( TJetEnvironment<Complex,double>* pje ) const
{
  return new TJL<Complex,double>( *(this->_jl), pje );
}


TJL<double,Complex>* TJet<Complex,double>::newJLOpp( TJetEnvironment<double,Complex>* pje ) const
{
  return new TJL<double,Complex>( *(this->_jl), pje );
}



// ----------------------------------------------------------
// ----------------------------------------------------------

TJet<Complex,double>::TJet<Complex,double>( const TJet<double,Complex>& x )
{
  _jl = x.newJLOpp();
  _constIterPtr = 0;
  _iterPtr      = 0;
}


TJet<Complex,double>& TJet<Complex,double>::operator=( const TJet<double,Complex>& x )
{
  if( --(_jl->_rc) == 0 ) delete _jl;
  _jl = x.newJLOpp();
  return *this; 
}


TJet<Complex,double>::TJet<Complex,double>( const TJet<double,Complex>& x, TJetEnvironment<Complex,double>* pje )
{
  _jl = x.newJLOpp( pje );
  _constIterPtr = 0;
  _iterPtr      = 0;
}


// ----------------------------------------------------------

TJL<Complex,double>::TJL<Complex,double>( const TJL<double,Complex>& x, TJetEnvironment<Complex,double>* pje ) 
{
 dlist_iterator getNext( (dlist&) x );
 TJLterm<double,Complex>* p;
 TJLterm<Complex,double>* q;
 
 
 _count = 0;   // Certain compilers need these initialized.
 _weight = 0;
 while((  p = (TJLterm<double,Complex>*) getNext()  )) {
   q = new TJLterm<Complex,double>( *p );
   append( q );
 }

 _count    = x._count;
 _weight   = x._weight;
 _accuWgt  = x._accuWgt;
 _myEnv    = pje;
 _rc       = 1;
}

TJLterm<Complex,double>::TJLterm<Complex,double>( const TJLterm<double,Complex>& x )
: _index( x._index ), _weight( x._weight ), _value( x._value, 0.0 )
{
}

TJLterm<Complex,double>::TJLterm<Complex,double>( const TJLterm<double,Complex>* x )
: _index( x->_index ), _weight( x->_weight ), _value( x->_value, 0.0 )
{
}

// ----------------------------------------------------------

TJetVector<Complex,double>& TJetVector<Complex,double>::operator=( const TJetVector<double,Complex>& x ) 
{
// #ifndef NOCHECKS
//   CHECKOUT(dim   != x.dim, "TJet<Complex,double>Vector::operator=", "Incompatible dimensions.")
//   CHECKOUT(_myEnv != x._myEnv, "TJet<Complex,double>Vector::operator=", "Incompatible environments.")
// #endif

  _myEnv = TJet<Complex,double>::CreateEnvFrom( (TJetEnvironment<double,Complex>*) (x.Env()) );
  Reconstruct( x.Dim(), _myEnv );
  for ( int i = 0; i < _dim; i++ ) _comp[i] = x(i);
  return *this;
}


// ----------------------------------------------------------
// ----------------------------------------------------------

TJet<double,Complex>::TJet<double,Complex>( const TJet<Complex,double>& x )
{
  _jl = x.newJLOpp();
  _constIterPtr = 0;
  _iterPtr      = 0;
}


TJet<double,Complex>& TJet<double,Complex>::operator=( const TJet<Complex,double>& x )
{
  if( --(_jl->_rc) == 0 ) delete _jl;
  _jl = x.newJLOpp();
  return *this; 
}


TJet<double,Complex>::TJet<double,Complex>( const TJet<Complex,double>& x, TJetEnvironment<double,Complex>* pje )
{
  _jl = x.newJLOpp( pje );
  _constIterPtr = 0;
  _iterPtr      = 0;
}


// ----------------------------------------------------------

TJL<double,Complex>::TJL<double,Complex>( const TJL<Complex,double>& x, TJetEnvironment<double,Complex>* pje ) 
{
 dlist_iterator getNext( (dlist&) x );
 TJLterm<Complex,double>*  p;
 TJLterm<double,Complex>* q;

 
 _count = 0;   // Certain compilers need these initialized.
 _weight = 0;
 while((  p = (TJLterm<Complex,double>*) getNext()  )) {
   q = new TJLterm<double,Complex>( *p );
   append( q );
 }

 _count    = x._count;
 _weight   = x._weight;
 _accuWgt  = x._accuWgt;
 _myEnv    = pje;
 _rc       = 1;
}

TJLterm<double,Complex>::TJLterm<double,Complex>( const TJLterm<Complex,double>& x )
: _index( x._index ), _weight( x._weight ), _value( real( x._value ) )
{
}

// ----------------------------------------------------------

TJetVector<double,Complex>& TJetVector<double,Complex>::operator=( const TJetVector<Complex,double>& x ) 
{
// #ifndef NOCHECKS
//   CHECKOUT(_dim   != x._dim, "TJetVector<double,Complex>::operator=", "Incompatible dimensions.")
//   CHECKOUT(_myEnv != x._myEnv, "TJetVector<double,Complex>::operator=", "Incompatible environments.")
// #endif

  _myEnv = TJet<double,Complex>::CreateEnvFrom( (TJetEnvironment<Complex,double>*) x.Env() );
  Reconstruct( x.Dim(), _myEnv );
  for ( int i = 0; i < _dim; i++ ) _comp[i] = x(i);
  return *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double,Complex> real( const TJet<Complex,double>& x ) 
{

  TJet<double,Complex>       z;
  TJLterm<Complex,double>*  p;
  TJLterm<double,Complex>*   q;
  TJL<Complex,double>*      xPtr;
  TJL<double,Complex>*       zPtr;
 
  TJetEnvironment<double,Complex>* pje_new 
    = TJet<double,Complex>::CreateEnvFrom( (TJetEnvironment<Complex,double>*) x.Env() );

  // Proceed ...
  xPtr = x._jl;
  zPtr = z._jl;
 
  dlist_iterator gx( *(dlist*) xPtr );


  // If the argument is void, then return ...
  if( xPtr->_count < 1 ) {
   z = 0.0;
   return z;
  }
 
  // .. otherwise, continue normal operations.
  while((  p = (TJLterm<Complex,double>*) gx()  )) {
   q = new TJLterm<double,Complex>( p->_index, real( p->_value ), pje_new );
   zPtr->addTerm( q );
  }
 
  // Set the maximum accurate _weight.
  zPtr->_accuWgt = xPtr->_accuWgt;
  
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double,Complex> imag( const TJet<Complex,double>& x ) 
{

  TJet<double,Complex>       z;
  TJLterm<Complex,double>*  p;
  TJLterm<double,Complex>*   q;
  TJL<Complex,double>*      xPtr;
  JL*       zPtr;
 
  TJetEnvironment<double,Complex>* pje_new = TJet<double,Complex>::CreateEnvFrom( x->_myEnv );

  // Proceed ...
  xPtr = x._jl;
  zPtr = z._jl;
 
  dlist_iterator gx( *(dlist*) xPtr );


  // If the argument is void, then return ...
  if( xPtr->_count < 1 ) {
   z = 0.0;
   return z;
  }
 
  // .. otherwise, continue normal operations.
  while((  p = (TJLterm<Complex,double>*) gx()  )) {
   q = new TJLterm<double,Complex>( p->_index, imag( p->_value ), pje_new );
   zPtr->addTerm( q );
  }
 
  // Set the maximum accurate _weight.
  zPtr->_accuWgt = xPtr->_accuWgt;
  
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||| Mixed arithmetic
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator+( const TJet<double,Complex>& x, const TJet<Complex,double>& y ) 
{
  return TJet<Complex,double>(x) + y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator+( const TJet<Complex,double>& x, const TJet<double,Complex>& y ) 
{
  return x + TJet<Complex,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator-( const TJet<double,Complex>& x, const TJet<Complex,double>& y ) 
{
  return TJet<Complex,double>(x) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator-( const TJet<Complex,double>& x, const TJet<double,Complex>& y ) 
{
  return x - TJet<Complex,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator*( const TJet<double,Complex>& x, const TJet<Complex,double>& y ) 
{
  return TJet<Complex,double>(x)*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator*( const TJet<Complex,double>& x, const TJet<double,Complex>& y ) 
{
  return x*TJet<Complex,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator/( const TJet<double,Complex>& x, const TJet<Complex,double>& y ) 
{
  return TJet<Complex,double>(x)/y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator/( const TJet<Complex,double>& x, const TJet<double,Complex>& y ) 
{
  return x/TJet<Complex,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator*( const Complex& y, const TJet<double,Complex>& x ) 
{  
 static TJet<Complex,double> z;
 static TJLterm<double,Complex>* p;
 static TJLterm<Complex,double>* q;
 static JL* xPtr;
 static TJL<Complex,double>* zPtr;
 static int testWeight;
 static Complex CZero( 0.0, 0.0 );
 
 z.Reconstruct( TJet<Complex,double>::CreateEnvFrom( x->_myEnv ) );

 p    = 0;
 q    = 0;
 xPtr = x._jl;
 zPtr = z._jl;
 testWeight = z->_accuWgt = x->_accuWgt;

 if( y == CZero ) {
   return z;
 }
 
 // ??? REMOVE // If x is void, return it ..
 // ??? REMOVE if( xPtr->_count < 1 ) {    // This is done in this way so that
 // ??? REMOVE   z.DeepCopy( x );         // what is returned does not own
 // ??? REMOVE   return z;                // the same data as x.
 // ??? REMOVE }

 // If x is void, return z, 
 // which is itself void at this point in the program.
 if( xPtr->_count < 1 ) {
   return z;
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (TJLterm<double,Complex>*) gx()  )) {
   if( p->_weight > testWeight ) break;
   q = new TJLterm<Complex,double>( p );
   q->_value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator*( const TJet<double,Complex>& x, const Complex& y ) 
{  
  return y*x;
}



// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// TJet<Complex,double> operator*( const Complex& x, const TJet<double,Complex>& y ) {
//   return x*TJet<Complex,double>(y);
// }
// 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// TJet<Complex,double> operator*( const TJet<double,Complex>& y, const Complex& x ) {
//   return operator*( x, y );
// }
// 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator/( const Complex& x, const TJet<double,Complex>& y ) 
{
  return x/TJet<Complex,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator/( const TJet<double,Complex>& y, const Complex& x ) 
{
  return TJet<Complex,double>(y)/x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator-( const Complex& x, const TJet<double,Complex>& y ) 
{
  return x - TJet<Complex,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator-( const TJet<double,Complex>& y, const Complex& x ) 
{
  return TJet<Complex,double>(y) - x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator+( const Complex& x, const TJet<double,Complex>& y ) 
{
  return x + TJet<Complex,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator+( const TJet<double,Complex>& y, const Complex& x ) 
{
  return operator+( x, y );
}
