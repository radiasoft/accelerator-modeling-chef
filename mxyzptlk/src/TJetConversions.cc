/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetConversions.cc
******  Version:   1.0
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab   
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
******  Revision History:
******                             
******  Feb 2005 - Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******  
******  Efficiency improvements.
******  - new memory management 
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <MathConstants.h>

#include <GenericException.h>
#include <complexAddon.h>
#include <VectorD.h>

#include <TJL.h>
#include <TJL.tcc>

#include <TJetEnvironment.h>
#include <TJetEnvironment.tcc>

#include <TJet.h>
#include <TJet.tcc>

#include <TJetVector.h>
#include <TJetVector.tcc>

#include <TLieOperator.h>
#include <TLieOperator.tcc>

#include <TMapping.h>
#include <TMapping.tcc>


#define CHECKOUT(test,fcn,message)    \
  if( test ) {                        \
    throw( GenericException(          \
             __FILE__, __LINE__,      \
             fcn, message        ) ); \
  }

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


template<> TJL<complex<double> ,double>* TJet<double,complex<double> >::newJLOpp() const;
template<> TJL<double,complex<double> >* TJet<complex<double> ,double>::newJLOpp() const;
template<> TJL<complex<double> ,double>* TJet<double,complex<double> >::newJLOpp( TJetEnvironment<complex<double> ,double>* pje ) const;
template<> TJL<double,complex<double> >* TJet<complex<double> ,double>::newJLOpp( TJetEnvironment<double,complex<double> >* pje ) const;


// ================================================================

// TJetEnvironment<double,complex<double> >* TJet<double,complex<double> >::_workEnv;
// TJetEnvironment<double,complex<double> >* TJet<double,complex<double> >::_lastEnv; 
// TJetEnvironment<complex<double> ,double>* TJet<complex<double> ,double>::_workEnv;
// TJetEnvironment<complex<double> ,double>* TJet<complex<double> ,double>::_lastEnv; 

// ================================================================

bool EnvEquivalent( const TJetEnvironment<double,complex<double> >* x, 
                    const TJetEnvironment<complex<double> ,double>* y ) 
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

bool EnvEquivalent( const TJetEnvironment<complex<double> ,double>* x, 
                    const TJetEnvironment<double,complex<double> >* y ) {
  return EnvEquivalent( y, x );
}

// ================================================================


TJetEnvironment<complex<double> ,double>::TJetEnvironment( const TJetEnvironment<double,complex<double> >& x )
: _maxWeight( x._maxWeight ), 
  _numVar( x._numVar ), 
  _spaceDim( x._spaceDim ),
  // OBSOLETE _numPaths(0), _monomial(0), _TJLmonomial(0), _TJLmml(0), _maxTerms(0)
  _monomial(0), _TJLmonomial(0), _TJLmml(0), _maxTerms(0), 
  _offset( x._maxWeight, x._numVar )
{
  if( TJet<complex<double> ,double>::_workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment<complex<double> ,double>::TJetEnvironment<complex<double> ,double>( const TJetEnvironment<double,complex<double> >& )",
           "Close open TJet<complex<double> ,double> environment before copying." ) );
  }

  if( _numVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment<complex<double> ,double>::TJetEnvironment<complex<double> ,double>     \n"
         << "*** WARNING *** ( const TJetEnvironment<double,complex<double> >& )              \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    _monomial           = 0;
    _TJLmonomial        = 0;
    _TJLmml             = 0;
    _maxTerms           = 0;
    _exponent           = 0;
    // OBSOLETE _expCode            = 0;
    _pbok               = 0;
    // OBSOLETE _numPaths           = 0;
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
  // OBSOLETE _expCode    = new char[ w + n ];
  _pbok       = x._pbok;

  // OBSOLETE _numPaths = new MatrixI( w+1, n );
  // OBSOLETE for( i = 0; i <= w; i++ )
  // OBSOLETE   for( j = 1; j <= n; j++ )
  // OBSOLETE     (*_numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  _dof                = x._dof;

  _refPoint = new complex<double> [ n ];
  _scale = new double[ n ];
  _allZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   _refPoint[i] = complex<double> ( x._refPoint[i], 0.0 );
   _scale[i] = x._scale[i];
   _allZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }

  // REMOVE: bcfr = bcfRec( w + n, n );
  // REMOVE: _monomial   = new complex<double> [ bcfr ];
  // REMOVE: _TJLmonomial = new TJet<complex<double> ,double>  [ bcfr ];
  // REMOVE: for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

  _buildScratchPads();

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


