/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJet.tcc
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

#ifndef TJET_TCC
#define TJET_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <fstream>

#include "GenericException.h"
#include "TJet.h"
#include "TJetVector.h"
#include "MathConstants.h"

#define PREPFORCHANGE(_jl)  if(((_jl)->_rc)>1){--((_jl)->_rc);(_jl) = new TJL<T1,T2>(_jl);}


// ================================================================
//      Global variables
//

#define DEFSCALE  0.001

template<typename T1, typename T2> slist TJet<T1,T2>::_environments;
template<typename T1, typename T2> int   TJet<T1,T2>::_currentIndex = 0;
template<typename T1, typename T2> slist TJet<T1,T2>::_newCoords;
template<typename T1, typename T2> slist TJet<T1,T2>::_newValues;
template<typename T1, typename T2> TJetEnvironment<T1,T2>*  TJet<T1,T2>::_lastEnv = 0;
template<typename T1, typename T2> TJetEnvironment<T1,T2>*  TJet<T1,T2>::_workEnv = 0;

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

// ***************************************************************
// ***************************************************************
// ***************************************************************

template<typename T1, typename T2>
TJet<T1,T2>::TJet( TJetEnvironment<T1,T2>* pje ) 
{
 _jl = new TJL<T1,T2>( pje );
 _constIterPtr = 0;
 _iterPtr     = 0;
}

template<typename T1, typename T2>
TJet<T1,T2>::TJet( T1 x, TJetEnvironment<T1,T2>* p_je ) 
{
 _jl = new TJL<T1,T2>( x, p_je );
 _constIterPtr = 0;
 _iterPtr     = 0;
}

template<typename T1, typename T2>
TJet<T1,T2>::TJet( const TJet& x ) 
{
 _jl = x._jl;
 ( _jl->_rc )++;

 _constIterPtr = 0;  // The iterators are NOT copied.
 _iterPtr     = 0;
}

template<typename T1, typename T2>
TJet<T1,T2>::~TJet() 
{
 if( 0 != _iterPtr ) {
   delete _iterPtr;
 }
 if( 0 != _constIterPtr ) {
   delete _constIterPtr;
 }
 if( --(_jl->_rc) == 0 ) {
   delete _jl;
 }
}

template<typename T1, typename T2>
void TJet<T1,T2>::Reconstruct()
{
 // Combines destructor and constructor functions.
 // Use when initializing a static TJet variable.
 static TJetEnvironment<T1,T2>* pje;
 pje = _jl->_myEnv;
 if( --(_jl->_rc) == 0 ) delete _jl;
 _jl = new TJL<T1,T2>( pje );
}

template<typename T1, typename T2>
void TJet<T1,T2>::Reconstruct( TJetEnvironment<T1,T2>* pje )
{
 // Combines destructor and constructor functions.
 // Use when initializing a static TJet variable.
 if( --(_jl->_rc) == 0 ) delete _jl;
 _jl = new TJL<T1,T2>( pje );
}


template<typename T1, typename T2>
void TJet<T1,T2>::setEnvTo( const TJet& x )
{
  if( _jl == NULL ) 
  {
    _jl = new TJL<T1,T2>( x._jl->_myEnv );
  }
  else if( _jl->_myEnv != x._jl->_myEnv ) 
  {
    PREPFORCHANGE(_jl);
    _jl->_myEnv = x._jl->_myEnv;
  }
}


template<typename T1, typename T2>
void TJet<T1,T2>::setEnvTo( TJetEnvironment<T1,T2>* pje )
{
  if( _jl == NULL ) 
  {
    _jl = new TJL<T1,T2>( pje );
  }
  else if( _jl->_myEnv != pje ) 
  {
    PREPFORCHANGE(_jl);
    _jl->_myEnv = (TJetEnvironment<T1,T2>*) pje;
  }
}


template<typename T1, typename T2>
void TJet<T1,T2>::getReference( T1* r ) const
{
 _jl->getReference( r );
}

template<typename T1, typename T2>
void TJet<T1,T2>::setVariable( const T1& x,
                               const int& j, 
                               TJetEnvironment<T1,T2>* pje )
{
 if( _jl == NULL ) {
   _jl = new TJL<T1,T2>( pje );
 }
 else {
   PREPFORCHANGE(_jl)
 }
 _jl->setVariable( x, j, pje );  // !! Alters the environment!
}

template<typename T1, typename T2>
void TJet<T1,T2>::setVariable( const T1& x,
                               const int& j )
{
 if( _jl == NULL ) {
   throw( GenericException( __FILE__, __LINE__, 
          "void TJet<T1,T2>::setVariable( const T2&, const int& )",
          "Impossible! _jl not initialized?????" ) );
 }
 else {
   PREPFORCHANGE(_jl)
 }
 _jl->setVariable( x, j, _jl->_myEnv );  // !! Alters the environment!
}

template<typename T1, typename T2>
void TJet<T1,T2>::setVariable( const int& j, 
                               TJetEnvironment<T1,T2>* pje ) 
{
 if( _jl == NULL ) {
   _jl = new TJL<T1,T2>( pje );
 }
 else {
   PREPFORCHANGE(_jl)
 }
 _jl->setVariable( j, pje );
}

template<typename T1, typename T2>
void TJet<T1,T2>::setVariable( const int& j )
{
 if( _jl == NULL ) {
   throw( GenericException( __FILE__, __LINE__, 
          "void TJet<T1,T2>::setVariable( const int& )",
          "Impossible: _jl not initialized?????" ) );
 }
 else {
   PREPFORCHANGE(_jl)
 }
 _jl->setVariable( j, _jl->_myEnv );
}

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator=( const TJet& x ) 
{
 if( _jl != x._jl ) {
   if( --(_jl->_rc) == 0 ) delete _jl;
   _jl = x._jl;
   (_jl->_rc)++;
 }
 return *this; 
}

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::DeepCopy( const TJet& x ) 
{
 if( this != &x ) {
  if( --(_jl->_rc) == 0 ) delete _jl;
  _jl = new TJL<T1,T2>( x->_myEnv );
  *_jl = *(x._jl);
 }
 return *this; 
}

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator=( const T1& x ) 
{
 TJetEnvironment<T1,T2>* pje = _jl->_myEnv;

 if( --(_jl->_rc) == 0 ) delete _jl;
 _jl = new TJL<T1,T2>(pje);
 *(_jl) = x;
 return *this; 
}

template<typename T1, typename T2>
istream& operator>>( istream& is,  TJet<T1,T2>& x ) 
{
 if( --(x._jl->_rc) == 0 ) delete x._jl;
 x._jl = new TJL<T1,T2>;
 return operator>>( is, *(x._jl) );
}

template<typename T1, typename T2>
ostream& operator<<( ostream& os, const TJet<T1,T2>& x ) 
{
 return operator<<( os, *(x._jl) );
}



// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      New initialization routines.
//      Must be called first by any program in order to establish the
//      number of (scalar) variables, n,
//      maximum weight of derivatives, w,
//      dimension of phase space, s,
//      default reference point, r.

template<typename T1, typename T2>
void TJet<T1,T2>::BeginEnvironment( int w ) 
{
  if( _workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "void TJet<T1,T2>::BeginEnvironment( int w )",
           "Cannot open two environments simultaneously. Close first." ) );
  }
  _workEnv = new TJetEnvironment<T1,T2>;
  _workEnv->_maxWeight = w;
  _currentIndex = 0;
}


template<typename T1, typename T2>
void TJet<T1,T2>::Parameters()
{
  if( _workEnv->_pbok ) {
   throw( GenericException( __FILE__, __LINE__, 
          "void TJet<T1,T2>::Parameters()",
          "Can only be called once per (open) environment." ) );
  }
  _workEnv->_pbok = 1;
  _workEnv->_spaceDim = _currentIndex;
}


