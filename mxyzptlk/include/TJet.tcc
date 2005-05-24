/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJet.tcc
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
******             ostiguy@fnal.gov
******
******  Efficiency improvements.
******  - new memory management
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

class Vector;

#include <iosetup.h>
#include <GenericException.h>
#include <TJet.h>
#include <TJetVector.h>
#include <MathConstants.h>



#define PREPFORCHANGE(_jl)  if(((_jl)->_rc)>1){--((_jl)->_rc);(_jl) = TJL<T1,T2>::makeTJL(_jl);}


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
using FNAL::pcerr;
using FNAL::pcout;


// ***************************************************************
// ***************************************************************
// ***************************************************************

template<typename T1, typename T2>
TJet<T1,T2>::TJet( TJetEnvironment<T1,T2>* pje ) 
{
 _jl = TJL<T1,T2>::makeTJL( pje );
 _constIterPtr = 0;
 _iterPtr     = 0;
}

template<typename T1, typename T2>
TJet<T1,T2>::TJet( T1 x, TJetEnvironment<T1,T2>* p_je ) 
{
 _jl = TJL<T1,T2>::makeTJL( x, p_je );
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
   TJL<T1,T2>::discardTJL(_jl); 
 }
}

template<typename T1, typename T2>
void TJet<T1,T2>::Reconstruct()
{
 // Combines destructor and constructor functions.
 // Use when initializing a static TJet variable.
 TJetEnvironment<T1,T2>* pje;
 pje = _jl->_myEnv;
 if( --(_jl->_rc) == 0 ) TJL<T1,T2>::discardTJL(_jl); 
  _jl = TJL<T1,T2>::makeTJL( pje );
}

template<typename T1, typename T2>
void TJet<T1,T2>::Reconstruct( TJetEnvironment<T1,T2>* pje )
{
 // Combines destructor and constructor functions.
 // Use when initializing a static TJet variable.
 if( --(_jl->_rc) == 0 ) TJL<T1,T2>::discardTJL(_jl); 
 _jl = TJL<T1,T2>::makeTJL( pje );
}


template<typename T1, typename T2>
void TJet<T1,T2>::Reconstruct( const IntArray& e, 
                               const T1& x, 
                               TJetEnvironment<T1,T2>* pje )
{
 // Combines destructor and constructor functions.
 // Use when initializing a static TJet variable.
 if( --(_jl->_rc) == 0 ) TJL<T1,T2>::discardTJL(_jl); 
 _jl = TJL<T1,T2>::makeTJL( e, x, pje );
}


template<typename T1, typename T2>
void TJet<T1,T2>::setEnvTo( const TJet& x )
{
  if( _jl == NULL ) 
  {
    _jl = TJL<T1,T2>::makeTJL( x._jl->_myEnv );
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
    _jl = TJL<T1,T2>::makeTJL( pje );
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
   _jl = TJL<T1,T2>::makeTJL( pje );
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
    _jl = TJL<T1,T2>::makeTJL( pje );
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
   if( --(_jl->_rc) == 0 ) TJL<T1,T2>::discardTJL(_jl); 
   _jl = x._jl;
   (_jl->_rc)++;
 }
 return *this; 
}

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::DeepCopy( const TJet& x ) 
{
 if( this != &x ) {
  if( --(_jl->_rc) == 0 ) TJL<T1,T2>::discardTJL(_jl); 
   _jl = TJL<T1,T2>::makeTJL( x->_myEnv );
  *_jl = *(x._jl);
 }
 return *this; 
}


template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator=( const T1& x ) 
{
 TJetEnvironment<T1,T2>* pje = _jl->_myEnv;

 if( --(_jl->_rc) == 0 ) TJL<T1,T2>::discardTJL(_jl); 
  _jl = TJL<T1,T2>::makeTJL(pje);
 *(_jl) = x;
 return *this; 
}