TJetEnvironment<complex<double> ,double>& TJetEnvironment<complex<double> ,double>::operator=( const TJetEnvironment<double,complex<double> >& x )
{
  if( TJet<complex<double> ,double>::_workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment<complex<double> ,double>& TJetEnvironment<complex<double> ,double>::operator=( const TJetEnvironment<double,complex<double> >& )",
           "Close the open TJet<complex<double> ,double> environment first." ) );
  }

  if( _numVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment<complex<double> ,double>::TJetEnvironment<complex<double> ,double>     \n"
         << "*** WARNING *** ( const TJetEnvironment<double,complex<double> >& )              \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    _monomial           = 0;
    _TJLmonomial        = 0;
    _TJLmml             = 0;
    _maxTerms           = 0;
    _exponent           = 0;
    // OBSOLETE _expCode            = 0;
    _pbok               = 0;
    // OBSOLETE _numPaths           = 0;
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
  // OBSOLETE delete [] _expCode;
  _exponent   = new int[ n ];
  // OBSOLETE _expCode    = new char[ w + n ];
  _pbok       = x._pbok;

  // OBSOLETE delete _numPaths;
  // OBSOLETE _numPaths = new MatrixI( w+1, n );
  // OBSOLETE for( i = 0; i <= w; i++ )
  // OBSOLETE   for( j = 1; j <= n; j++ )
  // OBSOLETE     (*_numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  _dof                = x._dof;

  delete [] _refPoint;
  delete [] _scale;
  _refPoint = new complex<double> [ n ];
  _scale = new double[ n ];
  _allZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   _refPoint[i] = complex<double> ( x._refPoint[i], 0.0 );
   _scale[i] = x._scale[i];
   _allZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }

  // Final constructions ...
  // REMOVE: bcfr = bcfRec( w + n, n );
  // REMOVE: _monomial   = new complex<double> [ bcfr ];
  // REMOVE: _TJLmonomial = new TJet<complex<double> ,double>  [ bcfr ];
  // REMOVE: for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

  _buildScratchPads();

  return *this;
}


// ----------------------------------------------------------

