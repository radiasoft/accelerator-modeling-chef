/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TLieOperator.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc.
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
******  Feb-may 2005   Jean-Francois Ostiguy
******                 ostiguy@fnal.gov 
****** 
****** - new memory management scheme 
******                                                            
******  Sep-Dec 2005  ostiguy@fnal.gov
******  
****** - refactored code to usea single class template parameter
******   instead of two. Mixed mode operations now handled using 
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - implementation details completely moved to TJL   
****** - header files support for both explicit and implicit template 
******   instantiations
******   (default for mxyzptlk = explicit)
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES
******
******  Dec 2006 ostiguy@fnal
******
******  - New TJetVector base class implementation. See  TJetVector.h for details. 
****** 
******                                                        
**************************************************************************
*************************************************************************/
#ifndef TLIEOPERATOR_TCC
#define TLIEOPERATOR_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/utils.h>
#include <basic_toolkit/GenericException.h>

#ifndef MX_SMALL
#define MX_SMALL    1.0e-12 // Used by Jet::addTerm to decide 
                            //   removal of a JetCterm.
#endif  // MX_SMALL

#ifndef MX_MAXITER
#define MX_MAXITER  100     // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER

using namespace std;

using FNAL::pcerr;
using FNAL::pcout;

// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//    Implementation of class TLieOperator<T>
//

//    Constructors and destructors    |||||||||||||||||||||||||||


