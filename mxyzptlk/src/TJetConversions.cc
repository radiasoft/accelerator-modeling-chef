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
#include "VectorD.h"

#include "TJL.tcc"
#include "TJetEnvironment.tcc"
#include "TJet.tcc"
#include "TJetVector.tcc"
#include "TLieOperator.tcc"
#include "TMapping.tcc"

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

// TJetEnvironment<double,Complex>* TJet<double,Complex>::_workEnv;
// TJetEnvironment<double,Complex>* TJet<double,Complex>::_lastEnv; 
// TJetEnvironment<Complex,double>* TJet<Complex,double>::_workEnv;
// TJetEnvironment<Complex,double>* TJet<Complex,double>::_lastEnv; 

// ================================================================

bool EnvEquivalent( const TJetEnvironment<double,Complex>* x, 
                    const TJetEnvironment<Complex,double>* y ) 
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
  _spaceDim( x._spaceDim ),
  _numPaths(0), _monomial(0), _TJLmonomial(0), _TJLmml(0), _maxTerms(0)
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
    _TJLmml             = 0;
    _maxTerms           = 0;
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

  // REMOVE: bcfr = bcfRec( w + n, n );
  // REMOVE: _monomial   = new Complex[ bcfr ];
  // REMOVE: _TJLmonomial = new TJet<Complex,double>  [ bcfr ];
  // REMOVE: for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

  _buildScratchPads();

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
    _TJLmml             = 0;
    _maxTerms           = 0;
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

  // Final constructions ...
  // REMOVE: bcfr = bcfRec( w + n, n );
  // REMOVE: _monomial   = new Complex[ bcfr ];
  // REMOVE: _TJLmonomial = new TJet<Complex,double>  [ bcfr ];
  // REMOVE: for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

  _buildScratchPads();

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
      throw( TJL<double,Complex>::BadReference ( ii, imag( x._refPoint[ii] ), 
                                __FILE__, __LINE__, 
                                "TJetEnvironment<double,Complex>::TJetEnvironment<double,Complex>( const TJetEnvironment<Complex,double>& )", 
                                "Cannot copy complex environment to real one." ) );
    }
  }

  if( TJet<double,Complex>::_workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment<double,Complex>::TJetEnvironment<double,Complex>( const TJetEnvironment<Complex,double>& )",
           "Close the open Jet environment before copying." ) );
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
    _TJLmonomial        = 0;
    _TJLmml             = 0;
    _maxTerms           = 0;
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

  // REMOVE: bcfr = bcfRec( w + n, n );
  // REMOVE: _monomial   = new double[ bcfr ];
  // REMOVE: _TJLmonomial = new TJet<double,Complex> [ bcfr ];
  // REMOVE: for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

  _buildScratchPads();

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