TJetEnvironment<double,complex<double> >::TJetEnvironment<double,complex<double> >( const TJetEnvironment<complex<double> ,double>& x )
: _maxWeight( x._maxWeight ), 
  _numVar( x._numVar ), 
  _spaceDim( x._spaceDim ), 
  _offset( x._maxWeight, x._numVar )
{
  for( int ii = 0; ii < _numVar; ii++ ) {
    if( imag( x._refPoint[ii] ) != 0.0 ) {
      throw( TJL<double,complex<double> >::BadReference ( ii, imag( x._refPoint[ii] ), 
                                __FILE__, __LINE__, 
                                "TJetEnvironment<double,complex<double> >::TJetEnvironment<double,complex<double> >( const TJetEnvironment<complex<double> ,double>& )", 
                                "Cannot copy complex environment to real one." ) );
    }
  }

  if( TJet<double,complex<double> >::_workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment<double,complex<double> >::TJetEnvironment<double,complex<double> >( const TJetEnvironment<complex<double> ,double>& )",
           "Close the open Jet environment before copying." ) );
  }

  if( _numVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment<double,complex<double> >::TJetEnvironment<double,complex<double> >       \n"
         << "*** WARNING *** ( const TJetEnvironment<complex<double> ,double>& )             \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    _monomial           = 0;
    _TJLmonomial        = 0;
    _TJLmml             = 0;
    _maxTerms           = 0;
    _exponent           = 0;
    // OBSOLETE _expCode            = 0;
    _pbok               = 0;
    // OBSOLETE _numPaths           = 0;
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
  // OBSOLETE _expCode    = new char[ w + n ];
  _pbok       = x._pbok;

  // OBSOLETE _numPaths = new MatrixI( w+1, n );
  // OBSOLETE for( i = 0; i <= w; i++ )
  // OBSOLETE   for( j = 1; j <= n; j++ )
  // OBSOLETE     (*_numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

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
  // REMOVE: _TJLmonomial = new TJet<double,complex<double> > [ bcfr ];
  // REMOVE: for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

  _buildScratchPads();

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


TJetEnvironment<double,complex<double> >& TJetEnvironment<double,complex<double> >::operator=( const TJetEnvironment<complex<double> ,double>& x )
{
  for( int ii = 0; ii < _numVar; ii++ ) {
    if( imag( x._refPoint[ii] ) != 0.0 ) {
      throw( TJL<double,complex<double> >::BadReference ( ii, imag( x._refPoint[ii] ), 
                                __FILE__, __LINE__, 
                                "TJetEnvironment<double,complex<double> >& TJetEnvironment<double,complex<double> >::operator=( const TJetEnvironment<complex<double> ,double>& )", 
                                "Cannot assign complex environment to real one." ) );
    }
  }

  if( TJet<double,complex<double> >::_workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment<double,complex<double> >& TJetEnvironment<double,complex<double> >::operator=( const TJetEnvironment<complex<double> ,double>& )",
           "Close the open Jet environment first." ) );
  }

  if( _numVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment<double,complex<double> >::operator=              \n"
         << "*** WARNING *** ( const TJetEnvironment<complex<double> ,double>& )             \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    _monomial           = 0;
    _TJLmonomial        = 0;
    _TJLmml             = 0;
    _maxTerms           = 0;
    _exponent           = 0;
    // OBSOLETE _expCode            = 0;
    _pbok               = 0;
    // OBSOLETE _numPaths           = 0;
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
  // OBSOLETE delete [] _expCode;
  _exponent   = new int[ n ];
  // OBSOLETE _expCode    = new char[ w + n ];
  _pbok       = x._pbok;

  // OBSOLETE delete _numPaths;
  // OBSOLETE _numPaths = new MatrixI( w+1, n );
  // OBSOLETE for( i = 0; i <= w; i++ )
  // OBSOLETE   for( j = 1; j <= n; j++ )
  // OBSOLETE     (*_numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

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
  // REMOVE: _TJLmonomial = new TJet<double,complex<double> > [ bcfr ];
  // REMOVE: for( i = 0; i < bcfr; i++ ) _TJLmonomial[i].Reconstruct( this );

  _buildScratchPads();

  return *this;
}

// ----------------------------------------------------------
// ----------------------------------------------------------

TJetEnvironment<complex<double> ,double>* TJet<complex<double> ,double>::CreateEnvFrom( TJetEnvironment<double,complex<double> >* x )
{
  // Check to see if an appropriate environment already exists
  slist_iterator g( TJet<complex<double> ,double>::_environments );
  TJetEnvironment<complex<double> ,double>* pje;
  while( pje = (TJetEnvironment<complex<double> ,double>*) g() ) {
    if( EnvEquivalent( x, pje ) ) return pje;
  }
 
  // If not, then create a new TJetEnvironment<complex<double> ,double> ...........
  TJetEnvironment<complex<double> ,double>* pje_new = new TJetEnvironment<complex<double> ,double>( *x );
  TJet<complex<double> ,double>::_environments.append( pje_new );
  return pje_new;
}

// ----------------------------------------------------------
// ----------------------------------------------------------

TJetEnvironment<double,complex<double> >* TJet<double,complex<double> >::CreateEnvFrom( TJetEnvironment<complex<double> ,double>* x )
{
  // Check to see if an appropriate environment already exists
  slist_iterator g( TJet<double,complex<double> >::_environments );
  TJetEnvironment<double,complex<double> >* pje;
  while( pje = (TJetEnvironment<double,complex<double> >*) g() ) {
    if( EnvEquivalent( pje, x ) ) return pje;
  }
 
  // If not, then create a new TJetEnvironment<double,complex<double> > ............
  TJetEnvironment<double,complex<double> >* pje_new = new TJetEnvironment<double,complex<double> >( *x );
  TJet<double,complex<double> >::_environments.append( pje_new );
  return pje_new;
}

// ----------------------------------------------------------
// ----------------------------------------------------------