template<typename T1, typename T2>
TJetEnvironment<T1,T2>* TJet<T1,T2>::EndEnvironment( double* scl )
{
  if( _currentIndex == 0 ) {
    delete _workEnv;
    _workEnv = 0;
    return 0;
  }

  Tcoord<T1,T2>*  p;
  T1* q;
  int i, j;

  if((  ( _currentIndex     != _newCoords.size() )  ||  
        ( _newCoords.size() != _newValues.size() )  )) {
    throw( TJL<T1,T2>::HorribleException( 
           _currentIndex, _newCoords.size(), _newValues.size(),
           __FILE__, __LINE__, 
           "TJetEnvironment<T1,T2>* TJet<T1,T2>::EndEnvironment( double* )",
           "" ) );
  }

  _workEnv->_numVar = _currentIndex;
  if( !_workEnv->_pbok ) { _workEnv->_spaceDim = _currentIndex; }
  _workEnv->_dof = _workEnv->_spaceDim / 2;
  if( _workEnv->_pbok && ( 2*_workEnv->_dof != _workEnv->_spaceDim ) ) {
    cerr << "\n\n"
         << "*** WARNING ***                                 \n"
         << "*** WARNING *** TJet<T1,T2>::EndEnvironment()   \n"
         << "*** WARNING *** Phase space has odd dimension.  \n"
         << "*** WARNING *** I hope you know what you        \n"
         << "*** WARNING *** are doing, but I doubt it.      \n"
         << "*** WARNING ***                                 \n"
         << endl;
  }
  
  int w = _workEnv->_maxWeight;
  int n = _workEnv->_numVar;
  int bcfr = bcfRec( w + n, n );

  _workEnv->_exponent    = new int[ n ];
  _workEnv->_expCode     = new char[ w + n ];
  _workEnv->_monomial    = new T1 [ bcfr ];
  _workEnv->_TJLmonomial = new TJet<T1,T2> [ bcfr ];
  for( i = 0; i < bcfr; i++ ) { _workEnv->_TJLmonomial[i].Reconstruct( _workEnv ); }

  // The reference point is set.
  _workEnv->_refPoint = new T1 [ n ];
  slist_iterator GetNextValue( _newValues );
  i = 0;
  while( ( q = (T1*) GetNextValue() ) != 0 ) {
    _workEnv->_refPoint[i++] = *q;
  }
  if( i != n ) {
    throw( TJL<T2,T2>::HideousException(i, n, __FILE__, __LINE__, 
             "TJetEnvironment<T1,T2>* TJet<T1,T2>::EndEnvironment( double* )", 
             "" ) );
  }
 
  _workEnv->_allZeroes.Reconstruct(n);
  for( i = 0; i < n; i++ ) _workEnv->_allZeroes(i) = 0;
 
  // Load the _numPaths array with binomial coefficients;
  // required by Wilf's algorithm for ranking _monomials.
  _workEnv->_numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ ) {
    for( j = 1; j <= n; j++ ) {
      (*(_workEnv->_numPaths))( i, j-1 ) = bcfRec( i + j - 1, i );
    }
  }

  // Initialize the coordinates
  i = 0;
  while((  p = (Tcoord<T1,T2>*) _newCoords.get()  )) {
    q = (T1*) _newValues.get();
    p->Reconstruct( _workEnv );
    // This reconstruction is not strictly necessary
    // when the data is represented by a list - which
    // can be empty. It is needed by other representations,
    // and is, in any case, safe.
    p->setVariable( *q, p->Index(), _workEnv );  
       // This alters the environment!
    _workEnv->_myCoords.append( p );
    delete q;
  }
  
  // Set the _scale array
  _workEnv->_scale = new double[ _workEnv->_numVar ];
  if( scl ) {
    for( i = 0; i < _workEnv->_numVar; i++ ) {
      _workEnv->_scale[i] = std::abs(scl[i]);
    }
  }
  else {
    for( i = 0; i < _workEnv->_numVar; i++ ) {
      _workEnv->_scale[i] = DEFSCALE;
    }
  }

  // Reset for the next environment and exit.
  TJetEnvironment<T1,T2>::_skipEnvEqTest = true;
  _lastEnv = _workEnv;

  TJet<T1,T2>::_environments.append( _workEnv );
  _workEnv = 0;        // This does NOT delete the environment.
  _newCoords.clear();  // Should be unnecessary.
  _newValues.clear();  // Should be unnecessary.
  _currentIndex = 0;

  return _lastEnv;
}


template<typename T1, typename T2>
void TJet<T1,T2>::EnlargeEnvironment( TJetEnvironment<T1,T2>* pje )
{
  int i;

  // Like TJet<T1,T2>::BeginEnvironment
  if( _workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "void TJet<T1,T2>::EnlargeEnvironment( TJetEnvironment<T1,T2>* )",
           "Close open environment before invoking this function." ) );
  }

  _workEnv = new TJetEnvironment<T1,T2>;
  _workEnv->_maxWeight = pje->_maxWeight;
  _currentIndex = 0;  

  // Creating new coordinates ...
  Tcoord<T1,T2>* p_coord;
  for( i = 0; i < pje->_numVar; i++ ) {
    p_coord = new Tcoord<T1,T2>( pje->_refPoint[ i ] );
    // _currentIndex is automatically increased by coord::coord
  }

  // Like TJet<T1,T2>::Parameters()
  if( pje->_pbok ) {
    _workEnv->_pbok = pje->_pbok;
    _workEnv->_spaceDim = pje->_spaceDim;
  }
}


// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//    Implementation of class Tcoord
//
 
