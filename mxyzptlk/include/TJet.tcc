/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJet.tcc
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
******             - Efficiency improvements.
******             - new memory management
****** 
******  Sept 2005   ostiguy@fnal.gov
******
******              - new code based on a single template parameter
******                instead of two. Mixed mode handled
******                using conversion operators.
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



#define PREPFORCHANGE(_jl)  if(((_jl)->_rc)>1){--((_jl)->_rc);(_jl) = TJL<T>::makeTJL(_jl);}


// ================================================================
//      Global variables
//

#define DEFSCALE  0.001

template<typename T> int   TJet<T>::_currentIndex = 0;
template<typename T> slist TJet<T>::_newCoords;
template<typename T> slist TJet<T>::_newValues;
template<typename T> TJetEnvironment<T>*  TJet<T>::_lastEnv = 0;
template<typename T> TJetEnvironment<T>*  TJet<T>::_workEnv = 0;

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

template<typename T>
TJet<T>::TJet( TJetEnvironment<T>* pje ) 
{
 _jl = TJL<T>::makeTJL( pje );
 _constIterPtr = 0;
 _iterPtr     = 0;
}

template<typename T>
TJet<T>::TJet( T x, TJetEnvironment<T>* p_je ) 
{
 _jl = TJL<T>::makeTJL( x, p_je );
 _constIterPtr = 0;
 _iterPtr     = 0;
}

template<typename T>
TJet<T>::TJet( const TJet& x ): gms::FastAllocator() 
{
 _jl = x._jl;
 ( _jl->_rc )++;

 _constIterPtr = 0;  // The iterators are NOT copied.
 _iterPtr     = 0;
}

template<typename T>
TJet<T>::~TJet() 
{
 if( 0 != _iterPtr ) {
   delete _iterPtr;
 }
 if( 0 != _constIterPtr ) {
   delete _constIterPtr;
 }
 if( --(_jl->_rc) == 0 ) {
   TJL<T>::discardTJL(_jl); 
 }
}

template<typename T>
void TJet<T>::Reconstruct()
{
 // Combines destructor and constructor functions.
 // Use when initializing a static TJet variable.
 TJetEnvironment<T>* pje;
 pje = _jl->_myEnv;
 if( --(_jl->_rc) == 0 ) TJL<T>::discardTJL(_jl); 
  _jl = TJL<T>::makeTJL( pje );
}

template<typename T>
void TJet<T>::Reconstruct( TJetEnvironment<T>* pje )
{
 // Combines destructor and constructor functions.
 // Use when initializing a static TJet variable.
 if( --(_jl->_rc) == 0 ) TJL<T>::discardTJL(_jl); 
 _jl = TJL<T>::makeTJL( pje );
}


template<typename T>
void TJet<T>::Reconstruct( const IntArray& e, 
                               const T& x, 
                               TJetEnvironment<T>* pje )
{
 // Combines destructor and constructor functions.
 // Use when initializing a static TJet variable.
 if( --(_jl->_rc) == 0 ) TJL<T>::discardTJL(_jl); 
 _jl = TJL<T>::makeTJL( e, x, pje );
}


template<typename T>
void TJet<T>::setEnvTo( const TJet& x )
{
  if( _jl == NULL ) 
  {
    _jl = TJL<T>::makeTJL( x._jl->_myEnv );
  }
  else if( _jl->_myEnv != x._jl->_myEnv ) 
  {
    PREPFORCHANGE(_jl);
    _jl->_myEnv = x._jl->_myEnv;
  }
}


template<typename T>
void TJet<T>::setEnvTo( TJetEnvironment<T>* pje )
{
  if( _jl == NULL ) 
  {
    _jl = TJL<T>::makeTJL( pje );
  }
  else if( _jl->_myEnv != pje ) 
  {
    PREPFORCHANGE(_jl);
    _jl->_myEnv = (TJetEnvironment<T>*) pje;
  }
}


template<typename T>
void TJet<T>::getReference( T* r ) const
{
 _jl->getReference( r );
}

template<typename T>
void TJet<T>::setVariable( const T& x,
                               const int& j, 
                               TJetEnvironment<T>* pje )
{
 if( _jl == NULL ) {
   _jl = TJL<T>::makeTJL( pje );
 }
 else {
   PREPFORCHANGE(_jl)
 }
 _jl->setVariable( x, j, pje );  // !! Alters the environment!
}

template<typename T>
void TJet<T>::setVariable( const T& x,
                               const int& j )
{
 if( _jl == NULL ) {
   throw( GenericException( __FILE__, __LINE__, 
          "void TJet<T>::setVariable( const T2&, const int& )",
          "Impossible! _jl not initialized?????" ) );
 }
 else {
   PREPFORCHANGE(_jl)
 }
 _jl->setVariable( x, j, _jl->_myEnv );  // !! Alters the environment!
}

template<typename T>
void TJet<T>::setVariable( const int& j, 
                               TJetEnvironment<T>* pje ) 
{
 if( _jl == NULL ) {
    _jl = TJL<T>::makeTJL( pje );
 }
 else {
   PREPFORCHANGE(_jl)
 }
 _jl->setVariable( j, pje );
}

template<typename T>
void TJet<T>::setVariable( const int& j )
{
 if( _jl == NULL ) {
   throw( GenericException( __FILE__, __LINE__, 
          "void TJet<T>::setVariable( const int& )",
          "Impossible: _jl not initialized?????" ) );
 }
 else {
   PREPFORCHANGE(_jl)
 }
 _jl->setVariable( j, _jl->_myEnv );
}

template<typename T>
TJet<T>& TJet<T>::operator=( const TJet& x ) 
{
 if( _jl != x._jl ) {
   if( --(_jl->_rc) == 0 ) TJL<T>::discardTJL(_jl); 
   _jl = x._jl;
   (_jl->_rc)++;
 }
 return *this; 
}