TJetEnvironment<double,complex<double> >* TJet<double,complex<double> >::CreateEnvFrom( const Vector& x, int order )
{
  // POSTCONDITIONS: The _lastEnv static pointer is not changed.
  //
  // Check to see if an appropriate environment already exists
  int n = x.Dim();
  int i = 0;
  slist_iterator g( TJet<double,complex<double> >::_environments );
  TJetEnvironment<double,complex<double> >* pje;
  while( pje = (TJetEnvironment<double,complex<double> >*) g() ) {
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
 
  // If not, then create a new TJetEnvironment<double,complex<double> > ...........
  TJetEnvironment<double,complex<double> >* storedEnv = TJet<double,complex<double> >::_lastEnv;

  TJet<double,complex<double> >::BeginEnvironment( order );
  for( int i = 0; i < n; i++ ) {
    new Tcoord<double,complex<double> >( x(i) );
    // Unfortunately, these cannot be deleted because
    // the TJetEnvironment stores the original pointers, 
    // not the copies.
    // This is very bad!!
  }
  pje = TJet<double,complex<double> >::EndEnvironment();

  TJet<double,complex<double> >::_lastEnv = storedEnv;
  return pje;
}

TJet<complex<double> ,double>::TJet<complex<double> ,double>( const TJet<double,complex<double> >& x )
{
  _jl = x.newJLOpp();
  _constIterPtr = 0;
  _iterPtr      = 0;
}


TJet<complex<double> ,double>& TJet<complex<double> ,double>::operator=( const TJet<double,complex<double> >& x )
{
  if( --(_jl->_rc) == 0 ) TJL<complex<double> ,double>::discardTJL(_jl); // delete _jl;
  _jl = x.newJLOpp();
  return *this; 
}


TJet<complex<double> ,double>::TJet<complex<double> ,double>( const TJet<double,complex<double> >& x, TJetEnvironment<complex<double> ,double>* pje )
{
  _jl = x.newJLOpp( pje );
  _constIterPtr = 0;
  _iterPtr      = 0;
}


// ----------------------------------------------------------

TJL<complex<double> ,double>::TJL<complex<double> ,double>( const TJL<double,complex<double> >& x, TJetEnvironment<complex<double> ,double>* pje ) 
{
 dlist_iterator getNext( x._theList );
 TJLterm<double,complex<double> >* p;
 TJLterm<complex<double> ,double>* q;

 initStore( x._jltermStoreCapacity );
 
 _count = 0;   // Certain compilers need these initialized.
 _weight = 0;

 // NOTE: we cannot do a memcpy here because x._value is of type real and _value is of type complex !

 while ( p = (TJLterm<double,complex<double> >*) getNext() ) {
    q = new ( storePtr() )  TJLterm<complex<double> ,double> ( p->_index, complex<double> (p->_value, 0.0), pje  );
    q->_deleted    = p->_deleted;// ??     
    q->_weight     = p->_weight; // ??     
    append(q);
 }
  
 _count    = x._count;  
 _weight   = x._weight;
 _accuWgt  = x._accuWgt;
 _myEnv    = pje;
 _rc       = 1;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJLterm<complex<double> ,double>::TJLterm<complex<double> ,double>( const TJLterm<double,complex<double> >& x )
: _deleted(x._deleted), _index( x._index ), _weight( x._weight ), _value( x._value, 0.0 )
{
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJLterm<complex<double> ,double>::TJLterm<complex<double> ,double>( const TJLterm<double,complex<double> >* x )
: _deleted(x->_deleted), _index( x->_index ), _weight( x->_weight ), _value( x->_value, 0.0 )
{
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJL<complex<double> ,double>* TJL<complex<double> ,double>::makeTJL( const TJL<double,complex<double> >& x, TJetEnvironment<complex<double> ,double>* pje ) 
{

 if ( _thePool.empty() ) return new TJL<complex<double> ,double>( x,pje); 

 TJL<complex<double> ,double>* p = _thePool.back(); _thePool.pop_back();

 if ( (p->_jltermStoreCapacity) < (x._jltermStoreCapacity) ) {

   delete[] p->_jltermStore; 
   p->initStore( x._jltermStoreCapacity );

 }; 

  // NOTE: we cannot do a memcpy here because x._value is of type real and _value is of type complex !

 dlist_iterator getNext( x._theList );
 TJLterm<double,complex<double> >* q;
 TJLterm<complex<double> ,double>* r;

  while ( q = (TJLterm<double,complex<double> >*) getNext() ) {
    r = new ( p->storePtr() )  TJLterm<complex<double> ,double> ( q->_index, complex<double> (q->_value, 0.0), pje  );
    r->_deleted    = q->_deleted;// ??     
    r->_weight     = q->_weight; // ??     
    p->append(r);
 }
  
 // p->_count    = x._count;  
 p->_weight   = x._weight;
 p->_accuWgt  = x._accuWgt;
 p->_myEnv    = pje; 
 p->_rc       = 1;

 return p;
}


// ----------------------------------------------------------

TJetVector<complex<double> ,double>& TJetVector<complex<double> ,double>::operator=( const TJetVector<double,complex<double> >& x ) 
{
// #ifndef NOCHECKS
//   CHECKOUT(dim   != x.dim, "TJet<complex<double> ,double>Vector::operator=", "Incompatible dimensions.")
//   CHECKOUT(_myEnv != x._myEnv, "TJet<complex<double> ,double>Vector::operator=", "Incompatible environments.")
// #endif

  _myEnv = TJet<complex<double> ,double>::CreateEnvFrom( (TJetEnvironment<double,complex<double> >*) (x.Env()) );
  Reconstruct( x.Dim(), _myEnv );
  for ( int i = 0; i < _dim; i++ ) { _comp[i] = x(i); }
  return *this;
}


// ----------------------------------------------------------

TMapping<complex<double> ,double>& TMapping<complex<double> ,double>::operator= ( const TMapping<double,complex<double> >& x )
{
  return (TMapping<complex<double> ,double>&) ( ( (TJetVector<complex<double> ,double>*) this )->operator=( (TJetVector<double,complex<double> >&) x ) ); 
}


// ----------------------------------------------------------
// ----------------------------------------------------------

TJet<double,complex<double> >::TJet<double,complex<double> >( const TJet<complex<double> ,double>& x )
{
  _jl = x.newJLOpp();
  _constIterPtr = 0;
  _iterPtr      = 0;
}


TJet<double,complex<double> >& TJet<double,complex<double> >::operator=( const TJet<complex<double> ,double>& x )
{
  if( --(_jl->_rc) == 0 )  TJL<double,complex<double> >::discardTJL(_jl); // delete _jl;
  _jl = x.newJLOpp();
  return *this; 
}


TJet<double,complex<double> >::TJet<double,complex<double> >( const TJet<complex<double> ,double>& x, TJetEnvironment<double,complex<double> >* pje )
{
  _jl = x.newJLOpp( pje );
  _constIterPtr = 0;
  _iterPtr      = 0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJL<double,complex<double> >::TJL<double,complex<double> >( const TJL<complex<double> ,double>& x, TJetEnvironment<double,complex<double> >* pje ) 
{
 dlist_iterator getNext( x._theList );
 TJLterm<complex<double> ,double>*  p = 0;
 TJLterm<double,complex<double> >*  q = 0;

 initStore( x._jltermStoreCapacity );
  
 _count = 0;   // Certain compilers need these initialized.
 _weight = 0;

// note: cannot use memcpy here since q->_value is real and p->_value is complex !

   while ( p = (TJLterm<complex<double> ,double>*) getNext() ) {
    q = new ( storePtr() ) TJLterm<double,complex<double> > ( p->_index, (p->_value).real(), pje); 
    q->_deleted = p->_deleted;
    q->_weight  = p->_weight;     
    append(q);
 };

 _count    = x._count;
 _weight   = x._weight;
 _accuWgt  = x._accuWgt;
 _myEnv    = pje;
 _rc       = 1;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJL<double,complex<double> >* TJL<double,complex<double> >::makeTJL( const TJL<complex<double> ,double>& x, TJetEnvironment<double,complex<double> >* pje ) 
{

 if ( _thePool.empty() ) return (new TJL<double,complex<double> >(x,pje) );

 TJL<double,complex<double> >* p = TJL<double,complex<double> >::_thePool.back();  TJL<double,complex<double> >::_thePool.pop_back();

 if ( p->_jltermStoreCapacity < x._jltermStoreCapacity ) {

   delete[] p->_jltermStore;
   p->initStore( x._jltermStoreCapacity );     

 }

 dlist_iterator getNext( x._theList );
 TJLterm<complex<double> ,double>*  q = 0;
 TJLterm<double,complex<double> >*  r = 0;


  
// note: cannot use memcpy here since q->_value is real and p->_value is complex !

   while ( q = (TJLterm<complex<double> ,double>*) getNext() ) {
    r = new ( p->storePtr() ) TJLterm<double,complex<double> > ( q->_index, (q->_value).real(), pje); 
    r->_deleted = q->_deleted;
    r->_weight  = q->_weight;     
    p->append(r);
 };

  // p->_count    = x._count;
  p->_weight   = x._weight;
  p->_accuWgt  = x._accuWgt;
  p->_myEnv    = pje;
  p->_rc       = 1;

  return p;

}


TJLterm<double,complex<double> >::TJLterm<double,complex<double> >( const TJLterm<complex<double> ,double>& x )
: _deleted(x._deleted), _index( x._index ), _weight( x._weight ), _value( real( x._value ) )
{
}

// ----------------------------------------------------------
// ----------------------------------------------------------


TJL<complex<double> ,double>* TJet<double,complex<double> >::newJLOpp() const
{
  return TJL<complex<double> ,double>::makeTJL( *(this->_jl), TJet<complex<double> ,double>::CreateEnvFrom( this->_jl->_myEnv ) );
}


TJL<double,complex<double> >* TJet<complex<double> ,double>::newJLOpp() const
{
  return TJL<double,complex<double> >::makeTJL( *(this->_jl), TJet<double,complex<double> >::CreateEnvFrom( this->_jl->_myEnv ) );
}

TJL<complex<double> ,double>* TJet<double,complex<double> >::newJLOpp( TJetEnvironment<complex<double> ,double>* pje ) const
{
  return TJL<complex<double> ,double>::makeTJL( *(this->_jl), pje );
}


TJL<double,complex<double> >* TJet<complex<double> ,double>::newJLOpp( TJetEnvironment<double,complex<double> >* pje ) const
{
  return TJL<double,complex<double> >::makeTJL( *(this->_jl), pje );
}


// ----------------------------------------------------------
// ----------------------------------------------------------

TJetVector<double,complex<double> >& TJetVector<double,complex<double> >::operator=( const TJetVector<complex<double> ,double>& x ) 
{
// #ifndef NOCHECKS
//   CHECKOUT(_dim   != x._dim, "TJetVector<double,complex<double> >::operator=", "Incompatible dimensions.")
//   CHECKOUT(_myEnv != x._myEnv, "TJetVector<double,complex<double> >::operator=", "Incompatible environments.")
// #endif

  _myEnv = TJet<double,complex<double> >::CreateEnvFrom( (TJetEnvironment<complex<double> ,double>*) x.Env() );
  Reconstruct( x.Dim(), _myEnv );
  for ( int i = 0; i < _dim; i++ ) _comp[i] = x(i);
  return *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double,complex<double> > real( const TJet<complex<double> ,double>& x ) 
{

  TJet<double,complex<double> >       z;
  TJLterm<complex<double> ,double>*  p;
  TJLterm<double,complex<double> >*   q;
  TJL<complex<double> ,double>*      xPtr;
  TJL<double,complex<double> >*       zPtr;
 
  TJetEnvironment<double,complex<double> >* pje_new 
    = TJet<double,complex<double> >::CreateEnvFrom( (TJetEnvironment<complex<double> ,double>*) x.Env() );

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
  while((  p = (TJLterm<complex<double> ,double>*) gx()  )) {
   q = new( zPtr->storePtr() ) TJLterm<double,complex<double> >( p->_index, real( p->_value ), pje_new );
   zPtr->addTerm( q );
  }
 
  // Set the maximum accurate _weight.
  zPtr->_accuWgt = xPtr->_accuWgt;
  
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double,complex<double> > imag( const TJet<complex<double> ,double>& x ) 
{

  TJet<double,complex<double> >      z;
  TJLterm<complex<double> ,double>*  p;
  TJLterm<double,complex<double> >*  q;
  TJL<complex<double> ,double>*      xPtr;
  TJL<double,complex<double> >*      zPtr;
 
  TJetEnvironment<double,complex<double> >* pje_new = TJet<double,complex<double> >::CreateEnvFrom( x->_myEnv );

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
  while((  p = (TJLterm<complex<double> ,double>*) gx()  )) {
   q = new ( zPtr->storePtr() ) TJLterm<double,complex<double> >( p->_index, imag( p->_value ), pje_new );
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

TJet<complex<double> ,double> operator+( const TJet<double,complex<double> >& x, const TJet<complex<double> ,double>& y ) 
{
  return TJet<complex<double> ,double>(x) + y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator+( const TJet<complex<double> ,double>& x, const TJet<double,complex<double> >& y ) 
{
  return x + TJet<complex<double> ,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator+( const complex<double> & x, const TJet<double,complex<double> >& y ) 
{
  return x + TJet<complex<double> ,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator+( const TJet<double,complex<double> >& y, const complex<double> & x ) 
{
  return operator+( x, y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator+( const double& x, const TJet<complex<double> ,double>& y ) 
{
  return complex<double> (x,0.0) + y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator+( const TJet<complex<double> ,double>& y, const double& x ) 
{
  return y + complex<double> (x,0.0);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator-( const TJet<double,complex<double> >& x, const TJet<complex<double> ,double>& y ) 
{
  return TJet<complex<double> ,double>(x) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator-( const TJet<complex<double> ,double>& x, const TJet<double,complex<double> >& y ) 
{
  return x - TJet<complex<double> ,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator-( const complex<double> & x, const TJet<double,complex<double> >& y ) 
{
  return x - TJet<complex<double> ,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator-( const TJet<double,complex<double> >& y, const complex<double> & x ) 
{
  return TJet<complex<double> ,double>(y) - x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator-( const double& x, const TJet<complex<double> ,double>& y ) 
{
  return complex<double> (x,0.0) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator-( const TJet<complex<double> ,double>& y, const double& x ) 
{
  return y - complex<double> (x,0.0);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator*( const TJet<double,complex<double> >& x, const TJet<complex<double> ,double>& y ) 
{
  return TJet<complex<double> ,double>(x)*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator*( const TJet<complex<double> ,double>& x, const TJet<double,complex<double> >& y ) 
{
  return x*TJet<complex<double> ,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator*( const complex<double> & y, const TJet<double,complex<double> >& x ) 
{  
  return y*TJet<complex<double> ,double>(x);
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator*( const TJet<double,complex<double> >& x, const complex<double> & y ) 
{  
  return TJet<complex<double> ,double>(x)*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator*( const double& y, const TJet<complex<double> ,double>& x ) 
{  
  return complex<double> (y)*x;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator*( const TJet<complex<double> ,double>& x, const double& y ) 
{  
  return x*complex<double> (y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator/( const TJet<double,complex<double> >& x, const TJet<complex<double> ,double>& y ) 
{

  return TJet<complex<double> ,double>(x)/y;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator/( const TJet<complex<double> ,double>& x, const TJet<double,complex<double> >& y ) 
{
  return x/TJet<complex<double> ,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator/( const complex<double> & x, const TJet<double,complex<double> >& y ) 
{
  return x/TJet<complex<double> ,double>(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator/( const TJet<double,complex<double> >& y, const complex<double> & x ) 
{
  return TJet<complex<double> ,double>(y)/x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator/(const TJet<complex<double> ,double>& x, const double& y)
{
  return x/complex<double> (y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> operator/(const double& x, const TJet<complex<double> ,double>& y)
{
  return complex<double> (x)/y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double,complex<double> > fabs( const TJet<double,complex<double> >& x )
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

TJet<double,complex<double> > erf( const TJet<double,complex<double> >& z ) 
{
  TJetEnvironment<double,complex<double> >* pje = z.Env();

  TJet<double, complex<double> > series    ( pje );
  TJet<double, complex<double> > oldseries ( pje );
  TJet<double, complex<double> > arg       ( pje );
  TJet<double, complex<double> > term      ( pje );

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

TJet<complex<double> ,double> erf( const TJet<complex<double> ,double>& z ) 
{
  TJetEnvironment<complex<double> ,double>* pje = z.Env();

  if( ( fabs(imag(z.standardPart())) > 3.9 ) || 
      ( fabs(real(z.standardPart())) > 3.0 ) ) {
    TJet<complex<double> ,double> u( pje );
    u = complex<double> ( 0., 1. )*z;
    u = complex<double> ( 1., 0. ) - exp(u*u)*w(u);
    return u;
  }

  TJet<complex<double> ,double>    series    ( pje );
  TJet<complex<double> ,double>    oldseries ( pje );
  TJet<complex<double> ,double>    arg       ( pje );
  TJet<complex<double> ,double>    term      ( pje );
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

  return complex<double> (2.0/MATH_SQRTPI,0.0)*z*series;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> ,double> w( const TJet<complex<double> ,double>& z ) 
{
  const complex<double>  mi( 0., -1. );
  double x;
  double y;
  TJet<complex<double> ,double>  answer( z.Env() );
  
  x = real( z.standardPart() );
  y = imag( z.standardPart() );

  if( ( x < 0.0 ) || ( y < 0.0  ) ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<complex<double> ,double> w( const TJet<complex<double> ,double>& ) ",
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

  else answer = exp( -z*z )*(  1.0 -erf( mi*z ) );

  return answer;
}



// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJetVector<complex<double> ,double> operator*( const TJetVector<complex<double> ,double>& x, const double& c )
{
  return x.operator*( complex<double> (c,0.0) );
}

TJetVector<complex<double> ,double> operator*( const double& c, const TJetVector<complex<double> ,double>& x )
{
  return x.operator*( complex<double> (c,0.0) );  // it's commutative
}

void TJetVector<double,complex<double> >::printCoeffs() const
{
  int i;
  cout << "\n\nBegin TJetVector<double,complex<double> >::printCoeffs() ......\n"
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
    cout << "TJetVector<double,complex<double> >::printCoeffs(): Component " << i << endl;
    _comp[i].printCoeffs();
  }
  cout << "End TJetVector<double,complex<double> >::printCoeffs() ......\n" << endl;
}


void TJetVector<complex<double> ,double>::printCoeffs() const
{
  int i;
  cout << "\n\nBegin TJetVector<complex<double> ,double>::printCoeffs() ......\n"
       << "Dimension: " << _dim 
       << ", Weight = " << Weight()
       << ", Max accurate weight = " << AccuWgt() 
       << endl;
  cout << "TJetVector<complex<double> ,double> reference point: " 
       << endl;
  for( i = 0; i < _myEnv->_numVar; i++ ) 
    cout << setw(20) << setprecision(12) 
         << real( _myEnv->_refPoint[i] )
         << " + i"
         << setw(20) << setprecision(12) 
         << imag( _myEnv->_refPoint[i] )
         << "\n" << endl;

  for ( i = 0; i < _dim; i++ ) {
    cout << "TJetVector<complex<double> ,double>::printCoeffs(): Component " << i << endl;
    _comp[i].printCoeffs();
  }
  cout << "End TJetVector<complex<double> ,double>::printCoeffs() ......\n" << endl;
}


void TJetVector<double,complex<double> >::Rotate ( TJetVector<double,complex<double> >& v, double theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( v._dim != 3 ),
           "TJetVector<double,complex<double> >::Rotate",
           "Dimension must be 3." )
#endif

  double c, s;
  TJetVector<double,complex<double> > e( 3, 0, _myEnv ), u( 3, 0, _myEnv);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v._comp[i] = u._comp[i];
}


void TJetVector<complex<double> ,double>::Rotate ( TJetVector<complex<double> ,double>&, double ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "void JetCVectorRotate ( JetCVector&, double ) const", 
         "Function does not exist." ) );
}


void TJetVector<double,complex<double> >::Rotate ( TJetVector<double,complex<double> >& v, 
                                 const TJet<double,complex<double> >& theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( v._dim != 3 ),
           "TJetVector<double,complex<double> >::Rotate",
           "Dimension must be 3." )
  CHECKOUT((_myEnv != v._myEnv)||(_myEnv != theta.Env()), 
           "TJetVector<double,complex<double> >::Rotate", 
           "Incompatible environments.")
#endif

  TJet<double,complex<double> > c( _myEnv ), s( _myEnv );
  TJetVector<double,complex<double> > e( 3, 0, _myEnv), u( 3, 0, _myEnv);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v._comp[i] = u._comp[i];
}


void TJetVector<complex<double> ,double>::Rotate ( TJetVector<complex<double> ,double>&, 
                                                const TJet<complex<double> ,double>& ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "void JetCVectorRotate ( JetCVector&, const JetC& ) const", 
         "Function does not exist." ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector TMapping<double,complex<double> >::operator()( const Vector& x ) const
{
 int i = x.Dim();
 if( ( i != _myEnv->_numVar ) || ( i != _dim ) ) {
   throw( GenericException(__FILE__, __LINE__, 
          "Vector TMapping<double,complex<double> >::operator()( const Vector& ) const",
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

Vector TMapping<complex<double> ,double>::operator()( const Vector& x ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "Vector TMapping<complex<double>, double>::operator()( const Vector& x ) const", 
         "This specialization is meaningless. It should not be invoked." ) );
}




