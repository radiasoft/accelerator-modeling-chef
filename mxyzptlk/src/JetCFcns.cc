#ifdef __VISUAL_CPP__
#include <iomanip>
using std::setprecision;
#else
#include <stdlib.h>
#include <iomanip.h>
#endif

#include "JetC.h"
#include "JetCVector.h"
#include "MathConstants.h"

#define PREPFORCHANGE(jl)  if(((jl)->rc)>1){--((jl)->rc);(jl) = new JLC(jl);}

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
#define MX_SMALL    1.0e-12 // Used by JLC::addTerm to decide 
                            //   removal of a JLCCterm.
#endif  // MX_SMALL

#ifndef MX_MAXITER
#define MX_MAXITER  100     // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::operator+=( const JetC& y ) {
                     // ??? Come up with a more efficient implementation SOON.
                                       // ??? This should be modified so that
                                       // terms beyond the accurate weight of
                                       // x or y are not computed and carried
                                       // into the answer.
 PREPFORCHANGE(jl)

 JLCterm* p;
 JLCterm* q;
 JLC* xPtr =  jl;
 JLC* yPtr = y.jl;


 // Check for consistency and set reference point of the sum.
 if( xPtr->myEnv != yPtr->myEnv )
 {
   cerr << "\n*** JLC ADDITION ERROR: Inconsistent reference points." << endl;
   exit(0);
 }
 
 // If one of the arguments is void, then return the other ..
 if( xPtr->count < 1 ) { (*this) = y; return; }
 if( yPtr->count < 1 ) return;

 dlist_iterator gy( *(dlist*) yPtr );
 
 // .. otherwise, continue normal operations.
 while((  p = (JLCterm*) gy()  )) {
   q = new JLCterm( p );
   xPtr->addTerm( q );
 }
 
 // Determine the maximum weight computed accurately.
 if( xPtr->accuWgt > yPtr->accuWgt ) xPtr->accuWgt = yPtr->accuWgt;
}

void operator-=( JetC& x, const JetC& y ) {
 x = x - y;
}

void operator*=( JetC& x, const JetC& y ) {
 x = x * y;
}

void operator/=( JetC& x, const JetC& y ) {
 x = x / y;
}