template<typename T>
TJet<T>& TJet<T>::DeepCopy( const TJet& x ) 
{
 if( this != &x ) {
  if( --(_jl->_rc) == 0 ) TJL<T>::discardTJL(_jl); 
   _jl = TJL<T>::makeTJL( x->_myEnv );
  *_jl = *(x._jl);
 }
 return *this; 
}


template<typename T>
TJet<T>& TJet<T>::operator=( const T& x ) 
{
 TJetEnvironment<T>* pje = _jl->_myEnv;

 if( --(_jl->_rc) == 0 ) TJL<T>::discardTJL(_jl); 
  _jl = TJL<T>::makeTJL(pje);
 *(_jl) = x;
 return *this; 
}

template<typename T>
istream& operator>>( istream& is,  TJet<T>& x ) 
{
 if( --(x._jl->_rc) == 0 )  TJL<T>::discardTJL(x._jl); 
 x._jl = TJL<T>::makeTJL();
 return operator>>( is, *(x._jl) );
}

template<typename T>
ostream& operator<<( ostream& os, const TJet<T>& x ) 
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

template<typename T>
void TJet<T>::BeginEnvironment( int w ) 
{
  if( _workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "void TJet<T>::BeginEnvironment( int w )",
           "Cannot open two environments simultaneously. Close first." ) );
  }
  _workEnv             = new TJetEnvironment<T>;
  _workEnv->_maxWeight = w;
  _currentIndex        = 0;
}

// ---------------------------------------------------------------------------

template<typename T>
void TJet<T>::Parameters()
{
  if( _workEnv->_pbok ) {
   throw( GenericException( __FILE__, __LINE__, 
          "void TJet<T>::Parameters()",
          "Can only be called once per (open) environment." ) );
  }
  _workEnv->_pbok     = 1;
  _workEnv->_spaceDim = _currentIndex;
}

// ---------------------------------------------------------------------------

template<typename T>
TJetEnvironment<T>* TJet<T>::EndEnvironment( double* scl )
{
  if( _currentIndex == 0 ) {
    delete _workEnv;
    _workEnv = 0;
    return 0;
  }

  Tcoord<T>*  p;
  T* q;
  int i;

  if((  ( _currentIndex     != _newCoords.size() )  ||  
        ( _newCoords.size() != _newValues.size() )  )) {
    throw( typename TJL<T>::HorribleException( 
           _currentIndex, _newCoords.size(), _newValues.size(),
           __FILE__, __LINE__, 
           "TJetEnvironment<T>* TJet<T>::EndEnvironment( double* )",
           "" ) );
  }

  _workEnv->_numVar = _currentIndex;

  if( !_workEnv->_pbok ) { _workEnv->_spaceDim = _currentIndex; }

  _workEnv->_dof = _workEnv->_spaceDim / 2;

  if( _workEnv->_pbok && ( 2*_workEnv->_dof != _workEnv->_spaceDim ) ) {
    (*pcerr) << "\n\n"
         << "*** WARNING ***                                 \n"
         << "*** WARNING *** TJet<T>::EndEnvironment()   \n"
         << "*** WARNING *** Phase space has odd dimension.  \n"
         << "*** WARNING *** I hope you know what you        \n"
         << "*** WARNING *** are doing, but I doubt it.      \n"
         << "*** WARNING ***                                 \n"
         << endl;
  }
  
  int w = _workEnv->_maxWeight;
  int n = _workEnv->_numVar;

  _workEnv->_offset.reconstruct( w, n, false );

  _workEnv->_exponent    = new int[ n ];

  // The reference point is set.
  _workEnv->_refPoint = new T [ n ];
  slist_iterator GetNextValue( _newValues );
  i = 0;
  while( ( q = (T*) GetNextValue() ) != 0 ) {
    _workEnv->_refPoint[i++] = *q;
  }
  if( i != n ) {
    throw( typename TJL<T>::HideousException(i, n, __FILE__, __LINE__, 
             "TJetEnvironment<T>* TJet<T>::EndEnvironment( double* )", 
             "" ) );
  }
 
  _workEnv->_allZeroes.Reconstruct(n);
  for( i = 0; i < n; i++ ) _workEnv->_allZeroes(i) = 0;
 

  // Initialize the coordinates
  i = 0;
  while((  p = (Tcoord<T>*) _newCoords.get()  )) {
    q = (T*) _newValues.get();
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
  TJetEnvironment<T>::_skipEnvEqTest = true;
  _lastEnv = _workEnv;

   TJetEnvironment<T>::_environments.append( _workEnv );
  _workEnv = 0;        // This does NOT delete the environment.
  _newCoords.clear();  // Should be unnecessary.
  _newValues.clear();  // Should be unnecessary.
  _currentIndex = 0;

  return _lastEnv;
}

// ---------------------------------------------------------------------------

template<typename T>
void TJet<T>::EnlargeEnvironment( const TJetEnvironment<T>* pje )
{
  // Like TJet<T>::BeginEnvironment

  if( _workEnv != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "void TJet<T>::EnlargeEnvironment( TJetEnvironment<T>* )",
           "Close open environment before invoking this function." ) );
  }

  _workEnv             = new TJetEnvironment<T>;
  _workEnv->_maxWeight = pje->_maxWeight;
  _currentIndex        = 0;  

  // Creating new coordinates ...
  Tcoord<T>* p_coord;

  for( int i = 0; i < pje->_numVar; i++ ) {
    p_coord = new Tcoord<T>( pje->_refPoint[ i ] );
    // _currentIndex is automatically increased by coord::coord
  }

  // Like TJet<T>::Parameters()

  if( pje->_pbok ) {
    _workEnv->_pbok     = pje->_pbok;
    _workEnv->_spaceDim = pje->_spaceDim;
  }
}


// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//    Implementation of class Tcoord
//
 