template<typename T1, typename T2>
istream& operator>>( istream& is,  TJet<T1,T2>& x ) 
{
 if( --(x._jl->_rc) == 0 )  TJL<T1,T2>::discardTJL(x._jl); 
 x._jl = TJL<T1,T2>::makeTJL();
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
    throw( typename TJL<T1,T2>::HorribleException( 
           _currentIndex, _newCoords.size(), _newValues.size(),
           __FILE__, __LINE__, 
           "TJetEnvironment<T1,T2>* TJet<T1,T2>::EndEnvironment( double* )",
           "" ) );
  }

  _workEnv->_numVar = _currentIndex;
  if( !_workEnv->_pbok ) { _workEnv->_spaceDim = _currentIndex; }
  _workEnv->_dof = _workEnv->_spaceDim / 2;
  if( _workEnv->_pbok && ( 2*_workEnv->_dof != _workEnv->_spaceDim ) ) {
    (*pcerr) << "\n\n"
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

  _workEnv->_offset.reconstruct( w, n, false );

  _workEnv->_exponent    = new int[ n ];
  // REMOVE: _workEnv->_expCode     = new char[ w + n ];
  // REMOVE: _workEnv->_monomial    = new T1 [ bcfr ];
  // REMOVE: _workEnv->_TJLmonomial = new TJet<T1,T2> [ bcfr ];
  // REMOVE: for( i = 0; i < bcfr; i++ ) { _workEnv->_TJLmonomial[i].Reconstruct( _workEnv ); }

  // The reference point is set.
  _workEnv->_refPoint = new T1 [ n ];
  slist_iterator GetNextValue( _newValues );
  i = 0;
  while( ( q = (T1*) GetNextValue() ) != 0 ) {
    _workEnv->_refPoint[i++] = *q;
  }
  if( i != n ) {
    throw( typename TJL<T2,T2>::HideousException(i, n, __FILE__, __LINE__, 
             "TJetEnvironment<T1,T2>* TJet<T1,T2>::EndEnvironment( double* )", 
             "" ) );
  }
 
  _workEnv->_allZeroes.Reconstruct(n);
  for( i = 0; i < n; i++ ) _workEnv->_allZeroes(i) = 0;
 
  // OBSOLETE // Load the _numPaths array with binomial coefficients;
  // OBSOLETE // required by Wilf's algorithm for ranking monomials.
  // OBSOLETE _workEnv->_numPaths = new MatrixI( w+1, n );
  // OBSOLETE for( i = 0; i <= w; i++ ) {
  // OBSOLETE   for( j = 1; j <= n; j++ ) {
  // OBSOLETE     (*(_workEnv->_numPaths))( i, j-1 ) = bcfRec( i + j - 1, i );
  // OBSOLETE   }
  // OBSOLETE }

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
    delete q; 

  }
  
  // Build the scratchpads: _monomial, _TJLmonomial, and _TJLmml
  _workEnv->_buildScratchPads();

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
: TJet<T1,T2>( TJet<T1,T2>::_workEnv ) {
 
 if( ! TJet<T1,T2>::_workEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "Tcoord<T1,T2>::coord( T1 ) ",
          "Use TJet<T1,T2>::BeginEnvironment() to open an environment first." ) );
 }

 _index = TJet<T1,T2>::_currentIndex++;
 
 TJet<T1,T2>::_newCoords.append( this );
 TJet<T1,T2>::_newValues.append( new T1( x ) );
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
void Tcoord<T1,T2>::operator=( const T1& ) 
{
  throw( GenericException( __FILE__, __LINE__, 
         "void Tcoord<T1,T2>::operator=( const T1& x ) ",
         "Changing the value of a coordinate is forbidden." ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// 
// template<typename T1, typename T2>
// void Tcoord<T1,T2>::operator=( const T1& x ) 
// {                                        // DANGER: Be careful!
//   setVariable( x, _index, this->Env() );  // This alters the environment!
// }
// 
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
    _constIterPtr = new dlist_iterator( _jl->_theList );
  }
  else {
    delete _constIterPtr;
    _constIterPtr = new dlist_iterator( _jl->_theList );
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
    _iterPtr = new dlist_iterator( _jl->_theList );
  }
  else {
    delete _iterPtr;
    _iterPtr = new dlist_iterator( _jl->_theList );
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
#define MX_SMALL       1.0e-12 // Used by TJL<T1,T2>::addTerm to decide 
#endif  // MX_SMALL
                            //   removal of a TJL<T1,T2>Cterm.
#ifndef MX_ABS_SMALL
#define MX_ABS_SMALL    1.0e-15 // Used by TJL<T1,T2>::addTerm to decide 
#endif


#ifndef MX_MAXITER
#define MX_MAXITER  20      // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator+=( const TJet<T1,T2>& y ) 
{

 TJet<T1,T2> z( *this );
 (*this) = z+y;
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
 

 TJet<T1,T2> z( *this );
 (*this) = z*y;
 return *this;

}

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator/=( const TJet<T1,T2>& y ) 
{

 TJet<T1,T2> z( *this );
 (*this) = z/y;
 return *this;

}

template<typename T1, typename T2>
TJet<T1,T2>& TJet<T1,T2>::operator/=( const T1& y ) 
{

 TJLterm<T1,T2>* p =0;

 // note: this does not work in mixed mode !

 for ( p = _jl->_jltermStore; p< _jl->_jltermStoreCurrentPtr; p++) {
   if ( !(p->_deleted) ) (p->_value) /= y;
 };  
 
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
{   
 if( _jl == NULL ) {                                         // ??? DANGER: what is the 
   _jl = TJL<T1,T2>::makeTJL();                              // ??? reference point?
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

 TJLterm<T1,T2>* p =0;

 // note: this code does not work in mixed mode (i.e. complex argument and double TJet)!

 for ( p = _jl->_jltermStore; p< _jl->_jltermStoreCurrentPtr; p++) {
   if ( !(p->_deleted) ) (p->_value) *= x;
 };  
 
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
      (*pcerr) << "\n\n"
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
      (*pcerr) << "\n\n"
           << "*** ERROR ***                                       \n"
           << "*** ERROR *** TJet<T1,T2> operator+( TJet<T1,T2>, TJet<T1,T2> )             \n"
           << "*** ERROR ***                                       \n"
           << "*** ERROR *** Null environment for the second       \n"
           << "*** ERROR *** TJet<T1,T2> argument.                         \n"
           << "*** ERROR ***                                       \n"
           << endl;
    }
  }

  // Check for consistency and set reference point of the sum.
  if( x->_myEnv != y->_myEnv ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<T1,T2> operator+( const TJet<T1,T2>& x, const TJet<T1,T2>& )",
           "Inconsistent environments." ) );
  }
  TJetEnvironment<T1,T2>* pje = x->_myEnv;

                              // ??? This should be modified so that
  TJet<T1,T2> z(pje);          // ??? terms beyond the accurate weight of
  TJLterm<T1,T2>* p = 0;        // ??? x or y are not computed and carried
  TJLterm<T1,T2>* q = 0;        // ??? into the answer.


  // If one of the arguments is void, then return the other ..
  if( x->_count < 1 ) {   // This is done in this way so that
    z.DeepCopy( y );      // what is returned does not own
    return z;             // the same data as x or y.
  }
  if( y->_count < 1 ) {
    z.DeepCopy( x );
    return z;
  }
                                 

  // .. otherwise, continue normal operations.

  dlist_iterator getp( x._jl->_theList );
  dlist_iterator getq( y._jl->_theList );

  p = (TJLterm<T1,T2>*) getp();
  q = (TJLterm<T1,T2>*) getq();

  bool pcont = p != 0;
  bool qcont = q != 0;
 
  T1 result; 

  while( qcont && pcont ) {

    if( *p <= *q ) {   
      if( *p %= *q ) { 

  
        result = p->_value + q->_value;
        z->append( new( z._jl->storePtr()) TJLterm<T1,T2>( p->_index, result ) );
       
        p = (TJLterm<T1,T2>*) getp();
        q = (TJLterm<T1,T2>*) getq();

        pcont = (p != 0);
        qcont = (q != 0);
      }
      else {
        z->append( new( z._jl->storePtr()) TJLterm<T1,T2>( p->_index, p->_value ) );
        p = (TJLterm<T1,T2>*) getp();
        pcont = (p != 0);
      }
    }
    else {
       z->append( new( z._jl->storePtr()) TJLterm<T1,T2>( q->_index, q->_value ) );
       q = (TJLterm<T1,T2>*) getq();
       qcont = (q != 0);
    }
  }

  if( pcont ) {
    while(p) {
      z->append( new( z._jl->storePtr() ) TJLterm<T1,T2>( p->_index, p->_value ) );
      p = (TJLterm<T1,T2>*) getp();
    }
  }

  if( qcont ) {
    while(q) {
      z->append( new( z._jl->storePtr() ) TJLterm<T1,T2>( q->_index, q->_value ) );
      q = (TJLterm<T1,T2>*) getq();
    }
  }


  // .....determine the maximum weight computed accurately.
  if( x->_accuWgt < y->_accuWgt ) { z->_accuWgt = x->_accuWgt; }
  else                            { z->_accuWgt = y->_accuWgt; }
 
 
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator+( const TJet<T1,T2>& x, const T1& y ) 
{
 TJet<T1,T2> z;           // !!! "TJet<T1,T2> z = x;" does not work.  The copy constructor
 z.DeepCopy( x );         // ??? is called, which makes x and z own the same data!!
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
 // If the argument is void, then return it ..
 if( y->_count < 1 ) { 
   return y;
 }

 TJet<T1,T2> z( y.Env() );
 TJLterm<T1,T2>* p = 0;

 z.DeepCopy( y ); 

 // .. otherwise continue normal operations.
 
 for ( p = z->_jltermStore; p < z->_jltermStoreCurrentPtr; p++) 
 {
 
  if ( !(p->_deleted) ) p->_value = - p->_value;

 }
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::Negate()
{
TJLterm<T1,T2>* p;

 // If the argument is void, then return ...
 if( _jl->_count < 1 ) return;

 // .. otherwise continue normal operations.
 PREPFORCHANGE(_jl)
 dlist_iterator getNext( _jl->_theList );

 while((  p = (TJLterm<T1,T2>*) getNext()  )) p->_value = - p->_value;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::Mult( const T1& x )
{
 TJLterm<T1,T2>* p;

 // If the argument is void, then return ...
 if( _jl->_count < 1 ) return;

 // .. otherwise continue normal operations.
 PREPFORCHANGE(_jl)
 dlist_iterator getNext( _jl->_theList );
 while((  p = (TJLterm<T1,T2>*) getNext()  )) p->_value *= x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator-( const TJet<T1,T2>& x, const TJet<T1,T2>& y ) 
{  
  return ( x + (-y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator*( const TJet<T1,T2>& x, const TJet<T1,T2>& y ) 
{

 // Check for consistency 

 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> operator*( const TJet<T1,T2>&, const TJet<T1,T2>& ) ",
          "Inconsistent environments." ) );
 };

 
 TJetEnvironment<T1,T2>* pje = x->_myEnv;
 TJet<T1,T2> z(pje);

 TJLterm<T1,T2>* p = 0;
 TJLterm<T1,T2>* q = 0;
 TJL<T1,T2>* xPtr  = x._jl;
 TJL<T1,T2>* yPtr  = y._jl;
 TJL<T1,T2>* zPtr  = z._jl;
 int testWeight    = 0;
 int trialWeight  =  0;
 
 int  indy                = 0;
 TJLterm<T1,T2>* zed      = 0;
 TJLterm<T1,T2>* upperzed = 0;
 T1  dummy                = T1();
 T1  product              = T1();



 // If one of the arguments is void, return it ..
 // Note:: DeepCopy not be necessary here ?

 if( xPtr->_count < 1 ) return z.DeepCopy(x); 
 if( yPtr->_count < 1 ) return z.DeepCopy(y);

 // Determine the maximum weight computed accurately.

 TJLterm<T1,T2> x_1stTerm( xPtr->lowTerm() );
 TJLterm<T1,T2> y_1stTerm( yPtr->lowTerm() );

 testWeight = xPtr->_accuWgt;
 if( ( y_1stTerm._weight != 0 ) && ( y_1stTerm._value !=  T1() ) )
 {
   testWeight += y_1stTerm._weight;
 }

 trialWeight = yPtr->_accuWgt;
 if( ( x_1stTerm._weight != 0 ) && ( x_1stTerm._value !=  T1() ) )
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
 if( (zPtr->_accuWgt) > (pje->_maxWeight) ) 
 { 
   zPtr->_accuWgt = pje->_maxWeight;
 }


 testWeight = zPtr->_accuWgt;

 // avoid dlists !!! 


 for(  p = (yPtr->_jltermStore); p < (yPtr->_jltermStoreCurrentPtr); p++  ) {

   if ( p->_deleted) continue; // ignore deleted terms ! 

   for(  q = (xPtr->_jltermStore); q < (xPtr->_jltermStoreCurrentPtr); q++  ) {

     if ( q->_deleted) continue;

     if( ( p->_weight + q->_weight ) > testWeight ) continue;   

     indy = pje->_offset.index( p->_index + q->_index );

     // Will work even when the exponents are all zero.

     product = p->_value * q->_value;

     dummy = pje->_TJLmml[indy]._value + product;

     if( (std::abs( dummy ) < MX_SMALL*std::abs( product )) || ( std::abs( dummy ) < MX_ABS_SMALL) ) {
       pje->_TJLmml[indy]._value = T1();
     } 
     else {
       pje->_TJLmml[indy]._value = dummy;
     }
 }}

 // At this point, the terms are guaranteed to be ordered !
 // clear the scratchpad and setup the JLterm* dlist 
 // this could be optimized further by using use another mean to clear scratch pad  

 // Transfer answer from the scratchpad and append only monomials with non-zero coefficients

 zed      = pje->_TJLmml;
 upperzed = pje->_TJLmml + pje->_maxTerms;

 for( p = zed; p < upperzed; ++p) 
 {
    if (  std::abs(p->_value) > 0.0)  {
      zPtr->append( new( zPtr->storePtr() ) TJLterm<T1,T2>( *p ) ); 
      p->_value = T1( );  // erase the scratchpad entry
    }
 
 };

 // IMPORTANT NOTE:
 // The result of a multiplication operation can be an empty Jet. 
 // This happens when the product of x and y would yield only high 
 // order monomial terms (i.e.) beyond the max weight allowed for
 // the environment.   

 return z;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator*( const TJet<T1,T2>& x, const T1& y ) 
{
 TJet<T1,T2> z(  x->_myEnv );
 
 if( y == T1( ) || x._jl->_count < 1 ) {
   return z;
 };
 
 z.DeepCopy( x ); // This is done in this way so that
                  // what is returned does not own
                  // the same data as x.
 

 // scale the values directly, regardless of order (i.e. without using the dlist)
 
 for ( TJLterm<T1,T2>* p = z._jl->_jltermStore; p < z._jl->_jltermStoreCurrentPtr; p++  ) 
 {
   if ( ! (p->_deleted) ) (p->_value) *= y;
 }
 
 return z;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator*( const T1& y, const TJet<T1,T2>& x ) 
{  
 
 return operator*( x, y );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator*( const TJet<T1,T2>& x, const int& j ) 
{  
  return operator*( x, T1(j) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator*( const int& j, const TJet<T1,T2>& x ) 
{ 
  return operator*( T1(j), x );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator/( const TJet<T1,T2>& x, const T1& y ) 
{ 
  // Check for division by zero ..
  if( y == 0.0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<T1,T2> operator/( const TJet<T1,T2>&, const T1& ) ",
           "Attempt to divide by a scalar zero." ) );
  }
 
  return ( x*( 1.0 / y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator/( const TJet<T1,T2>& x, const int& j ) 
{ 
  // Check for division by zero ..
  if( j == 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<T1,T2> operator/( const TJet<T1,T2>& x, const int& j )",
           "Attempt to divide by a scalar zero." ) );
  }
 
  return ( x*(1.0/((T1) j)) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> operator/( const T1& a, const TJet<T1,T2>& b ) 
{
  TJet<T1,T2> u( b->_myEnv ); 
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


 TJLterm<T1,T2>* p = 0;
 TJLterm<T1,T2>* q = 0;

 TJLterm<T1,T2>* zed;
 TJLterm<T1,T2>* upperzed;

 T1  dummy   = T1();
 T1  product = T1();

 int indy = 0;

 // If one of the arguments is void, return it ..

 if ( _jl->_count < 1 ) return *this;   // ??? Is this form going to cause

 if ( v->_count  < 1 ) return v;        // ??? a problem? 

 // Initializations

 TJetEnvironment<T1,T2>* pje = _jl->_myEnv;
 TJet<T1,T2> z( pje );
 TJL<T1,T2>* zPtr = z._jl;

 // .. otherwise continue normal operations.

 for ( p = v._jl->_jltermStore; p <v._jl->_jltermStoreCurrentPtr; p++ ) {

    if (p->_deleted) continue;

    for ( q = _jl->_jltermStore; q < _jl->_jltermStoreCurrentPtr; q++ ) {
  
      if (q->_deleted) continue;

      if( ( p->_weight + q->_weight ) > wl ) continue;

      indy = pje->_offset.index( p->_index + q->_index );

   // Will work even when the exponents are all zero.

      product = p->_value * q->_value;
      dummy = pje->_TJLmml[indy]._value + product;

      if( std::abs( dummy ) < MX_SMALL*std::abs( product ) ) {
        pje->_TJLmml[indy]._value = 0.0;
      } 
      else {
        pje->_TJLmml[indy]._value = dummy;
      }
    }
 }
 
 // Transfer answer from the scratchpad.

 zed      = pje->_TJLmml;
 upperzed = pje->_TJLmml + pje->_maxTerms;

 while( zed < upperzed ) {
   if( abs(zed->_value) > MX_ABS_SMALL ) {
     zPtr->append( new( zPtr->storePtr() ) TJLterm<T1,T2>( *zed ) );
     zed->_value = T1( );
   }
   zed++;
 }

 // Determine the maximum weight computed accurately.
 // ??? Is this step necessary???
 
 if( _jl->_accuWgt < v->_accuWgt ) z->_accuWgt = _jl->_accuWgt;
 else                              z->_accuWgt =   v->_accuWgt;
 
 // REMOVE: z->_myEnv = _jl->_myEnv;  // ??? Redundant?
 return z;
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

 // Initialize local variables and set the environment of the answer.
 // (These steps are not necessary, but they enforce a discipline.)

 TJet<T1,T2> v(wArg->_myEnv);
 TJet<T1,T2> vn(wArg->_myEnv);
 TJet<T1,T2> w(wArg->_myEnv);
 TJet<T1,T2> u(wArg->_myEnv);

 TJLterm<T1,T2>* qu = 0;
 TJLterm<T1,T2>* qw = 0;
 T1 u0 = T1();

 int i   = 0;
 int wgt = 0;
 int wl  = 0;
 int mw  =  wArg->_myEnv->_maxWeight;
 

 dlist_looper gu( uArg._jl->_theList );
 dlist_looper gw( wArg._jl->_theList );

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
 if(  (u0 = qu->_value ) == 0.0 ) {
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

 TJetEnvironment<T1,T2>* theEnv = x->_myEnv;
 TJet<T1,T2> z (theEnv);

 int*  m = new int [ theEnv->_numVar ];
 int*  n = new int [ theEnv->_numVar ];

if( theEnv->_pbok ) {
 
   for( int i = 0; i < theEnv->_numVar; i++ ) {
     m[i] = 0;
     n[i] = 0;
   }
 
   for( int i = 0; i < theEnv->_dof; i++ ) {
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

 const T1 zero = ((T1) 0.0); 	 
 TJetEnvironment<T1,T2>* pje = x.Env(); 	 
  	 
 // Initial Newton's step 	 
 
 TJet<T1,T2> z;	        
 TJet<T1,T2> dz

 z.DeepCopy( x ); 	 
 dz = ( sin(z) - x ) / cos(z); 	 
  	 
 // Setting up the iteration 	 
 int upperBound = 8; 	 
 
 int iter = 0;	   
 int indy = 0; 	 
 double compValue; 	 
 bool repeat = true; 	 
 
 TJLterm<T1,T2>* pz = 0; 	 
 	 
 // Iterated Newton's steps
 
 while( repeat && (iter < MX_MAXITER) ) 	 
 { 	 
     while( iter++ < upperBound ) { 	 
      // These two lines are the heart of the calculation: 	 
      z -= dz; 	 
      dz = ( sin(z) - x ) / cos(z);	      
     }

	 
     // The rest is just determining when to stop. 	  
     //   This procedure could be improved, but it's better 	 
     //   than the previous one, which was just comparing 	 
     //   dz to zero. 	 
     repeat = false; 	 
 	 
     // Load the current answer into a scratchpad 	   

     z.resetIterator(); 	 
     pz = z.stepIterator(); 	 
     while( 0 != pz ) { 	 
       indy = pje->_offset.index(pz->_index); 	 
       pje->_TJLmml[indy]._value = pz->_value; 	 
       pz = z.stepIterator(); 	 
     } 	 
 	 
     // Compare to the increment, one coefficient at a time 	   
     dz.resetIterator(); 	   
     pz = dz.stepIterator(); 	 
     while( 0 != pz ) { 	 
       indy = pje->_offset.index(pz->_index); 	 
       compValue = std::abs( pje->_TJLmml[indy]._value ); 	 
       if( compValue < MX_SMALL ) { 	 
         if( std::abs(pz->_value) > MX_SMALL ) { 	 
           repeat = true; 	 
           break; 	 
         } 	 
       } 	 
       else { 	 
         if( std::abs(pz->_value) > MX_SMALL*compValue ) { 	 
           repeat = true; 	 
           break; 	 
         } 	 
       } 	 
       pz = dz.stepIterator(); 	 
     } 	 
 	 
     // Clean the scratchpad. 	 
     z.resetIterator(); 	 
     pz = z.stepIterator(); 	 
     while( 0 != pz ) { 	 
       indy = pje->_offset.index(pz->_index); 	 
       pje->_TJLmml[indy]._value = zero; 	 
       pz = z.stepIterator(); 	 
     }
 	 
     // And continue 	 
     upperBound += 8; 	 
 } 	 
 
 // If stopped because too many iterations,
 // print a warning message

 if( iter >= MX_MAXITER ) {
  (*pcerr) << "*** WARNING ***                             \n";
  (*pcerr) << "*** WARNING *** TJL<T1,T2>& asin( TJL<T1,T2>& x ) {         \n";
  (*pcerr) << "*** WARNING *** Over " 
       << MX_MAXITER 
       << " iterations used;    \n";
  (*pcerr) << "*** WARNING *** result may be incorrect.    \n";
  (*pcerr) << "*** WARNING ***                             \n";
  (*pcerr) << "*** WARNING *** Initial value:              \n";
  (*pcerr) << "*** WARNING ***                             \n"
       << endl;

  x.printCoeffs();
  (*pcerr) << "*** WARNING *** A few iterations:             " << endl;
  for( iter = 0; iter < 4; iter++ ) {
   (*pcerr) << "*** WARNING *** Iteration No. " << ( iter+1 ) << endl;
   z -= dz;
   dz = ( sin(z) - x ) / cos(z);
   (*pcerr) << "*** WARNING *** dz = " << endl;
   dz.printCoeffs();
   (*pcerr) << "*** WARNING *** z = " << endl;
   z.printCoeffs();
  }
 }


 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> atan( const TJet<T1,T2>& x ) 
{   
 
 TJet<T1,T2> z;
 TJet<T1,T2> c, dz;
 int iter = 0;

 z.DeepCopy( x );
 c = cos( z );
 dz = c*( sin(z) - x*c );

 while( ( dz != ((T1) 0.0) ) && ( iter++ < MX_MAXITER ) ) {
  z -= dz;
  c = cos( z );
  dz = c*( sin(z) - x*c );
 }

 if( iter >= MX_MAXITER ) {
  (*pcerr) <<  "*** WARNING ***                             \n" 
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
 
 TJet<T1,T2> epsilon(x->_myEnv);

 T1 cs = T1();
 T1 sn = T1();

 TJLterm<T1,T2>* p = 0;

 dlist_iterator getNext( x._jl->_theList );
 
 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> cos( const TJet<T1,T2>& ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new( epsilon.storePtr() ) TJLterm<T1,T2>( x->_myEnv->_allZeroes, ((T1) 1.0), x->_myEnv ) );
   return epsilon;
 }
 
 p = (TJLterm<T1,T2>*) getNext();
 if( ( p -> _weight ) == 0 ) {         // x has non-zero standard part
   cs = std::cos( p -> _value );
   sn = std::sin( p -> _value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   p->_deleted = true; //delete p; // NOT WITH NEW MEM MANAGEMENT
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
 TJet<T1,T2> z = exp(x);
 z = ( z + ( 1.0 / z ) ) / 2.0;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> exp( const TJet<T1,T2>& x ) 
{ 
 
 TJet<T1,T2> epsilon( x->_myEnv );
 T1 factor = T1();
 TJLterm<T1,T2>* p = 0;

 dlist_iterator getNext( x._jl->_theList );

 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> exp( const TJet<T1,T2>& ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new( epsilon.storePtr() ) TJLterm<T1,T2>( x->_myEnv->_allZeroes, ((T1) 1.0), x->_myEnv ) );
   return epsilon;
 }
 
 p = (TJLterm<T1,T2>*) getNext();
 if( ( p -> _weight ) == 0 ) {         // x has non-zero standard part
   factor = std::exp( p -> _value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   p->_deleted = true; // delete p; NOT WITH NEW MEM MNGMNT
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
  TJet<T1,T2>   epsilon(x->_myEnv);
  TJet<T1,T2>   u(x->_myEnv);
  TJet<T1,T2>   w(x->_myEnv);

  T1 std   = T1();
  double n = 0.0;
  TJLterm<T1,T2>*  p = 0;

  dlist_iterator  getNext( x._jl->_theList );
 
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
   p->_deleted = true; //delete p; // NOT WITH NEW MEM MANAGEMENT
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

 TJet<T1,T2> epsilon(x->_myEnv );
 T1 factor = T1();
 T1 std    = T1();
 TJLterm<T1,T2>* p = 0;
 int u = 0;

 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> pow( const TJet<T1,T2>&, double ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new( epsilon.storePtr() ) TJLterm<T1,T2>( x->_myEnv->_allZeroes, T1(), x->_myEnv ) );
   return epsilon;
 }
 
 dlist_iterator getNext( x._jl->_theList );

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
   p->_deleted = true; // delete p; NOT WITH NEW MEM MNGMT
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

  TJet<T1,T2> z( x->_myEnv ); 

  if( n == 0 ) z = ((T1) 1.0);
  else if( n > 0 ) {
    z.DeepCopy( x );
    for( int i = 2; i <= n; i++ ) z *= x;
  }
  else {
    TJet<T1,T2> xr;
    z = 1.0 / x;
    xr = z;
    for( int i = -2; i >= n; i-- ) z *= xr;
  }
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> sin( const TJet<T1,T2>& x ) 
{ 
 
 TJet<T1,T2> epsilon(x->_myEnv);
 T1 cs = T1();
 T1 sn = T1();

 TJLterm<T1,T2>* p = 0;

 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> sin( const TJet<T1,T2>&, double ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new( epsilon.storePtr() ) TJLterm<T1,T2>( x->_myEnv->_allZeroes, T1(), x->_myEnv ) );
   return epsilon;
 }
 
 dlist_iterator getNext( x._jl->_theList );
 p = (TJLterm<T1,T2>*) getNext();
 if( ( p -> _weight ) == 0 ) {         // x has non-zero standard part
   cs = std::cos( p -> _value );
   sn = std::sin( p -> _value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   p->_deleted = true; // delete p; NOT WITH NEW MEM MNGMNT
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


 TJLterm<T1,T2>* p = 0;

 TJet<T1,T2> epsilon( x->_myEnv );
 T1 factor = T1();
 T1 std    = T1();

 if( x->_count == 0 ) {

   // * the code below should be replaced by a more permanent 
   // * solution; possibly a function that clears the scratchpads for all
   // * existing environments. 
   // * The scratchpads should be cleared when *any* exception is thrown
   // * within mxyzptlk because the TJLterm _value fields may contain  
   // * "Nan"s. -JFO 

   (*pcout) << "Resetting Environment Scratchpad" << std::endl;
   //for (int i=0; i< x._jl->_myEnv->_maxTerms; i++ ) {
   //      x._jl->_myEnv->_TJLmml[i]._value = 0.0;
   //      std::(*pcout) << x._jl->_myEnv->_TJLmml[i]._index << std::endl;
   //}          
   
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T1,T2> sqrt( const TJet<T1,T2>& ) { ",
          "Argument is zero." ) );
 }
 
 dlist_iterator getNext( x._jl->_theList );
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
     p->_deleted = true; // delete p; NOT WITH NEW MEM MNGMNT
     epsilon.scaleBy( 1.0/std );
     return factor * TJet<T1,T2>::_epsSqrt( epsilon );
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


template<typename T1, typename T2>
TJet<T1,T2> erfc( const TJet<T1,T2>& z ) 
{
  static const T1 one = ((T1) 1.0);
  return ( one - erf( z ) );
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
 
 TJet<T1,T2> z;
 TJet<T1,T2> epsq, term;
 double n;

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
 TJet<T1,T2> z;
 TJet<T1,T2> term;
 double n;
 
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
 TJet<T1,T2> z;
 TJet<T1,T2> term;
 double f;
 double n;

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
 
 TJet<T1,T2> z;
 TJet<T1,T2> epsq, term;
 double n;
 
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

 TJet<T1,T2>     z;
 TJet<T1,T2>     term;
 
 z->_myEnv = epsilon->_myEnv;
 z = ((T1) 1.0);
 
 double f    = 1.0 / 2.0;
 double n    = 1.0;
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
 dlist_traversor getNext( _jl->_theList );
 int i;
 TJLterm<T1,T2>* p;
 dlink* q;
 (*pcout) << "\nCount  = "
      << _jl->_count
      << " Weight = "
      << _jl->_weight
      << " Max accurate weight = "
      << _jl->_accuWgt
      << endl;
 (*pcout) << "Reference point: ";
 for( i = 0; i < _jl->_myEnv->_numVar; i++ ) 
    (*pcout) << setprecision(12) << _jl->_myEnv->_refPoint[i] << "  ";
 (*pcout) << endl;
 while((  q = getNext()  )) {
   p = (TJLterm<T1,T2>*) q->info();
   (*pcout) << "Weight: "
        << p -> _weight
        << "   Value: "
        << p -> _value
        << " || ";
   (*pcout) << "Addresses: "
        << (int) q->prevPtr() << "  "
        << (int) q            << "  "
        << (int) q->nextPtr() 
        << " : "
        << (int) p
        << endl;
   (*pcout) << "Index:  ";
   for( i = 0; i < _jl->_myEnv->_numVar; i++ )
     (*pcout) << (p -> _index)(i) << "  ";
   (*pcout) << "\n" << endl;
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJet<T1,T2>::printCoeffs() const 
{
 dlist_iterator getNext( _jl->_theList );
 int i;
 TJLterm<T1,T2>* p;

 (*pcout) << "\nCount  = " << _jl->_count 
      << ", Weight = " << _jl->_weight 
      << ", Max accurate weight = " << _jl->_accuWgt << endl;
 (*pcout) << "Reference point: " 
      << _jl->_myEnv->_refPoint[0];
 for( i = 1; i < _jl->_myEnv->_numVar; i++ ) {
   (*pcout) << ", ";
   (*pcout) << _jl->_myEnv->_refPoint[i];
 }
 (*pcout) << endl;

 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   if( p->_weight > _jl->_accuWgt ) break;
   (*pcout) << "Index:  " 
        << p->_index
        << "   Value: "
        << p->_value
        << endl;
 }

 (*pcout) << "\n" << endl;
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
 dlist_iterator getNext( thisPtr->_theList );
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
     q = new( zPtr->storePtr() ) TJLterm<T1,T2>( p );
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
 TJet<T1,T2> z( _jl->_myEnv );
 TJL<T1,T2>* zPtr  = z._jl ;
 TJLterm<T1,T2>* p = 0;
 TJLterm<T1,T2>* q = 0;


 dlist_iterator getNext( _jl->_theList );

 int i  = 0;
 int nv = 0;
 IntArray oldIndex;

 nv = _jl->_myEnv->_numVar;
 oldIndex.Reconstruct( nv );

 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
  oldIndex = p->_index;

  if( (*f)( p->_index, p->_value ) ) {
   q = new( zPtr->storePtr() ) TJLterm<T1,T2>( p );
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

 TJetEnvironment<T1,T2>* theEnv = _jl->_myEnv;
 TJet<T1,T2> v(( ((theEnv)->_TJLmonomial )[0] )->_myEnv );
 TJLterm<T1,T2>* p = 0;

 
 
 dlist_iterator getNext( _jl->_theList );
 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   if( p->_weight > _jl->_accuWgt ) break;
   v += ( p->_value ) * ( theEnv->_TJLmonomial[ theEnv->_offset.index(p->_index) ] );
 }
 
 return v;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::operator() ( const TJetVector<T1,T2>& y ) const 
{
 int n = _jl->_myEnv->_numVar;   

 TJet<T1,T2> z;      // ??? so as to be self-contained.

 TJet<T1,T2>* u = new TJet<T1,T2> [ n ];

 for( int i = 0; i < n; i++ ) u[i] = y(i);
 
 z = operator()( u );

 delete [] u;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TJet<T1,T2>::operator() ( const TJet<T1,T2>* y ) const 
{ 
 TJetEnvironment<T1,T2>* theEnv = _jl->_myEnv;

 TJet<T1,T2>*     u = new TJet<T1,T2> [ theEnv->_numVar ];
 TJet<T1,T2>      z(y[0]->_myEnv);

 TJet<T1,T2>      term;         // Initializing term should not be necessary. 

 int  i = 0;
 int  j = 0;
 int  w = 0;




 // Check consistency of reference points and
 // subtract reference point prior to concatenation.
 for( int i = 0; i < theEnv->_numVar; i++ ) {
   if( y[i]->_myEnv != y[0]->_myEnv ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T1,T2> TJet<T1,T2>::operator() ( const TJet<T1,T2>* ) const ",
            "Inconsistent environments." ) );
   }
   u[i] = y[i] - theEnv->_refPoint[i];
 }
 
 // Evaluate and store monomials.
 
 // The zeroth one.
 ( ( theEnv->_TJLmonomial )[0] )->_myEnv = y[0]->_myEnv; // Needed by 
                                                         // TJet<T1,T2>::concat
 theEnv->_TJLmonomial[0] = ((T1) 1.0);

 // For all higher weights ...
 for( int w = 1; w <= _jl->_accuWgt; w++ )
 
   // Get the next set of _exponents of weight w.
   while( nexcom( w, theEnv->_numVar, theEnv->_exponent ) ) {
 
     // Find the first non-zero _exponent.
     i = 0;
     while( !( theEnv->_exponent[i++] ) ) ;
     i--;
 
     // The value of the monomial associated with this composition
     // is obtained by multiplying a factor into a previously
     // computed monomial.
     ( theEnv->_exponent[i] )--;
     // REMOVE: theEnv->_monoCode();
     // REMOVE: term = theEnv->_TJLmonomial[ theEnv->_monoRank() ];
     term = theEnv->_TJLmonomial[ theEnv->_offset.index( theEnv->_exponent ) ];
     ( theEnv->_exponent[i] )++;
     // REMOVE: theEnv->_monoCode();
     // REMOVE: theEnv->_TJLmonomial[ theEnv->_monoRank() ] = term * u[i]; // ??? Is this OK???
     theEnv->_TJLmonomial[ theEnv->_offset.index( theEnv->_exponent ) ] = term * u[i];
 
   } // End while loop.
 
 // Monomials have been stored.
 // Now traverse the TJL<T1,T2> variable and evaluate.
 // Formerly done by using concat routine, which
 // I copy here
 {

  dlist_iterator getNext( _jl->_theList );
  TJLterm<T1,T2>* p = 0;
 
  z->_myEnv = (( theEnv->_TJLmonomial )[0] )->_myEnv;
  
  while((  p = (TJLterm<T1,T2>*) getNext()  )) {
    if( p->_weight > _jl->_accuWgt ) break;
    // REMOVE: for( int i = 0; i < theEnv->_numVar; i++ )
    // REMOVE:   theEnv->_exponent[i] = (p->_index)(i);
    // REMOVE: theEnv->_monoCode();
    // REMOVE: z += ( p->_value ) * ( theEnv->_TJLmonomial[ theEnv->_monoRank() ] );
    z += ( p->_value ) * ( theEnv->_TJLmonomial[ theEnv->_offset.index(p->_index) ] );
  }
 }

 
 // Finish...
 delete [] u;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
T1 TJet<T1,T2>::operator() ( const Vector& x ) const 
{
 return _jl->operator()( x );
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
 bool doit = false;

 int f = 0;
 int j = 0;
 int w = 0;

 TJet<T1,T2> z( _jl->_myEnv);
 TJLterm<T1,T2>* p = 0;
 TJLterm<T1,T2>* q = 0;

 dlist_iterator getNext( _jl->_theList );
 
 // --- Preliminary check of _index set.
 
 w = 0;
 
 for( int i = 0; i < _jl->_myEnv->_numVar; i++ ) {
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
 
   q = new( z->storePtr() ) TJLterm<T1,T2>( p );
 
   doit = true;
   // -- Reset the _index.
   for( int i = 0; i < _jl->_myEnv->_numVar; i++ ) 
     doit = doit && ( ( q->_index(i) -= n[i] ) >= 0 );
 
   if( doit ) {
     // -- Build factorial multiplier.
     f = 1;
     for( int k = 0; k < _jl->_myEnv->_numVar; k++ ) {
       j = q->_index(k);
       for( int i = 0; i < n[k]; i++ ) f *= ++j;
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
   
     z->append( q );
   } 

   else {
    q->_deleted = true;  
    // delete q;                   // ??? Is this correct?   NOT WITH NEW MEM MNGMNT
                                      // ??? It wasn't here before!
   }
 }
 
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

 bool doit = false;

 int f = 0;
 int j = 0;
 int w = 0;

 TJet<T1,T2> z(_jl->_myEnv );

 dlist_iterator getNext( _jl->_theList );

 TJLterm<T1,T2>* p = 0;
 TJLterm<T1,T2>* q = 0;
 
 // --- Initializations.
 f = j = w = 0;

 // --- Preliminary check of _index set.
 
 w = 0;
 
 for( int i = 0; i < _jl->_myEnv->_numVar; i++ ) {
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
 
   q = new( z->storePtr() ) TJLterm<T1,T2>( p );
 
   doit = true;
   // -- Reset the _index.
   for( int i = 0; i < _jl->_myEnv->_numVar; i++ ) 
     doit = doit && ( ( q->_index(i) -= n(i) ) >= 0 );
 
   if( doit ) {
     // -- Build factorial multiplier.
     f = 1;
     for( int k = 0; k < _jl->_myEnv->_numVar; k++ ) {
       j = q->_index(k);
       for( int i = 0; i < n(k); i++ ) f *= ++j;
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
   
     z->append( q );
   }

   else {

   q->_deleted = true;
   // delete q;                   // ??? Is this correct? NOT WITH NEW MEM MNGMNT

  }    
                                   // ??? It wasn't here before.
 }
 
 // --- Finally, adjust _accuWgt and return value
 
 z->_accuWgt = _jl->_accuWgt - w;       // ??? Is this correct ???

 return z;
}

// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------

#undef PREPFORCHANGE


#endif // TJET_TCC
