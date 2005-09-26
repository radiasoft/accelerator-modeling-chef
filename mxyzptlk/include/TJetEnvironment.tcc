/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetEnvironment.cc
******  Version:   2.0
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab   
******                All Rights Reserved                             
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
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
******  Feb  2005 - Jean-Francois Ostiguy
******              ostiguy@fnal.gov
******  
******              - Efficiency improvements.
******              - new memory management
******  
******  Sept 2005   ostiguy@fnal.gov
******
******              - new code based on a single template parameter
******                instead of two. Mixed mode now handled
******                using conversion operators.
******
******                                                                
**************************************************************************
*************************************************************************/
#ifndef TJETENVIRONMENT_TCC
#define TJETENVIRONMENT_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <iosetup.h>
#include <GenericException.h>
#include <TJetEnvironment.h>
#include <TJet.h>
#include <VectorD.h>   // Used by .approxEq

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


#define PREPFORCHANGE(_jl)  if(((_jl)->_rc)>1){--((_jl)->_rc);(_jl) = TJL<T> makeTJL(_jl);}

// ================================================================
//      Global variables
//

#define DEFSCALE  0.001  // Default _scale for all coordinates

template<typename T> bool TJetEnvironment<T>::_skipEnvEqTest = false;
  // Used by the EndEnvironment function.


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


// ================================================================
//      Implementation of TJetEnvironment
//      ??? Belongs in JL.cc


template<typename T>
TJetEnvironment<T>::TJetEnvironment() 
: _offset( 2, 1 )
{
  _numVar            = 0;
  _spaceDim          = -1;
  _dof               = 0;            
  _refPoint          = 0;       
  _scale             = 0;
  _maxWeight         = 0;
  _maxTerms          = 0;
  _monomial          = 0;
  _TJLmonomial       = 0;
  _TJLmml            = 0;
  _exponent          = 0;
  _pbok              = 0;
}
 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetEnvironment<T>::TJetEnvironment( const TJetEnvironment& x )
