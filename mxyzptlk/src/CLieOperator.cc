/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                ****
******  MXYZPTLK:  A C++ implementation of differential algebra.      ****
******                                                                ****
******  Copyright (c) 1990  Universities Research Association, Inc.   ****
******                All Rights Reserved                             ****
******                                                                ****
******  Author:    Leo Michelotti                                     ****
******                                                                ****
******             Fermilab                                           ****
******             P.O.Box 500                                        ****
******             Mail Stop 345                                      ****
******             Batavia, IL   60510                                ****
******                                                                ****
******             Phone: (708) 840 4956                              ****
******             Email: michelot@hazel.fnal.gov                     ****
******                    michelotti@adcalc.fnal.gov                  ****
******                    almond::michelotti                          ****
******                                                                ****
******  Release    Version 1.0 : January   31, 1990                   ****
******  Dates:              .1 : October   11, 1990                   ****
******                      .2 : November   6, 1990                   ****
******                      .3 : October    2, 1992                   ****
******                      .4 : July      20, 1993                   ****
******                     2.0   August    12, 1993                   ****
******                     3.0   May       19, 1994                   ****
******                      .1   September 14, 1994                   ****
******                                                                ****
**************************************************************************
**************************************************************************
******                                                                ****
******  This material resulted from work developed under a            ****
******  Government Contract and is subject to the following           ****
******  license: The Government retains a paid-up, nonexclusive,      ****
******  irrevocable worldwide license to reproduce, prepare           ****
******  derivative works, perform publicly and display publicly       ****
******  by or for the Government, including the right to distribute   ****
******  to other Government contractors.  Neither the United          ****
******  States nor the United States Department of Energy, nor        ****
******  any of their employees, makes any warranty, express or        ****
******  implied, or assumes any legal liability or responsibility     ****
******  for the accuracy, completeness or usefulness of any           ****
******  information, apparatus, product, or process disclosed, or     ****
******  represents that its use would not infringe privately owned    ****
******  rights.                                                       ****
******                                                                ****
**************************************************************************
**************************************************************************
**************************************************************************
*************************************************************************/

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
   cerr << "\n*** ERROR *** CLieOperator::CLieOperator called, \n"
        <<   "*** ERROR *** for a zero-dimensional phase space.\n"
        << endl;
   exit(0);
 }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CLieOperator::CLieOperator( CLieOperator& x ) 