void operator/=( JetC& x, const Complex y ) {
 x = x / y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//     Member functions(public)  |||||||||||||||||||||||||||||||||||||||

void JetC::addTerm( JLCterm* a) {
 PREPFORCHANGE(jl)
 jl->addTerm( a );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


//***************************************************************
//***************************************************************
//***************************************************************
//
//      The overloaded operators for class JetC
//

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const JetC& x, const JetC& y ) {
 return *(x.jl) == *(y.jl);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const JetC& x, const Complex& y ) {
 return *(x.jl) == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const Complex& y, const JetC& x ){
 return *(x.jl) == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const JetC& x, const JetC& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const JetC& x, const Complex& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const Complex& x, const JetC& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::operator+=( const Complex& x ) {   // ??? Untested!!
 if( jl == NULL ) {                           // ??? DANGER: what is the 
   jl = new JLC;                              // ??? reference point?
 }              
 else {
   PREPFORCHANGE(jl)
 }
 jl->operator+=( x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::operator-=( const Complex& x ) {
 operator+=( -x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator+( const JetC& x, const JetC& y ) { 

 // Possibility: constant JetC argument
 if( !(x->myEnv) ) {
   if( x->count == 1 ) {
     return x.standardPart() + y;
   }
   else {
     cerr << "\n\n"
          << "*** ERROR ***                                       \n"
          << "*** ERROR *** JetC operator+( JetC, JetC )             \n"
          << "*** ERROR ***                                       \n"
          << "*** ERROR *** Null environment for the first        \n"
          << "*** ERROR *** JetC argument.                         \n"
          << "*** ERROR ***                                       \n"
          << endl;
   }
 }

 if( !(y->myEnv) ) {
   if( y->count == 1 ) {
     return x + y.standardPart();
   }
   else {
     cerr << "\n\n"
          << "*** ERROR ***                                       \n"
          << "*** ERROR *** JetC operator+( JetC, JetC )             \n"
          << "*** ERROR ***                                       \n"
          << "*** ERROR *** Null environment for the second       \n"
          << "*** ERROR *** JetC argument.                         \n"
          << "*** ERROR ***                                       \n"
          << endl;
   }
 }

                                        // ??? This should be modified so that
 static JetC z;                         // terms beyond the accurate weight of
 static JLCterm* p;                     // x or y are not computed and carried
 static JLCterm* q;                     // into the answer.

 p    = 0;
 q    = 0;

 // Check for consistency and set reference point of the sum.
 if( x->myEnv != y->myEnv ) {
     cerr << "\n\n*** JetC::operator+ ERROR: Inconsistent environments." 
          << endl;
     exit(0);
 }
 
 // If one of the arguments is void, then return the other ..
 if( x->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( y );      // what is returned does not own
   return z;             // the same data as x or y.
 }

 if( y->count < 1 ) {
   z.DeepCopy( x );
   return z;
 }
                                
 dlist_iterator gx( *(dlist*) x.jl );
 dlist_iterator gy( *(dlist*) y.jl );

 // .. otherwise, continue normal operations.
 if( x->count > y->count ) {
   z.DeepCopy( x );
   while((  p = (JLCterm*) gy()  )) 
     z->addTerm( new JLCterm( p ) );
 }
 else {
   z.DeepCopy( y );
   while((  p = (JLCterm*) gx()  )) 
     z->addTerm( new JLCterm( p ) );
 }
 
 // Determine the maximum weight computed accurately.
 if( x->accuWgt < y->accuWgt ) z->accuWgt = x->accuWgt;
 else                          z->accuWgt = y->accuWgt;
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator+( const JetC& x, const Complex& y ) {
 static JetC z;    // !!! "JetC z = x;" does not work.  The copy constructor
 z.DeepCopy( x );  // ??? is called, which makes x and z own the same data!!
 z += y;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator+( const Complex& y, const JetC& x ) {
 JetC z;
 z.DeepCopy( x );
 z += y;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator+( const JetC& x, const double& y ) {
 static JetC z;    // !!! "JetC z = x;" does not work.  The copy constructor
 z.DeepCopy( x );  // ??? is called, which makes x and z own the same data!!
 z += Complex( y, 0.0 );
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator+( const double& y, const JetC& x ) {
 JetC z;
 z.DeepCopy( x );
 z += Complex( y, 0.0 );
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator-( const JetC& x, const double& y ) {
 return operator+( x, -y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator-( const double& y, const JetC& x ) {
 return operator+( y, -x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator-( const JetC& x, const Complex& y ) {
 return x + (-y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator-( const Complex& y, const JetC& x ) {
 return y + (-x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator-( const JetC& y ) {  
 static JetC z;
 static JLCterm* p;

 z.DeepCopy( y ); 

 // If the argument is void, then return it ..
 if( y->count < 1 ) { 
   return z;
 }

 // .. otherwise continue normal operations.
 dlist_iterator getNext( *(dlist*) z.jl );

 while((  p = (JLCterm*) getNext()  )) {
   p->value = - p->value;
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::Negate()
{
 static JLCterm* p;

 // If the argument is void, then return ...
 if( jl->count < 1 ) return;

 // .. otherwise continue normal operations.
 PREPFORCHANGE(jl)
 dlist_iterator getNext( *(dlist*) jl );
 while((  p = (JLCterm*) getNext()  )) p->value = - p->value;

 return;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::Mult( const Complex& x )
{
 static JLCterm* p;

 // If the argument is void, then return ...
 if( jl->count < 1 ) return;

 // .. otherwise continue normal operations.
 PREPFORCHANGE(jl)
 dlist_iterator getNext( *(dlist*) jl );
 while((  p = (JLCterm*) getNext()  )) p->value *= x;

 return;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator-( const JetC& x, const JetC& y ) {  
                                       // ??? This should be modified so that
 static JetC z;                         // terms beyond the accurate weight of
 static JLCterm* p;                     // x or y are not computed and carried
 static JLCterm* q;                     // into the answer.

 p    = 0;
 q    = 0;

 // Check for consistency and set reference point of the difference.
 if( x->myEnv != y->myEnv ) {
     cerr << "\n\n*** JetC::operator- ERROR: Inconsistent environments." 
          << endl;
     exit(0);
 }
 
 // If one of the arguments is void, then return the other ..
 if( x->count < 1 ) {    // This is done in this way so that
   z = -y;               // what is returned does not own
   return z;             // the same data as x or y.
 }

 if( y->count < 1 ) {
   z.DeepCopy( x );
   return z;
 }
                                
 dlist_iterator gx( *(dlist*) x.jl );
 dlist_iterator gy( *(dlist*) y.jl );

 // .. otherwise, continue normal operations.
 if( x->count > y->count ) {
   z.DeepCopy( x );
   while((  p = (JLCterm*) gy()  )) {
     q = new JLCterm( p );
     ( q->value ) = - ( q->value );
     z->addTerm( q );
   }
 }
 else {
   z = - y;
   while((  p = (JLCterm*) gx()  )) 
     z->addTerm( new JLCterm( p ) );
 }
 
 // Determine the maximum weight computed accurately.
 if( x->accuWgt < y->accuWgt ) z->accuWgt = x->accuWgt;
 else                          z->accuWgt = y->accuWgt;
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const JetC& x, const JetC& y ) 
{
 static JetC z;
 static JLCterm* p;
 static JLCterm* q;
 static JLCterm* r;
 static JLC* xPtr;
 static JLC* yPtr;
 static JLC* zPtr;
 static int testWeight, trialWeight;

 // Check for consistency 
 if( x->myEnv != y->myEnv ) {
     cerr << "\n\n*** JetC::operator* ERROR: Inconsistent environments." 
          << endl;
     exit(0);
 }
 else {
   z.Reconstruct( x->myEnv );
 }
 

 // Initializations
 p 	     = 0;
 q 	     = 0;
 r 	     = q;
 xPtr 	     = x.jl;
 yPtr 	     = y.jl;
 zPtr        = z.jl;
 testWeight  = 0;
 trialWeight = 0;

 // If one of the arguments is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x or y.
 }

 if( yPtr->count < 1 ) {
   z.DeepCopy( y );
   return z;
 }
                                

 // Determine the maximum weight computed accurately.
 JLCterm x_1stTerm( xPtr->lowTerm() );
 JLCterm y_1stTerm( yPtr->lowTerm() );

 testWeight = xPtr->accuWgt;
 if( ( y_1stTerm.weight != 0 ) && ( y_1stTerm.value != complex_0 ) ) 
 {
   testWeight += y_1stTerm.weight;
 }

 trialWeight = yPtr->accuWgt;
 if( ( x_1stTerm.weight != 0 ) && ( x_1stTerm.value != complex_0 ) ) 
 {
   trialWeight += x_1stTerm.weight;
 }

 if( testWeight < trialWeight )
 {
   zPtr->accuWgt = testWeight;
 }
 else
 {
   zPtr->accuWgt = trialWeight;
 }
 if( (zPtr->accuWgt) > (zPtr->myEnv->MaxWeight) ) 
 { 
   zPtr->accuWgt = zPtr->myEnv->MaxWeight;
 }


 // .. and continue normal operations.
 dlist_looper gx( *(dlist*) xPtr );
 dlist_looper gy( *(dlist*) yPtr );

 testWeight = zPtr->accuWgt;
 while((  p = (JLCterm*) gy()  )) {
 while((  q = (JLCterm*) gx()  )) {
   if( ( p->weight + q->weight ) > testWeight ) continue;
   r = new JLCterm( (*p)*(*q) ); 
   zPtr->addTerm( r );
 }}
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const JetC& x, const Complex& y ) {  
 static JetC z;
 static JLCterm* p;
 static JLCterm* q;
 static JLC* xPtr;
 static JLC* zPtr;
 static int testWeight;
 static Complex CZero( 0.0, 0.0 );
 
 z.Reconstruct( x->myEnv );

 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;

 testWeight = z->accuWgt = x->accuWgt;

 if( y == CZero ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLCterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLCterm( p );
   q->value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const JetC& x, const int& j ) {  
 static JetC z;
 static JLCterm* p;
 static JLCterm* q;
 static JLC* xPtr;
 static JLC* zPtr;
 static int testWeight;
 static Complex y;  // ????? START HERE
 
 z.Reconstruct( x->myEnv );

 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;

 testWeight = z->accuWgt = x->accuWgt;

 if( ( y = (Complex) j ) == 0.0 ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLCterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLCterm( p );
   q->value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const Complex& y, const JetC& x ) {  
 static JetC z;
 static JLCterm* p;
 static JLCterm* q;
 static JLC* xPtr;
 static JLC* zPtr;
 static int testWeight;
 static Complex CZero( 0.0, 0.0 );
 
 z.Reconstruct( x->myEnv );

 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;
 testWeight = z->accuWgt = x->accuWgt;

 if( y == CZero ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLCterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLCterm( p );
   q->value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const int& j, const JetC& x ) { 
 static JetC z;
 static JLCterm* p;
 static JLCterm* q;
 static JLC* xPtr;
 static JLC* zPtr;
 static int testWeight;
 static Complex y;
 
 z.Reconstruct( x->myEnv );

 y    = 0.0;
 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;

 testWeight = z->accuWgt = x->accuWgt;

 if( ( y = (Complex) j ) == 0.0 ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLCterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLCterm( p );
   q->value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const double& j, const JetC& x ) { 
 static JetC z;
 static JLCterm* p;
 static JLCterm* q;
 static JLC* xPtr;
 static JLC* zPtr;
 static int testWeight;
 static Complex y;
 
 z.Reconstruct( x->myEnv );

 y    = 0.0;
 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;

 testWeight = z->accuWgt = x->accuWgt;

 if( ( y = (Complex) j ) == 0.0 ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLCterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLCterm( p );
   q->value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator*( const JetC& x, const double& j ) { 
  return j*x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator/( const JetC& x, const Complex& y ) { 
 static JetC z;
 static JLCterm* p;
 static JLCterm* q;
 static JLC* xPtr;
 static JLC* zPtr;
 static int testWeight;
 static Complex CZero( 0.0, 0.0 );
 
 z.Reconstruct( x->myEnv ); 

 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;

 testWeight = z->accuWgt = x->accuWgt;

 // Check for division by zero ..
 if( y == CZero ) {
   cerr << "\n*** JetC::operator/ ERROR *** "
        << "Attempt to divide by a scalar zero.\n"
        << endl;
   exit(0);
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLCterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLCterm( p );
   q->value /= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator/( const JetC& x, const double& y ) { 
 static JetC z;
 static JLCterm* p;
 static JLCterm* q;
 static JLC* xPtr;
 static JLC* zPtr;
 static int testWeight;
 
 z.Reconstruct( x->myEnv ); 

 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;

 testWeight = z->accuWgt = x->accuWgt;

 // Check for division by zero ..
 if( y == 0.0 ) {
   cerr << "\n*** JetC::operator/ ERROR *** "
        << "Attempt to divide by a scalar zero.\n"
        << endl;
   exit(0);
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLCterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLCterm( p );
   q->value /= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator/( const JetC& x, const int& j ) { 
 static JetC z;
 static JLCterm* p;
 static JLCterm* q;
 static JLC* xPtr;
 static JLC* zPtr;
 static int testWeight;
 static Complex y;
 
 z.Reconstruct( x->myEnv ); 

 p    = 0;
 q    = 0;
 y    = 0.0;
 xPtr = x.jl;
 zPtr = z.jl;

 z->myEnv   = x->myEnv;
 testWeight = z->accuWgt = x->accuWgt;

 // Check for division by zero ..
 if( ( y = (Complex) j ) == 0.0 ) {
   cerr << "\n*** JetC::operator/ ERROR *** "
        << "Attempt to divide by a scalar zero.\n"
        << endl;
   exit(0);
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLCterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLCterm( p );
   q->value /= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator/( const Complex& a, const JetC& b ) {
  static JetC u;
  u.Reconstruct( b->myEnv ); 
  u = a;
  return u/b;
} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator/( const double& a, const JetC& b ) {
  static JetC u;
  u.Reconstruct( b->myEnv ); 
  u = Complex( a, 0 );
  return u/b;
} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC JetC::TruncMult( const JetC& v, const int& wl ) const 
{ 
 /*
 Truncated multiplication: used only by the division operator.
 No checks are made on the environments.
 */
 static JetC z;
 static JLCterm* p;
 static JLCterm* q;
 static JLCterm* r;

 // If one of the arguments is void, return it ..
 if ( jl->count < 1 ) return *this;   // ??? Is this form going to cause
 if ( v->count  < 1 ) return v;       // ??? a problem? 

 // Initializations
 z.Reconstruct( jl->myEnv );
 p = 0;
 q = 0;
 r = 0;

 dlist_looper gu( *(dlist*)   jl );
 dlist_looper gv( *(dlist*) v.jl );
 
 // .. otherwise continue normal operations.
 while((  p = (JLCterm*) gv()  )) {
 while((  q = (JLCterm*) gu()  )) {
   if( ( p->weight + q->weight ) > wl ) continue;
   r = new JLCterm( jl->myEnv );
   *r = (*p)*(*q);
   z.addTerm( r );
 }}
 
 // Determine the maximum weight computed accurately.
 if( jl->accuWgt < v->accuWgt ) z->accuWgt = jl->accuWgt;
 else                           z->accuWgt = v->accuWgt;
 
 z->myEnv = jl->myEnv;  // ??? Redundant?
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator/( const JetC& wArg, const JetC& uArg ) { 

 // Check for void operators ..
 if ( wArg->count < 1 ) return wArg;
 if ( uArg->count < 1 ) {
   cerr << "\n*** JLC ERROR *** "
        << "Attempt to divide by a void JLC variable.\n"
        << endl;
   exit(0);
 }
 
 // Check for consistency 
 if( wArg->myEnv != uArg->myEnv ) {
     cerr << "\n\n*** JetC::operator/ ERROR: Inconsistent environments." 
          << endl;
     exit(0);
 }

 dlist_looper gu( *(dlist*) uArg.jl );
 dlist_looper gw( *(dlist*) wArg.jl );

 static JetC v;
 static JetC vn;
 static JetC w, u;
 static JLCterm* qu;
 static JLCterm* qw;
 static Complex u0;
 static int i, wgt, wl, mw;
 
 // Initialize local variables and set the environment of the answer.
 // (These steps are not necessary, but they enforce a discipline.)
 v.Reconstruct( wArg->myEnv );
 vn.Reconstruct( wArg->myEnv );
 w.Reconstruct( wArg->myEnv );
 u.Reconstruct( wArg->myEnv );

 qu = 0;
 qw = 0;
 u0 = 0.0;
 wgt = 0;
 i = 0;
 wl = 0;
 mw = wArg->myEnv->MaxWeight;

 // Normalize the denominator
 if( ( qu = (JLCterm*) gu() ) == 0 ) {
   cerr <<  "\n*** JLC ERROR ***  Division algorithm called with uninitialized JLC.\n"
        << endl;
   exit(0);
   }
 if( ( wgt = qu->weight ) != 0 ) {
   cerr <<  "\n*** JLC ERROR ***  Attempt to divide by nilpotent element.\n"
        << endl;
   exit(0);
   }
 if( ( u0 = qu->value ) == 0.0 ) {
   cerr <<  "\n*** JLC ERROR ***  Attempt to divide by zero.\n"
        << endl;
   exit(0);
   }
 u = uArg / u0;
 
 // Recursive algorithm
 qw = (JLCterm*) gw();
 if( qw->weight == 0 ) v = ( qw->value );
 else                  v = complex_0; 
 v->myEnv = wArg->myEnv;  

 wl = 0;
 while ( wl < mw ) {
   wl++;
   vn = u.TruncMult( v, wl );
   vn = vn.filter( wl, wl );
   w = wArg.filter( wl, wl );
   v += ( w - vn );
   }
 
 // Correct normalization of the answer ..
 v /= u0;
 
 // Determine the maximum weight computed accurately.
 if( wArg->accuWgt < uArg->accuWgt ) v->accuWgt = wArg->accuWgt;
 else                                v->accuWgt = uArg->accuWgt;
 
 // .. and return value.
 v->myEnv = wArg->myEnv;  // Rampant paranoia ...
 return v;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC operator^( const JetC& x, const JetC& y ) { 

 // Check for consistency 
 if( x->myEnv != y->myEnv ) {
     cerr << "\n\n*** JetC::operator^ ERROR: Inconsistent environments." 
          << endl;
     exit(1);
 }

 static JetC__environment* theEnv;
 static JetC z;
 static int i;
 static int* m;
 static int* n;

 theEnv = x->myEnv;
 z.Reconstruct( theEnv ); 

 i = 0;
 m = new int [ theEnv->NumVar ];
 n = new int [ theEnv->NumVar ];

 if( theEnv->PBOK ) {
 
   for( i = 0; i < theEnv->NumVar; i++ ) {
     m[i] = 0;
     n[i] = 0;
   }
 
   for( i = 0; i < theEnv->dof; i++ ) {
     m[ i ] = 1;
     n[ i + theEnv->dof ] = 1;
 
     z += ( ( x.D(m) * y.D(n) ) - ( x.D(n) * y.D(m) ) );
 
     m[ i ] = 0;
     n[ i + theEnv->dof ] = 0;
   }
 
   delete [] m;
   delete [] n;
   return z;
 }
 
 cerr << "\n*** ERROR ***                                \n"
      <<   "*** ERROR *** JLC operator^( JLC&, JLC& )       \n"
      <<   "*** ERROR *** Wrong JLCSetup used; cannot     \n"
      <<   "*** ERROR *** perform bracket.               \n"
      <<   "*** ERROR ***                                \n"
      << endl;
 
 if( 2 + 3 == 5 ) exit(1);       // Done to avoid compiler warnings
 delete [] m;                    // about leaving the function without
 delete [] n;                    // returning something.
 return z;                       
}


//***************************************************************
//***************************************************************
//***************************************************************
//
//      Overloaded analytic functions

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC acos( const JetC&  ) {   // ??? Write this damned thing!!!  It's trivial!!
 static JetC z;
 cerr <<  "JetC acos( JetC& x ) to be written.\n" 
      << endl;
 exit(0);
 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC asin( const JetC& x ) { 
 static JetC z;
 static JetC dz;
 static int iter;

 iter = 0;

 z.DeepCopy( x );
 dz = ( sin(z) - x ) / cos(z);

 while( ( dz != complex_0 ) && ( iter++ < MX_MAXITER ) ) {
  z -= dz;
  dz = ( sin(z) - x ) / cos(z);
 }

 if( iter >= MX_MAXITER ) {
  cerr << "*** WARNING ***                             \n";
  cerr << "*** WARNING *** JLC& asin( JLC& x ) {         \n";
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

JetC atan( const JetC& x ) {   // ??? START HERE, LEO!!!
 static JetC z;
 static JetC c, dz;
 static int iter;

 iter = 0;

 z.DeepCopy( x );
 c = cos( z );
 dz = c*( sin(z) - x*c );

 while( ( dz != complex_0 ) && ( iter++ < MX_MAXITER ) ) {
  z -= dz;
  c = cos( z );
  dz = c*( sin(z) - x*c );
 }

 if( iter >= MX_MAXITER ) {
  cerr <<  "*** WARNING ***                             \n" 
       <<  "*** WARNING *** JLC& atan( JLC& x ) {         \n" 
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

JetC cos( const JetC& x ) { 
 
 // ??? REMOVE: JetC epsSin( const JetC& );
 // ??? REMOVE: JetC epsCos( const JetC& );
 // ??? REMOVE: 
 static JetC epsilon;
 static Complex cs, sn;
 dlist_iterator getNext( *(dlist*) x.jl );
 static JLCterm* p;
 
 // Paranoid initializations
 epsilon.Reconstruct( x->myEnv );
 cs = 0.0;
 sn = 0.0;
 p  = 0;

 if( x->count == 0 ) {
   if( epsilon->count != 0  ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** JetC cos( JetC )                    \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Horrible, inexplicable error!!!     \n"
          << "*** ERROR *** epsilon->count = " << epsilon->count 
          <<                                  "                 \n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
   }
   epsilon.addTerm( new JLCterm( x->myEnv->AllZeroes, Complex( 1.0, 0.0 ), x->myEnv ) );
   return epsilon;
 }
 
 p = (JLCterm*) getNext();
 if( ( p -> weight ) == 0 ) {         // x has non-zero standard part
   cs = std::cos( p -> value );
   sn = std::sin( p -> value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   delete p;
   return cs*JetC::epsCos( epsilon ) - sn*JetC::epsSin( epsilon );
   }
 else {                               // x is pure infinitesimal
   return JetC::epsCos( x );
   }
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC cosh( const JetC& x ) { 
 static JetC z;
 z = exp(x);
 z = ( z + ( 1.0 / z ) ) / 2.0;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC exp( const JetC& x ) { 
 
 // ??? REMOVE: JetC epsExp( const JetC& );
 static JetC epsilon;
 static Complex factor;
 dlist_iterator getNext( *(dlist*) x.jl );
 static JLCterm* p;

 // Paranoid initializations
 epsilon.Reconstruct( x->myEnv );
 factor = 0.0;
 p = 0;
 
 if( x->count == 0 ) {
   if( epsilon->count != 0  ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** JetC exp( JetC )                    \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Horrible, inexplicable error!!!     \n"
          << "*** ERROR *** epsilon->count = " << epsilon->count 
          <<                                  "                 \n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
   }
   epsilon.addTerm( new JLCterm( x->myEnv->AllZeroes, Complex( 1.0, 0.0 ), x->myEnv ) );
   return epsilon;
 }
 
 p = (JLCterm*) getNext();
 if( ( p -> weight ) == 0 ) {         // x has non-zero standard part
   factor = std::exp( p -> value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   delete p;
   return factor*JetC::epsExp( epsilon );
   }
 else {                               // x is pure infinitesimal
   return JetC::epsExp( x );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC log ( const JetC& x ) { 
 static JetC             epsilon;
 static JetC             u, w;
 static Complex          std;
 static double           n;
 static JLCterm*         p;
 dlist_iterator  getNext( *(dlist*) x.jl );
 
 // Paranoid initializations
 epsilon.Reconstruct( x->myEnv );
 u.Reconstruct( x->myEnv );
 w.Reconstruct( x->myEnv );
 
 std = 0.0;
 n = 0.0;
 p = 0;
 
 if( x->count == 0 ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** JetC log( JetC )                    \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** x->count == 0 implies a zero argument.\n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
 }
 
 p = (JLCterm*) getNext();
 if( ( ( p -> weight      ) ==  0   ) &&
     ( ( std = p -> value ) !=  0.0 )
   )                                  // x has non-zero standard part
   {
   if( x->count == 1 ) {             // x may have no derivatives
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
   while( u->count > 0 ) {           // ln( 1 + e/s ) = ln( 1 - ( -e/s )) 
     u *= epsilon;                   //               = - sum ( - e/s )^n / n
     w += ( u / ++n );
   }
 
   return ( log(std) - w );
 // -------------------
 
   }
 else                                 // x has zero standard part
   {
   cerr <<  "\n\n*** ERROR *** \n"                                  
        <<      "*** ERROR *** JLC log called with argument whose\n" 
        <<      "*** ERROR *** standard part vanishes.\n"           
        <<      "*** ERROR *** \n\n"                                
        << endl;
   if( 1 + 1 == 2 ) exit(1);
   return x;
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC log10( const JetC& x ) {
 static const Complex logE = 0.4342944819032518276511289;
 return  logE*log(x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC pow( const JetC& x, const double& s ) { 
 // ??? REMOVE: JetC epsPow( const JetC&, const double& );
 static JetC epsilon;
 static Complex factor;
 static Complex std;
 dlist_iterator getNext( *(dlist*) x.jl );
 static JLCterm* p;
 static int u;

 // Paranoid initializations
 epsilon.Reconstruct( x->myEnv );
 factor = std = 0.0;
 p = 0;
 u = 0;
 
 if( x->count == 0 ) {
   if( epsilon->count != 0  ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** JetC pow( JetC, double )            \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Horrible, inexplicable error!!!     \n"
          << "*** ERROR *** epsilon->count = " << epsilon->count 
          <<                                  "                 \n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
   }
   epsilon.addTerm( new JLCterm( x->myEnv->AllZeroes, Complex( 0.0, 0.0 ), x->myEnv ) );
   return epsilon;
 }
 
 p = (JLCterm*) getNext();
 if( ( ( p -> weight      ) ==  0   ) &&
     ( ( std = p -> value ) !=  0.0 )
   )                                  // x has non-zero standard part
   {
   if( x->count == 1 ) {             // x may have no derivatives
     epsilon = pow( std, s );
     return epsilon;
   }
   factor = std::pow( std, s );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 //   pops the standard part off epsilon
   delete p;
   epsilon.scaleBy( 1.0/std );
   epsilon = factor*JetC::epsPow( epsilon, s );
   return epsilon;
   }
 else                                 // x is pure infinitesimal
   {
   u = nearestInteger( s );
   if( s != (Complex) u ) {
     cerr <<  "\n*** ERROR::JLC pow: attempt to use infinitesimal " 
          <<  "as base with non-integer exponent.\n" 
          << endl;
     exit(1);
     }
   epsilon = complex_1;
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

JetC pow( const JetC& x, int n ) {
  static int i;
  static JetC z;

  // Paranoid initializations
  z.Reconstruct( x->myEnv );
  i = 0;

  if( n == 0 ) z = complex_1;
  else if( n > 0 ) {
    z.DeepCopy( x );
    for( i = 2; i <= n; i++ ) z *= x;
  }
  else {
    JetC xr;
    z = 1.0 / x;
    xr = z;
    for( i = -2; i >= n; i-- ) z *= xr;
  }
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC sin( const JetC& x ) { 
 
 // ??? REMOVE: JetC epsSin( const JetC& );
 // ??? REMOVE: JetC epsCos( const JetC& );
 static JetC epsilon;
 static Complex cs, sn;
 dlist_iterator getNext( *(dlist*) x.jl );
 static JLCterm* p;

 // Paranoid initializations
 epsilon.Reconstruct( x->myEnv );
 cs = sn = 0.0;
 p = 0; 
 
 if( x->count == 0 ) {
   if( epsilon->count != 0  ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** JetC sin( JetC )                    \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Horrible, inexplicable error!!!     \n"
          << "*** ERROR *** epsilon->count = " << epsilon->count 
          <<                                  "                 \n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
   }
   epsilon.addTerm( new JLCterm( x->myEnv->AllZeroes, Complex( 0.0, 0.0 ), x->myEnv ) );
   return epsilon;
 }
 
 p = (JLCterm*) getNext();
 if( ( p -> weight ) == 0 ) {         // x has non-zero standard part
   cs = std::cos( p -> value );
   sn = std::sin( p -> value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   delete p;
   return sn*JetC::epsCos( epsilon ) + cs*JetC::epsSin( epsilon );
   }
 else {                               // x is pure infinitesimal
   return JetC::epsSin( x );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC sinh( const JetC& x ) {
 JetC z;
 z = exp(x);
 return ( z - (1.0/z))/2.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC sqrt( const JetC& x ) {
 // ??? REMOVE: JetC epsSqrt( const JetC& );
 static JetC epsilon;
 static Complex factor;
 static Complex std;
 dlist_iterator getNext( *(dlist*) x.jl );
 static JLCterm* p;

 // Paranoid initializations
 epsilon.Reconstruct( x->myEnv );
 factor = std = 0.0;
 p = 0; 
 
 if( x->count == 0 ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** JetC sqrt( JetC )                   \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** x->count == 0 implies a zero argument.\n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
 }
 
 p = (JLCterm*) getNext();
 if( ( ( p -> weight      ) ==  0   ) &&
     ( ( std = p -> value ) !=  0.0 )
   )                                  // x has non-zero standard part
   {
   factor = std::sqrt( std );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   if( epsilon->count == 1 )          // x may have no derivatives
     {
     epsilon = factor;
     return epsilon;
     }
   else                               // normal case
     {
     p = epsilon.get();               // pops the standard part off epsilon
     delete p;
     epsilon.scaleBy( 1.0/std );
     return factor*JetC::epsSqrt( epsilon );
     }
   }
 else                                 // x is pure infinitesimal
   {
   cerr <<  "\n*** ERROR::JLC sqrt: attempt to use infinitesimal argument." 
        << endl;
   if( 1 + 1 == 2 ) exit(1);
   return x;
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC tan( const JetC& x ) { 
 return sin(x) / cos(x) ;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC tanh( const JetC& x ) { 
 return sinh(x) / cosh(x);
}
 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC fabs( const JetC& x ) {
 static Complex u;

 if( x->count == 0 ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** JetC fabs( JetC )                   \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** x->count == 0 implies a zero argument.\n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
 }
 
 if( (u = x.standardPart()) != 0.0 ) 
 {
   if( u > complex_0 ) return x;
   else          return -x;
 }
 else
 {
   cerr << "\n*** ERROR::JetC fabs: attempt to use infinitesimal argument."
        << endl;
   if( 1 + 1 == 2 ) exit(1);   // This weird stuff done to avoid
   return x;                   // compiler warnings.
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC erf( const JetC& z ) 
{
  JetC__environment* pje = z.Env();

  if( ( fabs(imag(z.standardPart())) > 3.9 ) || 
      ( fabs(real(z.standardPart())) > 3.0 ) ) {
    JetC u( pje );
    u = Complex( 0., 1. )*z;
    u = 1.0 - exp(u*u)*w(u);

    return u;
  }

         JetC    series    ( pje );
         JetC    oldseries ( pje );
         JetC    arg       ( pje );
         JetC    term      ( pje );
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
  while( ( series != oldseries ) || counter++ < pje->MaxWeight ) {
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

JetC erfc( const JetC& z ) 
{
  static const Complex one( 1.0, 0.0 );
  return ( one - erf( z ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC w( const JetC& z ) 
{
  static const Complex mi( 0., -1. );
  static double x;
  static double y;
         JetC   answer( z.Env() );
  
  x = real( z.standardPart() );
  y = imag( z.standardPart() );

  if( ( x < 0.0 ) || ( y < 0.0  ) ) {
    cout << "*** ERROR ***                                     \n"
            "*** ERROR *** JetC w( const JetC& )               \n"
            "*** ERROR *** Argument must have positive         \n"
            "*** ERROR *** standard part.                      \n"
            "*** ERROR ***                                     \n"
         << endl;
    exit(1);
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



//***************************************************************
//***************************************************************
//***************************************************************
//
//      Auxiliary functions which operate on infinitesimals.
//      It is assumed that the calling program checks to be sure
//      that the argument has zero standard part.
 
JetC JetC::epsCos( const JetC& epsilon ) { 
 
 static JetC z;
 static JetC epsq, term;
 static double n;

 z->myEnv = epsilon->myEnv;
 z = complex_1;
 
 epsq = - epsilon*epsilon;
 
 term = epsq / 2.0;
 n = 2.0;
 
 z->myEnv = epsilon->myEnv;
 
 while( term->count > 0 ) {
   z += term;
   term = ( ( term*epsq ) / ++n ) / ++n;
   }
 
 z->accuWgt = epsilon->accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC JetC::epsExp( const JetC& epsilon ) { 
 static JetC z;
 static JetC term;
 static double n;
 
 z->myEnv = epsilon->myEnv;
 z = complex_1;
 
 term = epsilon;
 n = 1.0;
 
 z->myEnv = epsilon->myEnv;  // ??? Redundant?
 
 while( term->count > 0 ) {
   z += term;
   term = ( term*epsilon ) / ++n;
 }
 
 z->accuWgt = epsilon->accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC JetC::epsPow( const JetC& epsilon, const double& s ) { 
 static JetC z;
 static JetC term;
 static double f;
 static double n;

 z->myEnv = epsilon->myEnv;
 z = complex_1;
 
 f = s;
 n = 1.0;
 term = s*epsilon;
 
 z->myEnv = epsilon->myEnv;
 
 while( term->count > 0 ) {
   z += term;
   term = ( (--f) * term * epsilon ) / ++n;
   }
 
 z->accuWgt = epsilon->accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC JetC::epsSin( const JetC& epsilon ) { 
 
 static JetC z;
 static JetC epsq, term;
 static double n;
 
 epsq = - epsilon*epsilon;
 z = epsilon;
 term = ( epsilon*epsq ) / 6.0;
 n = 3.0;
 
 while( term->count > 0 ) {
   z += term;
   term = ( ( term*epsq ) / ++n ) / ++n;
 }
 
 z->accuWgt = epsilon->accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC JetC::epsSqrt( const JetC& epsilon ) {  

 // This function is identical to epsPow
 // with the substitution  s = 1/2

 static JetC     z;
 static JetC     term;
 static double   f;
 static double   n;
 
 z->myEnv = epsilon->myEnv;
 z = complex_1;
 
 f    = 1.0 / 2.0;
 n    = 1.0;
 term = f*epsilon;
 
 z->myEnv = epsilon->myEnv;
 
 while( term->count > 0 ) {
   z += term;
   term *= ( (--f) * epsilon ) / ++n;
 }
 
 z->accuWgt = epsilon->accuWgt;

 return z;
}
 

//***************************************************************
//***************************************************************
//***************************************************************
//
//      Implementation of Class JetC

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::peekAt() const {
 dlist_traversor getNext( *(dlist*) jl );
 int i;
 JLCterm* p;
 dlink* q;
 cout << "\nCount  = "
      << jl->count
      << " Weight = "
      << jl->weight
      << " Max accurate weight = "
      << jl->accuWgt
      << endl;
 cout << "Reference point: ";
 for( i = 0; i < jl->myEnv->NumVar; i++ ) 
    cout << setprecision(12) << jl->myEnv->refPoint[i] << "  ";
 cout << endl;
 while((  q = getNext()  )) {
   p = (JLCterm*) q->info();
   cout << "Weight: "
        << p -> weight
        << "   Value: "
        << p -> value
        << " || ";
   cout << "Addresses: "
        << (int) q->prevPtr() << "  "
        << (int) q            << "  "
        << (int) q->nextPtr() 
        << " : "
        << (int) p
        << endl;
   cout << "Index:  ";
   for( i = 0; i < jl->myEnv->NumVar; i++ )
     cout << (p -> index)(i) << "  ";
   cout << "\n" << endl;
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::printCoeffs() const {
 dlist_iterator getNext( *(dlist*) jl );
 int i;
 JLCterm* p;

 cout << "\nCount  = " << jl->count 
      << ", Weight = " << jl->weight 
      << ", Max accurate weight = " << jl->accuWgt << endl;
 cout << "Reference point: " 
      << jl->myEnv->refPoint[0];
 for( i = 1; i < jl->myEnv->NumVar; i++ ) {
   cout << ", ";
   cout << jl->myEnv->refPoint[i];
 }
 cout << endl;

 while((  p = (JLCterm*) getNext()  )) {
   if( p->weight > jl->accuWgt ) break;
   cout << "Index:  " 
        << p->index
        << "   Value: "
        << p->value
        << endl;
 }

 cout << "\n" << endl;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::peekAt( int  ) const {
 dlist_traversor getNext( *(dlist*) jl );
 int i;
 JLCterm* p;
 dlink* q;
 fprintf( JLC::scratchFile,
          "\nCount  = %d, Weight = %d, Max accurate weight = %d\n",
          jl->count, jl->weight, jl->accuWgt );
 fprintf( JLC::scratchFile, "Reference point:  \n" );
 for( i = 0; i < jl->myEnv->NumVar; i++ )
   fprintf( JLC::scratchFile, "( %e, %e )", 
            real( jl->myEnv->refPoint[i] ), 
            imag( jl->myEnv->refPoint[i] ) );
 fprintf( JLC::scratchFile, "\n" );
 while((  q = getNext()  )) {
   p = (JLCterm*) q->info();
   fprintf( JLC::scratchFile, "Weight: %d   Value: %e  %e  || ",
           p -> weight, 
           real( p -> value ), 
           imag( p -> value ));
   fprintf( JLC::scratchFile, "Addresses: %d %d %d : %d\n",
                  q->prevPtr(), q, q->nextPtr(), p );
   fprintf( JLC::scratchFile, "Index:  ");
   for( i = 0; i < jl->myEnv->NumVar; i++ )
     fprintf( JLC::scratchFile, "%d  ", (p -> index)(i) );
   fprintf( JLC::scratchFile, "\n\n");
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::printCoeffs( int  ) const {
 dlist_traversor getNext( *(dlist*) jl );
 int i;
 JLCterm* p;
 dlink* q;
 fprintf( JLC::scratchFile, "\nCount  = %d, Weight = %d, Max accurate weight = %d\n",
          jl->count, jl->weight, jl->accuWgt );
 fprintf( JLC::scratchFile, "Reference point:  " );
 for( i = 0; i < jl->myEnv->NumVar; i++ ) 
   fprintf( JLC::scratchFile, "( %e, %e )", 
            real( jl->myEnv->refPoint[i] ), 
            imag( jl->myEnv->refPoint[i] ) );
 fprintf( JLC::scratchFile, "\n\n" );
 while((  q = getNext()  )) {
   p = (JLCterm*) q->info();
   if( p->weight > jl->accuWgt ) break;
   fprintf( JLC::scratchFile,"Index:  ");
   for( i = 0; i < jl->myEnv->NumVar; i++ )
     fprintf( JLC::scratchFile, "%3d ", (p -> index)(i) );
   fprintf( JLC::scratchFile, "   Value: %14.6e  %14.6e  ", 
            real( p -> value ), 
            imag( p -> value ) );
   fprintf( JLC::scratchFile,"\n");
   }
 fprintf( JLC::scratchFile, "\n\n" );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ??? REMOVE 
// ??? REMOVE dlist JetC::image() {      // ??? Problem: nothing is recorded about the
// ??? REMOVE                           // ??? reference point.
// ??? REMOVE  return jl->image();
// ??? REMOVE }
// ??? REMOVE 
// ??? REMOVE // |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ??? REMOVE 
void JetC::writeToFile( char* fileName ) const {
 jl->writeToFile( fileName );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::writeToFile( FILE* filePtr ) const {
 jl->writeToFile( filePtr );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::scaleBy( Complex y ) {
 PREPFORCHANGE(jl)
 jl->scaleBy( y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLCterm* JetC::get() {
 PREPFORCHANGE(jl)
 return jl->get();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLCterm JetC::firstTerm() const {
 return jl->firstTerm();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLCterm JetC::lowTerm() const {
 return jl->lowTerm();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Complex JetC::standardPart() const {
 return jl->standardPart();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC::clear() {
 PREPFORCHANGE(jl)
 jl->clear();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Complex JetC::weightedDerivative( const int* ind ) const {
 return jl->weightedDerivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Complex JetC::derivative( const int* ind ) const {
 return jl->derivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC JetC::filter( const int& wgtLo, const int& wgtHi ) const { 
 JetC z;
 JLC* zPtr = z.jl;
 JLC* thisPtr = jl;
 dlist_iterator getNext( *(dlist*) thisPtr );
 JLCterm* p;
 JLCterm* q;
 int numTerms;
 int wgt, upperWgt;
 
 z.jl->myEnv = jl->myEnv;

 if( ( wgtLo <= 0 ) && 
     ( wgtHi >= thisPtr->weight )   
   ) {
   z = *this;
   return z;
 } 

 numTerms = 0;
 upperWgt = 0;
 
 while((  p = (JLCterm*) getNext()  )) {
   wgt = p->weight;
   if( ( wgt >= wgtLo ) && ( wgt <= wgtHi ) ) {
     q = new JLCterm( p );
     zPtr->append( q );
     if( wgt > upperWgt ) upperWgt = wgt;
     numTerms++;
     }
   }
 
 zPtr->count = numTerms;   // ??? Is this necessary?
 zPtr->weight = upperWgt;  // ??? Where is the copy of the re
 zPtr->accuWgt = thisPtr->accuWgt;   // ??? reference point???

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC JetC::filter( char (*f) ( const IntArray&, const Complex& ) ) const { 
 static JetC z;
 static JLC* zPtr;
 static JLCterm* p;
 static JLCterm* q;

 z.Reconstruct( jl->myEnv ); 

 zPtr = z.jl;
 p    = 0;
 q    = 0;

 dlist_iterator getNext( *(dlist*) jl );

 static int i;
 static int nv;
 static IntArray oldIndex;

 nv = jl->myEnv->NumVar;
 oldIndex.Reconstruct( nv );

 while((  p = (JLCterm*) getNext()  )) {
  oldIndex = p->index;

  if( (*f)( p->index, p->value ) ) {
   q = new JLCterm( p );
   zPtr->append( q );
  }

  for( i = 0; i < nv; i++ ) 
   if( oldIndex(i) != p->index(i) ) {
    cerr <<  "\n"
         <<  "*** ERROR ***                                      \n" 
         <<  "*** ERROR *** JetC JetC::filter( char (*f) ( int*, Complex ) \n" 
         <<  "*** ERROR *** The test routine is not allowed      \n" 
         <<  "*** ERROR *** to change the values in the index    \n" 
         <<  "*** ERROR *** array!!                              \n" 
         <<  "*** ERROR ***                                      \n" 
         << endl;
    exit(0);
   }
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC JetC::concat() const {
 static JetC v;
 dlist_iterator getNext( *(dlist*) jl );
 static JLCterm* p;
 static int i;
 
 p = 0;
 i = 0;

 v.Reconstruct( (( jl->myEnv->JLCmonomial )[0] )->myEnv );
 
 while((  p = (JLCterm*) getNext()  )) {
   if( p->weight > jl->accuWgt ) break;
   for( i = 0; i < jl->myEnv->NumVar; i++ )
     jl->myEnv->exponent[i] = (p->index)(i);
   jl->myEnv->monoCode();
   v += ( p->value ) * ( jl->myEnv->JLCmonomial[ jl->myEnv->monoRank() ] );
 }
 
 return v;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC 
JetC::operator() ( /* const */ JetCVector& y ) const 
{
 static int i, n;   // ??? This should be rewritten 
 static JetC z;      // ??? so as to be self-contained.

 n = jl->myEnv->NumVar;
 JetC* u = new JetC [ n ];

 for( i = 0; i < n; i++ ) u[i] = y(i);
 z = operator()( u );

 delete [] u;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC JetC::operator() ( const JetC* y ) const { 
 JetC*     u = new JetC [ jl->myEnv->NumVar ];
 static JetC      z, term;
 static int      i, j, w;

 z.Reconstruct( y[0]->myEnv );

 // Initializing term should not be necessary.

 i = j = w = 0;

 // Check consistency of reference points and
 // subtract reference point prior to concatenation.
 for( i = 0; i < jl->myEnv->NumVar; i++ ) {
   if( y[i]->myEnv != y[0]->myEnv ) {
     cerr << "\n\n"
 	  << " *** ERROR ***                                \n"
 	  << " *** ERROR *** JetC::operator()( JetC* )        \n"
 	  << " *** ERROR ***                                \n"
 	  << " *** ERROR *** Inconsistent environments.     \n"
 	  << " *** ERROR ***                                \n"
 	  << endl;
     exit(1);
   }
   u[i] = y[i] - jl->myEnv->refPoint[i];
 }
 
 // Evaluate and store monomials.
 
 // The zeroth one.
 ( ( jl->myEnv->JLCmonomial )[0] )->myEnv = y[0]->myEnv; // Needed by 
                                                        // JetC::concat
 jl->myEnv->JLCmonomial[0] = complex_1;

 // For all higher weights ...
 for( w = 1; w <= jl->accuWgt; w++ )
 
   // Get the next set of exponents of weight w.
   while( nexcom( w, jl->myEnv->NumVar, jl->myEnv->exponent ) ) {
 
     // Find the first non-zero exponent.
     i = 0;
     while( !( jl->myEnv->exponent[i++] ) ) ;
     i--;
 
     // The value of the monomial associated with this composition
     // is obtained by multiplying a factor into a previously
     // computed monomial.
     ( jl->myEnv->exponent[i] )--;
     jl->myEnv->monoCode();
     term = jl->myEnv->JLCmonomial[ jl->myEnv->monoRank() ];
     ( jl->myEnv->exponent[i] )++;
     jl->myEnv->monoCode();
     jl->myEnv->JLCmonomial[ jl->myEnv->monoRank() ] = term * u[i]; // ??? Is this OK???
 
   } // End while loop.
 
 // Monomials have been stored.
 // Now traverse the JLC variable and evaluate.
 // Formerly done by using concat routine, which
 // I copy here
 {
  dlist_iterator getNext( *(dlist*) jl );
  static JLCterm* p;
  static int i;
  
  p = 0;
  i = 0;
 
  z->myEnv = (( jl->myEnv->JLCmonomial )[0] )->myEnv;
  
  while((  p = (JLCterm*) getNext()  )) {
    if( p->weight > jl->accuWgt ) break;
    for( i = 0; i < jl->myEnv->NumVar; i++ )
      jl->myEnv->exponent[i] = (p->index)(i);
    jl->myEnv->monoCode();
    z += ( p->value ) * ( jl->myEnv->JLCmonomial[ jl->myEnv->monoRank() ] );
  }
 }

 
 // Finish...
 delete [] u;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Complex JetC::operator() ( const Complex* x ) const {
 return jl->operator()( x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC JetC::D( const int* n ) const {
 // ??? REMOVE static char noTermAdded;
 static char doit;
 static int f, i, j, k, w;
 static JetC z;
 dlist_iterator getNext( *(dlist*) jl );
 static JLCterm* p;
 static JLCterm* q;
 
 // --- Initializations.
 // ??? REMOVE noTermAdded = 1;
 f = i = j = k = w = 0;

 z.Reconstruct( jl->myEnv );

 p = 0;
 q = 0;
 

 // --- Preliminary check of index set.
 
 w = 0;
 
 for( i = 0; i < jl->myEnv->NumVar; i++ ) {
   if( n[i] < 0 ) {
     cerr <<  "\n*** JLC ERROR: Cannot differentiate with negative index." 
          <<  "\n*******************************************************\n" 
          << endl;
     exit(0);
     }
   w += n[i];
 }
 
 if( w > jl->accuWgt ) {
   cerr <<  "\n*** JLC ERROR: Differentiation request beyond accuracy allowed." 
        <<  "\n**************************************************************\n" 
        << endl;
   exit(0);
 }
 
 if( w == 0 ) {
   z = *this;
   return z;
 }


 // --- Construct the derivative one link at a time.
 // --- ( See note Obs.4 )
 
 while((  p = (JLCterm*) getNext()  )) {
 
   q = new JLCterm( p );
 
   doit = 1;
   // -- Reset the index.
   for( i = 0; i < jl->myEnv->NumVar; i++ ) 
     doit = doit && ( ( q->index(i) -= n[i] ) >= 0 );
 
   if( doit ) {
     // -- Build factorial multiplier.
     f = 1;
     for( k = 0; k < jl->myEnv->NumVar; k++ ) {
       j = q->index(k);
       for( i = 0; i < n[k]; i++ ) f *= ++j;
     }
     if( f <= 0 ) {
       cerr <<  "\n*** JLC ERROR: Horrible, unexplainable error while differentiating!" 
            <<  "\n******************************************************************\n" 
            << endl;
       exit(0);
     }                           // Super fussbudget!! ( f "must" be positive )
   
     // -- Make final changes in private data of the JLCterm and
     //    absorb it into the answer.
     (q->value) *= f;              // OK. So I'm a fussbudget with parentheses again ..
     (q->weight ) -= w;
   
     // ??? REMOVE noTermAdded = 0;
     z->addTerm( q );               // ??? This also may be unnecessarily conservative.
                                    // The append function may be adequate, if the
                                    // ordering is preserved. ???
   } 

   else delete q;                   // ??? Is this correct?
                                    // ??? It wasn't here before!
 }
 
 // ??? REMOVE if( noTermAdded ) z = 0.0;
 
 
 // --- Finally, adjust accuWgt and return value
 
 z->accuWgt = jl->accuWgt - w;       // ??? Is this correct ???

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC JetC::D( const IntArray& n ) const 
{

 if( n.Dim() != jl->myEnv->NumVar ) {
    cerr << "\n\n"
         << "*** ERROR ***                                   \n"
         << "*** ERROR *** JetC::D( IntArray )                \n"
         << "*** ERROR ***                                   \n"
         << "*** ERROR *** Inconsistent dimensions.          \n"
         << "*** ERROR ***                                   \n"
         << endl;
    exit(1);
 }

 // ??? REMOVE static char noTermAdded;
 static char doit;
 static int f, i, j, k, w;
 static JetC z;
 dlist_iterator getNext( *(dlist*) jl );
 static JLCterm* p;
 static JLCterm* q;
 
 // --- Initializations.
 // ??? REMOVE noTermAdded = 1;
 f = i = j = k = w = 0;

 z.Reconstruct( jl->myEnv );

 p = 0;
 q = 0;
 

 // --- Preliminary check of index set.
 
 w = 0;
 
 for( i = 0; i < jl->myEnv->NumVar; i++ ) {
   if( n(i) < 0 ) {
     cerr <<  "\n*** JLC ERROR: Cannot differentiate with negative index." 
          <<  "\n*******************************************************\n" 
          << endl;
     exit(0);
     }
   w += n(i);
 }
 
 if( w > jl->accuWgt ) {
   cerr <<  "\n*** JLC ERROR: Differentiation request beyond accuracy allowed." 
        <<  "\n**************************************************************\n" 
        << endl;
   exit(0);
 }
 
 if( w == 0 ) {
   z = *this;
   return z;
 }


 // --- Construct the derivative one link at a time.
 // --- ( See note Obs.4 )
 
 while((  p = (JLCterm*) getNext()  )) {
 
   q = new JLCterm( p );
 
   doit = 1;
   // -- Reset the index.
   for( i = 0; i < jl->myEnv->NumVar; i++ ) 
     doit = doit && ( ( q->index(i) -= n(i) ) >= 0 );
 
   if( doit ) {
     // -- Build factorial multiplier.
     f = 1;
     for( k = 0; k < jl->myEnv->NumVar; k++ ) {
       j = q->index(k);
       for( i = 0; i < n(k); i++ ) f *= ++j;
       }
     if( f <= 0 ) {
       cerr <<  "\n*** JLC ERROR: Horrible, unexplainable error while differentiating!" 
            <<  "\n******************************************************************\n" 
            << endl;
       exit(0);
       }                           // Super fussbudget!! ( f "must" be positive )
   
     // -- Make final changes in private data of the JLCterm and
     //    absorb it into the answer.
     (q->value) *= f;              // OK. So I'm a fussbudget with parentheses again ..
     (q->weight ) -= w;
   
     // ??? REMOVE noTermAdded = 0;
     z->addTerm( q );               // ??? This also may be unnecessarily conservative.
                                    // The append function may be adequate, if the
                                    // ordering is preserved. ???
   }

   else delete q;                   // ??? Is this correct?
                                    // ??? It wasn't here before.
 }
 
 // ??? REMOVE if( noTermAdded ) z = 0.0;
 
 
 // --- Finally, adjust accuWgt and return value
 
 z->accuWgt = jl->accuWgt - w;       // ??? Is this correct ???

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// POSTPONED JetC JetC::Inverse() const {
// POSTPONED  static JetC      z;
// POSTPONED  static JLCterm*  p;
// POSTPONED  static Complex   zero;
// POSTPONED  static char     t1, t2;
// POSTPONED 
// POSTPONED  // Initializations
// POSTPONED  p = 0;
// POSTPONED  zero = 0.0;
// POSTPONED  t1 = 0;
// POSTPONED  t2 = 0;
// POSTPONED 
// POSTPONED  if( jl->myEnv->SpaceDim != 1 ) {
// POSTPONED   cerr << "\n" 
// POSTPONED           "*** ERROR ***                                     \n" 
// POSTPONED           "*** ERROR ***  JetC JetC::Inverse()                 \n" 
// POSTPONED           "*** ERROR ***  This method can be called only     \n" 
// POSTPONED           "*** ERROR ***  if there is one and only one       \n" 
// POSTPONED           "*** ERROR ***  coordinate.  You have " 
// POSTPONED                                              << jl->myEnv->SpaceDim 
// POSTPONED                                                          << "\n"
// POSTPONED           "*** ERROR ***                                     \n" 
// POSTPONED        << endl;
// POSTPONED   exit(1);
// POSTPONED  }
// POSTPONED 
// POSTPONED  if( ( t1 = ( jl->myEnv->refPoint[0]    == 0.0 ) ) && 
// POSTPONED      ( t2 = ( jl->standardPart()        == 0.0 ) ) 
// POSTPONED    ) return EpsInverse();
// POSTPONED 
// POSTPONED  z = *this;   // Deep copy needed here.
// POSTPONED 
// POSTPONED  if( !t2 ) { 
// POSTPONED   p = z.get();
// POSTPONED   z->addTerm( new JLCterm( z->myEnv->AllZeroes, 0.0, z->myEnv ) );
// POSTPONED  }
// POSTPONED 
// POSTPONED  if( !t1 ) z->myEnv->refPoint[0] = 0.0;
// POSTPONED 
// POSTPONED  z = z.EpsInverse();
// POSTPONED 
// POSTPONED  if( t2 ) z->myEnv->refPoint[0] = 0.0;
// POSTPONED  else     z->myEnv->refPoint[0] = p->value;
// POSTPONED 
// POSTPONED  delete p;    // POSTPONED OK POSTPONED
// POSTPONED 
// POSTPONED  if( !t1 ) {
// POSTPONED   // A little check
// POSTPONED   if( z.standardPart() != 0.0 ) {
// POSTPONED    cerr << "*** WARNING ***                                              \n"
// POSTPONED            "*** WARNING *** JetC::Inverse()                               \n"
// POSTPONED            "*** WARNING *** z.standardPart() = " << z.standardPart() << "\n"
// POSTPONED            "*** WARNING ***                                              \n"
// POSTPONED         << endl;
// POSTPONED   }
// POSTPONED   z->addTerm( new JLCterm( z->myEnv->AllZeroes, 
// POSTPONED                           z->myEnv->refPoint[0] ),
// POSTPONED                           z->myEnv );
// POSTPONED  }
// POSTPONED 
// POSTPONED  return z;
// POSTPONED }
// POSTPONED 
// POSTPONED // |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// POSTPONED // |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// POSTPONED 
// POSTPONED JetC JetC::EpsInverse() const {
// POSTPONED  static JetC        z, x, v, g;
// POSTPONED  static const int  d(1);
// POSTPONED  static int        i;
// POSTPONED  static JetC__environment*  newEnv;
// POSTPONED 
// POSTPONED  newEnv = new JetC__environment( jl->myEnv );
// POSTPONED  JetC::environments.append( newEnv );
// POSTPONED 
// POSTPONED  x.setVariable( 0.0, 0, jl->myEnv );
// POSTPONED  g = D(&d);
// POSTPONED  
// POSTPONED  i = 0;
// POSTPONED  z.DeepCopy( x );
// POSTPONED  v = ( operator()(&z) - x ) / g(&z);
// POSTPONED  while( ( i++ < MX_MAXITER ) &&  ( v != 0.0 ) ) {
// POSTPONED   z -= v;
// POSTPONED   v = ( operator()(&z) - x ) / g(&z);
// POSTPONED  }
// POSTPONED 
// POSTPONED  return z;
// POSTPONED }