: _maxWeight( x._maxWeight ),
  _monomial(0), 
  _maxTerms( x._maxTerms ),
  _numVar( x._numVar ), 
  _spaceDim( x._spaceDim ),
  _pbok( x._pbok ),
  _dof( x._dof ),
  // OBSOLETE _numPaths(0), 
  _TJLmonomial(0), 
  _TJLmml(0),
  _offset( x._maxWeight, x._numVar )
{
  if( TJet<T>::workEnvironment() != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment::TJetEnvironment( TJetEnvironment& )",
           "Close open environment before copying." ) );
  }

  if( _numVar == 0 ) {
    (*pcerr) << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment::TJetEnvironment       \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    _monomial    = 0;
    _TJLmonomial = 0;
    _TJLmml      = 0;
    _exponent    = 0;
    // OBSOLETE _expCode     = 0;
    _pbok        = 0;
    // OBSOLETE _numPaths    = 0;
    _numVar      = 0;
    _maxWeight   = 0;
    _spaceDim    = -1;
    _dof         = 0;            
    _refPoint    = 0;       
    _scale       = 0;

    return;
  }

  int w = _maxWeight;
  int n = _numVar;
  int i, j;

  _exponent   = new int[ n ];

  _refPoint   = new T[ n ];
  _scale      = new double[ n ];

  _allZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   _refPoint[i] = x._refPoint[i];
   _scale[i] = x._scale[i];
   _allZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }

  _buildScratchPads();

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJetEnvironment<T>::_buildScratchPads()
{

  if(    0 < _maxWeight && 0 < _numVar && 0 != _exponent ) {
    int w = _maxWeight;
    int n = _numVar;
    _maxTerms = bcfRec( w + n, n );

    if( _monomial ) { delete [] _monomial; }
    _monomial    = new T[ _maxTerms ];

    if( _TJLmonomial ) { delete [] _TJLmonomial; }
    _TJLmonomial = new TJet<T>[ _maxTerms ];
    int i;
    for( i = 0; i < _maxTerms; i++ ) {
      _TJLmonomial[i].Reconstruct( this ); 
    }

    if( _TJLmml ) { TJLterm<T>::array_deallocate(_TJLmml);}
   
    _TJLmml =  TJLterm<T>::array_allocate(_maxTerms);
 
    IntArray powers(n);

    T startValue(0.0);

    _TJLmml[0].Reconstruct( powers, startValue );

    i = 1;
    for( int wd = 1; wd <= w; wd++ ) {
      while( nexcom( wd, _numVar, _exponent ) ) {
        powers.Set(_exponent);
        if( i < _maxTerms ) {
          _TJLmml[i].Reconstruct( powers, startValue );
        }
        else {
          throw( GenericException( __FILE__, __LINE__, 
                 "TJetEnvironment::_buildScratchPads()",
                 "Overran number of possible monomial terms." ) );
        }
        i++;
      }
    }
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment::_buildScratchPads()",
           "Function called by underconstructed environment." ) );
    
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetEnvironment<T>::~TJetEnvironment() 
{
  if( _monomial   )  { delete [] _monomial;    _monomial = 0;    }
  if( _TJLmonomial ) { delete [] _TJLmonomial; _TJLmonomial = 0; }
  if( _TJLmml     )  { TJLterm<T>:: array_deallocate(_TJLmml);      
                       _TJLmml = 0;      
                     }
  if( _exponent   )  { delete [] _exponent;    _exponent = 0;    }
  // OBSOLETE if( _expCode    )  { delete [] _expCode;     _expCode = 0;     }
  // OBSOLETE if( _numPaths   )  { delete    _numPaths;    _numPaths = 0;    }
  if( _refPoint   )  { delete [] _refPoint;    _refPoint = 0;    }
  if( _scale      )  { delete [] _scale;       _scale = 0;       }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetEnvironment<T>& TJetEnvironment<T>::operator=( const TJetEnvironment& x )
{
  // The first line is useful for the EndEnvironment function.
  if( TJetEnvironment::_skipEnvEqTest == true ) {
    TJetEnvironment::_skipEnvEqTest = false;
  }
  else if( TJet<T>::workEnvironment() != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment& TJetEnvironment::operator=( const TJetEnvironment& )",
           "Close open environment before assigning." ) );
  }

  _maxWeight = x._maxWeight;
  _numVar    = x._numVar;
  _spaceDim  = x._spaceDim;
  _maxTerms  = x._maxTerms;

  _offset.reconstruct( x._offset );

  if( _numVar == 0 ) {
    (*pcerr) << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment::operator=              \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    _monomial           = 0;
    _TJLmonomial        = 0;
    _TJLmml             = 0;
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

  int w = _maxWeight;
  int n = _numVar;
  int i, j;

  if( _monomial ) { delete [] _monomial; }      // this seems redundant and not necessary
  _monomial   = new T[ bcfRec( w + n, n ) ];    // done by _buildScratchPads() ???

  if( _exponent ) { delete [] _exponent; }
  _exponent   = new int[ n ];

  _pbok       = x._pbok;

  _dof                = x._dof;

  if( _refPoint ) { delete [] _refPoint; }
  _refPoint = new T[ n ];

  if( _scale ) { delete [] _scale; }
  _scale = new double[ n ];

  _allZeroes.Reconstruct( n );
  
  for( i = 0; i < n; i++ ) {
   _refPoint[i] = x._refPoint[i];
   _scale[i] = x._scale[i];
   _allZeroes(i) = 0;   // ??? Redundant and unnecessary
  }

  _buildScratchPads();

  return *this;
}


template<typename T>
bool TJetEnvironment<T>::operator==( const TJetEnvironment& x ) const
{
  int i;
  if( x._numVar != _numVar ) return false;
  if( x._spaceDim != _spaceDim ) return false;
  if( x._maxWeight != _maxWeight ) return false;
  for( i = 0; i < _numVar; i++ ) {
    if( _refPoint[i] != x._refPoint[i] ) return false;
    if( _scale[i] != x._scale[i] ) return false;
  }
  return true;  
}


template<typename T>
bool TJetEnvironment<T>::approxEq( const TJetEnvironment& x, const Vector& tolerance ) const
{
  int i;
  if( x._numVar != _numVar ) return false;
  if( x._spaceDim != _spaceDim ) return false;
  if( x._maxWeight != _maxWeight ) return false;
  for( i = 0; i < _numVar; i++ ) {
    if( std::abs(_refPoint[i] - x._refPoint[i]) > std::abs(tolerance(i)) ) 
    { return false; }
    // Note: unlike operator=, there is no test for _scale here.
  }
  return true;  
}


template<typename T>
bool TJetEnvironment<T>::approxEq( const TJetEnvironment& x, const double* tolerance ) const
{
  Vector w( _numVar, tolerance );
  return this->approxEq( x, w );
}