: JetCVector( x.myEnv->SpaceDim, 0, x.myEnv )
{
 int i;
 
 myEnv = x.myEnv;

 for ( i = 0; i < myEnv->SpaceDim; i++ ) {
   if( --((comp[i].jl)->rc) == 0 ) delete comp[i].jl;
   comp[i].jl = x.comp[i].jl;    // Perform shallow copy.
   ( comp[i]->rc )++;         // Increase reference counter.
 }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CLieOperator::CLieOperator( const JetC& x ) 
: JetCVector( x->myEnv->SpaceDim, 0, x->myEnv )
{ 
 static int i;
 static JetC__environment* pje;

 i = 0;
 pje = x->myEnv;

 int s = pje->SpaceDim;
 int n = pje->NumVar;
 int* ndx = new int [n];

 if( s == 0 ) {
  cerr << "\n*** ERROR: CLieOperator::CLieOperator( JetC ) called "
       << "with zero-dimensional phase space."
       << endl;
  exit(0);
 }
 
 if( 2*(s/2) != s ) {
  cerr << "*** ERROR ***                                           \n"
          "*** ERROR *** CLieOperator::CLieOperator( const JetC& )    \n"
          "*** ERROR *** Phase space has odd dimension = " << s
                                                           << "    \n"
          "*** ERROR ***                                           \n"
       << endl;
  exit(0);
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
   comp[i]->myEnv = pje;
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
   cerr << "\n*** ERROR: CLieOperator::CLieOperator called " << endl;
   cerr << "but phase space has dimension 0.\n" << endl;
   exit(0);
   }
 
 if( !pje ) {
   cerr << "\n*** CLieOperator::CLieOperator( char* ) ERROR *** " << endl;
   cerr << "No environment defined." << endl;
   exit(0);
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
 int i;
 
 if( myEnv != x->myEnv ) {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** CLieOperator::setVariable( JetC, int )     \n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** Inconsistent environments.               \n"
         << "*** ERROR ***                                          \n"
         << endl;
    exit(1);
 }

 if( j < 0 || myEnv->SpaceDim <= j ) {
  cerr << "\n***ERROR ***                                            \n" << endl;
  cerr << "***ERROR *** CLieOperator::setVariable( JetC&, int )        \n" << endl;
  cerr << "***ERROR *** It is certainly not true                     \n" << endl;
  cerr << "***ERROR *** that 0 <= " << j << " < " 
                                    << myEnv->SpaceDim << "            \n" << endl;
  cerr << "***ERROR ***                                              \n" << endl;
  cerr << "***ERROR *** Sorry.  Good bye.                            \n" << endl;
  cerr << "***ERROR ***                                              \n" << endl;
  exit(0);
 }
 
 comp[j] = x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CLieOperator::setVariable( const Complex& x, const int& j ) {

 if( j < 0 || dim <= j ) {
  printf( "\n***ERROR ***                                              \n" );
  printf(   "***ERROR *** CLieOperator::setVariable( Complex, int )      \n" );
  printf(   "***ERROR *** It is certainly not true                     \n" );
  printf(   "***ERROR *** that 0 <= %d < %d                            \n",
            j, dim );
  printf(   "***ERROR ***                                              \n" );
  printf(   "***ERROR *** Sorry.  Good bye.                            \n" );
  printf(   "***ERROR ***                                              \n" );
  exit(0);
 }
 
 myEnv->refPoint[j] = x;  // WARNING: The environment is altered!
 comp[j].Reconstruct( myEnv );
 
 int n = myEnv->NumVar;
 
 static IntArray ndx;
 ndx.Reconstruct( n );

 comp[j]->insert( new JLCterm( ndx, x, myEnv ) );
 ndx(j) = 1;
 comp[j]->append( new JLCterm( ndx, 1.0, myEnv ) );

 for( int i = 0; i < dim; i++ ) comp[i]->myEnv = myEnv;
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


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC CLieOperator::operator^( JetC& x ) { 

 if( myEnv != x->myEnv ) {
    cerr << "\n\n"
         << "*** ERROR ***                                   \n"
         << "*** ERROR *** CLieOperator::operator^( JetC )   \n"
         << "*** ERROR ***                                   \n"
         << "*** ERROR *** Inconsistent environments.        \n"
         << "*** ERROR ***                                   \n"
         << endl;
    exit(1);
 }

 static JetC__environment* pje;
 static JetC answer;
 static int  s;
 static int aw, i, jw;
 static IntArray ndx;
 static char adjustWeight;

 pje = myEnv;
 answer.Reconstruct( pje );
 aw = pje->MaxWeight;
 s = pje->SpaceDim;
 ndx.Reconstruct( pje->NumVar );

 aw = myEnv->MaxWeight;
 i = jw = 0;

      // Differentiation will normally lower 
      //  the maximum accurate order by one.  This is 
      //  offset if the constant term in comp[] vanishes.
      //
      adjustWeight = 1;
      for( i = 0; i < s; i++ ) {
        if( comp[i]->standardPart() == 0.0 ) {
         jw = comp[i]->accuWgt;
         if( jw > x->accuWgt ) jw = x->accuWgt;
         if( jw < aw ) aw = jw;
        }
        else adjustWeight = 0;
      }
      if( adjustWeight ) ( x->accuWgt )++;  // Not true, of course,
                                            // but it fools the algorithm.
 ndx(0) = 1;
 answer = comp[0]*x.D( ndx );
 ndx(0) = 0;

 if( s > 1 ) for( i = 1; i < s; i++ ) {
  ndx(i) = 1;
  answer += comp[i]*x.D( ndx );
  ndx(i) = 0;

 }

 // The accuracy of the answer is adjusted only if
 //  all components of comp[] are differentials.
 // 
 if( adjustWeight ) {
   ( x->accuWgt )--;
   answer->accuWgt = aw;   // ??? Actually, this should be automatic.
 }
 answer.stacked = 1;
 return answer;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CLieOperator operator^( /* const */ CLieOperator& x, 
                       /* const */ CLieOperator& y ) 
{
 if( x.myEnv != y.myEnv ) {
   cerr << "\n\n"
        << "*** ERROR ***                                    \n"
        << "*** ERROR *** CLieOperator operator^( CLieOp CLieOp ) \n"
        << "*** ERROR ***                                    \n"
        << "*** ERROR *** Arguments have different           \n"
        << "*** ERROR *** environments.                      \n"
        << "*** ERROR ***                                    \n"
        << endl;
   exit(1);
 }

 // ??? const has to be systematically fixed throughout MXYZPTLK
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

 answer.stacked = 1;
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

 answer.stacked = 1;
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