TJetEnvironment<double,Complex>& TJetEnvironment<double,Complex>::operator=( const TJetEnvironment<Complex,double>& x )
{
  for( int ii = 0; ii < _numVar; ii++ ) {
    if( imag( x._refPoint[ii] ) != 0.0 ) {
      throw( TJL<double,Complex>::BadReference ( ii, imag( x._refPoint[ii] ), 
                                __FILE__, __LINE__, 
                                "TJetEnvironment<double,Complex>& TJetEnvironment<double,Complex>::operator=( const TJetEnvironment<Complex,double>& )", 
                                "Cannot assign complex environment to real one." ) );
    }
  }

  if( TJet<double,Complex>::_workEnv != 0 ) {
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
    _TJLmonomial        = 0;
    _TJLmml             = 0;
    _maxTerms           = 0;
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


  // Final constructions ...
  // REMOVE: bcfr = bcfRec( w + n, n );
  // REMOVE: _monomial   = new double[ bcfr ];
  // REMOVE: _TJLmonomial = new TJet<double,Complex> [ bcfr ];
  // REMOVE: for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

  _buildScratchPads();

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
 
  // If not, then create a new TJetEnvironment<Complex,double> ...........
  TJetEnvironment<Complex,double>* pje_new = new TJetEnvironment<Complex,double>( *x );
  TJet<Complex,double>::_environments.append( pje_new );
  return pje_new;
}

// ----------------------------------------------------------
// ----------------------------------------------------------

TJetEnvironment<double,Complex>* TJet<double,Complex>::CreateEnvFrom( TJetEnvironment<Complex,double>* x )
{
  // Check to see if an appropriate environment already exists
  slist_iterator g( TJet<double,Complex>::_environments );
  TJetEnvironment<double,Complex>* pje;
  while( pje = (TJetEnvironment<double,Complex>*) g() ) {
    if( EnvEquivalent( pje, x ) ) return pje;
  }
 
  // If not, then create a new TJetEnvironment<double,Complex> ............
  TJetEnvironment<double,Complex>* pje_new = new TJetEnvironment<double,Complex>( *x );
  TJet<double,Complex>::_environments.append( pje_new );
  return pje_new;
}

// ----------------------------------------------------------
// ----------------------------------------------------------

TJetEnvironment<double,Complex>* TJet<double,Complex>::CreateEnvFrom( const Vector& x, int order )
{
  // POSTCONDITIONS: The _lastEnv static pointer is not changed.
  //
  // Check to see if an appropriate environment already exists
  int n = x.Dim();
  int i = 0;
  slist_iterator g( TJet<double,Complex>::_environments );
  TJetEnvironment<double,Complex>* pje;
  while( pje = (TJetEnvironment<double,Complex>*) g() ) {
    bool passed = ( ( order == pje->_maxWeight       ) &&
                    ( n == pje->_spaceDim            ) && 
                    ( pje->_spaceDim == pje->_numVar )    );
    i = 0;
    while( passed && ( i < n ) ) {
      if( std::abs(pje->_refPoint[i] - x(i)) > 1.0e-8 ) { passed = false; }
      i++;
    }
    if( passed ) { return pje; }
  }
 
  // If not, then create a new TJetEnvironment<double,Complex> ...........
  TJetEnvironment<double,Complex>* storedEnv = TJet<double,Complex>::_lastEnv;

  TJet<double,Complex>::BeginEnvironment( order );
  for( int i = 0; i < n; i++ ) {
    new Tcoord<double,Complex>( x(i) );
    // Unfortunately, these cannot be deleted because
    // the TJetEnvironment stores the original pointers, 
    // not the copies.
    // This is very bad!!
  }
  pje = TJet<double,Complex>::EndEnvironment();

  TJet<double,Complex>::_lastEnv = storedEnv;
  return pje;
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
    ( *(this->_jl), TJet<double,Complex>::CreateEnvFrom( this->_jl->_myEnv ) );
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
 dlist_iterator getNext( x._theList );
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
  for ( int i = 0; i < _dim; i++ ) { _comp[i] = x(i); }
  return *this;
}


// ----------------------------------------------------------

TMapping<Complex,double>& TMapping<Complex,double>::operator= ( const TMapping<double,Complex>& x )
{
  return (TMapping<Complex,double>&) ( ( (TJetVector<Complex,double>*) this )->operator=( (TJetVector<double,Complex>&) x ) ); 
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
 dlist_iterator getNext( x._theList );
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
 
  dlist_iterator gx( xPtr->_theList );


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

  TJet<double,Complex>      z;
  TJLterm<Complex,double>*  p;
  TJLterm<double,Complex>*  q;
  TJL<Complex,double>*      xPtr;
  TJL<double,Complex>*      zPtr;
 
  TJetEnvironment<double,Complex>* pje_new = TJet<double,Complex>::CreateEnvFrom( x->_myEnv );

  // Proceed ...
  xPtr = x._jl;
  zPtr = z._jl;
 
  dlist_iterator gx( xPtr->_theList );


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

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator+( const double& x, const TJet<Complex,double>& y ) 
{
  return Complex(x,0.0) + y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator+( const TJet<Complex,double>& y, const double& x ) 
{
  return y + Complex(x,0.0);
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

TJet<Complex,double> operator-( const double& x, const TJet<Complex,double>& y ) 
{
  return Complex(x,0.0) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator-( const TJet<Complex,double>& y, const double& x ) 
{
  return y - Complex(x,0.0);
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

TJet<Complex,double> operator*( const Complex& y, const TJet<double,Complex>& x ) 
{  
  return y*TJet<Complex,double>(x);
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator*( const TJet<double,Complex>& x, const Complex& y ) 
{  
  return TJet<Complex,double>(x)*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator*( const double& y, const TJet<Complex,double>& x ) 
{  
  return Complex(y)*x;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator*( const TJet<Complex,double>& x, const double& y ) 
{  
  return x*Complex(y);
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

TJet<Complex,double> operator/(const TJet<Complex,double>& x, const double& y)
{
  return x/Complex(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<Complex,double> operator/(const double& x, const TJet<Complex,double>& y)
{
  return Complex(x)/y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double,FNAL::Complex> fabs( const TJet<double,FNAL::Complex>& x )
{
 static double u;

 if( x->_count == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "Jet fabs( const Jet& ) { ",
          "Argument is zero." ) );
 }
 
 if( (u = x.standardPart()) != 0.0 ) 
 {
   if( u > 0.0 ) return x;
   else          return -x;
 }
 else
 {
   throw( GenericException( __FILE__, __LINE__, 
          "Jet fabs( const Jet& ) ",
          "Argument is nilpotent." ) );
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double,FNAL::Complex> erf( const TJet<double,FNAL::Complex>& z ) 
{
  TJetEnvironment<double,FNAL::Complex>* pje = z.Env();

  TJet<double,FNAL::Complex> series    ( pje );
  TJet<double,FNAL::Complex> oldseries ( pje );
  TJet<double,FNAL::Complex> arg       ( pje );
  TJet<double,FNAL::Complex> term      ( pje );

  static double  den;
  static double  fctr_x;
  static int     counter;

  series        = 1.0;
  oldseries     = 0.0;
  arg           = - z*z;
  den           = 1.0;
  term          = 1.0;
  fctr_x        = 0.0;

  counter = 0;
  while( ( series != oldseries ) || counter++ < pje->_maxWeight ) {
    oldseries = series;
    den      += 2.0;
    fctr_x   += 1.0;
    term     *= arg/fctr_x;
    series   += term/den;
  }  

  return (2.0/MATH_SQRTPI)*z*series;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<FNAL::Complex,double> erf( const TJet<FNAL::Complex,double>& z ) 
{
  TJetEnvironment<FNAL::Complex,double>* pje = z.Env();

  if( ( fabs(imag(z.standardPart())) > 3.9 ) || 
      ( fabs(real(z.standardPart())) > 3.0 ) ) {
    TJet<FNAL::Complex,double> u( pje );
    u = FNAL::Complex( 0., 1. )*z;
    u = FNAL::Complex( 1., 0. ) - exp(u*u)*w(u);
    return u;
  }

  TJet<FNAL::Complex,double>    series    ( pje );
  TJet<FNAL::Complex,double>    oldseries ( pje );
  TJet<FNAL::Complex,double>    arg       ( pje );
  TJet<FNAL::Complex,double>    term      ( pje );
  static double  den;
  static double  fctr_x;
  static int     counter;

  series        = complex_1;
  oldseries     = complex_0;  // ??? Why necessary?
  // ??? REMOVE oldseries     = 0.0;
  arg           = - z*z;
  den           = 1.0;
  term          = complex_1;
  fctr_x        = 0.0;

  counter = 0;
  while( ( series != oldseries ) || counter++ < pje->_maxWeight ) {
    oldseries = series;
    den      += 2.0;
    fctr_x   += 1.0;
    term     *= arg/fctr_x;
    series   += term/den;
  }  

  return FNAL::Complex(2.0/MATH_SQRTPI,0.0)*z*series;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<FNAL::Complex,double> w( const TJet<FNAL::Complex,double>& z ) 
{
  static const FNAL::Complex mi( 0., -1. );
  static double x;
  static double y;
  TJet<FNAL::Complex,double>  answer( z.Env() );
  
  x = real( z.standardPart() );
  y = imag( z.standardPart() );

  if( ( x < 0.0 ) || ( y < 0.0  ) ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<FNAL::Complex,double> w( const TJet<FNAL::Complex,double>& ) ",
           "Argument must have positive standard part." ) );
  }

  if( ( x > 6.0 ) || ( y > 6.0 ) ) 
    answer = ( - mi * z * (
                          ( 0.5124242  /( z*z - 0.2752551 )) + 
                          ( 0.05176536 /( z*z - 2.724745  ))
                          ) 
             );

  else if( ( x > 3.9 ) || ( y > 3.0 ) ) 
    answer = ( - mi * z * (
                          ( 0.4613135   /( z*z - 0.1901635 )) + 
                          ( 0.09999216  /( z*z - 1.7844927 )) + 
                          ( 0.002883894 /( z*z - 5.5253437 ))
                          ) 
             );

  else answer = exp( -z*z )*( 1.0 - erf( mi*z ) );

  return answer;
}



// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJetVector<FNAL::Complex,double> operator*( const TJetVector<FNAL::Complex,double>& x, const double& c )
{
  return x.operator*( FNAL::Complex(c,0.0) );
}

TJetVector<FNAL::Complex,double> operator*( const double& c, const TJetVector<FNAL::Complex,double>& x )
{
  return x.operator*( FNAL::Complex(c,0.0) );  // it's commutative
}

void TJetVector<double,FNAL::Complex>::printCoeffs() const
{
  int i;
  cout << "\n\nBegin TJetVector<double,FNAL::Complex>::printCoeffs() ......\n"
       << "Dimension: " << _dim 
       << ", Weight = " << Weight()
       << ", Max accurate weight = " << AccuWgt() 
       << endl;
  cout << "JetVector reference point: " 
       << endl;
  for( i = 0; i < _myEnv->_numVar; i++ ) 
    cout << setw(20) << setprecision(12) 
         << _myEnv->_refPoint[i]
         << "\n" << endl;

  for ( i = 0; i < _dim; i++ ) {
    cout << "TJetVector<double,FNAL::Complex>::printCoeffs(): Component " << i << endl;
    _comp[i].printCoeffs();
  }
  cout << "End TJetVector<double,FNAL::Complex>::printCoeffs() ......\n" << endl;
}


void TJetVector<FNAL::Complex,double>::printCoeffs() const
{
  int i;
  cout << "\n\nBegin TJetVector<FNAL::Complex,double>::printCoeffs() ......\n"
       << "Dimension: " << _dim 
       << ", Weight = " << Weight()
       << ", Max accurate weight = " << AccuWgt() 
       << endl;
  cout << "TJetVector<FNAL::Complex,double> reference point: " 
       << endl;
  for( i = 0; i < _myEnv->_numVar; i++ ) 
    cout << setw(20) << setprecision(12) 
         << real( _myEnv->_refPoint[i] )
         << " + i"
         << setw(20) << setprecision(12) 
         << imag( _myEnv->_refPoint[i] )
         << "\n" << endl;

  for ( i = 0; i < _dim; i++ ) {
    cout << "TJetVector<FNAL::Complex,double>::printCoeffs(): Component " << i << endl;
    _comp[i].printCoeffs();
  }
  cout << "End TJetVector<FNAL::Complex,double>::printCoeffs() ......\n" << endl;
}


void TJetVector<double,FNAL::Complex>::Rotate ( TJetVector<double,FNAL::Complex>& v, double theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( v._dim != 3 ),
           "TJetVector<double,FNAL::Complex>::Rotate",
           "Dimension must be 3." )
#endif

  double c, s;
  TJetVector<double,FNAL::Complex> e( 3, 0, _myEnv ), u( 3, 0, _myEnv);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v._comp[i] = u._comp[i];
}


void TJetVector<FNAL::Complex,double>::Rotate ( TJetVector<FNAL::Complex,double>&, double ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "void JetCVectorRotate ( JetCVector&, double ) const", 
         "Function does not exist." ) );
}


void TJetVector<double,FNAL::Complex>::Rotate ( TJetVector<double,FNAL::Complex>& v, 
                                 const TJet<double,FNAL::Complex>& theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( v._dim != 3 ),
           "TJetVector<double,FNAL::Complex>::Rotate",
           "Dimension must be 3." )
  CHECKOUT((_myEnv != v._myEnv)||(_myEnv != theta.Env()), 
           "TJetVector<double,FNAL::Complex>::Rotate", 
           "Incompatible environments.")
#endif

  TJet<double,FNAL::Complex> c( _myEnv ), s( _myEnv );
  TJetVector<double,FNAL::Complex> e( 3, 0, _myEnv), u( 3, 0, _myEnv);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v._comp[i] = u._comp[i];
}


void TJetVector<FNAL::Complex,double>::Rotate ( TJetVector<FNAL::Complex,double>&, 
                                                const TJet<FNAL::Complex,double>& ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "void JetCVectorRotate ( JetCVector&, const JetC& ) const", 
         "Function does not exist." ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector TMapping<double,Complex>::operator()( const Vector& x ) const
{
 int i = x.Dim();
 if( ( i != _myEnv->_numVar ) || ( i != _dim ) ) {
   throw( GenericException(__FILE__, __LINE__, 
          "Vector TMapping<double,Complex>::operator()( const Vector& ) const",
          "Incompatible dimensions." ) );
 }

 Vector z( _dim );

 for( i = 0; i < _myEnv->_spaceDim; i++) {
  z(i) = _comp[i]( x );
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector TMapping<Complex,double>::operator()( const Vector& x ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "Vector TMapping<Complex,double>::operator()( const Vector& x ) const", 
         "This specialization is meaningless. It should not be invoked." ) );
}