template<typename T>
bool TJetEnvironment<T>::hasReferencePoint( const Vector& x ) const
{
  if( x.Dim() != _numVar ) { return false; }
  for( int i = 0; i < _numVar; i++ ) {
    if( _refPoint[i] != x(i) ) return false;
  }
  return true;  
}


template<typename T>
bool TJetEnvironment<T>::hasReferencePoint( const double* x ) const
{
  Vector w( _numVar, x );
  return this->hasReferencePoint( w );
}


template<typename T>
bool TJetEnvironment<T>::hasApproxReferencePoint( const Vector& x, const Vector& tolerance ) const
{
  if( x.Dim() != _numVar || tolerance.Dim() != _numVar ) 
  { return false; }
  for( int i = 0; i < _numVar; i++ ) {
    if( std::abs(_refPoint[i] - x(i)) > std::abs(tolerance(i)) ) 
    { return false; }
  }
  return true;  
}


template<typename T>
bool TJetEnvironment<T>::hasApproxReferencePoint( const double* x, const Vector& tolerance ) const
{
  Vector w( _numVar, x );
  return this->hasApproxReferencePoint( w, tolerance );
}


template<typename T>
bool TJetEnvironment<T>::hasApproxReferencePoint( const Vector& x, const double* tolerance ) const
{
  Vector w( _numVar, tolerance );
  return this->hasApproxReferencePoint( x, w );
}


template<typename T>
bool TJetEnvironment<T>::hasApproxReferencePoint( const double* x, const double* tolerance ) const
{
  Vector w( _numVar, x );
  Vector z( _numVar, tolerance );
  return this->hasApproxReferencePoint( w, z );
}


template<typename T>
bool TJetEnvironment<T>::operator!=( const TJetEnvironment& x ) const
{
  return !operator==( x );
}


template<typename T>
ostream& operator<<( ostream& os, const TJetEnvironment<T>& x )
{
  int i;
  os << x._numVar << endl;
  os << x._spaceDim << endl;
  for( i = 0; i < x._numVar; i++ ) os << setprecision(30) 
                                     << x._refPoint[i] << endl;
  for( i = 0; i < x._numVar; i++ ) os << setprecision(30) 
                                     << x._scale[i] << endl;
  os << x._maxWeight << endl;
  os << x._pbok << endl;
  return os;
}

template<typename T>
istream& streamIn( istream& is, TJetEnvironment<T>** x )
{
  static slist envs;
  slist_iterator getNext( envs );
  TJetEnvironment<T>* pje;
  int i(0), j(0);

  pje = new TJetEnvironment<T>;
  is >> pje->_numVar;
  is >> pje->_spaceDim;

  if( pje->_numVar < pje->_spaceDim ) {
    throw( GenericException( __FILE__, __LINE__, 
           "istream& streamIn( istream&, TJetEnvironment** )",
           "Jet_enviroment dimensions are wrong." ) );
  }
  
  pje->_refPoint = new T[ pje->_numVar ];
  pje->_scale = new double[ pje->_numVar ];
  for( i = 0; i < pje->_numVar; i++ ) is >> pje->_refPoint[i];
  for( i = 0; i < pje->_numVar; i++ ) is >> pje->_scale[i];

  is >> pje->_maxWeight;
  is >> pje->_pbok;


  // --- Calculations, just as in Jet::EndEnvironment
  int n = pje->_numVar;
  int w = pje->_maxWeight;

  pje->_offset.reconstruct( w, n );

  pje->_exponent   = new int[ n ];
  // OBSOLETE pje->_expCode    = new char[ w + n ];

  pje->_allZeroes.Reconstruct(n);
  for( i = 0; i < n; i++ ) pje->_allZeroes(i) = 0;


  pje->_buildScratchPads();

  // Initialize the coordinates
  // ??? HOW ???

  // --- Test to see if environment was already read
  TJetEnvironment<T>* q;
  bool found( false );
  while((  !found && ( q = (TJetEnvironment<T>*) getNext()  ) )) 
  {
    if( *pje == *q ) 
    {
      delete pje;
      pje = q;
      found = true;
    }
  }
  if( !found ) {
    envs.append( pje );
  }

  // --- Finished
  *x = pje;
  return is;
}


#undef PREPFORCHANGE

#endif // TJETENVIRONMENT_TCC
