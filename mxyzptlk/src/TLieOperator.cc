/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TLieOperator.cc
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

#include <stdlib.h>

#include "GenericException.h"
#include "TLieOperator.h"

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

#ifndef MX_SMALL
#define MX_SMALL    1.0e-12 // Used by Jet::addTerm to decide 
                            //   removal of a JetCterm.
#endif  // MX_SMALL

#ifndef MX_MAXITER
#define MX_MAXITER  100     // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER

using namespace std;

// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//    Implementation of class TLieOperator<T1,T2>
//

//    Constructors and destructors    |||||||||||||||||||||||||||


template<typename T1, typename T2>
TLieOperator<T1,T2>::TLieOperator<T1,T2>( TJetEnvironment<T1,T2>* theEnv ) 
: TJetVector<T1,T2>( theEnv->_spaceDim, 0, theEnv )
{
 _myEnv = theEnv;
 
 if( theEnv->_spaceDim == 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TLieOperator<T1,T2>::TLieOperator<T1,T2>( TJetEnvironment<T1,T2>* ) ",
          "Phase space has dimension zero." ) );
 }
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TLieOperator<T1,T2>::TLieOperator<T1,T2>( const TLieOperator<T1,T2>& x ) 
: TJetVector<T1,T2>( x._myEnv->_spaceDim, 0, x._myEnv )
{
 int i;
 
 _myEnv = x._myEnv;

 for ( i = 0; i < _myEnv->_spaceDim; i++ ) {
   _comp[i] = x._comp[i];
 }
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TLieOperator<T1,T2>::TLieOperator<T1,T2>( const TJet<T1,T2>& x ) 
: TJetVector<T1,T2>( (x.Env())->_spaceDim, 0, x.Env() )
{ 
 int i = 0;
 TJetEnvironment<T1,T2>* pje = x.Env();

 int s = pje->_spaceDim;
 int n = pje->_numVar;
 int* ndx = new int [n];

 if( s == 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TLieOperator<T1,T2>::TLieOperator<T1,T2>( const TJet<T1,T2>& ) ",
          "Phase space has dimension zero." ) );
 }
 
 if( 2*(s/2) != s ) {
   ostringstream uic;
   uic  << "Phase space has odd dimension = " << s;
   throw( GenericException( __FILE__, __LINE__, 
          "TLieOperator<T1,T2>::TLieOperator<T1,T2>( const TJet<T1,T2>& ) ",
          uic.str().c_str() ) );
 }

 for( i = 0; i < n; i++ ) ndx[i] = 0;

 for( i = 0; i < s/2; i++ ) {
  ndx[i + s/2] = 1;
  _comp[i] = x.D( ndx );
  ndx[i + s/2] = 0;
 }

 for( i = s/2; i < s; i++ ) {
  ndx[i - s/2] = 1;
  _comp[i] = - x.D( ndx );
  ndx[i - s/2] = 0;
 }

 for( i = s; i < n; i++ ) {
   _comp[i].setEnvTo( pje );
   _comp[i] = ((T1) 0.0);
 }

 delete [] ndx;
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TLieOperator<T1,T2>::TLieOperator<T1,T2>( char*, TJetEnvironment<T1,T2>* pje  ) 
: TJetVector<T1,T2>( pje->_spaceDim, 0, pje )
{ 
 int i;
 
 if( !pje ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TLieOperator<T1,T2>::TLieOperator<T1,T2>( char*, TJetEnvironment<T1,T2>* ) ",
          "Jet__environment pointer is null." ) );
   }
 if( pje->_spaceDim == 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TLieOperator<T1,T2>::TLieOperator<T1,T2>( char*, TJetEnvironment<T1,T2>* ) ",
          "Phase space has dimension zero." ) );
   }
 _myEnv = pje;

 for( i = 0; i < pje->_spaceDim; i++ ) 
  _comp[i].setVariable( i, pje );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TLieOperator<T1,T2>::~TLieOperator<T1,T2>() 
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TLieOperator<T1,T2>::setVariable( const TJet<T1,T2>& x, short int j ) 
{
 if( _myEnv != x.Env() ) {
   throw( GenericException(__FILE__, __LINE__, 
          "void TLieOperator<T1,T2>::setVariable( const TJet<T1,T2>&, int ) ",
          "Inconsistent environments." ) );
 }
 if( j < 0 || _myEnv->_spaceDim <= j ) {
   ostringstream uic;
   uic  << "Argument j = " << j
        << ": it should be within [ 0, "
        << _myEnv->_spaceDim
        << " ].";
   throw( GenericException( __FILE__, __LINE__, 
          "void TLieOperator<T1,T2>::setVariable( const TJet<T1,T2>& x, int j ) ",
          uic.str().c_str() ) );
 }
 
 _comp[j] = x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TLieOperator<T1,T2>::setVariable( const T1& x, short int j ) 
{

 if( j < 0 || _dim <= j ) {
   ostringstream uic;
   uic  << "Argument j = " << j
        << ": it should be within [ 0, "
        << _dim
        << " ].";
   throw( GenericException( __FILE__, __LINE__, 
          "void TLieOperator<T1,T2>::setVariable( const T1& x, const int& j )",
          uic.str().c_str() ) );
 }
 
 _myEnv->_refPoint[j] = x;  // WARNING: The environment is altered!
 _comp[j].Reconstruct( _myEnv );
 
 int n = _myEnv->_numVar;
 
 static IntArray ndx;
 ndx.Reconstruct( n );

 _comp[j].addTerm( new TJLterm<T1,T2>( ndx, x, _myEnv ) );
 ndx(j) = 1;
 _comp[j].addTerm( new TJLterm<T1,T2>( ndx, ((T1) 1.0), _myEnv ) );

 for( int i = 0; i < _dim; i++ ) _comp[i].setEnvTo( _myEnv );
}


//     Operators   |||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
istream& operator>>(istream& is,  TLieOperator<T1,T2>& x) 
{
 int i;
 int ival;
  char buf[100];
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  for( i = 0; i < x.Env()->_numVar; i++ ) {
    is >> buf;
    is >> buf;
    is >> buf;
    is >> buf;
    is >> buf;
    is >> ival;
    is >> x._comp[i];
 }
 return  is;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
ostream& operator<<(ostream& os,  TLieOperator<T1,T2>& x) 
{
 int i;
 os << "\n************ Begin TLieOperator<T1,T2>  printCoeffs ********\n";
 os << "Weight: " << x.Weight() << endl;
 for( i = 0; i < x.Env()->_numVar; i++ ) {
   os << "\n******************\n**** Component index = " << i << endl;
   os << x._comp[i];
   }
 return  os << "\n************ End   TLieOperator<T1,T2>  printCoeffs ********\n";
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TLieOperator<T1,T2>::operator^( const TJet<T1,T2>& x ) const 
{ 

 if( _myEnv != x.Env() ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TJet<T1,T2> TLieOperator<T1,T2>::operator^( const TJet<T1,T2>& ) const ",
          "Inconsistent environments." ) );
 }

 static TJetEnvironment<T1,T2>* pje;
 static TJet<T1,T2> answer;
 static int  s;
 static int  i;
 static IntArray ndx;
 static char adjustWeight;

 pje = _myEnv;
 answer.Reconstruct( pje );
 s = pje->_spaceDim;
 ndx.Reconstruct( pje->_numVar );

 i = 0;

 ndx(0) = 1; 
 answer = _comp[0]*x.D( ndx );
 ndx(0) = 0;

 if( s > 1 ) for( i = 1; i < s; i++ ) {
  ndx(i) = 1;
  answer += _comp[i]*x.D( ndx );
  ndx(i) = 0;
 }

 return answer;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TLieOperator<T1,T2> operator^( const TLieOperator<T1,T2>& x, const TLieOperator<T1,T2>& y )
{
 // ??? const has to be systematically fixed throughout MXYZPTLK

 if( x._myEnv != y._myEnv ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TLieOperator<T1,T2> operator^( const TLieOperator<T1,T2>& , const TLieOperator<T1,T2>& y )",
          "Arguments have different environments." ) );
 }

 TLieOperator<T1,T2> z( x._myEnv );
 int i;
 for( i = 0; i < x._dim; i++ ) 
  z.SetComponent( i, ( x^(y(i)) ) - ( y^(x(i)) ) );
 return z;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TLieOperator<T1,T2>::expMap( const T1& t, const TJet<T1,T2>& x ) 
{ 
 TJet<T1,T2> answer;
 TJet<T1,T2> u;
 double f;
 int count;
 
 answer.DeepCopy( x );
 u     .DeepCopy( x );
 f      = 1.0;
 count  = 0;

 u = ( t/f++ )*( operator^( u ) );
 while( ( count++ < MX_MAXITER ) && ( u != ((T1) 0.0) ) ) {
  answer += u;                       // ??? How does this
  u = ( t/f++ )*( operator^( u ) );  // ??? affect efficiency?
 }

 if( count >= MX_MAXITER ) {
  cerr << "\n" 
       << "*** WARNING ***                                         \n" 
       << "*** WARNING *** TJet<T1,T2> TLieOperator<T1,T2>::expMap()               \n" 
       << "*** WARNING *** Number of iterations has exceeded " << MX_MAXITER << "\n"
       << "*** WARNING *** without achieving convergence.          \n" 
       << "*** WARNING *** Results may be incorrect.               \n" 
       << "*** WARNING ***                                         \n" 
       << endl;
 }

 return answer;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJet<T1,T2> TLieOperator<T1,T2>::expMap( const TJet<T1,T2>& t, const TJet<T1,T2>& x ) 
{ 
 TJet<T1,T2> answer;
 TJet<T1,T2> u;
 double f;
 int count;
 
 answer.DeepCopy( x );
 u     .DeepCopy( x );
 f      = 1.0;
 count  = 0;

 u = ( t/f++ )*( operator^( u ) );
 while( ( count++ < MX_MAXITER ) && ( u != ((T1) 0.0) ) ) {
  answer += u;                       // ??? How does this
  u = ( t/f++ )*( operator^( u ) );  // ??? affect efficiency?
 }

 if( count >= MX_MAXITER ) {
  cerr << "\n" 
       << "*** WARNING ***                                         \n" 
       << "*** WARNING *** TJet<T1,T2> TLieOperator<T1,T2>::expMap()               \n" 
       << "*** WARNING *** Number of iterations has exceeded " << MX_MAXITER  << "\n"
       << "*** WARNING *** without achieving convergence.          \n" 
       << "*** WARNING *** Results may be incorrect.               \n" 
       << "*** WARNING ***                                         \n" 
       << endl;
 }

 return answer;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJetVector<T1,T2> TLieOperator<T1,T2>::expMap( const T1& t , const TJetVector<T1,T2>& x ) 
{
 TJetVector<T1,T2> z;
 
 int i; // O.K.
 
 for(  i = 0; i < _dim; i++ ) 
  z(i) = expMap( t, x(i) );

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJetVector<T1,T2> TLieOperator<T1,T2>::expMap( const TJet<T1,T2>& t, const TJetVector<T1,T2>& x ) 
{
 TJetVector<T1,T2> z;
 int i; // O.K.
 
 for(  i = 0; i < _dim; i++ ) 
  z(i) = expMap( t, x(i) );

 return z;
}

template TLieOperator<double,FNAL::Complex>;
template TLieOperator<FNAL::Complex,double>;
