/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      CLieOperator.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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
#include "CLieOperator.h"

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

#ifdef OBJECT_DEBUG
int CLieOperator::objectCount = 0;
#endif

using namespace std;
using FNAL::Complex;

//***************************************************************
//***************************************************************
//***************************************************************
//
//    Implementation of class CLieOperator
//

//    Constructors and destructors    |||||||||||||||||||||||||||

CLieOperator::CLieOperator( JetC__environment* theEnv ) 
: JetCVector( theEnv->SpaceDim, 0, theEnv )
{
 
 myEnv = theEnv;
 
 if( theEnv->SpaceDim == 0 ) {
   throw( JLC::GenericException(__FILE__, __LINE__, 
          "CLieOperator::CLieOperator( JetC__environment* ) ",
          "Phase space has dimension zero." ) );
 }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CLieOperator::CLieOperator( const CLieOperator& x ) 
: JetCVector( x.myEnv->SpaceDim, 0, x.myEnv )
{
 int i;
 
 myEnv = x.myEnv;

 // ??? REMOVE: for ( i = 0; i < myEnv->SpaceDim; i++ ) {
 // ??? REMOVE:   if( --((comp[i].jl)->rc) == 0 ) delete comp[i].jl;
 // ??? REMOVE:   comp[i].jl = x.comp[i].jl;    // Perform shallow copy.
 // ??? REMOVE:   ( comp[i]->rc )++;            // Increase reference counter.
 // ??? REMOVE: }

 for ( i = 0; i < myEnv->SpaceDim; i++ ) {
   comp[i] = x.comp[i];
 }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CLieOperator::CLieOperator( const JetC& x ) 
: JetCVector( (x.Env())->SpaceDim, 0, x.Env() )
{ 
 static int i;
 static JetC__environment* pje;

 i = 0;
 pje = x.Env();

 int s = pje->SpaceDim;
 int n = pje->NumVar;
 int* ndx = new int [n];

 if( s == 0 ) {
   throw( JLC::GenericException(__FILE__, __LINE__, 
          "CLieOperator::CLieOperator( const JetC& ) ",
          "Phase space has dimension zero." ) );
 }
 
 if( 2*(s/2) != s ) {
   ostringstream uic;
   uic  << "Phase space has odd dimension = " << s;
   throw( JLC::GenericException( __FILE__, __LINE__, 
          "CLieOperator::CLieOperator( const JetC& ) ",
          uic.str().c_str() ) );
 }

 for( i = 0; i < n; i++ ) ndx[i] = 0;

 for( i = 0; i < s/2; i++ ) {
  ndx[i + s/2] = 1;
  comp[i] = x.D( ndx );
  ndx[i + s/2] = 0;
 }

 for( i = s/2; i < s; i++ ) {
  ndx[i - s/2] = 1;
  comp[i] = - x.D( ndx );
  ndx[i - s/2] = 0;
 }

 for( i = s; i < n; i++ ) {
   comp[i].setEnvTo( pje );
   comp[i] = complex_0;
 }

 delete [] ndx;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CLieOperator::CLieOperator( char*, JetC__environment* pje  ) 
: JetCVector( pje->SpaceDim, 0, pje )
{ 
 int i;
 
 if( pje->SpaceDim == 0 ) {
   throw( JLC::GenericException(__FILE__, __LINE__, 
          "CLieOperator::CLieOperator( char*, JetC__environment* ) ",
          "Phase space has dimension zero." ) );
   }
 
 if( !pje ) {
   throw( JLC::GenericException(__FILE__, __LINE__, 
          "CLieOperator::CLieOperator( char*, JetC__environment* ) ",
          "Jet__environment pointer is null." ) );
   }
 
 myEnv = pje;

 for( i = 0; i < pje->SpaceDim; i++ ) 
  comp[i].setVariable( i, pje );


#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CLieOperator::~CLieOperator() {
#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CLieOperator::setVariable( const JetC& x, int j ) {

 
 if( myEnv != x.Env() ) {
   throw( JLC::GenericException(__FILE__, __LINE__, 
          "void CLieOperator::setVariable( const JetC&, int ) ",
          "Inconsistent environments." ) );
 }

 if( j < 0 || myEnv->SpaceDim <= j ) {
   ostringstream uic;
   uic  << "Argument j = " << j
        << ": it should be within [ 0, "
        << myEnv->SpaceDim
        << " ].";
   throw( JLC::GenericException( __FILE__, __LINE__, 
          "void CLieOperator::setVariable( const JetC& x, int j ) ",
          uic.str().c_str() ) );
 }
 
 comp[j] = x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CLieOperator::setVariable( const Complex& x, const int& j ) {

 if( j < 0 || dim <= j ) {
   ostringstream uic;
   uic  << "Argument j = " << j
        << ": it should be within [ 0, "
        << dim
        << " ].";
   throw( JLC::GenericException( __FILE__, __LINE__, 
          "void CLieOperator::setVariable( const Complex& x, const int& j )",
          uic.str().c_str() ) );
 }
 
 myEnv->refPoint[j] = x;  // WARNING: The environment is altered!
 comp[j].Reconstruct( myEnv );
 
 int n = myEnv->NumVar;
 
 static IntArray ndx;
 ndx.Reconstruct( n );

 comp[j].addTerm( new JLCterm( ndx, x, myEnv ) );
 ndx(j) = 1;
 comp[j].addTerm( new JLCterm( ndx, complex_1, myEnv ) );

 for( int i = 0; i < dim; i++ ) comp[i].setEnvTo( myEnv );
}


//     Operators   |||||||||||||||||||||||||||||||||||||||||||||

istream& operator>>(istream& is,  CLieOperator& x) {
 int i;
 int ival;
  char buf[100];
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  for( i = 0; i < x.Env()->NumVar; i++ ) {
    is >> buf;
    is >> buf;
    is >> buf;
    is >> buf;
    is >> buf;
    is >> ival;
    is >> x.comp[i];
 }
 return  is;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<(ostream& os,  CLieOperator& x) {
 int i;
 os << "\n************ Begin CLieOperator  printCoeffs ********\n";
 os << "Weight: " << x.Weight() << endl;
 for( i = 0; i < x.Env()->NumVar; i++ ) {
   os << "\n******************\n**** Component index = " << i << endl;
   os << x.comp[i];
   }
 return  os << "\n************ End   CLieOperator  printCoeffs ********\n";
}


// ??? REMOVE: // |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ??? REMOVE: 
// ??? REMOVE: JetC CLieOperator::operator^( const JetC& x_arg ) const 
// ??? REMOVE: { 
// ??? REMOVE: 
// ??? REMOVE:  if( myEnv != x.Env() ) {
// ??? REMOVE:     cerr << "\n\n"
// ??? REMOVE:          << "*** ERROR ***                                   \n"
// ??? REMOVE:          << "*** ERROR *** CLieOperator::operator^( JetC )   \n"
// ??? REMOVE:          << "*** ERROR ***                                   \n"
// ??? REMOVE:          << "*** ERROR *** Inconsistent environments.        \n"
// ??? REMOVE:          << "*** ERROR ***                                   \n"
// ??? REMOVE:          << endl;
// ??? REMOVE:     exit(1);
// ??? REMOVE:  }
// ??? REMOVE: 
// ??? REMOVE:  static JetC__environment* pje;
// ??? REMOVE:  static JetC answer;
// ??? REMOVE:  static JetC x;
// ??? REMOVE:  static int  s;
// ??? REMOVE:  static int aw, i, jw;
// ??? REMOVE:  static IntArray ndx;
// ??? REMOVE:  static char adjustWeight;
// ??? REMOVE: 
// ??? REMOVE:  x = x_arg;
// ??? REMOVE: 
// ??? REMOVE:  pje = myEnv;
// ??? REMOVE:  answer.Reconstruct( pje );
// ??? REMOVE:  aw = pje->MaxWeight;
// ??? REMOVE:  s = pje->SpaceDim;
// ??? REMOVE:  ndx.Reconstruct( pje->NumVar );
// ??? REMOVE: 
// ??? REMOVE:  aw = myEnv->MaxWeight;
// ??? REMOVE:  i = jw = 0;
// ??? REMOVE: 
// ??? REMOVE:       // Differentiation will normally lower 
// ??? REMOVE:       //  the maximum accurate order by one.  This is 
// ??? REMOVE:       //  offset if the constant term in comp[] vanishes.
// ??? REMOVE:       //
// ??? REMOVE:       adjustWeight = 1;
// ??? REMOVE:       for( i = 0; i < s; i++ ) {
// ??? REMOVE:         if( comp[i].standardPart() == 0.0 ) {
// ??? REMOVE:          jw = comp[i].getAccuWgt();
// ??? REMOVE:          if( jw > x.getAccuWgt() ) jw = x.getAccuWgt();
// ??? REMOVE:          if( jw < aw ) aw = jw;
// ??? REMOVE:         }
// ??? REMOVE:         else adjustWeight = 0;
// ??? REMOVE:       }
// ??? REMOVE:       if( adjustWeight ) ( x->accuWgt )++;  // Not true, of course,
// ??? REMOVE:                                             // but it fools the algorithm.
// ??? REMOVE:  ndx(0) = 1;                                // It is necessary to do it
// ??? REMOVE:  answer = comp[0]*x.D( ndx );               // this way because of break
// ??? REMOVE:  ndx(0) = 0;                                // statements.
// ??? REMOVE: 
// ??? REMOVE:  if( s > 1 ) for( i = 1; i < s; i++ ) {
// ??? REMOVE:   ndx(i) = 1;
// ??? REMOVE:   answer += comp[i]*x.D( ndx );
// ??? REMOVE:   ndx(i) = 0;
// ??? REMOVE: 
// ??? REMOVE:  }
// ??? REMOVE: 
// ??? REMOVE:  // The accuracy of the answer is adjusted only if
// ??? REMOVE:  //  all components of comp[] are differentials.
// ??? REMOVE:  // 
// ??? REMOVE:  if( adjustWeight ) {
// ??? REMOVE:    ( x->accuWgt )--;
// ??? REMOVE:    answer->accuWgt = aw;   // ??? Actually, this should be automatic.
// ??? REMOVE:  }
// ??? REMOVE:  return answer;
// ??? REMOVE: }
// ??? REMOVE: 
// ??? REMOVE: 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC CLieOperator::operator^( const JetC& x ) const 
{ 

 if( myEnv != x.Env() ) {
   throw( JLC::GenericException(__FILE__, __LINE__, 
          "JetC CLieOperator::operator^( const JetC& ) const ",
          "Inconsistent environments." ) );
 }

 static JetC__environment* pje;
 static JetC answer;
 static int  s;
 static int  i;
 static IntArray ndx;
 static char adjustWeight;

 pje = myEnv;
 answer.Reconstruct( pje );
 s = pje->SpaceDim;
 ndx.Reconstruct( pje->NumVar );

 i = 0;

 ndx(0) = 1; 
 answer = comp[0]*x.D( ndx );
 ndx(0) = 0;

 if( s > 1 ) for( i = 1; i < s; i++ ) {
  ndx(i) = 1;
  answer += comp[i]*x.D( ndx );
  ndx(i) = 0;
 }

 return answer;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CLieOperator operator^( /* const */ CLieOperator& x, /* const */ CLieOperator& y )
{
 // ??? const has to be systematically fixed throughout MXYZPTLK

 if( x.myEnv != y.myEnv ) {
   throw( JLC::GenericException(__FILE__, __LINE__, 
          "CLieOperator operator^( const CLieOperator& , const CLieOperator& y )",
          "Arguments have different environments." ) );
 }

 CLieOperator z( x.myEnv );
 int i;
 for( i = 0; i < x.dim; i++ ) 
  z.SetComponent( i, ( x^(y(i)) ) - ( y^(x(i)) ) );
 return z;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC CLieOperator::expMap( const Complex& t, const JetC& x ) { 
 JetC answer;
 JetC u;
 double f;
 int count;
 
 answer.DeepCopy( x );
 u     .DeepCopy( x );
 f      = 1.0;
 count  = 0;

 u = ( t/f++ )*( operator^( u ) );
 while( ( count++ < MX_MAXITER ) && ( u != complex_0 ) ) {
  answer += u;                       // ??? How does this
  u = ( t/f++ )*( operator^( u ) );  // ??? affect efficiency?
 }

 if( count >= MX_MAXITER ) {
  cerr << "\n" 
       << "*** WARNING ***                                         \n" 
       << "*** WARNING *** JetC CLieOperator::expMap()               \n" 
       << "*** WARNING *** Number of iterations has exceeded " << MX_MAXITER << "\n"
       << "*** WARNING *** without achieving convergence.          \n" 
       << "*** WARNING *** Results may be incorrect.               \n" 
       << "*** WARNING ***                                         \n" 
       << endl;
 }

 return answer;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC CLieOperator::expMap( const JetC& t, const JetC& x ) { 
 JetC answer;
 JetC u;
 double f;
 int count;
 
 answer.DeepCopy( x );
 u     .DeepCopy( x );
 f      = 1.0;
 count  = 0;

 u = ( t/f++ )*( operator^( u ) );
 while( ( count++ < MX_MAXITER ) && ( u != complex_0 ) ) {
  answer += u;                       // ??? How does this
  u = ( t/f++ )*( operator^( u ) );  // ??? affect efficiency?
 }

 if( count >= MX_MAXITER ) {
  cerr << "\n" 
       << "*** WARNING ***                                         \n" 
       << "*** WARNING *** JetC CLieOperator::expMap()               \n" 
       << "*** WARNING *** Number of iterations has exceeded " << MX_MAXITER  << "\n"
       << "*** WARNING *** without achieving convergence.          \n" 
       << "*** WARNING *** Results may be incorrect.               \n" 
       << "*** WARNING ***                                         \n" 
       << endl;
 }

 return answer;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetCVector CLieOperator::expMap( const Complex& t , const JetCVector& x ) {
 JetCVector z;
 
 int i; // O.K.
 
 for(  i = 0; i < dim; i++ ) 
  z(i) = expMap( t, x(i) );

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetCVector CLieOperator::expMap( const JetC& t, const JetCVector& x ) {
 JetCVector z;
 int i; // O.K.
 
 for(  i = 0; i < dim; i++ ) 
  z(i) = expMap( t, x(i) );

 return z;
}