template<typename T1, typename T2>
Tcoord<T1,T2>::Tcoord( T1 x ) 
: TJet<T1,T2>( _workEnv ) {
 
 if( !_workEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "Tcoord<T1,T2>::coord( double ) ",
          "Use TJet<T1,T2>::BeginEnvironment() to open an environment first." ) );
 }

 _index = _currentIndex++;
 
 _newCoords.append( this );
 _newValues.append( new T1( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
Tcoord<T1,T2>::~Tcoord() 
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
Tcoord<T1,T2>::Tcoord( const Tcoord<T1,T2>&  ) 
{
 throw( GenericException( __FILE__, __LINE__, 
        "Tcoord<T1,T2>::coord( const coord& )",
        "Coordinate copy constructor called; this is forbidden." ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void Tcoord<T1,T2>::operator=( const T1& x ) 
{                                        // DANGER: Be careful!
  setVariable( x, _index, this->Env() );  // This alters the environment!
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
Tcoord<T1,T2>& Tcoord<T1,T2>::operator=( const Tcoord& ) 
{
 throw( GenericException( __FILE__, __LINE__, 
        "Tcoord& Tcoord<T1,T2>::operator=( const Tcoord& )",
        "It is forbidden to change the value of a coordinate." ) );
 return *this;  // Never executed, of course.
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
Tcoord<T1,T2>& Tcoord<T1,T2>::operator=( const TJet<T1,T2>& ) 
{
 throw( GenericException( __FILE__, __LINE__, 
        "Tcoord& Tcoord<T1,T2>::operator=( const TJet& )",
        "It is forbidden to change the value of a coordinate." ) );
 return *this;  // Never executed, of course.
}



//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
//    Iteration routines
// 

template<typename T1, typename T2>
void TJet<T1,T2>::resetConstIterator() 
{
  if( 0 == _constIterPtr ) {
    _constIterPtr = new dlist_iterator( *(dlist*) _jl );
  }
  else {
    delete _constIterPtr;
    _constIterPtr = new dlist_iterator( *(dlist*) _jl );
    // Note: _jl may have changed.
  }
}

template<typename T1, typename T2>
TJLterm<T1,T2> TJet<T1,T2>::stepConstIterator()  const
{
  if( _constIterPtr ) {
    return TJLterm<T1,T2>( (TJLterm<T1,T2>*) (_constIterPtr->operator()()) );
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "TJLterm<T1,T2> TJet<T1,T2>::stepConstIterator()  const",
           "You must first resetConstIterator." ) );
  }
}

template<typename T1, typename T2>
void TJet<T1,T2>::resetIterator()
{
  if( 0 == _iterPtr ) {
    _iterPtr = new dlist_iterator( *(dlist*) _jl );
  }
  else {
    delete _iterPtr;
    _iterPtr = new dlist_iterator( *(dlist*) _jl );
    // Note: _jl may have changed.
  }
}

template<typename T1, typename T2>
TJLterm<T1,T2>* TJet<T1,T2>::stepIterator()
{
  PREPFORCHANGE(_jl)
  // This has to be done each time, since _jl could have
  // been given to a new TJet since the last invocation.

  if( _iterPtr ) {
    return (TJLterm<T1,T2>*) (_iterPtr->operator()());
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "TJLterm<T1,T2>* TJet<T1,T2>::stepIterator()",
           "You must first resetIterator." ) );
  }
}


// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// 
// Functions formerly in the file TJetFcns.cc
// 

#ifndef MX_SMALL
#define MX_SMALL    1.0e-12 // Used by TJL<T1,T2>::addTerm to decide 
                            //   removal of a TJL<T1,T2>Cterm.
#endif  // MX_SMALL

#ifndef MX_MAXITER
#define MX_MAXITER  100     // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator+=( const TJet<T1,T2>& y ) 
{
                     // ??? Come up with a more efficient implementation SOON.
                     // ??? This should be modified so that
                     // terms beyond the accurate weight of
                     // x or y are not computed and carried
                     // into the answer.
 PREPFORCHANGE(_jl)

 TJLterm<T1,T2>* p;
 TJLterm<T1,T2>* q;
 TJL<T1,T2>* xPtr =  _jl;
 TJL<T1,T2>* yPtr = y._jl;


 // Check for consistency and set reference point of the sum.
 if( xPtr->_myEnv != yPtr->_myEnv )
 {
   throw( GenericException( __FILE__, __LINE__, 
          "void TJet<T1,T2>::operator+=( const TJet<T1,T2>& )",
          "Inconsistent reference points." ) );
 }
 
 // If one of the arguments is void, then return the other ..
 if( xPtr->_count < 1 ) { (*this) = y; return *this; }
 if( yPtr->_count < 1 ) { return *this; }

 dlist_iterator gy( *(dlist*) yPtr );
 
 // .. otherwise, continue normal operations.
 while((  p = (TJLterm<T1,T2>*) gy()  )) {
   q = new TJLterm<T1,T2>( p );
   xPtr->addTerm( q );
 }
 
 // Determine the maximum weight computed accurately.
 if( xPtr->_accuWgt > yPtr->_accuWgt ) xPtr->_accuWgt = yPtr->_accuWgt;

 return *this;
}

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator-=( const TJet<T1,T2>& y ) 
{
  return this->operator+=( - y );
}

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator*=( const TJet<T1,T2>& y ) 
{
 static TJet<T1,T2> z;
 z = *this;  
 (*this) = z*y;
 return *this;
}

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator/=( const TJet<T1,T2>& y ) 
{
  static TJet<T1,T2> z;
  z = *this;
  *this = z/y;
  return *this;
}

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator/=( const T1& y ) 
{
  static TJet<T1,T2> z;
  z = *this;
  *this = z/y;
  return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//     Member functions(public)  |||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::addTerm( TJLterm<T1,T2>* a) 
{
 PREPFORCHANGE(_jl)
 _jl->addTerm( a );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************************
// **************************************************************
// **************************************************************
//
//      The overloaded operators for class TJet<T1,T2>
//

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool TJet<T1,T2>::operator==( const TJet<T1,T2>& y ) const
{
 return *(this->_jl) == *(y._jl);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool TJet<T1,T2>::operator==( const T1& y ) const
{
 return *(this->_jl) == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool operator!=( const TJet<T1,T2>& x, const TJet<T1,T2>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool operator!=( const TJet<T1,T2>& x, const T1& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
bool operator!=( const T1& x, const TJet<T1,T2>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator+=( const T1& x ) 
{   // ??? Untested!!
 if( _jl == NULL ) {                           // ??? DANGER: what is the 
   _jl = new TJL<T1,T2>;                              // ??? reference point?
 }              
 else {
   PREPFORCHANGE(_jl)
 }
 _jl->operator+=( x );
 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator-=( const T1& x ) 
{
 this->operator+=( -x );
 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator*=( const T1& x ) 
{
  static TJet<T1,T2> z;
  z = *this;
  *this = z*x;
  return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator+( const TJet<T1,T2>& x, const TJet<T1,T2>& y ) 
{ 

 // Possibility: constant TJet<T1,T2> argument
 if( !(x->_myEnv) ) {
   if( x->_count == 1 ) {
     return x.standardPart() + y;
   }
   else {
     cerr << "\n\n"
          << "*** ERROR ***                                       \n"
          << "*** ERROR *** TJet<T1,T2> operator+( TJet<T1,T2>, TJet<T1,T2> )             \n"
          << "*** ERROR ***                                       \n"
          << "*** ERROR *** Null environment for the first        \n"
          << "*** ERROR *** TJet<T1,T2> argument.                         \n"
          << "*** ERROR ***                                       \n"
          << endl;
   }
 }

 if( !(y->_myEnv) ) {
   if( y->_count == 1 ) {
     return x + y.standardPart();
   }
   else {
     cerr << "\n\n"
          << "*** ERROR ***                                       \n"
          << "*** ERROR *** TJet<T1,T2> operator+( TJet<T1,T2>, TJet<T1,T2> )             \n"
          << "*** ERROR ***                                       \n"
          << "*** ERROR *** Null environment for the second       \n"
          << "*** ERROR *** TJet<T1,T2> argument.                         \n"
          << "*** ERROR ***                                       \n"
          << endl;
   }
 }

                                        // ??? This should be modified so that
 static TJet<T1,T2> z;                         // terms beyond the accurate weight of
 static TJLterm<T1,T2>* p;                     // x or y are not computed and carried
 static TJLterm<T1,T2>* q;                     // into the answer.

 p    = 0;
 q    = 0;

 // Check for consistency and set reference point of the sum.
 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator+( const TJet<T1,T2>& x, const TJet<T1,T2>& )",
          "Inconsistent environments." ) );
 }
 
 // If one of the arguments is void, then return the other ..
 if( x->_count < 1 ) {    // This is done in this way so that
   z.DeepCopy( y );      // what is returned does not own
   return z;             // the same data as x or y.
 }

 if( y->_count < 1 ) {
   z.DeepCopy( x );
   return z;
 }
                                
 dlist_iterator gx( *(dlist*) x._jl );
 dlist_iterator gy( *(dlist*) y._jl );

 // .. otherwise, continue normal operations.
 if( x->_count > y->_count ) {
   z.DeepCopy( x );
   while((  p = (TJLterm<T1,T2>*) gy()  )) 
     z->addTerm( new TJLterm<T1,T2>( p ) );
 }
 else {
   z.DeepCopy( y );
   while((  p = (TJLterm<T1,T2>*) gx()  )) 
     z->addTerm( new TJLterm<T1,T2>( p ) );
 }
 
 // Determine the maximum weight computed accurately.
 if( x->_accuWgt < y->_accuWgt ) z->_accuWgt = x->_accuWgt;
 else                          z->_accuWgt = y->_accuWgt;
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator+( const TJet<T1,T2>& x, const T1& y ) 
{
 static TJet<T1,T2> z;    // !!! "TJet<T1,T2> z = x;" does not work.  The copy constructor
 z.DeepCopy( x );  // ??? is called, which makes x and z own the same data!!
 z += y;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator+( const T1& y, const TJet<T1,T2>& x ) 
{
 TJet<T1,T2> z;
 z.DeepCopy( x );
 z += y;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator-( const TJet<T1,T2>& x, const T1& y ) 
{
 return x + (-y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator-( const T1& y, const TJet<T1,T2>& x ) 
{
 return y + (-x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator-( const TJet<T1,T2>& y ) 
{  
 static TJet<T1,T2> z;
 static TJLterm<T1,T2>* p;

 z.DeepCopy( y ); 

 // If the argument is void, then return it ..
 if( y->_count < 1 ) { 
   return z;
 }

 // .. otherwise continue normal operations.
 dlist_iterator getNext( *(dlist*) z._jl );

 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   p->_value = - p->_value;
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::Negate()
{
 static TJLterm<T1,T2>* p;

 // If the argument is void, then return ...
 if( _jl->_count < 1 ) return;

 // .. otherwise continue normal operations.
 PREPFORCHANGE(_jl)
 dlist_iterator getNext( *(dlist*) _jl );
 while((  p = (TJLterm<T1,T2>*) getNext()  )) p->_value = - p->_value;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::Mult( const T1& x )
{
 static TJLterm<T1,T2>* p;

 // If the argument is void, then return ...
 if( _jl->_count < 1 ) return;

 // .. otherwise continue normal operations.
 PREPFORCHANGE(_jl)
 dlist_iterator getNext( *(dlist*) _jl );
 while((  p = (TJLterm<T1,T2>*) getNext()  )) p->_value *= x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator-( const TJet<T1,T2>& x, const TJet<T1,T2>& y ) 
{  
                                       // ??? This should be modified so that
 static TJet<T1,T2> z;                         // terms beyond the accurate weight of
 static TJLterm<T1,T2>* p;                     // x or y are not computed and carried
 static TJLterm<T1,T2>* q;                     // into the answer.

 p    = 0;
 q    = 0;

 // Check for consistency and set reference point of the difference.
 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator-( const TJet<T1,T2>&, const TJet<T1,T2>& )  ",
          "Inconsistent environments." ) );
 }
 
 // If one of the arguments is void, then return the other ..
 if( x->_count < 1 ) {    // This is done in this way so that
   z = -y;               // what is returned does not own
   return z;             // the same data as x or y.
 }

 if( y->_count < 1 ) {
   z.DeepCopy( x );
   return z;
 }
                                
 dlist_iterator gx( *(dlist*) x._jl );
 dlist_iterator gy( *(dlist*) y._jl );

 // .. otherwise, continue normal operations.
 if( x->_count > y->_count ) {
   z.DeepCopy( x );
   while((  p = (TJLterm<T1,T2>*) gy()  )) {
     q = new TJLterm<T1,T2>( p );
     ( q->_value ) = - ( q->_value );
     z->addTerm( q );
   }
 }
 else {
   z = - y;
   while((  p = (TJLterm<T1,T2>*) gx()  )) 
     z->addTerm( new TJLterm<T1,T2>( p ) );
 }
 
 // Determine the maximum weight computed accurately.
 if( x->_accuWgt < y->_accuWgt ) z->_accuWgt = x->_accuWgt;
 else                          z->_accuWgt = y->_accuWgt;
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator*( const TJet<T1,T2>& x, const TJet<T1,T2>& y ) 
{
 static TJet<T1,T2> z;
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;
 static TJLterm<T1,T2>* r;
 static TJL<T1,T2>* xPtr;
 static TJL<T1,T2>* yPtr;
 static TJL<T1,T2>* zPtr;
 static int testWeight, trialWeight;

 // Check for consistency 
 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator*( const TJet<T1,T2>&, const TJet<T1,T2>& ) ",
          "Inconsistent environments." ) );
 }
 else {
   z.Reconstruct( x->_myEnv );
 }
 

 // Initializations
 p 	     = 0;
 q 	     = 0;
 r 	     = q;
 xPtr 	     = x._jl;
 yPtr 	     = y._jl;
 zPtr        = z._jl;
 testWeight  = 0;
 trialWeight = 0;

 // If one of the arguments is void, return it ..
 if( xPtr->_count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x or y.
 }

 if( yPtr->_count < 1 ) {
   z.DeepCopy( y );
   return z;
 }
                                

 // Determine the maximum weight computed accurately.
 TJLterm<T1,T2> x_1stTerm( xPtr->lowTerm() );
 TJLterm<T1,T2> y_1stTerm( yPtr->lowTerm() );

 testWeight = xPtr->_accuWgt;
 if( ( y_1stTerm._weight != 0 ) && ( y_1stTerm._value != ((T1) 0.0) ) ) 
 {
   testWeight += y_1stTerm._weight;
 }

 trialWeight = yPtr->_accuWgt;
 if( ( x_1stTerm._weight != 0 ) && ( x_1stTerm._value != ((T1) 0.0) ) ) 
 {
   trialWeight += x_1stTerm._weight;
 }

 if( testWeight < trialWeight )
 {
   zPtr->_accuWgt = testWeight;
 }
 else
 {
   zPtr->_accuWgt = trialWeight;
 }
 if( (zPtr->_accuWgt) > (zPtr->_myEnv->_maxWeight) ) 
 { 
   zPtr->_accuWgt = zPtr->_myEnv->_maxWeight;
 }


 // .. and continue normal operations.
 dlist_looper gx( *(dlist*) xPtr );
 dlist_looper gy( *(dlist*) yPtr );

 testWeight = zPtr->_accuWgt;
 while((  p = (TJLterm<T1,T2>*) gy()  )) {
 while((  q = (TJLterm<T1,T2>*) gx()  )) {
   if( ( p->_weight + q->_weight ) > testWeight ) continue;
   r = new TJLterm<T1,T2>( (*p)*(*q) ); 
   zPtr->addTerm( r );
 }}
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator*( const TJet<T1,T2>& x, const T1& y ) 
{  
 static TJet<T1,T2> z;
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;
 static TJL<T1,T2>* xPtr;
 static TJL<T1,T2>* zPtr;
 static int testWeight;
 
 z.Reconstruct( x->_myEnv );

 p    = 0;
 q    = 0;
 xPtr = x._jl;
 zPtr = z._jl;

 testWeight = z->_accuWgt = x->_accuWgt;

 if( y == 0.0 ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->_count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (TJLterm<T1,T2>*) gx()  )) {
   if( p->_weight > testWeight ) break;
   q = new TJLterm<T1,T2>( p );
   q->_value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator*( const T1& y, const TJet<T1,T2>& x ) 
{  
 static TJet<T1,T2> z;
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;
 static TJL<T1,T2>* xPtr;
 static TJL<T1,T2>* zPtr;
 static int testWeight;
 
 z.Reconstruct( x->_myEnv );

 p    = 0;
 q    = 0;
 xPtr = x._jl;
 zPtr = z._jl;
 testWeight = z->_accuWgt = x->_accuWgt;

 if( y == 0.0 ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->_count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (TJLterm<T1,T2>*) gx()  )) {
   if( p->_weight > testWeight ) break;
   q = new TJLterm<T1,T2>( p );
   q->_value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator*( const TJet<T1,T2>& x, const int& j ) 
{  
 static TJet<T1,T2> z;
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;
 static TJL<T1,T2>* xPtr;
 static TJL<T1,T2>* zPtr;
 static int testWeight;
 static T1 y;  // ????? START HERE
 
 z.Reconstruct( x->_myEnv );

 p    = 0;
 q    = 0;
 xPtr = x._jl;
 zPtr = z._jl;

 testWeight = z->_accuWgt = x->_accuWgt;

 if( ( y = (T1) j ) == 0.0 ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->_count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (TJLterm<T1,T2>*) gx()  )) {
   if( p->_weight > testWeight ) break;
   q = new TJLterm<T1,T2>( p );
   q->_value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator*( const int& j, const TJet<T1,T2>& x ) 
{ 
 static TJet<T1,T2> z;
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;
 static TJL<T1,T2>* xPtr;
 static TJL<T1,T2>* zPtr;
 static int testWeight;
 static T1 y;
 
 z.Reconstruct( x->_myEnv );

 y    = 0.0;
 p    = 0;
 q    = 0;
 xPtr = x._jl;
 zPtr = z._jl;

 testWeight = z->_accuWgt = x->_accuWgt;

 if( ( y = (T1) j ) == 0.0 ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->_count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (TJLterm<T1,T2>*) gx()  )) {
   if( p->_weight > testWeight ) break;
   q = new TJLterm<T1,T2>( p );
   q->_value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator/( const TJet<T1,T2>& x, const T1& y ) 
{ 
 static TJet<T1,T2> z;
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;
 static TJL<T1,T2>* xPtr;
 static TJL<T1,T2>* zPtr;
 static int testWeight;
 
 z.Reconstruct( x->_myEnv ); 

 p    = 0;
 q    = 0;
 xPtr = x._jl;
 zPtr = z._jl;

 testWeight = z->_accuWgt = x->_accuWgt;

 // Check for division by zero ..
 if( y == 0.0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator/( const TJet<T1,T2>&, const T1& ) ",
          "Attempt to divide by a scalar zero." ) );
 }
 
 // If x is void, return it ..
 if( xPtr->_count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (TJLterm<T1,T2>*) gx()  )) {
   if( p->_weight > testWeight ) break;
   q = new TJLterm<T1,T2>( p );
   q->_value /= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator/( const TJet<T1,T2>& x, const int& j ) 
{ 
 static TJet<T1,T2> z;
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;
 static TJL<T1,T2>* xPtr;
 static TJL<T1,T2>* zPtr;
 static int testWeight;
 static T1 y;
 
 z.Reconstruct( x->_myEnv ); 

 p    = 0;
 q    = 0;
 y    = 0.0;
 xPtr = x._jl;
 zPtr = z._jl;

 z->_myEnv   = x->_myEnv;
 testWeight = z->_accuWgt = x->_accuWgt;

 // Check for division by zero ..
 if( ( y = (T1) j ) == 0.0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator/( const Jet&, const int& )",
          "Attempt to divide by a scalar zero." ) );
 }
 
 // If x is void, return it ..
 if( xPtr->_count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (TJLterm<T1,T2>*) gx()  )) {
   if( p->_weight > testWeight ) break;
   q = new TJLterm<T1,T2>( p );
   q->_value /= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator/( const T1& a, const TJet<T1,T2>& b ) 
{
  static TJet<T1,T2> u;
  u.Reconstruct( b->_myEnv ); 
  u = a;
  return u/b;
} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::_truncMult( const TJet<T1,T2>& v, const int& wl ) const 
{ 
 //
 // Truncated multiplication: used only by the division operator.
 // No checks are made on the environments.
 //
 static TJet<T1,T2> z;
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;
 static TJLterm<T1,T2>* r;

 // If one of the arguments is void, return it ..
 if ( _jl->_count < 1 ) return *this;   // ??? Is this form going to cause
 if ( v->_count  < 1 ) return v;       // ??? a problem? 

 // Initializations
 z.Reconstruct( _jl->_myEnv );
 p = 0;
 q = 0;
 r = 0;

 dlist_looper gu( *(dlist*)   _jl );
 dlist_looper gv( *(dlist*) v._jl );
 
 // .. otherwise continue normal operations.
 while((  p = (TJLterm<T1,T2>*) gv()  )) {
 while((  q = (TJLterm<T1,T2>*) gu()  )) {
   if( ( p->_weight + q->_weight ) > wl ) continue;
   r = new TJLterm<T1,T2>( _jl->_myEnv );
   *r = (*p)*(*q);
   z.addTerm( r );
 }}
 
 // Determine the maximum weight computed accurately.
 if( _jl->_accuWgt < v->_accuWgt ) z->_accuWgt = _jl->_accuWgt;
 else                           z->_accuWgt = v->_accuWgt;
 
 z->_myEnv = _jl->_myEnv;  // ??? Redundant?
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<FNAL::Complex,double> operator/(const TJet<FNAL::Complex,double>& x, const double& y)
{
  return x/FNAL::Complex(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<FNAL::Complex,double> operator/(const double& x, const TJet<FNAL::Complex,double>& y)
{
  return FNAL::Complex(x)/y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator/( const TJet<T1,T2>& wArg, const TJet<T1,T2>& uArg ) 
{ 

 // Check for void operators ..
 if ( wArg->_count < 1 ) return wArg;
 if ( uArg->_count < 1 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator/( const TJet<T1,T2>&, const TJet<T1,T2>& )",
          "Attempt to divide by a void TJL<T1,T2> variable." ) );
 }
 
 // Check for consistency 
 if( wArg->_myEnv != uArg->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator/( const TJet<T1,T2>&, const TJet<T1,T2>& )",
          "Inconsistent environments." ) );
 }

 dlist_looper gu( *(dlist*) uArg._jl );
 dlist_looper gw( *(dlist*) wArg._jl );

 static TJet<T1,T2> v;
 static TJet<T1,T2> vn;
 static TJet<T1,T2> w, u;
 static TJLterm<T1,T2>* qu;
 static TJLterm<T1,T2>* qw;
 static T1 u0;
 static int i, wgt, wl, mw;
 
 // Initialize local variables and set the environment of the answer.
 // (These steps are not necessary, but they enforce a discipline.)
 v.Reconstruct( wArg->_myEnv );
 vn.Reconstruct( wArg->_myEnv );
 w.Reconstruct( wArg->_myEnv );
 u.Reconstruct( wArg->_myEnv );

 qu = 0;
 qw = 0;
 u0 = 0.0;
 wgt = 0;
 i = 0;
 wl = 0;
 mw = wArg->_myEnv->_maxWeight;

 // Normalize the denominator
 if( ( qu = (TJLterm<T1,T2>*) gu() ) == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator/( const TJet<T1,T2>&, const TJet<T1,T2>& )",
          "Division algorithm called with uninitialized JL." ) );
   }
 if( ( wgt = qu->_weight ) != 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator/( const TJet<T1,T2>&, const TJet<T1,T2>& )",
          "Attempt to divide by nilpotent element." ) );
   }
 if( ( u0 = qu->_value ) == 0.0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator/( const TJet<T1,T2>&, const TJet<T1,T2>& )",
          "Attempt to divide by zero." ) );
   }
 u = uArg / u0;
 
 // Recursive algorithm
 qw = (TJLterm<T1,T2>*) gw();
 if( qw->_weight == 0 ) v = ( qw->_value );
 else                   v = ((T1) 0.0); 
 v->_myEnv = wArg->_myEnv;  

 wl = 0;
 while ( wl < mw ) {
   wl++;
   vn = u._truncMult( v, wl );
   vn = vn.filter( wl, wl );
   w = wArg.filter( wl, wl );
   v += ( w - vn );
   }
 
 // Correct normalization of the answer ..
 v /= u0;
 
 // Determine the maximum weight computed accurately.
 if( wArg->_accuWgt < uArg->_accuWgt ) v->_accuWgt = wArg->_accuWgt;
 else                                v->_accuWgt = uArg->_accuWgt;
 
 // .. and return _value.
 v->_myEnv = wArg->_myEnv;  // Rampant paranoia ...
 return v;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator^( const TJet<T1,T2>& x, const TJet<T1,T2>& y ) 
{ 

 // Check for consistency 
 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator^( const TJet<T1,T2>&, const TJet<T1,T2>& )",
          "Inconsistent environments." ) );
 }

 static TJetEnvironment<T1,T2>* theEnv;
 static TJet<T1,T2> z;
 static int i;
 static int* m;
 static int* n;

 theEnv = x->_myEnv;
 z.Reconstruct( theEnv ); 

 i = 0;
 m = new int [ theEnv->_numVar ];
 n = new int [ theEnv->_numVar ];

 if( theEnv->_pbok ) {
 
   for( i = 0; i < theEnv->_numVar; i++ ) {
     m[i] = 0;
     n[i] = 0;
   }
 
   for( i = 0; i < theEnv->_dof; i++ ) {
     m[ i ] = 1;
     n[ i + theEnv->_dof ] = 1;
 
     z += ( ( x.D(m) * y.D(n) ) - ( x.D(n) * y.D(m) ) );
 
     m[ i ] = 0;
     n[ i + theEnv->_dof ] = 0;
   }
 
   delete [] m;
   delete [] n;
   return z;
 }
 else {
   delete [] m;
   delete [] n;
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator^( const TJet<T1,T2>&, const TJet<T1,T2>& )",
          "Environment not correct for performing bracket." ) );
 }
}


// **************************************************************
// **************************************************************
// **************************************************************
//
//      Overloaded analytic functions

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> acos( const TJet<T1,T2>& x ) 
{
  // Returns answer in (0,pi) if asin returns (-pi/2,pi/2).
  return ( ((T1) M_PI_2) - asin(x) );
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> asin( const TJet<T1,T2>& x ) 
{ 
 static TJet<T1,T2> z;
 static TJet<T1,T2> dz;
 static int iter;

 iter = 0;

 z.DeepCopy( x );
 dz = ( sin(z) - x ) / cos(z);

 while( ( dz != ((T1) 0.0) ) && ( iter++ < MX_MAXITER ) ) {
  z -= dz;
  dz = ( sin(z) - x ) / cos(z);
 }

 if( iter >= MX_MAXITER ) {
  cerr << "*** WARNING ***                             \n";
  cerr << "*** WARNING *** TJL<T1,T2>& asin( TJL<T1,T2>& x ) {         \n";
  cerr << "*** WARNING *** Over " 
       << MX_MAXITER 
       << " iterations used;    \n";
  cerr << "*** WARNING *** result may be incorrect.    \n";
  cerr << "*** WARNING ***                             \n";
  cerr << "*** WARNING *** Initial value:              \n";
  cerr << "*** WARNING ***                             \n"
       << endl;
  x.printCoeffs();
  cerr << "*** WARNING *** A few iterations:             " << endl;
  for( iter = 0; iter < 4; iter++ ) {
   cerr << "*** WARNING *** Iteration No. " << ( iter+1 ) << endl;
   z -= dz;
   dz = ( sin(z) - x ) / cos(z);
   cerr << "*** WARNING *** dz = " << endl;
   dz.printCoeffs();
   cerr << "*** WARNING *** z = " << endl;
   z.printCoeffs();
  }
 }

 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> atan( const TJet<T1,T2>& x ) 
{   // ??? START HERE, LEO!!!
 static TJet<T1,T2> z;
 static TJet<T1,T2> c, dz;
 static int iter;

 iter = 0;

 z.DeepCopy( x );
 c = cos( z );
 dz = c*( sin(z) - x*c );

 while( ( dz != ((T1) 0.0) ) && ( iter++ < MX_MAXITER ) ) {
  z -= dz;
  c = cos( z );
  dz = c*( sin(z) - x*c );
 }

 if( iter >= MX_MAXITER ) {
  cerr <<  "*** WARNING ***                             \n" 
       <<  "*** WARNING *** TJL<T1,T2>& atan( TJL<T1,T2>& x ) {         \n" 
       <<  "*** WARNING *** Over " 
                << MX_MAXITER  << " iterations used;    \n"
       <<  "*** WARNING *** result may be incorrect.    \n" 
       <<  "*** WARNING ***                             \n" 
       << endl;
 }

 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> cos( const TJet<T1,T2>& x ) 
{ 
 
 // ??? REMOVE: TJet<T1,T2> epsSin( const TJet<T1,T2>& );
 // ??? REMOVE: TJet<T1,T2> epsCos( const TJet<T1,T2>& );
 // ??? REMOVE: 
 static TJet<T1,T2> epsilon;
 static T1 cs, sn;
 dlist_iterator getNext( *(dlist*) x._jl );
 static TJLterm<T1,T2>* p;
 
 // Paranoid initializations
 epsilon.Reconstruct( x->_myEnv );
 cs = 0.0;
 sn = 0.0;
 p  = 0;

 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> cos( const TJet<T1,T2>& ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new TJLterm<T1,T2>( x->_myEnv->_allZeroes, ((T1) 1.0), x->_myEnv ) );
   return epsilon;
 }
 
 p = (TJLterm<T1,T2>*) getNext();
 if( ( p -> _weight ) == 0 ) {         // x has non-zero standard part
   cs = std::cos( p -> _value );
   sn = std::sin( p -> _value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   delete p;
   return cs*TJet<T1,T2>::_epsCos( epsilon ) - sn*TJet<T1,T2>::_epsSin( epsilon );
   }
 else {                               // x is pure infinitesimal
   return TJet<T1,T2>::_epsCos( x );
   }
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> cosh( const TJet<T1,T2>& x ) 
{ 
 static TJet<T1,T2> z;
 z = exp(x);
 z = ( z + ( 1.0 / z ) ) / 2.0;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> exp( const TJet<T1,T2>& x ) 
{ 
 
 // ??? REMOVE: TJet<T1,T2> epsExp( const TJet<T1,T2>& );
 static TJet<T1,T2> epsilon;
 static T1 factor;
 dlist_iterator getNext( *(dlist*) x._jl );
 static TJLterm<T1,T2>* p;

 // Paranoid initializations
 epsilon.Reconstruct( x->_myEnv );
 factor = 0.0;
 p = 0;
 
 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> exp( const TJet<T1,T2>& ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new TJLterm<T1,T2>( x->_myEnv->_allZeroes, ((T1) 1.0), x->_myEnv ) );
   return epsilon;
 }
 
 p = (TJLterm<T1,T2>*) getNext();
 if( ( p -> _weight ) == 0 ) {         // x has non-zero standard part
   factor = std::exp( p -> _value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   delete p;
   return factor*TJet<T1,T2>::_epsExp( epsilon );
   }
 else {                               // x is pure infinitesimal
   return TJet<T1,T2>::_epsExp( x );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> log ( const TJet<T1,T2>& x ) 
{ 
 static TJet<T1,T2>             epsilon;
 static TJet<T1,T2>             u, w;
 static T1 std;
 static double           n;
 static TJLterm<T1,T2>*         p;
 dlist_iterator  getNext( *(dlist*) x._jl );
 
 // Paranoid initializations
 epsilon.Reconstruct( x->_myEnv );
 u.Reconstruct( x->_myEnv );
 w.Reconstruct( x->_myEnv );
 
 std = 0.0;
 n = 0.0;
 p = 0;
 
 if( x->_count == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> log ( const TJet<T1,T2>& ) { ",
          "Argument is zero." ) );
 }
 
 p = (TJLterm<T1,T2>*) getNext();
 if( ( ( p -> _weight      ) ==  0   ) &&
     ( ( std = p -> _value ) !=  0.0 )
   )                                  // x has non-zero standard part
   {
   if( x->_count == 1 ) {             // x may have no derivatives
     epsilon = log( std );
     return epsilon;
   }
   epsilon.DeepCopy( x );            // x must not be altered by the routine
   p = epsilon.get();                //   pops the standard part off epsilon
   delete p;
   epsilon.scaleBy( - 1.0/std );
   w = epsilon;
   u = epsilon;                      // We use here the formulae
   n = 1.0;                          // ln( s + e )   = ln s + ln( 1 + e/s ), and
   while( u->_count > 0 ) {           // ln( 1 + e/s ) = ln( 1 - ( -e/s )) 
     u *= epsilon;                   //               = - sum ( - e/s )^n / n
     w += ( u / ++n );
   }
 
   return ( log(std) - w );
 // -------------------
 
   }
 else                                 // x has zero standard part
   {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> log ( const TJet<T1,T2>& ) ",
          "Argument's standard part vanishes; it is nilpotent." ) );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> log10( const TJet<T1,T2>& x ) 
{
 static const T1 logE = 0.4342944819032518276511289;
 return  logE*log(x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> pow( const TJet<T1,T2>& x, const double& s ) 
{ 
 // ??? REMOVE: TJet<T1,T2> epsPow( const TJet<T1,T2>&, const double& );
 static TJet<T1,T2> epsilon;
 static T1 factor;
 static T1 std;
 dlist_iterator getNext( *(dlist*) x._jl );
 static TJLterm<T1,T2>* p;
 static int u;

 // Paranoid initializations
 epsilon.Reconstruct( x->_myEnv );
 factor = std = 0.0;
 p = 0;
 u = 0;
 
 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> pow( const TJet<T1,T2>&, double ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new TJLterm<T1,T2>( x->_myEnv->_allZeroes, ((T1) 0.0), x->_myEnv ) );
   return epsilon;
 }
 
 p = (TJLterm<T1,T2>*) getNext();
 if( ( ( p -> _weight      ) ==  0   ) &&
     ( ( std = p -> _value ) !=  0.0 )
   )                                  // x has non-zero standard part
   {
   if( x->_count == 1 ) {             // x may have no derivatives
     epsilon = pow( std, s );
     return epsilon;
   }
   factor = std::pow( std, s );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 //   pops the standard part off epsilon
   delete p;
   epsilon.scaleBy( 1.0/std );
   epsilon = factor*TJet<T1,T2>::_epsPow( epsilon, s );
   return epsilon;
   }
 else                                 // x is pure infinitesimal
   {
   u = nearestInteger( s );
   if( s != (T1) u ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> pow( const TJet<T1,T2>&, const double& )",
            "Cannot use infinitesimal as base with non-integer _exponent." ) );
   }
   epsilon = ((T1) 1.0);
   if ( u == 0 ) {
     return epsilon;
   }
   if ( u > 0 ) {
     while( u-- > 0 ) epsilon *= x;
     return epsilon;
     }
   else {
     while( u++ < 0 ) epsilon *= x;
     epsilon = 1.0 / epsilon;
     return epsilon;
     }
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> pow( const TJet<T1,T2>& x, int n ) 
{
  static int i;
  static TJet<T1,T2> z;

  // Paranoid initializations
  z.Reconstruct( x->_myEnv );
  i = 0;

  if( n == 0 ) z = ((T1) 1.0);
  else if( n > 0 ) {
    z.DeepCopy( x );
    for( i = 2; i <= n; i++ ) z *= x;
  }
  else {
    TJet<T1,T2> xr;
    z = 1.0 / x;
    xr = z;
    for( i = -2; i >= n; i-- ) z *= xr;
  }
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> sin( const TJet<T1,T2>& x ) 
{ 
 
 // ??? REMOVE: TJet<T1,T2> epsSin( const TJet<T1,T2>& );
 // ??? REMOVE: TJet<T1,T2> epsCos( const TJet<T1,T2>& );
 static TJet<T1,T2> epsilon;
 static T1 cs, sn;
 dlist_iterator getNext( *(dlist*) x._jl );
 static TJLterm<T1,T2>* p;

 // Paranoid initializations
 epsilon.Reconstruct( x->_myEnv );
 cs = sn = 0.0;
 p = 0; 
 
 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> sin( const TJet<T1,T2>&, double ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new TJLterm<T1,T2>( x->_myEnv->_allZeroes, ((T1) 0.0), x->_myEnv ) );
   return epsilon;
 }
 
 p = (TJLterm<T1,T2>*) getNext();
 if( ( p -> _weight ) == 0 ) {         // x has non-zero standard part
   cs = std::cos( p -> _value );
   sn = std::sin( p -> _value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   delete p;
   return sn*TJet<T1,T2>::_epsCos( epsilon ) + cs*TJet<T1,T2>::_epsSin( epsilon );
   }
 else {                               // x is pure infinitesimal
   return TJet<T1,T2>::_epsSin( x );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> sinh( const TJet<T1,T2>& x ) 
{
 TJet<T1,T2> z;
 z = exp(x);
 return ( z - (1.0/z))/2.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> sqrt( const TJet<T1,T2>& x ) 
{
 // ??? REMOVE: TJet<T1,T2> epsSqrt( const TJet<T1,T2>& );
 static TJet<T1,T2> epsilon;
 static T1 factor;
 static T1 std;
 dlist_iterator getNext( *(dlist*) x._jl );
 static TJLterm<T1,T2>* p;

 // Paranoid initializations
 epsilon.Reconstruct( x->_myEnv );
 factor = std = 0.0;
 p = 0; 
 
 if( x->_count == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> sqrt( const TJet<T1,T2>& ) { ",
          "Argument is zero." ) );
 }
 
 p = (TJLterm<T1,T2>*) getNext();
 if( ( ( p -> _weight      ) ==  0   ) &&
     ( ( std = p -> _value ) !=  0.0 )
   )                                  // x has non-zero standard part
   {
   factor = std::sqrt( std );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   if( epsilon->_count == 1 )          // x may have no derivatives
     {
     epsilon = factor;
     return epsilon;
     }
   else                               // normal case
     {
     p = epsilon.get();               // pops the standard part off epsilon
     delete p;
     epsilon.scaleBy( 1.0/std );
     return factor*TJet<T1,T2>::_epsSqrt( epsilon );
     }
   }
 else                                 // x is pure infinitesimal
   {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> sqrt( const TJet<T1,T2>& ) ",
          "Argument's standard part vanishes; it is nilpotent." ) );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> tan( const TJet<T1,T2>& x ) 
{ 
 return sin(x) / cos(x) ;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> tanh( const TJet<T1,T2>& x ) 
{ 
 return sinh(x) / cosh(x);
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

template<typename T1, typename T2>
TJet<T1,T2> erfc( const TJet<T1,T2>& z ) 
{
  static const T1 one = ((T1) 1.0);
  return ( one - erf( z ) );
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



// **************************************************************
// **************************************************************
// **************************************************************
//
//      Auxiliary functions which operate on infinitesimals.
//      It is assumed that the calling program checks to be sure
//      that the argument has zero standard part.
 
template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::_epsCos( const TJet<T1,T2>& epsilon ) 
{ 
 
 static TJet<T1,T2> z;
 static TJet<T1,T2> epsq, term;
 static double n;

 z->_myEnv = epsilon->_myEnv;
 z = ((T1) 1.0);
 
 epsq = - epsilon*epsilon;
 
 term = epsq / 2.0;
 n = 2.0;
 
 z->_myEnv = epsilon->_myEnv;
 
 while( term->_count > 0 ) {
   z += term;
   term = ( ( term*epsq ) / ++n ) / ++n;
   }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::_epsExp( const TJet<T1,T2>& epsilon ) 
{ 
 static TJet<T1,T2> z;
 static TJet<T1,T2> term;
 static double n;
 
 z->_myEnv = epsilon->_myEnv;
 z = ((T1) 1.0);
 
 term = epsilon;
 n = 1.0;
 
 z->_myEnv = epsilon->_myEnv;  // ??? Redundant?
 
 while( term->_count > 0 ) {
   z += term;
   term = ( term*epsilon ) / ++n;
 }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::_epsPow( const TJet<T1,T2>& epsilon, const double& s ) 
{ 
 static TJet<T1,T2> z;
 static TJet<T1,T2> term;
 static double f;
 static double n;

 z->_myEnv = epsilon->_myEnv;
 z = ((T1) 1.0);
 
 f = s;
 n = 1.0;
 term = s*epsilon;
 
 z->_myEnv = epsilon->_myEnv;
 
 while( term->_count > 0 ) {
   z += term;
   term = ( ((T1) (--f)) * term * epsilon ) / ++n;
   }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::_epsSin( const TJet<T1,T2>& epsilon ) 
{ 
 
 static TJet<T1,T2> z;
 static TJet<T1,T2> epsq, term;
 static double n;
 
 epsq = - epsilon*epsilon;
 z = epsilon;
 term = ( epsilon*epsq ) / 6.0;
 n = 3.0;
 
 while( term->_count > 0 ) {
   z += term;
   term = ( ( term*epsq ) / ++n ) / ++n;
 }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::_epsSqrt( const TJet<T1,T2>& epsilon ) 
{  

 // This function is identical to epsPow
 // with the substitution  s = 1/2

 static TJet<T1,T2>     z;
 static TJet<T1,T2>     term;
 static double   f;
 static double   n;
 
 z->_myEnv = epsilon->_myEnv;
 z = ((T1) 1.0);
 
 f    = 1.0 / 2.0;
 n    = 1.0;
 term = f*epsilon;
 
 z->_myEnv = epsilon->_myEnv;
 
 while( term->_count > 0 ) {
   z += term;
   term *= ( ((T1) (--f)) * epsilon ) / ++n;
 }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}
 

// **************************************************************
// **************************************************************
// **************************************************************
//
//      Implementation of Class TJet<T1,T2>

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::peekAt() const 
{
 dlist_traversor getNext( *(dlist*) _jl );
 int i;
 TJLterm<T1,T2>* p;
 dlink* q;
 cout << "\nCount  = "
      << _jl->_count
      << " Weight = "
      << _jl->_weight
      << " Max accurate weight = "
      << _jl->_accuWgt
      << endl;
 cout << "Reference point: ";
 for( i = 0; i < _jl->_myEnv->_numVar; i++ ) 
    cout << setprecision(12) << _jl->_myEnv->_refPoint[i] << "  ";
 cout << endl;
 while((  q = getNext()  )) {
   p = (TJLterm<T1,T2>*) q->info();
   cout << "Weight: "
        << p -> _weight
        << "   Value: "
        << p -> _value
        << " || ";
   cout << "Addresses: "
        << (int) q->prevPtr() << "  "
        << (int) q            << "  "
        << (int) q->nextPtr() 
        << " : "
        << (int) p
        << endl;
   cout << "Index:  ";
   for( i = 0; i < _jl->_myEnv->_numVar; i++ )
     cout << (p -> _index)(i) << "  ";
   cout << "\n" << endl;
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::printCoeffs() const 
{
 dlist_iterator getNext( *(dlist*) _jl );
 int i;
 TJLterm<T1,T2>* p;

 cout << "\nCount  = " << _jl->_count 
      << ", Weight = " << _jl->_weight 
      << ", Max accurate weight = " << _jl->_accuWgt << endl;
 cout << "Reference point: " 
      << _jl->_myEnv->_refPoint[0];
 for( i = 1; i < _jl->_myEnv->_numVar; i++ ) {
   cout << ", ";
   cout << _jl->_myEnv->_refPoint[i];
 }
 cout << endl;

 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   if( p->_weight > _jl->_accuWgt ) break;
   cout << "Index:  " 
        << p->_index
        << "   Value: "
        << p->_value
        << endl;
 }

 cout << "\n" << endl;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::writeToFile( char* fileName ) const 
{
 ofstream outStr( fileName );
 this->writeToFile( outStr );
 outStr.close();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::writeToFile( ofstream& outStr ) const 
{
 _jl->writeToFile( outStr );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::scaleBy( T1 y ) 
{
 PREPFORCHANGE(_jl)
 _jl->scaleBy( y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJLterm<T1,T2>* TJet<T1,T2>::get() 
{
 PREPFORCHANGE(_jl)
 return _jl->get();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJLterm<T1,T2> TJet<T1,T2>::firstTerm() const 
{
 return _jl->firstTerm();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJLterm<T1,T2> TJet<T1,T2>::lowTerm() const 
{
 return _jl->lowTerm();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
T1 TJet<T1,T2>::standardPart() const 
{
 return _jl->standardPart();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::clear() 
{
 PREPFORCHANGE(_jl)
 _jl->clear();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
T1 TJet<T1,T2>::weightedDerivative( const int* ind ) const 
{
 return _jl->weightedDerivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
T1 TJet<T1,T2>::derivative( const int* ind ) const 
{
 return _jl->derivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::filter( const int& wgtLo, const int& wgtHi ) const 
{ 
 TJet<T1,T2> z;
 TJL<T1,T2>* zPtr = z._jl;
 TJL<T1,T2>* thisPtr = _jl;
 dlist_iterator getNext( *(dlist*) thisPtr );
 TJLterm<T1,T2>* p;
 TJLterm<T1,T2>* q;
 int numTerms;
 int wgt, upperWgt;
 
 z._jl->_myEnv = _jl->_myEnv;

 if( ( wgtLo <= 0 ) && 
     ( wgtHi >= thisPtr->_weight )   
   ) {
   z = *this;
   return z;
 } 

 numTerms = 0;
 upperWgt = 0;
 
 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   wgt = p->_weight;
   if( ( wgt >= wgtLo ) && ( wgt <= wgtHi ) ) {
     q = new TJLterm<T1,T2>( p );
     zPtr->append( q );
     if( wgt > upperWgt ) upperWgt = wgt;
     numTerms++;
     }
   }
 
 zPtr->_count = numTerms;   // ??? Is this necessary?
 zPtr->_weight = upperWgt;  // ??? Where is the copy of the re
 zPtr->_accuWgt = thisPtr->_accuWgt;   // ??? reference point???

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::filter( bool (*f) ( const IntArray&, const T1& ) ) const 
{ 
 static TJet<T1,T2> z;
 static TJL<T1,T2>* zPtr;
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;

 z.Reconstruct( _jl->_myEnv ); 

 zPtr = z._jl;
 p    = 0;
 q    = 0;

 dlist_iterator getNext( *(dlist*) _jl );

 static int i;
 static int nv;
 static IntArray oldIndex;

 nv = _jl->_myEnv->_numVar;
 oldIndex.Reconstruct( nv );

 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
  oldIndex = p->_index;

  if( (*f)( p->_index, p->_value ) ) {
   q = new TJLterm<T1,T2>( p );
   zPtr->append( q );
  }

  for( i = 0; i < nv; i++ ) 
   if( oldIndex(i) != p->_index(i) ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<T1,T2> TJet<T1,T2>::filter( char (*f) ( const IntArray&, "
           "const T1& ) ) const",
           "Test routine changes value of _index array." ) );
   }
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::_concat() const 
{
 static TJet<T1,T2> v;
 dlist_iterator getNext( *(dlist*) _jl );
 static TJLterm<T1,T2>* p;
 static int i;
 
 p = 0;
 i = 0;

 v.Reconstruct( (( (_jl->_myEnv)->_TJLmonomial )[0] )->_myEnv );
 
 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   if( p->_weight > _jl->_accuWgt ) break;
   for( i = 0; i < _jl->_myEnv->_numVar; i++ )
     _jl->_myEnv->_exponent[i] = (p->_index)(i);
   _jl->_myEnv->_monoCode();
   v += ( p->_value ) * ( _jl->_myEnv->_TJLmonomial[ _jl->_myEnv->_monoRank() ] );
 }
 
 return v;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::operator() ( const TJetVector<T1,T2>& y ) const 
{
 static int i, n;   // ??? This should be rewritten 
 static TJet<T1,T2> z;      // ??? so as to be self-contained.

 n = _jl->_myEnv->_numVar;
 TJet<T1,T2>* u = new TJet<T1,T2> [ n ];

 for( i = 0; i < n; i++ ) u[i] = y(i);
 z = operator()( u );

 delete [] u;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::operator() ( const TJet<T1,T2>* y ) const 
{ 
 TJet<T1,T2>*     u = new TJet<T1,T2> [ _jl->_myEnv->_numVar ];
 static TJet<T1,T2>      z, term;
 static int      i, j, w;

 z.Reconstruct( y[0]->_myEnv );

 // Initializing term should not be necessary.

 i = j = w = 0;

 // Check consistency of reference points and
 // subtract reference point prior to concatenation.
 for( i = 0; i < _jl->_myEnv->_numVar; i++ ) {
   if( y[i]->_myEnv != y[0]->_myEnv ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> TJet<T1,T2>::operator() ( const TJet<T1,T2>* ) const ",
            "Inconsistent environments." ) );
   }
   u[i] = y[i] - _jl->_myEnv->_refPoint[i];
 }
 
 // Evaluate and store _monomials.
 
 // The zeroth one.
 ( ( _jl->_myEnv->_TJLmonomial )[0] )->_myEnv = y[0]->_myEnv; // Needed by 
                                                        // TJet<T1,T2>::concat
 _jl->_myEnv->_TJLmonomial[0] = ((T1) 1.0);

 // For all higher weights ...
 for( w = 1; w <= _jl->_accuWgt; w++ )
 
   // Get the next set of _exponents of weight w.
   while( nexcom( w, _jl->_myEnv->_numVar, _jl->_myEnv->_exponent ) ) {
 
     // Find the first non-zero _exponent.
     i = 0;
     while( !( _jl->_myEnv->_exponent[i++] ) ) ;
     i--;
 
     // The value of the _monomial associated with this composition
     // is obtained by multiplying a factor into a previously
     // computed _monomial.
     ( _jl->_myEnv->_exponent[i] )--;
     _jl->_myEnv->_monoCode();
     term = _jl->_myEnv->_TJLmonomial[ _jl->_myEnv->_monoRank() ];
     ( _jl->_myEnv->_exponent[i] )++;
     _jl->_myEnv->_monoCode();
     _jl->_myEnv->_TJLmonomial[ _jl->_myEnv->_monoRank() ] = term * u[i]; // ??? Is this OK???
 
   } // End while loop.
 
 // Monomials have been stored.
 // Now traverse the TJL<T1,T2> variable and evaluate.
 // Formerly done by using concat routine, which
 // I copy here
 {
  dlist_iterator getNext( *(dlist*) _jl );
  static TJLterm<T1,T2>* p;
  static int i;
  
  p = 0;
  i = 0;
 
  z->_myEnv = (( _jl->_myEnv->_TJLmonomial )[0] )->_myEnv;
  
  while((  p = (TJLterm<T1,T2>*) getNext()  )) {
    if( p->_weight > _jl->_accuWgt ) break;
    for( i = 0; i < _jl->_myEnv->_numVar; i++ )
      _jl->_myEnv->_exponent[i] = (p->_index)(i);
    _jl->_myEnv->_monoCode();
    z += ( p->_value ) * ( _jl->_myEnv->_TJLmonomial[ _jl->_myEnv->_monoRank() ] );
  }
 }

 
 // Finish...
 delete [] u;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
T1 TJet<T1,T2>::operator() ( const T1* x ) const 
{
 return _jl->operator()( x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::D( const int* n ) const 
{
 // ??? REMOVE static char noTermAdded;
 static char doit;
 static int f, i, j, k, w;
 static TJet<T1,T2> z;
 dlist_iterator getNext( *(dlist*) _jl );
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;
 
 // --- Initializations.
 // ??? REMOVE noTermAdded = 1;
 f = i = j = k = w = 0;

 z.Reconstruct( _jl->_myEnv );

 p = 0;
 q = 0;
 

 // --- Preliminary check of _index set.
 
 w = 0;
 
 for( i = 0; i < _jl->_myEnv->_numVar; i++ ) {
   if( n[i] < 0 ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> TJet<T1,T2>::D( const int* ) const",
            "Cannot differentiate with negative _index." ) );
     }
   w += n[i];
 }
 
 if( w > _jl->_accuWgt ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> TJet<T1,T2>::D( const int* ) const",
          "Differentiation request beyond accuracy allowed." ) );
 }
 
 if( w == 0 ) {
   z = *this;
   return z;
 }


 // --- Construct the derivative one link at a time.
 // --- ( See note Obs.4 )
 
 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
 
   q = new TJLterm<T1,T2>( p );
 
   doit = 1;
   // -- Reset the _index.
   for( i = 0; i < _jl->_myEnv->_numVar; i++ ) 
     doit = doit && ( ( q->_index(i) -= n[i] ) >= 0 );
 
   if( doit ) {
     // -- Build factorial multiplier.
     f = 1;
     for( k = 0; k < _jl->_myEnv->_numVar; k++ ) {
       j = q->_index(k);
       for( i = 0; i < n[k]; i++ ) f *= ++j;
     }
     if( f <= 0 ) {
       throw( GenericException( __FILE__, __LINE__, 
              "TJet<T1,T2> TJet<T1,T2>::D( const int* ) const",
              "Horrible, unexplainable error while differentiating!" ) );
     }                           // Super fussbudget!! ( f "must" be positive )
   
     // -- Make final changes in private data of the TJLterm<T1,T2> and
     //    absorb it into the answer.
     (q->_value) *= f;              // OK. So I'm a fussbudget with parentheses again ..
     (q->_weight ) -= w;
   
     // ??? REMOVE noTermAdded = 0;
     z->addTerm( q );               // ??? This also may be unnecessarily conservative.
                                    // The append function may be adequate, if the
                                    // ordering is preserved. ???
   } 

   else delete q;                   // ??? Is this correct?
                                    // ??? It wasn't here before!
 }
 
 // ??? REMOVE if( noTermAdded ) z = 0.0;
 
 
 // --- Finally, adjust _accuWgt and return value
 
 z->_accuWgt = _jl->_accuWgt - w;       // ??? Is this correct ???

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::D( const IntArray& n ) const 
{

 if( n.Dim() != _jl->_myEnv->_numVar ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> TJet<T1,T2>::D( const IntArray& ) const ",
          "Inconsistent dimensions." ) );
 }

 // ??? REMOVE static char noTermAdded;
 static char doit;
 static int f, i, j, k, w;
 static TJet<T1,T2> z;
 dlist_iterator getNext( *(dlist*) _jl );
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;
 
 // --- Initializations.
 // ??? REMOVE noTermAdded = 1;
 f = i = j = k = w = 0;

 z.Reconstruct( _jl->_myEnv );

 p = 0;
 q = 0;
 

 // --- Preliminary check of _index set.
 
 w = 0;
 
 for( i = 0; i < _jl->_myEnv->_numVar; i++ ) {
   if( n(i) < 0 ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> TJet<T1,T2>::D( const IntArray& ) const ",
            "Cannot differentiate with negative _index." ) );
   }
   w += n(i);
 }
 
 if( w > _jl->_accuWgt ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> TJet<T1,T2>::D( const IntArray& ) const ",
          "Differentiation request beyond accuracy allowed." ) );
 }
 
 if( w == 0 ) {
   z = *this;
   return z;
 }


 // --- Construct the derivative one link at a time.
 // --- ( See note Obs.4 )
 
 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
 
   q = new TJLterm<T1,T2>( p );
 
   doit = 1;
   // -- Reset the _index.
   for( i = 0; i < _jl->_myEnv->_numVar; i++ ) 
     doit = doit && ( ( q->_index(i) -= n(i) ) >= 0 );
 
   if( doit ) {
     // -- Build factorial multiplier.
     f = 1;
     for( k = 0; k < _jl->_myEnv->_numVar; k++ ) {
       j = q->_index(k);
       for( i = 0; i < n(k); i++ ) f *= ++j;
       }
     if( f <= 0 ) {
       throw( GenericException( __FILE__, __LINE__, 
              "TJet<T1,T2> TJet<T1,T2>::D( const IntArray& ) const",
              "Horrible, unexplainable error while differentiating!" ) );
       }                           // Super fussbudget!! ( f "must" be positive )
   
     // -- Make final changes in private data of the TJLterm<T1,T2> and
     //    absorb it into the answer.
     (q->_value) *= f;              // OK. So I'm a fussbudget with parentheses again ..
     (q->_weight ) -= w;
   
     // ??? REMOVE noTermAdded = 0;
     z->addTerm( q );               // ??? This also may be unnecessarily conservative.
                                    // The append function may be adequate, if the
                                    // ordering is preserved. ???
   }

   else delete q;                   // ??? Is this correct?
                                    // ??? It wasn't here before.
 }
 
 // ??? REMOVE if( noTermAdded ) z = 0.0;
 
 
 // --- Finally, adjust _accuWgt and return value
 
 z->_accuWgt = _jl->_accuWgt - w;       // ??? Is this correct ???

 return z;
}


// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------

#undef PREPFORCHANGE

#endif // TJET_TCC
