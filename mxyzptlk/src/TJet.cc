/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJet.cc
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

#include <iomanip>

#include "GenericException.h"
#include "TJet.h"

using namespace std;

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
  setVariable( x, index, this->Env() );  // This alters the environment!
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


template TJet<double,FNAL::Complex>;
template TJet<FNAL::Complex,double>;