template<typename T>
TLieOperator<T>::TLieOperator( EnvPtr<T> const theEnv ) 
: TJetVector<T>( theEnv->spaceDim(), theEnv )
{
 TLieOperator<T>::myEnv_ = theEnv;
 
 if( theEnv->spaceDim() == 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TLieOperator<T>::TLieOperator( TJetEnvironment<T>* ) ",
          "Phase space has dimension zero." ) );
 }
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TLieOperator<T>::TLieOperator( TLieOperator<T> const& x ) 
: TJetVector<T>( x )
{}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TLieOperator<T>::TLieOperator( TJet<T> const& x ) 
: TJetVector<T>( (x.Env())->spaceDim(), x.Env() )
{ 
 int i = 0;
 EnvPtr<T> pje = x.Env();

 int s = pje->spaceDim();
 
 int n = pje->numVar();
 int* ndx = new int [n];

 if( s == 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TLieOperator<T>::TLieOperator( const TJet<T>& ) ",
          "Phase space has dimension zero." ) );
 }
 
 if( 2*(s/2) != s ) {
   ostringstream uic;
   uic  << "Phase space has odd dimension = " << s;
   throw( GenericException( __FILE__, __LINE__, 
          "TLieOperator<T>::TLieOperator( const TJet<T>& ) ",
          uic.str().c_str() ) );
 }

 for( i = 0; i < n; i++ ) ndx[i] = 0;

 for( i = 0; i < s/2; i++ ) {
  ndx[i + s/2] = 1;
  TLieOperator<T>::comp_[i] = x.D( ndx );
  ndx[i + s/2] = 0;
 }

 for( i = s/2; i < s; i++ ) {
  ndx[i - s/2] = 1;
  TLieOperator<T>::comp_[i] = - x.D( ndx );
  ndx[i - s/2] = 0;
 }

 for( i = s; i < n; i++ ) {
  TLieOperator<T>::comp_[i].setEnvTo( pje );
  TLieOperator<T>::comp_[i] = T();
 }

 delete [] ndx;
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TLieOperator<T>::TLieOperator( char*, EnvPtr<T> const pje  ) 
: TJetVector<T>( pje->spaceDim(), pje )
{ 
 int i;
 
 if( !pje ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TLieOperator<T>::TLieOperator( char*, TJetEnvironment<T>* ) ",
          "Jet__environment pointer is null." ) );
   }
 if( pje->spaceDim() == 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TLieOperator<T>::TLieOperator( char*, TJetEnvironment<T>* ) ",
          "Phase space has dimension zero." ) );
   }
 TLieOperator<T>::myEnv_ = pje;

 for( i = 0; i < pje->spaceDim(); i++ ) 
  TLieOperator<T>::comp_[i].setVariable( i, pje );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TLieOperator<T>::~TLieOperator() 
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TLieOperator<T>::setVariable( const TJet<T>& x, int j ) 
{
 if( TLieOperator<T>::myEnv_ != x.Env() ) {
   throw( GenericException(__FILE__, __LINE__, 
          "void TLieOperator<T>::setVariable( const TJet<T>&, int ) ",
          "Inconsistent environments." ) );
 }
 if( j < 0 || TLieOperator<T>::myEnv_->spaceDim() <= j ) {
   ostringstream uic;
   uic  << "Argument j = " << j
        << ": it should be within [ 0, "
        << TLieOperator<T>::myEnv_->spaceDim()
        << " ].";
   throw( GenericException( __FILE__, __LINE__, 
          "void TLieOperator<T>::setVariable( const TJet<T>& x, int j ) ",
          uic.str().c_str() ) );
 }
 
 TLieOperator<T>::comp_[j] = x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0 
template<typename T>
void TLieOperator<T>::setVariable( const T& x, int j ) 
{

 if( (j < 0) || (TLieOperator<T>::comp_->size() <= j ) ) {
   ostringstream uic;
   uic  << "Argument j = " << j
        << ": it should be within [ 0, "
        << TLieOperator<T>::comp_->size()
        << " ].";
   throw( GenericException( __FILE__, __LINE__, 
          "void TLieOperator<T>::setVariable( const T& x, const int& j )",
          uic.str().c_str() ) );
 }
 
 TLieOperator<T>::myEnv_->resetRefPoint(j,x);  // WARNING: The environment is altered!
 TLieOperator<T>::comp_[j].Reconstruct( TLieOperator<T>::myEnv_ );
 
 int n = TLieOperator<T>::myEnv_->numVar();
 
 IntArray ndx;

 // NOTE: TJet<>::operator->() is overloaded and returns the TJL<>* _jl; 
 
 TLieOperator<T>::comp_[j].addTerm( TJLterm<T>( ndx, x, TLieOperator<T>::myEnv_ ) );
 ndx(j) = 1;
 TLieOperator<T>::comp_[j].addTerm( TJLterm<T>( ndx, ((T) 1.0), TLieOperator<T>::myEnv_ ) );

 for( int i = 0; i < TLieOperator<T>::comp_->size(); i++ ) TLieOperator<T>::comp_[i].setEnvTo(TLieOperator<T>::myEnv_ );
}

#endif

//     Operators   |||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
istream& operator>>(istream& is,  TLieOperator<T>& x) 
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
  for( i = 0; i < x.Env()->numVar(); i++ ) {
    is >> buf;
    is >> buf;
    is >> buf;
    is >> buf;
    is >> buf;
    is >> ival;
    is >> x.comp_[i];
 }
 return  is;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
ostream& operator<<(ostream& os,  TLieOperator<T> const& x) 
{
 int i;
 os << "\n************ Begin TLieOperator<T>  printCoeffs ********\n";
 os << "Weight: " << x.Weight() << endl;
 for( i = 0; i < x.Dim();  ++i) {
   os << "\n******************\n**** Component index = " << i << endl;
   os << x.comp_[i];
   }
 return  os << "\n************ End   TLieOperator<T>  printCoeffs ********\n";
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TLieOperator<T>::operator^( TJet<T> const& x ) const 
{ 

 if( TLieOperator<T>::myEnv_ != x.Env() ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TJet<T> TLieOperator<T>::operator^( const TJet<T>& ) const ",
          "Inconsistent environments." ) );
 }

 EnvPtr<T> pje = TLieOperator<T>::myEnv_;
 TJet<T> answer( pje );
 const int s = pje->spaceDim();
 IntArray ndx( pje->numVar() );


 ndx(0) = 1; 
 answer = TLieOperator<T>::comp_[0]*x.D( ndx );
 ndx(0) = 0;

 if( s > 1 ) for( int i=1; i< s; ++i) {
  ndx(i) = 1;
  answer += TLieOperator<T>::comp_[i]*x.D( ndx );
  ndx(i) = 0;
 }

 return answer;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TLieOperator<T> operator^( TLieOperator<T> const& x, TLieOperator<T> const& y )
{

 if( x.myEnv_ != y.myEnv_ ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TLieOperator<T> operator^( const TLieOperator<T>& , const TLieOperator<T>& y )",
          "Arguments have different environments." ) );
 }

 TLieOperator<T> z( x.myEnv_ );

 for( int i = 0; i < x.Dim(); ++i) 
  z.SetComponent( i, ( x^(y(i)) ) - ( y^(x(i)) ) );

 return z;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TLieOperator<T>::expMap( T const& t, TJet<T> const& x ) 
{ 
 double f  = 1.0;
 int count = 0;
 
 TJet<T> u = ( t/f++ )*( (*this)^( x ) );
 TJet<T> answer(x);

 while(   (++count < MX_MAXITER ) && (  u  != T()  ) ) {
  answer += u;                       
  u  = ( t/f++ )* ( (*this)^( u ) );  
 }

 if( count >= MX_MAXITER ) {
  (*pcerr) << "\n" 
       << "*** WARNING ***                                         \n" 
       << "*** WARNING *** TJet<T> TLieOperator<T>::expMap()               \n" 
       << "*** WARNING *** Number of iterations has exceeded " << MX_MAXITER << "\n"
       << "*** WARNING *** without achieving convergence.          \n" 
       << "*** WARNING *** Results may be incorrect.               \n" 
       << "*** WARNING ***                                         \n" 
       << endl;
 }

  
 return answer;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJet<T> TLieOperator<T>::expMap( TJet<T> const& t, TJet<T> const& x ) 
{ 

 double f      = 1.0;
 int    count  = 0;

 TJet<T> u = ( t/f++ )*( (*this)^( x ) );
 TJet<T> answer(x);

 while( ( ++count < MX_MAXITER ) && ( u != T() ) ) {
  answer += u;                       
  u  = ( t/f++ )* ( (*this)^( u ) );  
 }

 if( count >= MX_MAXITER ) {
  (*pcerr) << "\n" 
       << "*** WARNING ***                                         \n" 
       << "*** WARNING *** TJet<T> TLieOperator<T>::expMap()               \n" 
       << "*** WARNING *** Number of iterations has exceeded " << MX_MAXITER  << "\n"
       << "*** WARNING *** without achieving convergence.          \n" 
       << "*** WARNING *** Results may be incorrect.               \n" 
       << "*** WARNING ***                                         \n" 
       << endl;
 }

 return answer;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TLieOperator<T>::expMap( T const& t , TJetVector<T> const& x ) 
{

 TJetVector<T> z( x );

 // Copy constructor used only for formal initialization
 // of attributes.  The initial value of z's components
 // are not relevant.                             - lpjm
 
 for(  int i=0;  i < x.Dim(); ++i) {
  z(i) = expMap( t, x(i) );
 }
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJetVector<T> TLieOperator<T>::expMap( TJet<T> const& t, TJetVector<T> const& x ) 
{
  TJetVector<T> z( x ); // used only for formal initialization of attributes.  
                       // The initial value not relevant.  - lpjm

 for(  int i = 0; i < x.Dim(); ++i) {
  z(i) = expMap( t, x(i) );
 }

 return z;
}

#endif // TLIEOPERATOR_TCC