template<typename T>
Tcoord<T>::Tcoord( T x ) 
: TJet<T>( TJet<T>::_workEnv ) {
 
 if( ! TJet<T>::_workEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "Tcoord<T>::coord( T ) ",
          "Use TJet<T>::BeginEnvironment() to open an environment first." ) );
 }

 _index = TJet<T>::_currentIndex++;
 
 TJet<T>::_newCoords.append( this );
 TJet<T>::_newValues.append( new T( x ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
Tcoord<T>::~Tcoord() 
{
}


//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
//    Iteration routines
// 

template<typename T>
void TJet<T>::resetConstIterator() 
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

template<typename T>
TJLterm<T> TJet<T>::stepConstIterator()  const
{
  if( _constIterPtr ) {
    return TJLterm<T>( (TJLterm<T>*) (_constIterPtr->operator()()) );
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "TJLterm<T> TJet<T>::stepConstIterator()  const",
           "You must first resetConstIterator." ) );
  }
}

template<typename T>
const TJLterm<T>& TJet<T>::stepConstIteratorRef()  const
{
  if( _constIterPtr ) {
    return *( (TJLterm<T>*)( _constIterPtr->operator()() ) );
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "const TJLterm<T>& TJet<T>::stepConstIterator()  const",
           "You must first resetConstIterator." ) );
  }
}


template<typename T>
const TJLterm<T>* TJet<T>::stepConstIteratorPtr()  const
{
  if( _constIterPtr ) {
    return ( (const TJLterm<T>*) ( _constIterPtr->operator()() ) );
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "const TJLterm<T>& TJet<T>::stepConstIterator()  const",
           "You must first resetConstIterator." ) );
  }
}


template<typename T>
void TJet<T>::resetIterator()
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

template<typename T>
TJLterm<T>* TJet<T>::stepIterator()
{
  PREPFORCHANGE(_jl)
  // This has to be done each time, since _jl could have
  // been given to a new TJet since the last invocation.

  if( _iterPtr ) {
    return (TJLterm<T>*) (_iterPtr->operator()());
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "TJLterm<T>* TJet<T>::stepIterator()",
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
#define MX_SMALL       1.0e-12 // Used by TJL<T>::addTerm to decide 
#endif  // MX_SMALL
                            //   removal of a TJL<T>Cterm.
#ifndef MX_ABS_SMALL
#define MX_ABS_SMALL    1.0e-15 // Used by TJL<T>::addTerm to decide 
#endif


#ifndef MX_MAXITER
#define MX_MAXITER  20      // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
TJet<T>& TJet<T>::operator+=( const TJet<T>& y ) 
{

 TJet<T> z( *this );
 (*this) = z+y;
 return *this;

}

template<typename T>
TJet<T>& TJet<T>::operator-=( const TJet<T>& y ) 
{

  return this->operator+=( - y );
}

template<typename T>
TJet<T>& TJet<T>::operator*=( const TJet<T>& y ) 
{
 

 TJet<T> z( *this );
 (*this) = z*y;
 return *this;

}

template<typename T>
TJet<T>& TJet<T>::operator/=( const TJet<T>& y ) 
{

 TJet<T> z( *this );
 (*this) = z/y;
 return *this;

}

template<typename T>
TJet<T>& TJet<T>::operator/=( const T& y ) 
{

 TJLterm<T>* p =0;

 // note: this does not work in mixed mode !

 for ( p = _jl->_jltermStore; p< _jl->_jltermStoreCurrentPtr; p++) {
   if ( !(p->_deleted) ) (p->_value) /= y;
 };  
 
 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//     Member functions(public)  |||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::addTerm( TJLterm<T>* a) 
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
//      The overloaded operators for class TJet<T>
//

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJet<T>::operator==( const TJet<T>& y ) const
{
 return *(this->_jl) == *(y._jl);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJet<T>::operator==( const T& y ) const
{
 return *(this->_jl) == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TJet<T>& x, const TJet<T>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TJet<T>& x, const T& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const T& x, const TJet<T>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator+=( const T& x ) 
{   
 if( _jl == NULL ) {                                         // ??? DANGER: what is the 
   _jl = TJL<T>::makeTJL();                              // ??? reference point?
 }              
 else {
   PREPFORCHANGE(_jl)
 }
 _jl->operator+=( x );
 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator-=( const T& x ) 
{
 this->operator+=( -x );
 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T>& TJet<T>::operator*=( const T& x ) 
{

 TJLterm<T>* p =0;

 // note: this code does not work in mixed mode (i.e. complex argument and double TJet)!

 for ( p = _jl->_jltermStore; p< _jl->_jltermStoreCurrentPtr; p++) {
   if ( !(p->_deleted) ) (p->_value) *= x;
 };  
 
 return *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator+( const TJet<T>& x, const TJet<T>& y ) 
{ 
  // Possibility: constant TJet<T> argument
  if( !(x->_myEnv) ) {
    if( x->_count == 1 ) {
      return x.standardPart() + y;
    }
    else {
      (*pcerr) << "\n\n"
           << "*** ERROR ***                                       \n"
           << "*** ERROR *** TJet<T> operator+( TJet<T>, TJet<T> )             \n"
           << "*** ERROR ***                                       \n"
           << "*** ERROR *** Null environment for the first        \n"
           << "*** ERROR *** TJet<T> argument.                         \n"
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
           << "*** ERROR *** TJet<T> operator+( TJet<T>, TJet<T> )             \n"
           << "*** ERROR ***                                       \n"
           << "*** ERROR *** Null environment for the second       \n"
           << "*** ERROR *** TJet<T> argument.                         \n"
           << "*** ERROR ***                                       \n"
           << endl;
    }
  }

  // Check for consistency and set reference point of the sum.
  if( x->_myEnv != y->_myEnv ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<T> operator+( const TJet<T>& x, const TJet<T>& )",
           "Inconsistent environments." ) );
  }
  TJetEnvironment<T>* pje = x->_myEnv;

                              // ??? This should be modified so that
  TJet<T> z(pje);          // ??? terms beyond the accurate weight of
  TJLterm<T>* p = 0;        // ??? x or y are not computed and carried
  TJLterm<T>* q = 0;        // ??? into the answer.


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

  p = (TJLterm<T>*) getp();
  q = (TJLterm<T>*) getq();

  bool pcont = p != 0;
  bool qcont = q != 0;
 
  T result; 

  while( qcont && pcont ) {

    if( *p <= *q ) {   
      if( *p %= *q ) { 

  
        result = p->_value + q->_value;
        z->append( new( z._jl->storePtr()) TJLterm<T>( p->_index, result ) );
       
        p = (TJLterm<T>*) getp();
        q = (TJLterm<T>*) getq();

        pcont = (p != 0);
        qcont = (q != 0);
      }
      else {
        z->append( new( z._jl->storePtr()) TJLterm<T>( p->_index, p->_value ) );
        p = (TJLterm<T>*) getp();
        pcont = (p != 0);
      }
    }
    else {
       z->append( new( z._jl->storePtr()) TJLterm<T>( q->_index, q->_value ) );
       q = (TJLterm<T>*) getq();
       qcont = (q != 0);
    }
  }

  if( pcont ) {
    while(p) {
      z->append( new( z._jl->storePtr() ) TJLterm<T>( p->_index, p->_value ) );
      p = (TJLterm<T>*) getp();
    }
  }

  if( qcont ) {
    while(q) {
      z->append( new( z._jl->storePtr() ) TJLterm<T>( q->_index, q->_value ) );
      q = (TJLterm<T>*) getq();
    }
  }


  // .....determine the maximum weight computed accurately.
  if( x->_accuWgt < y->_accuWgt ) { z->_accuWgt = x->_accuWgt; }
  else                            { z->_accuWgt = y->_accuWgt; }
 
 
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator+( const TJet<T>& x, const T& y ) 
{
 TJet<T> z;           // !!! "TJet<T> z = x;" does not work.  The copy constructor
 z.DeepCopy( x );         // ??? is called, which makes x and z own the same data!!
 z += y;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator+( const T& y, const TJet<T>& x ) 
{
 TJet<T> z;
 z.DeepCopy( x );
 z += y;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator-( const TJet<T>& x, const T& y ) 
{
 return x + (-y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator-( const T& y, const TJet<T>& x ) 
{
 return y + (-x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator-( const TJet<T>& y ) 
{  
 // If the argument is void, then return it ..
 if( y->_count < 1 ) { 
   return y;
 }

 TJet<T> z( y.Env() );
 TJLterm<T>* p = 0;

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

template<typename T>
void TJet<T>::Negate()
{
TJLterm<T>* p;

 // If the argument is void, then return ...
 if( _jl->_count < 1 ) return;

 // .. otherwise continue normal operations.
 PREPFORCHANGE(_jl)
 dlist_iterator getNext( _jl->_theList );

 while((  p = (TJLterm<T>*) getNext()  )) p->_value = - p->_value;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::Mult( const T& x )
{
 TJLterm<T>* p;

 // If the argument is void, then return ...
 if( _jl->_count < 1 ) return;

 // .. otherwise continue normal operations.
 PREPFORCHANGE(_jl)
 dlist_iterator getNext( _jl->_theList );
 while((  p = (TJLterm<T>*) getNext()  )) p->_value *= x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator-( const TJet<T>& x, const TJet<T>& y ) 
{  
  return ( x + (-y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( const TJet<T>& x, const TJet<T>& y ) 
{

 // Check for consistency 

 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> operator*( const TJet<T>&, const TJet<T>& ) ",
          "Inconsistent environments." ) );
 };

 
 TJetEnvironment<T>* pje = x->_myEnv;
 TJet<T> z(pje);

 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;
 TJL<T>* xPtr  = x._jl;
 TJL<T>* yPtr  = y._jl;
 TJL<T>* zPtr  = z._jl;
 int testWeight    = 0;
 int trialWeight  =  0;
 
 int  indy                = 0;
 TJLterm<T>* zed      = 0;
 TJLterm<T>* upperzed = 0;
 T  dummy                = T();
 T  product              = T();



 // If one of the arguments is void, return it ..
 // Note:: DeepCopy not be necessary here ?

 if( xPtr->_count < 1 ) return z.DeepCopy(x); 
 if( yPtr->_count < 1 ) return z.DeepCopy(y);

 // Determine the maximum weight computed accurately.

 TJLterm<T> x_1stTerm( xPtr->lowTerm() );
 TJLterm<T> y_1stTerm( yPtr->lowTerm() );

 testWeight = xPtr->_accuWgt;
 if( ( y_1stTerm._weight != 0 ) && ( y_1stTerm._value !=  T() ) )
 {
   testWeight += y_1stTerm._weight;
 }

 trialWeight = yPtr->_accuWgt;
 if( ( x_1stTerm._weight != 0 ) && ( x_1stTerm._value !=  T() ) )
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
       pje->_TJLmml[indy]._value = T();
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
      zPtr->append( new( zPtr->storePtr() ) TJLterm<T>( *p ) ); 
      p->_value = T( );  // erase the scratchpad entry
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

template<typename T>
TJet<T> operator*( const TJet<T>& x, const T& y ) 
{
 TJet<T> z(  x->_myEnv );
 
 if( y == T( ) || x._jl->_count < 1 ) {
   return z;
 };
 
 z.DeepCopy( x ); // This is done in this way so that
                  // what is returned does not own
                  // the same data as x.
 

 // scale the values directly, regardless of order (i.e. without using the dlist)
 
 for ( TJLterm<T>* p = z._jl->_jltermStore; p < z._jl->_jltermStoreCurrentPtr; p++  ) 
 {
   if ( ! (p->_deleted) ) (p->_value) *= y;
 }
 
 return z;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( const T& y, const TJet<T>& x ) 
{  
 
 return operator*( x, y );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( const TJet<T>& x, const int& j ) 
{  
  return operator*( x, T(j) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator*( const int& j, const TJet<T>& x ) 
{ 
  return operator*( T(j), x );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator/( const TJet<T>& x, const T& y ) 
{ 
  // Check for division by zero ..
  if( y == 0.0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<T> operator/( const TJet<T>&, const T& ) ",
           "Attempt to divide by a scalar zero." ) );
  }
 
  return ( x*( 1.0 / y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator/( const TJet<T>& x, const int& j ) 
{ 
  // Check for division by zero ..
  if( j == 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<T> operator/( const TJet<T>& x, const int& j )",
           "Attempt to divide by a scalar zero." ) );
  }
 
  return ( x*(1.0/((T) j)) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> operator/( const T& a, const TJet<T>& b ) 
{
  TJet<T> u( b->_myEnv ); 
  u = a;
  return u/b;
} 


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::_truncMult( const TJet<T>& v, const int& wl ) const 
{ 
 //
 // Truncated multiplication: used only by the division operator.
 // No checks are made on the environments.
 //


 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;

 TJLterm<T>* zed;
 TJLterm<T>* upperzed;

 T  dummy   = T();
 T  product = T();

 int indy = 0;

 // If one of the arguments is void, return it ..

 if ( _jl->_count < 1 ) return *this;   // ??? Is this form going to cause

 if ( v->_count  < 1 ) return v;        // ??? a problem? 

 // Initializations

 TJetEnvironment<T>* pje = _jl->_myEnv;
 TJet<T> z( pje );
 TJL<T>* zPtr = z._jl;

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
     zPtr->append( new( zPtr->storePtr() ) TJLterm<T>( *zed ) );
     zed->_value = T( );
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

template<typename T>
TJet<T> operator/( const TJet<T>& wArg, const TJet<T>& uArg ) 
{ 

 // Check for void operators ..
 if ( wArg->_count < 1 ) return wArg;
 if ( uArg->_count < 1 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> operator/( const TJet<T>&, const TJet<T>& )",
          "Attempt to divide by a void TJL<T> variable." ) );
 }
 
 // Check for consistency 
 if( wArg->_myEnv != uArg->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> operator/( const TJet<T>&, const TJet<T>& )",
          "Inconsistent environments." ) );
 }

 // Initialize local variables and set the environment of the answer.
 // (These steps are not necessary, but they enforce a discipline.)

 TJet<T> v(wArg->_myEnv);
 TJet<T> vn(wArg->_myEnv);
 TJet<T> w(wArg->_myEnv);
 TJet<T> u(wArg->_myEnv);

 TJLterm<T>* qu = 0;
 TJLterm<T>* qw = 0;
 T u0 = T();

 int wgt = 0;
 int wl  = 0;
 int mw  =  wArg->_myEnv->_maxWeight;
 

 dlist_looper gu( uArg._jl->_theList );
 dlist_looper gw( wArg._jl->_theList );

 // Normalize the denominator
 if( ( qu = (TJLterm<T>*) gu() ) == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> operator/( const TJet<T>&, const TJet<T>& )",
          "Division algorithm called with uninitialized JL." ) );
   }
 if( ( wgt = qu->_weight ) != 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> operator/( const TJet<T>&, const TJet<T>& )",
          "Attempt to divide by nilpotent element." ) );
   }
 if(  (u0 = qu->_value ) == 0.0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> operator/( const TJet<T>&, const TJet<T>& )",
          "Attempt to divide by zero." ) );
   }

 u = uArg / u0;
 
 // Recursive algorithm
 qw = (TJLterm<T>*) gw();
 if( qw->_weight == 0 ) v = ( qw->_value );
 else                   v = T(); 
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

template<typename T>
TJet<T> operator^( const TJet<T>& x, const TJet<T>& y ) 
{ 

 // Check for consistency 
 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> operator^( const TJet<T>&, const TJet<T>& )",
          "Inconsistent environments." ) );
 }

 TJetEnvironment<T>* theEnv = x->_myEnv;
 TJet<T> z (theEnv);

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
          "TJet<T> operator^( const TJet<T>&, const TJet<T>& )",
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

template<typename T>
TJet<T> acos( const TJet<T>& x ) 
{
  // Returns answer in (0,pi) if asin returns (-pi/2,pi/2).
  return ( ((T) M_PI_2) - asin(x) );
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> asin( const TJet<T>& x ) 
{ 

 const T zero = T(); 	 
 TJetEnvironment<T>* pje = x.Env(); 	 
  	 
 // Initial Newton's step 	 
 
 TJet<T> z;	        
 TJet<T> dz;

 z.DeepCopy( x ); 	 
 dz = ( sin(z) - x ) / cos(z); 	 
  	 
 // Setting up the iteration 	 
 int upperBound = 8; 	 
 
 int iter = 0;	   
 int indy = 0; 	 
 double compValue; 	 
 bool repeat = true; 	 
 
 TJLterm<T>* pz = 0; 	 
 	 
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
  (*pcerr) << "*** WARNING *** TJL<T>& asin( TJL<T>& x ) {         \n";
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

template<typename T>
TJet<T> atan( const TJet<T>& x ) 
{   
 
 TJet<T> z;
 TJet<T> c, dz;
 int iter = 0;

 z.DeepCopy( x );
 c = cos( z );
 dz = c*( sin(z) - x*c );

 while( ( dz != T() ) && ( iter++ < MX_MAXITER ) ) {
  z -= dz;
  c = cos( z );
  dz = c*( sin(z) - x*c );
 }

 if( iter >= MX_MAXITER ) {
  (*pcerr) <<  "*** WARNING ***                             \n" 
       <<  "*** WARNING *** TJL<T>& atan( TJL<T>& x ) {         \n" 
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

template<typename T>
TJet<T> cos( const TJet<T>& x ) 
{ 
 
 TJet<T> epsilon(x->_myEnv);

 T cs = T();
 T sn = T();

 TJLterm<T>* p = 0;

 dlist_iterator getNext( x._jl->_theList );
 
 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T> cos( const TJet<T>& ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new( epsilon.storePtr() ) TJLterm<T>( x->_myEnv->_allZeroes, ((T) 1.0), x->_myEnv ) );
   return epsilon;
 }
 
 p = (TJLterm<T>*) getNext();
 if( ( p -> _weight ) == 0 ) {         // x has non-zero standard part
   cs = std::cos( p -> _value );
   sn = std::sin( p -> _value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   p->_deleted = true; //delete p; // NOT WITH NEW MEM MANAGEMENT
   return cs*TJet<T>::_epsCos( epsilon ) - sn*TJet<T>::_epsSin( epsilon );
   }
 else {                               // x is pure infinitesimal
   return TJet<T>::_epsCos( x );
   }
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> cosh( const TJet<T>& x ) 
{ 
 TJet<T> z = exp(x);
 z = ( z + ( 1.0 / z ) ) / 2.0;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> exp( const TJet<T>& x ) 
{ 
 
 TJet<T> epsilon( x->_myEnv );
 T factor = T();
 TJLterm<T>* p = 0;

 dlist_iterator getNext( x._jl->_theList );

 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T> exp( const TJet<T>& ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new( epsilon.storePtr() ) TJLterm<T>( x->_myEnv->_allZeroes, ((T) 1.0), x->_myEnv ) );
   return epsilon;
 }
 
 p = (TJLterm<T>*) getNext();
 if( ( p -> _weight ) == 0 ) {         // x has non-zero standard part
   factor = std::exp( p -> _value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   p->_deleted = true; // delete p; NOT WITH NEW MEM MNGMNT
   return factor*TJet<T>::_epsExp( epsilon );
   }
 else {                               // x is pure infinitesimal
   return TJet<T>::_epsExp( x );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> log ( const TJet<T>& x ) 
{ 
  TJet<T>   epsilon(x->_myEnv);
  TJet<T>   u(x->_myEnv);
  TJet<T>   w(x->_myEnv);

  T std   = T();
  double n = 0.0;
  TJLterm<T>*  p = 0;

  dlist_iterator  getNext( x._jl->_theList );
 
 if( x->_count == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> log ( const TJet<T>& ) { ",
          "Argument is zero." ) );
 }
 
 p = (TJLterm<T>*) getNext();
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
          "TJet<T> log ( const TJet<T>& ) ",
          "Argument's standard part vanishes; it is nilpotent." ) );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> log10( const TJet<T>& x ) 
{
 static const T logE = 0.4342944819032518276511289;
 return  logE*log(x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> pow( const TJet<T>& x, const double& s ) 
{ 

 TJet<T> epsilon(x->_myEnv );
 T factor = T();
 T std    = T();
 TJLterm<T>* p = 0;
 int u = 0;

 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T> pow( const TJet<T>&, double ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new( epsilon.storePtr() ) TJLterm<T>( x->_myEnv->_allZeroes, T(), x->_myEnv ) );
   return epsilon;
 }
 
 dlist_iterator getNext( x._jl->_theList );

 p = (TJLterm<T>*) getNext();
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
   epsilon = factor*TJet<T>::_epsPow( epsilon, s );
   return epsilon;
   }
 else                                 // x is pure infinitesimal
   {
   u = nearestInteger( s );
   if( s != (T) u ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T> pow( const TJet<T>&, const double& )",
            "Cannot use infinitesimal as base with non-integer _exponent." ) );
   }
   epsilon = ((T) 1.0);
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

template<typename T>
TJet<T> pow( const TJet<T>& x, int n ) 
{

  TJet<T> z( x->_myEnv ); 

  if( n == 0 ) z = ((T) 1.0);
  else if( n > 0 ) {
    z.DeepCopy( x );
    for( int i = 2; i <= n; i++ ) z *= x;
  }
  else {
    TJet<T> xr;
    z = 1.0 / x;
    xr = z;
    for( int i = -2; i >= n; i-- ) z *= xr;
  }
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> sin( const TJet<T>& x ) 
{ 
 
 TJet<T> epsilon(x->_myEnv);
 T cs = T();
 T sn = T();

 TJLterm<T>* p = 0;

 if( x->_count == 0 ) {
   if( epsilon->_count != 0  ) {
     ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T> sin( const TJet<T>&, double ) ",
            uic.str().c_str() ) );
   }
   epsilon.addTerm( new( epsilon.storePtr() ) TJLterm<T>( x->_myEnv->_allZeroes, T(), x->_myEnv ) );
   return epsilon;
 }
 
 dlist_iterator getNext( x._jl->_theList );
 p = (TJLterm<T>*) getNext();
 if( ( p -> _weight ) == 0 ) {         // x has non-zero standard part
   cs = std::cos( p -> _value );
   sn = std::sin( p -> _value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   p->_deleted = true; // delete p; NOT WITH NEW MEM MNGMNT
   return sn*TJet<T>::_epsCos( epsilon ) + cs*TJet<T>::_epsSin( epsilon );
   }
 else {                               // x is pure infinitesimal
   return TJet<T>::_epsSin( x );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> sinh( const TJet<T>& x ) 
{
 TJet<T> z;
 z = exp(x);
 return ( z - (1.0/z))/2.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> sqrt( const TJet<T>& x ) 
{


 TJLterm<T>* p = 0;

 TJet<T> epsilon( x->_myEnv );
 T factor = T();
 T std    = T();

 if( x->_count == 0 ) {

   // * the code below should be replaced by a more permanent 
   // * solution; possibly a function that clears the scratchpads for all
   // * existing environments. 
   // * The scratchpads should be cleared when *any* exception is thrown
   // * within mxyzptlk because the TJLterm _value fields may contain  
   // * "Nan"s. 
   // *DO NOT REMOVE OR COMMENT OUT UNLESS YOU HAVE IMPLEMENTED A BETTER SOLUTION !* 
   //   
   // -JFO 

   //--------------------------------------------------------------------------

   (*pcout) << "Resetting Environment Scratchpad" << std::endl;
   for (int i=0; i< x._jl->_myEnv->_maxTerms; i++ ) {
         x._jl->_myEnv->_TJLmml[i]._value = 0.0;
   }          
   
   //--------------------------------------------------------------------------

   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> sqrt( const TJet<T>& ) { ",
          "Argument is zero." ) );
 }
 
 dlist_iterator getNext( x._jl->_theList );
 p = (TJLterm<T>*) getNext();


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
     return factor * TJet<T>::_epsSqrt( epsilon );
     }
   }
 else                                 // x is pure infinitesimal
   {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> sqrt( const TJet<T>& ) ",
          "Argument's standard part vanishes; it is nilpotent." ) );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> tan( const TJet<T>& x ) 
{ 
 return sin(x) / cos(x) ;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> tanh( const TJet<T>& x ) 
{ 
 return sinh(x) / cosh(x);
}
 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJet<T> erfc( const TJet<T>& z ) 
{
  static const T one = ((T) 1.0);
  return ( one - erf( z ) );
}



// **************************************************************
// **************************************************************
// **************************************************************
//
//      Auxiliary functions which operate on infinitesimals.
//      It is assumed that the calling program checks to be sure
//      that the argument has zero standard part.
 
template<typename T>
TJet<T> TJet<T>::_epsCos( const TJet<T>& epsilon ) 
{ 
 
 TJet<T> z;
 TJet<T> epsq, term;
 double n;

 z->_myEnv = epsilon->_myEnv;
 z = ((T) 1.0);
 
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

template<typename T>
TJet<T> TJet<T>::_epsExp( const TJet<T>& epsilon ) 
{ 
 TJet<T> z;
 TJet<T> term;
 double n;
 
 z->_myEnv = epsilon->_myEnv;
 z = ((T) 1.0);
 
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

template<typename T>
TJet<T> TJet<T>::_epsPow( const TJet<T>& epsilon, const double& s ) 
{ 
 TJet<T> z;
 TJet<T> term;
 double f;
 double n;

 z->_myEnv = epsilon->_myEnv;
 z = ((T) 1.0);
 
 f = s;
 n = 1.0;
 term = s*epsilon;
 
 z->_myEnv = epsilon->_myEnv;
 
 while( term->_count > 0 ) {
   z += term;
   term = ( ((T) (--f)) * term * epsilon ) / ++n;
   }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::_epsSin( const TJet<T>& epsilon ) 
{ 
 
 TJet<T> z;
 TJet<T> epsq, term;
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

template<typename T>
TJet<T> TJet<T>::_epsSqrt( const TJet<T>& epsilon ) 
{  

 // This function is identical to epsPow
 // with the substitution  s = 1/2

 TJet<T>     z;
 TJet<T>     term;
 
 z->_myEnv = epsilon->_myEnv;
 z = ((T) 1.0);
 
 double f    = 1.0 / 2.0;
 double n    = 1.0;
 term = f*epsilon;
 
 z->_myEnv = epsilon->_myEnv;
 
 while( term->_count > 0 ) {
   z += term;
   term *= ( ((T) (--f)) * epsilon ) / ++n;
 }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}
 

// **************************************************************
// **************************************************************
// **************************************************************
//
//      Implementation of Class TJet<T>

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::peekAt() const 
{
 dlist_traversor getNext( _jl->_theList );
 int i;
 TJLterm<T>* p;
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
   p = (TJLterm<T>*) q->info();
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
   (*pcout) << "Index:  " << (p -> _index) << endl;

   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::printCoeffs() const 
{
 dlist_iterator getNext( _jl->_theList );
 int i;
 TJLterm<T>* p;

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

 while((  p = (TJLterm<T>*) getNext()  )) {
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

template<typename T>
void TJet<T>::writeToFile( char* fileName ) const 
{
 ofstream outStr( fileName );
 this->writeToFile( outStr );
 outStr.close();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::writeToFile( ofstream& outStr ) const 
{
 _jl->writeToFile( outStr );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::scaleBy( T y ) 
{
 PREPFORCHANGE(_jl)
 _jl->scaleBy( y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>* TJet<T>::get() 
{
 PREPFORCHANGE(_jl)
 return _jl->get();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJet<T>::firstTerm() const 
{
 return _jl->firstTerm();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJet<T>::lowTerm() const 
{
 return _jl->lowTerm();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::standardPart() const 
{
 return _jl->standardPart();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJet<T>::clear() 
{
 PREPFORCHANGE(_jl)
 _jl->clear();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::weightedDerivative( const int* ind ) const 
{
 return _jl->weightedDerivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::derivative( const int* ind ) const 
{
 return _jl->derivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::filter( const int& wgtLo, const int& wgtHi ) const 
{ 
 TJet<T> z;
 TJL<T>* zPtr = z._jl;
 TJL<T>* thisPtr = _jl;
 dlist_iterator getNext( thisPtr->_theList );
 TJLterm<T>* p;
 TJLterm<T>* q;
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
 
 while((  p = (TJLterm<T>*) getNext()  )) {
   wgt = p->_weight;
   if( ( wgt >= wgtLo ) && ( wgt <= wgtHi ) ) {
     q = new( zPtr->storePtr() ) TJLterm<T>( p );
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

template<typename T>
TJet<T> TJet<T>::filter( bool (*f) ( const IntArray&, const T& ) ) const 
{ 
 TJet<T> z( _jl->_myEnv );
 TJL<T>* zPtr  = z._jl ;
 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;


 dlist_iterator getNext( _jl->_theList );

 int i  = 0;
 int nv = 0;
 IntArray oldIndex;

 nv = _jl->_myEnv->_numVar;
 oldIndex.Reconstruct( nv );

 while((  p = (TJLterm<T>*) getNext()  )) {
  oldIndex = p->_index;

  if( (*f)( p->_index, p->_value ) ) {
   q = new( zPtr->storePtr() ) TJLterm<T>( p );
   zPtr->append( q );
  }

  for( i = 0; i < nv; i++ ) 
   if( oldIndex(i) != p->_index(i) ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<T> TJet<T>::filter( char (*f) ( const IntArray&, "
           "const T& ) ) const",
           "Test routine changes value of _index array." ) );
   }
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::_concat() const 
{

 TJetEnvironment<T>* theEnv = _jl->_myEnv;
 TJet<T> v(( ((theEnv)->_TJLmonomial )[0] )->_myEnv );
 TJLterm<T>* p = 0;

 
 
 dlist_iterator getNext( _jl->_theList );
 while((  p = (TJLterm<T>*) getNext()  )) {
   if( p->_weight > _jl->_accuWgt ) break;
   v += ( p->_value ) * ( theEnv->_TJLmonomial[ theEnv->_offset.index(p->_index) ] );
 }
 
 return v;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::operator() ( const TJetVector<T>& y ) const 
{
 int n = _jl->_myEnv->_numVar;   

 TJet<T> z;      // ??? so as to be self-contained.

 TJet<T>* u = new TJet<T> [ n ];

 for( int i = 0; i < n; i++ ) u[i] = y(i);
 
 z = operator()( u );

 delete [] u;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::operator() ( const TJet<T>* y ) const 
{ 
 TJetEnvironment<T>* theEnv = _jl->_myEnv;

 TJet<T>*     u = new TJet<T> [ theEnv->_numVar ];
 TJet<T>      z(y[0]->_myEnv);

 TJet<T>      term;         // Initializing term should not be necessary. 

 int  i = 0;
 int  j = 0;
 int  w = 0;




 // Check consistency of reference points and
 // subtract reference point prior to concatenation.
 for( int i = 0; i < theEnv->_numVar; i++ ) {
   if( y[i]->_myEnv != y[0]->_myEnv ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T> TJet<T>::operator() ( const TJet<T>* ) const ",
            "Inconsistent environments." ) );
   }
   u[i] = y[i] - theEnv->_refPoint[i];
 }
 
 // Evaluate and store monomials.
 
 // The zeroth one.
 ( ( theEnv->_TJLmonomial )[0] )->_myEnv = y[0]->_myEnv; // Needed by 
                                                         // TJet<T>::concat
 theEnv->_TJLmonomial[0] = ((T) 1.0);

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
 // Now traverse the TJL<T> variable and evaluate.
 // Formerly done by using concat routine, which
 // I copy here
 {

  dlist_iterator getNext( _jl->_theList );
  TJLterm<T>* p = 0;
 
  z->_myEnv = (( theEnv->_TJLmonomial )[0] )->_myEnv;
  
  while((  p = (TJLterm<T>*) getNext()  )) {
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

template<typename T>
T TJet<T>::operator() ( const Vector& x ) const 
{
 return _jl->operator()( x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJet<T>::operator() ( const T* x ) const 
{
 return _jl->operator()( x );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TJet<T>::D( const int* n ) const 
{
 bool doit = false;

 int f = 0;
 int j = 0;
 int w = 0;

 TJet<T> z( _jl->_myEnv);
 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;

 dlist_iterator getNext( _jl->_theList );
 
 // --- Preliminary check of _index set.
 
 w = 0;
 
 for( int i = 0; i < _jl->_myEnv->_numVar; i++ ) {
   if( n[i] < 0 ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T> TJet<T>::D( const int* ) const",
            "Cannot differentiate with negative _index." ) );
     }
   w += n[i];
 }
 
 if( w > _jl->_accuWgt ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> TJet<T>::D( const int* ) const",
          "Differentiation request beyond accuracy allowed." ) );
 }
 
 if( w == 0 ) {
   z = *this;
   return z;
 }


 // --- Construct the derivative one link at a time.
 // --- ( See note Obs.4 )
 
 while((  p = (TJLterm<T>*) getNext()  )) {
 
   q = new( z->storePtr() ) TJLterm<T>( p );
 
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
              "TJet<T> TJet<T>::D( const int* ) const",
              "Horrible, unexplainable error while differentiating!" ) );
     }                           // Super fussbudget!! ( f "must" be positive )
   
     // -- Make final changes in private data of the TJLterm<T> and
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

template<typename T>
TJet<T> TJet<T>::D( const IntArray& n ) const 
{

 if( n.Dim() != _jl->_myEnv->_numVar ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> TJet<T>::D( const IntArray& ) const ",
          "Inconsistent dimensions." ) );
 }

 bool doit = false;

 int f = 0;
 int j = 0;
 int w = 0;

 TJet<T> z(_jl->_myEnv );

 dlist_iterator getNext( _jl->_theList );

 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;
 
 // --- Initializations.
 f = j = w = 0;

 // --- Preliminary check of _index set.
 
 w = 0;
 
 for( int i = 0; i < _jl->_myEnv->_numVar; i++ ) {
   if( n(i) < 0 ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T> TJet<T>::D( const IntArray& ) const ",
            "Cannot differentiate with negative _index." ) );
   }
   w += n(i);
 }
 
 if( w > _jl->_accuWgt ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJet<T> TJet<T>::D( const IntArray& ) const ",
          "Differentiation request beyond accuracy allowed." ) );
 }
 
 if( w == 0 ) {
   z = *this;
   return z;
 }


 // --- Construct the derivative one link at a time.
 // --- ( See note Obs.4 )
 
 while((  p = (TJLterm<T>*) getNext()  )) {
 
   q = new( z->storePtr() ) TJLterm<T>( p );
 
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
              "TJet<T> TJet<T>::D( const IntArray& ) const",
              "Horrible, unexplainable error while differentiating!" ) );
       }                           // Super fussbudget!! ( f "must" be positive )
   
     // -- Make final changes in private data of the TJLterm<T> and
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
