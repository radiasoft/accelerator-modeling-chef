/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      JetFcns.cc
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


#include <iomanip>

#include "mxyzptlk.h"
#include "MathConstants.h"
#include "JetVector.h"

#define PREPFORCHANGE(jl)  if(((jl)->rc)>1){--((jl)->rc);(jl) = new JL(jl);}

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
#define MX_SMALL    1.0e-12 // Used by JL::addTerm to decide 
                            //   removal of a JLterm.
#endif  // MX_SMALL

#ifndef MX_MAXITER
#define MX_MAXITER  100     // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER


using namespace std;


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Jet::operator+=( const Jet& y ) {
                     // ??? Come up with a more efficient implementation SOON.
                                       // ??? This should be modified so that
                                       // terms beyond the accurate weight of
                                       // x or y are not computed and carried
                                       // into the answer.
 PREPFORCHANGE(jl)
                    
 JLterm* p;
 JLterm* q;
 JL* xPtr =   jl;
 JL* yPtr = y.jl;

 
 // Check for consistency and set reference point of the sum.
 if( xPtr->myEnv != yPtr->myEnv )
 {
   cerr << "\n*** JL ADDITION ERROR: Inconsistent reference points." << endl;
   exit(0);
 }
 
 // If one of the arguments is void, then return the other ..
 if( xPtr->count < 1 ) { (*this) = y; return; }
 if( yPtr->count < 1 ) return;

 dlist_iterator gy( *(dlist*) yPtr );

 // .. otherwise, continue normal operations.
 while((  p = (JLterm*) gy()  )) {
   q = new JLterm( p );
   xPtr->addTerm( q );
 }
 
 // Determine the maximum weight computed accurately.
 if( xPtr->accuWgt > yPtr->accuWgt ) xPtr->accuWgt = yPtr->accuWgt;
}

void operator-=( Jet& x, const Jet& y ) {
 x = x - y;
}

void operator*=( Jet& x, const Jet& y ) {
 x = x * y;
}

void operator/=( Jet& x, const Jet& y ) {
 x = x / y;
}

void operator/=( Jet& x, const double& y ) {
 x = x / y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//     Member functions(public)  |||||||||||||||||||||||||||||||

void Jet::addTerm( JLterm* a) {
 PREPFORCHANGE(jl)
 jl->addTerm( a );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


//***************************************************************
//***************************************************************
//***************************************************************
//
//      The overloaded operators for class Jet
//

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const Jet& x, const Jet& y ) {
 return *(x.jl) == *(y.jl);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const Jet& x, const double& y ) {
 return *(x.jl) == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const double& y, const Jet& x ){
 return *(x.jl) == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const Jet& x, const Jet& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const Jet& x, const double& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const double& x, const Jet& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Jet::operator+=( const double& x ) {   // ??? Untested!!
 if( jl == NULL ) {                         // ??? DANGER: what is the 
   jl = new JL;                             // ??? reference point?
 }
 else {
   PREPFORCHANGE(jl)
 }
 jl->operator+=( x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Jet::operator-=( const double& x ) {
 operator+=( -x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator+( const Jet& x, const Jet& y ) { 

 // Possibility: constant Jet argument
 if( !(x->myEnv) ) {
   if( x->count == 1 ) {
     return x.standardPart() + y;
   }
   else {
     cerr << "\n\n"
          << "*** ERROR ***                                       \n"
          << "*** ERROR *** Jet operator+( Jet, Jet )             \n"
          << "*** ERROR ***                                       \n"
          << "*** ERROR *** Null environment for the first        \n"
          << "*** ERROR *** Jet argument.                         \n"
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
          << "*** ERROR *** Jet operator+( Jet, Jet )             \n"
          << "*** ERROR ***                                       \n"
          << "*** ERROR *** Null environment for the second       \n"
          << "*** ERROR *** Jet argument.                         \n"
          << "*** ERROR ***                                       \n"
          << endl;
   }
 }


 static Jet z;
 static JLterm* p;
 static JLterm* q;

 p    = 0;
 q    = 0;

 // Check for consistency and set reference point of the sum.
 if( x->myEnv != y->myEnv ) {
     cerr << "\n\n*** Jet::operator+ ERROR: Inconsistent environments." 
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
   while((  p = (JLterm*) gy()  )) 
     z->addTerm( new JLterm( p ) );
 }
 else {
   z.DeepCopy( y );
   while((  p = (JLterm*) gx()  )) 
     z->addTerm( new JLterm( p ) );
 }
 
 // Determine the maximum weight computed accurately.
 if( x->accuWgt < y->accuWgt ) z->accuWgt = x->accuWgt;
 else                          z->accuWgt = y->accuWgt;
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator+( const Jet& x, const double& y ) {
 static Jet z;     // !!! "Jet z = x;" does not work.  The copy constructor
 z.DeepCopy( x );  // ??? is called, which makes x and z own the same data!!
 z += y;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator+( const double& y, const Jet& x ) {
 Jet z;
 z.DeepCopy( x );
 z += y;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator-( const Jet& x, const double& y ) {
 return x + (-y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator-( const double& y, const Jet& x ) {
 return y + (-x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator-( const Jet& y ) {  
 static Jet z;
 static JLterm* p;

 z.DeepCopy( y );

 // If the argument is void, then return it ..
 if( y->count < 1 ) { 
   return z;
 }

 // .. otherwise continue normal operations.
 dlist_iterator getNext( *(dlist*) z.jl );

 while((  p = (JLterm*) getNext()  )) {
   p->value = - p->value;
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Jet::Negate()
{
 static JLterm* p;

 // If the argument is void, then return ...
 if( jl->count < 1 ) return;

 // .. otherwise continue normal operations.
 PREPFORCHANGE(jl)
 dlist_iterator getNext( *(dlist*) jl );
 while((  p = (JLterm*) getNext()  )) p->value = - p->value;

 return;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Jet::Mult( const double& x )
{
 static JLterm* p;

 // If the argument is void, then return ...
 if( jl->count < 1 ) return;

 // .. otherwise continue normal operations.
 PREPFORCHANGE(jl)
 dlist_iterator getNext( *(dlist*) jl );
 while((  p = (JLterm*) getNext()  )) p->value *= x;

 return;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator-( const Jet& x, const Jet& y ) {  
                                       // ??? This should be modified so that
 static Jet z;                         // terms beyond the accurate weight of
 static JLterm* p;                     // x or y are not computed and carried
 static JLterm* q;                     // into the answer.

 p    = 0;
 q    = 0;

 // Check for consistency and set reference point of the difference.
 if( x->myEnv != y->myEnv ) {
     cerr << "\n\n*** Jet::operator- ERROR: Inconsistent environments." 
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
   while((  p = (JLterm*) gy()  )) {
     q = new JLterm( p );
     ( q->value ) = - ( q->value );
     z->addTerm( q );
   }
 }
 else {
   z = - y;
   while((  p = (JLterm*) gx()  )) 
     z->addTerm( new JLterm( p ) );
 }
 
 // Determine the maximum weight computed accurately.
 if( x->accuWgt < y->accuWgt ) z->accuWgt = x->accuWgt;
 else                          z->accuWgt = y->accuWgt;
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator*( const Jet& x, const Jet& y ) 
{
 static Jet z;
 static JLterm* p;
 static JLterm* q;
 static JLterm* r;
 static JL* xPtr;
 static JL* yPtr;
 static JL* zPtr;
 static int testWeight, trialWeight;

 // Check for consistency 
 if( x->myEnv != y->myEnv ) {
     cerr << "\n\n*** Jet::operator* ERROR: Inconsistent environments." 
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
 JLterm x_1stTerm( xPtr->lowTerm() );
 JLterm y_1stTerm( yPtr->lowTerm() );

 testWeight = xPtr->accuWgt;
 if( ( y_1stTerm.weight != 0 ) && ( y_1stTerm.value != 0.0 ) ) 
 {
   testWeight += y_1stTerm.weight;
 }
 // ??? REMOVE: testWeight  = xPtr->accuWgt + (yPtr->firstTerm()).weight;

 trialWeight = yPtr->accuWgt;
 if( ( x_1stTerm.weight != 0 ) && ( x_1stTerm.value != 0.0 ) ) 
 {
   trialWeight += x_1stTerm.weight;
 }
 // ??? REMOVE: trialWeight = yPtr->accuWgt + (xPtr->firstTerm()).weight;

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
 while((  p = (JLterm*) gy()  )) {
 while((  q = (JLterm*) gx()  )) {
   if( ( p->weight + q->weight ) > testWeight ) continue;
   r = new JLterm( (*p)*(*q) ); 
   zPtr->addTerm( r );
 }}
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator*( const Jet& x, const double& y ) {  
 static Jet z;
 static JLterm* p;
 static JLterm* q;
 static JL* xPtr;
 static JL* zPtr;
 static int testWeight;
 
 z.Reconstruct( x->myEnv );

 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;

 testWeight = z->accuWgt = x->accuWgt;

 if( y == 0.0 ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLterm( p );
   q->value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator*( const Jet& x, const int& j ) {  
 static Jet z;
 static JLterm* p;
 static JLterm* q;
 static JL* xPtr;
 static JL* zPtr;
 static int testWeight;
 static double y;
 
 z.Reconstruct( x->myEnv );

 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;

 testWeight = z->accuWgt = x->accuWgt;

 if( ( y = (double) j ) == 0.0 ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLterm( p );
   q->value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator*( const double& y, const Jet& x ) {  
 static Jet z;
 static JLterm* p;
 static JLterm* q;
 static JL* xPtr;
 static JL* zPtr;
 static int testWeight;
 
 z.Reconstruct( x->myEnv );

 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;
 testWeight = z->accuWgt = x->accuWgt;

 if( y == 0.0 ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLterm( p );
   q->value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator*( const int& j, const Jet& x ) { 
 static Jet z;
 static JLterm* p;
 static JLterm* q;
 static JL* xPtr;
 static JL* zPtr;
 static int testWeight;
 static double y;
 
 z.Reconstruct( x->myEnv );

 y    = 0.0;
 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;

 testWeight = z->accuWgt = x->accuWgt;

 if( ( y = (double) j ) == 0.0 ) {
   return z;
 }
 
 // If x is void, return it ..
 if( xPtr->count < 1 ) {    // This is done in this way so that
   z.DeepCopy( x );         // what is returned does not own
   return z;                // the same data as x.
 }

 dlist_iterator gx( *(dlist*) xPtr );

 while((  p = (JLterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLterm( p );
   q->value *= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator/( const Jet& x, const double& y ) { 
 static Jet z;
 static JLterm* p;
 static JLterm* q;
 static JL* xPtr;
 static JL* zPtr;
 static int testWeight;
 
 z.Reconstruct( x->myEnv ); 

 p    = 0;
 q    = 0;
 xPtr = x.jl;
 zPtr = z.jl;

 testWeight = z->accuWgt = x->accuWgt;

 // Check for division by zero ..
 if( y == 0.0 ) {
   cerr << "\n*** Jet::operator/ ERROR *** "
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

 while((  p = (JLterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLterm( p );
   q->value /= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator/( const Jet& x, const int& j ) { 
 static Jet z;
 static JLterm* p;
 static JLterm* q;
 static JL* xPtr;
 static JL* zPtr;
 static int testWeight;
 static double y;
 
 z.Reconstruct( x->myEnv ); 

 p    = 0;
 q    = 0;
 y    = 0.0;
 xPtr = x.jl;
 zPtr = z.jl;

 z->myEnv   = x->myEnv;
 testWeight = z->accuWgt = x->accuWgt;

 // Check for division by zero ..
 if( ( y = (double) j ) == 0.0 ) {
   cerr << "\n*** Jet::operator/ ERROR *** "
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

 while((  p = (JLterm*) gx()  )) {
   if( p->weight > testWeight ) break;
   q = new JLterm( p );
   q->value /= y;
   zPtr->addTerm( q );
 }
 
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet operator/( const double& a, const Jet& b ) {
  static Jet u;
  u.Reconstruct( b->myEnv ); 
  u = a;
  return u/b;
} 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet Jet::TruncMult( const Jet& v, const int& wl ) const
{ 
 /*
 Truncated multiplication: used only by the division operator.
 No checks are made on the environments.
 */
 static Jet z;
 static JLterm* p;
 static JLterm* q;
 static JLterm* r;

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
 while((  p = (JLterm*) gv()  )) {
 while((  q = (JLterm*) gu()  )) {
   if( ( p->weight + q->weight ) > wl ) continue;
   r = new JLterm( jl->myEnv );
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

Jet operator/( const Jet& wArg, const Jet& uArg ) { 

 // Check for void operators ..
 if ( wArg->count < 1 ) return wArg;
 if ( uArg->count < 1 ) {
   cerr << "\n*** JL ERROR *** "
        << "Attempt to divide by a void JL variable.\n"
        << endl;
   exit(0);
 }
 
 // Check for consistency 
 if( wArg->myEnv != uArg->myEnv ) {
     cerr << "\n\n*** Jet::operator/ ERROR: Inconsistent environments." 
          << endl;
     exit(0);
 }

 dlist_looper gu( *(dlist*) uArg.jl );
 dlist_looper gw( *(dlist*) wArg.jl );

 static Jet v;
 static Jet vn;
 static Jet w, u;
 static JLterm* qu;
 static JLterm* qw;
 static double u0;
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
 if( ( qu = (JLterm*) gu() ) == 0 ) {
   cerr <<  "\n*** JL ERROR ***  Division algorithm called with uninitialized JL.\n"
        << endl;
   exit(0);
   }
 if( ( wgt = qu->weight ) != 0 ) {
   cerr <<  "\n*** JL ERROR ***  Attempt to divide by nilpotent element.\n"
        << endl;
   exit(0);
   }
 if( ( u0 = qu->value ) == 0.0 ) {
   cerr <<  "\n*** JL ERROR ***  Attempt to divide by zero.\n"
        << endl;
   exit(0);
   }
 u = uArg / u0;
 
 // Recursive algorithm
 qw = (JLterm*) gw();
 if( qw->weight == 0 ) v = ( qw->value );
 else                  v = 0.0; 
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

Jet operator^( const Jet& x, const Jet& y ) { 

 // Check for consistency 
 if( x->myEnv != y->myEnv ) {
     cerr << "\n\n*** Jet::operator^ ERROR: Inconsistent environments." 
          << endl;
     exit(1);
 }

 static Jet__environment* theEnv;
 static Jet z;
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
      <<   "*** ERROR *** JL operator^( JL&, JL& )       \n"
      <<   "*** ERROR *** Wrong JLSetup used; cannot     \n"
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

Jet acos( const Jet&  ) {   // ??? Write this damned thing!!!  It's trivial!!
 static Jet z;
 cerr <<  "Jet acos( Jet& x ) to be written.\n" 
      << endl;
 exit(0);
 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet asin( const Jet& x ) { 
 static Jet z;
 static Jet dz;
 static int iter;

 iter = 0;

 z.DeepCopy( x );
 dz = ( sin(z) - x ) / cos(z);

 while( ( fabs(dz( z->myEnv->scale )) > 1.0e-10*fabs(z( z->myEnv->scale )) ) 
        && ( iter++ < MX_MAXITER ) 
      ) {
 // while( ( dz != 0.0 ) && ( iter++ < MX_MAXITER ) ) {
  z -= dz;
  dz = ( sin(z) - x ) / cos(z);
 }

 if( iter >= MX_MAXITER ) {
  cout << "*** WARNING ***                             \n";
  cout << "*** WARNING *** JL& asin( JL& x ) {         \n";
  cout << "*** WARNING *** Over " 
       << MX_MAXITER 
       << " iterations used;    \n";
  cout << "*** WARNING *** result may be incorrect.    \n";
  cout << "*** WARNING ***                             \n";
  cout << "*** WARNING *** Initial value:              \n";
  cout << "*** WARNING ***                             \n"
       << endl;
  x.printCoeffs();
  cout << "*** WARNING *** A few iterations:             " << endl;
  for( iter = 0; iter < 4; iter++ ) {
   cout << "*** WARNING *** Iteration No. " << ( iter+1 ) << endl;
   z -= dz;
   dz = ( sin(z) - x ) / cos(z);
   cout << "*** WARNING *** dz = " << endl;
   dz.printCoeffs();
   cout << "*** WARNING *** z = " << endl;
   z.printCoeffs();
  }
 }

 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet atan( const Jet& x ) {
 static Jet z;
 static Jet c, dz;
 static int iter;

 iter = 0;

 z.DeepCopy( x );
 c = cos( z );
 dz = c*( sin(z) - x*c );

 while( ( dz != 0.0 ) && ( iter++ < MX_MAXITER ) ) {
  z -= dz;
  c = cos( z );
  dz = c*( sin(z) - x*c );
 }

 if( iter >= MX_MAXITER ) {
  cerr <<  "*** WARNING ***                             \n" 
       <<  "*** WARNING *** JL& atan( JL& x ) {         \n" 
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

Jet cos( const Jet& x ) { 
 
 // ??? REMOVE: Jet epsSin( const Jet& );
 // ??? REMOVE: Jet epsCos( const Jet& );
 // ??? REMOVE: 
 static Jet epsilon;
 static double cs, sn;
 dlist_iterator getNext( *(dlist*) x.jl );
 static JLterm* p;
 
 // Paranoid initializations
 epsilon.Reconstruct( x->myEnv );
 cs = 0.0;
 sn = 0.0;
 p  = 0;

 if( x->count == 0 ) {
   if( epsilon->count != 0  ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Jet cos( Jet )                      \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Horrible, inexplicable error!!!     \n"
          << "*** ERROR *** epsilon->count = " << epsilon->count 
          <<                                  "                 \n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
   }
   epsilon.addTerm( new JLterm( x->myEnv->AllZeroes, 1.0, x->myEnv ) );
   return epsilon;
 }
 
 p = (JLterm*) getNext();
 if( ( p -> weight ) == 0 ) {         // x has non-zero standard part
   cs = cos( p -> value );
   sn = sin( p -> value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   delete p;
   return cs*Jet::epsCos( epsilon ) - sn*Jet::epsSin( epsilon );
   }
 else {                               // x is pure infinitesimal
   return Jet::epsCos( x );
   }
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet cosh( const Jet& x ) { 
 static Jet z;
 z = exp(x);
 z = ( z + ( 1.0 / z ) ) / 2.0;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet exp( const Jet& x ) { 
 
 // ??? REMOVE: Jet epsExp( const Jet& );
 static Jet epsilon;
 static double factor;
 static JLterm* p;

 // Paranoid initializations
 epsilon.Reconstruct( x->myEnv );
 factor = 0.0;
 p = 0;
 
 if( x->count == 0 ) {
   if( epsilon->count != 0  ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Jet exp( Jet )                      \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Horrible, inexplicable error!!!     \n"
          << "*** ERROR *** epsilon->count = " << epsilon->count 
          <<                                  "                 \n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
   }
   epsilon.addTerm( new JLterm( x->myEnv->AllZeroes, 1.0, x->myEnv ) );
   return epsilon;
 }
 
 dlist_iterator getNext( *(dlist*) x.jl );

 p = (JLterm*) getNext();
 if( ( p -> weight ) == 0 ) {         // x has non-zero standard part
   factor = exp( p -> value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   delete p;
   return factor*Jet::epsExp( epsilon );
   }
 else {                               // x is pure infinitesimal
   return Jet::epsExp( x );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet log ( const Jet& x ) { 
 static Jet             epsilon;
 static Jet             u, w;
 static double          std;
 static double          n;
 static JLterm*         p;
 
 // Paranoid initializations
 epsilon.Reconstruct( x->myEnv );
 u.Reconstruct( x->myEnv );
 w.Reconstruct( x->myEnv );
 
 std = n = 0.0;
 p = 0;
 
 if( x->count == 0 ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Jet log( Jet )                      \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** x->count == 0 implies a zero argument.\n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
 }
 
 dlist_iterator  getNext( *(dlist*) x.jl );

 p = (JLterm*) getNext();
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
        <<      "*** ERROR *** JL log called with argument whose\n" 
        <<      "*** ERROR *** standard part vanishes.\n"           
        <<      "*** ERROR *** \n\n"                                
        << endl;
   if( 1 + 1 == 2 ) exit(1);
   return x;
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet log10( const Jet& x ) {
 static const double logE = 0.4342944819032518276511289;
 return  logE*log(x);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet pow( const Jet& x, const double& s ) { 
 // ??? REMOVE: Jet epsPow( const Jet&, const double );
 static Jet epsilon;
 static double factor;
 static double std;
 static JLterm* p;
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
          << "*** ERROR *** Jet pow( Jet, double )              \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Horrible, inexplicable error!!!     \n"
          << "*** ERROR *** epsilon->count = " << epsilon->count 
          <<                                  "                 \n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
   }
   epsilon.addTerm( new JLterm( x->myEnv->AllZeroes, 0.0, x->myEnv ) );
   return epsilon;
 }
 
 dlist_iterator getNext( *(dlist*) x.jl );

 p = (JLterm*) getNext();
 if( ( ( p -> weight      ) ==  0   ) &&
     ( ( std = p -> value ) !=  0.0 )
   )                                  // x has non-zero standard part
   {
   if( x->count == 1 ) {             // x may have no derivatives
     epsilon = pow( std, s );
     return epsilon;
   }
   factor = pow( std, s );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 //   pops the standard part off epsilon
   delete p;
   epsilon.scaleBy( 1.0/std );
   epsilon = factor*Jet::epsPow( epsilon, s );
   return epsilon;
   }
 else                                 // x is pure infinitesimal
   {
   u = nearestInteger( s );
   if( s != (double) u ) {
     cerr <<  "\n*** ERROR::JL pow: attempt to use infinitesimal " 
          <<  "as base with non-integer exponent.\n" 
          << endl;
     exit(1);
     }
   epsilon = 1.0;
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

Jet pow( const Jet& x, int n ) {
  static int i;
  static Jet z;

  // Paranoid initializations
  z.Reconstruct( x->myEnv );
  i = 0;

  if( n == 0 ) z = 1.0;
  else if( n > 0 ) {
    z.DeepCopy( x );
    for( i = 2; i <= n; i++ ) z *= x;
  }
  else {
    Jet xr;
    z = 1.0 / x;
    xr = z;
    for( i = -2; i >= n; i-- ) z *= xr;
  }
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet sin( const Jet& x ) { 
 
 // ??? REMOVE: Jet epsSin( const Jet& );
 // ??? REMOVE: Jet epsCos( const Jet& );
 static Jet epsilon;
 static double cs, sn;
 static JLterm* p;

 // Paranoid initializations
 epsilon.Reconstruct( x->myEnv );
 cs = sn = 0.0;
 p = 0; 
 
 if( x->count == 0 ) {
   if( epsilon->count != 0  ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Jet sin( Jet )                      \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Horrible, inexplicable error!!!     \n"
          << "*** ERROR *** epsilon->count = " << epsilon->count 
          <<                                  "                 \n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
   }
   epsilon.addTerm( new JLterm( x->myEnv->AllZeroes, 0.0, x->myEnv ) );
   return epsilon;
 }
 
 dlist_iterator getNext( *(dlist*) x.jl );

 p = (JLterm*) getNext();
 if( ( p -> weight ) == 0 ) {         // x has non-zero standard part
   cs = cos( p -> value );
   sn = sin( p -> value );
   epsilon.DeepCopy( x );             // x must not be altered by the routine
   p = epsilon.get();                 // pops the standard part off epsilon
   delete p;
   return sn*Jet::epsCos( epsilon ) + cs*Jet::epsSin( epsilon );
   }
 else {                               // x is pure infinitesimal
   return Jet::epsSin( x );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet sinh( const Jet& x ) {
 Jet z;
 z = exp(x);
 return ( z - (1.0/z))/2.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet sqrt( const Jet& x ) {
 // ??? REMOVE: Jet epsSqrt( const Jet& );
 static Jet epsilon;
 static double factor;
 static double std;
 static JLterm* p;

 // Paranoid initializations
 epsilon.Reconstruct( x->myEnv );
 factor = std = 0.0;
 p = 0; 
 
 if( x->count == 0 ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Jet sqrt( Jet )                     \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** x->count == 0 implies a zero argument.\n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
 }
 
 dlist_iterator getNext( *(dlist*) x.jl );

 p = (JLterm*) getNext();
 if( ( ( p -> weight      ) ==  0   ) &&
     ( ( std = p -> value ) !=  0.0 )
   )                                  // x has non-zero standard part
   {
   factor = sqrt( std );
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
     return factor*Jet::epsSqrt( epsilon );
     }
   }
 else                                 // x is pure infinitesimal
   {
   cerr <<  "\n*** ERROR::JL sqrt: attempt to use infinitesimal argument." 
        << endl;
   if( 1 + 1 == 2 ) exit(1);
   return x;
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet tan( const Jet& x ) { 
 return sin(x) / cos(x) ;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet tanh( const Jet& x ) { 
 return sinh(x) / cosh(x);
}
 

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet fabs( const Jet& x ) {
 static double u;

 if( x->count == 0 ) {
     cerr << "\n\n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** Jet fabs( Jet )                     \n"
          << "*** ERROR ***                                     \n"
          << "*** ERROR *** x->count == 0 implies a zero argument.\n"
          << "*** ERROR ***                                     \n"
          << endl;
     exit(1);
 }
 
 if( (u = x.standardPart()) != 0.0 ) 
 {
   if( u > 0.0 ) return x;
   else          return -x;
 }
 else
 {
   cerr << "\n*** ERROR::Jet fabs: attempt to use infinitesimal argument."
        << endl;
   if( 1 + 1 == 2 ) exit(1);   // This weird stuff done to avoid
   return x;                   // compiler warnings.
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet erf( const Jet& z ) 
{
  Jet__environment* pje = z.Env();

         Jet    series    ( pje );
         Jet    oldseries ( pje );
         Jet    arg       ( pje );
         Jet    term      ( pje );
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

Jet erfc( const Jet& z ) 
{
  return ( 1.0 - erf( z ) );
}


//***************************************************************
//***************************************************************
//***************************************************************
//
//      Auxiliary functions which operate on infinitesimals.
//      It is assumed that the calling program checks to be sure
//      that the argument has zero standard part.
 
Jet Jet::epsCos( const Jet& epsilon ) { 
 
 static Jet z;
 static Jet epsq, term;
 static double n;

 z.setEnvTo( epsilon );
 // ??? REMOVE: z->myEnv = epsilon->myEnv;
 z = 1.0;
 
 epsq = - epsilon*epsilon;
 
 term = epsq / 2.0;
 n = 2.0;
 
 z.setEnvTo( epsilon );   // ??? Why does this appear twice?
 // ??? REMOVE: z->myEnv = epsilon->myEnv;
 
 while( term.jl->count > 0 ) {
   z += term;
   term = ( ( term*epsq ) / ++n ) / ++n;
   }
 
 z->accuWgt = epsilon->accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet Jet::epsExp( const Jet& epsilon ) { 
 static Jet z;
 static Jet term;
 double n;
 
 z.setEnvTo( epsilon );
 // ??? REMOVE: z->myEnv = epsilon->myEnv;
 z = 1.0;
 
 term = epsilon;
 n = 1.0;
 
 z.setEnvTo( epsilon );   // ??? Redundant?
 // ??? REMOVE:  z->myEnv = epsilon->myEnv;  // ??? Redundant?
 
 while( term.jl->count > 0 ) {
   z += term;
   term = ( term*epsilon ) / ++n;
 }
 
 z->accuWgt = epsilon->accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet Jet::epsPow( const Jet& epsilon, const double& s ) { 
 static Jet z;
 static Jet term;
 static double f, n;

 z.setEnvTo( epsilon );
 // ??? REMOVE: z->myEnv = epsilon->myEnv;
 z = 1.0;
 
 f = s;
 n = 1.0;
 term = s*epsilon;
 
 z.setEnvTo( epsilon );
 // ??? REMOVE:  z->myEnv = epsilon->myEnv;
 
 while( term.jl->count > 0 ) {
   z += term;
   term = ( (--f) * term * epsilon ) / ++n;
   }
 
 z->accuWgt = epsilon->accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet Jet::epsSin( const Jet& epsilon ) { 
 
 static Jet z;
 static Jet epsq, term;
 static double n;
 
 epsq = - epsilon*epsilon;
 z = epsilon;
 term = ( epsilon*epsq ) / 6.0;
 n = 3.0;
 
 while( term.jl->count > 0 ) {
   z += term;
   term = ( ( term*epsq ) / ++n ) / ++n;
 }
 
 z->accuWgt = epsilon->accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet Jet::epsSqrt( const Jet& epsilon ) {  

 // This function is identical to epsPow
 // with the substitution  s = 1/2

 static Jet     z;
 static Jet     term;
 static double  f, n;
 
 z.setEnvTo( epsilon );
 // ??? REMOVE:  z->myEnv = epsilon->myEnv;
 z = 1.0;
 
 f    = 1.0 / 2.0;
 n    = 1.0;
 term = f*epsilon;
 
 z.setEnvTo( epsilon );
 // ??? REMOVE:  z->myEnv = epsilon->myEnv;
 
 while( term.jl->count > 0 ) {
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
//      Implementation of Class Jet

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Jet::peekAt() const {
 dlist_traversor getNext( *(dlist*) jl );
 int i;
 JLterm* p;
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
   p = (JLterm*) q->info();
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

void Jet::printCoeffs() const {
 dlist_iterator getNext( *(dlist*) jl );
 int i;
 JLterm* p;

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

 while((  p = (JLterm*) getNext()  )) {
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

void Jet::peekAt( int  ) const {
 dlist_traversor getNext( *(dlist*) jl );
 int i;
 JLterm* p;
 dlink* q;
 fprintf( JL::scratchFile,
          "\nCount  = %d, Weight = %d, Max accurate weight = %d\n",
          jl->count, jl->weight, jl->accuWgt );
 fprintf( JL::scratchFile, "Reference point:  \n" );
 for( i = 0; i < jl->myEnv->NumVar; i++ )
   fprintf( JL::scratchFile, "%e  ", jl->myEnv->refPoint[i] );
 fprintf( JL::scratchFile, "\n" );
 while((  q = getNext()  )) {
   p = (JLterm*) q->info();
   fprintf( JL::scratchFile, "Weight: %d   Value: %e  || ",
           p -> weight, p -> value );
   fprintf( JL::scratchFile, "Addresses: %d %d %d : %d\n",
                  q->prevPtr(), q, q->nextPtr(), p );
   fprintf( JL::scratchFile, "Index:  ");
   for( i = 0; i < jl->myEnv->NumVar; i++ )
     fprintf( JL::scratchFile, "%d  ", (p -> index)(i) );
   fprintf( JL::scratchFile, "\n\n");
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Jet::printCoeffs( int  ) const {
 dlist_traversor getNext( *(dlist*) jl );
 int i;
 JLterm* p;
 dlink* q;
 fprintf( JL::scratchFile, "\nCount  = %d, Weight = %d, Max accurate weight = %d\n",
          jl->count, jl->weight, jl->accuWgt );
 fprintf( JL::scratchFile, "Reference point:  " );
 for( i = 0; i < jl->myEnv->NumVar; i++ ) 
   fprintf( JL::scratchFile, "%e  ", jl->myEnv->refPoint[i] );
 fprintf( JL::scratchFile, "\n\n" );
 while((  q = getNext()  )) {
   p = (JLterm*) q->info();
   if( p->weight > jl->accuWgt ) break;
   fprintf( JL::scratchFile,"Index:  ");
   for( i = 0; i < jl->myEnv->NumVar; i++ )
     fprintf( JL::scratchFile, "%3d ", (p -> index)(i) );
   fprintf( JL::scratchFile, "   Value: %14.6e  ", p -> value );
   fprintf( JL::scratchFile,"\n");
   }
 fprintf( JL::scratchFile, "\n\n" );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ??? REMOVE 
// ??? REMOVE dlist Jet::image() {      // ??? Problem: nothing is recorded about the
// ??? REMOVE                           // ??? reference point.
// ??? REMOVE  return jl->image();
// ??? REMOVE }
// ??? REMOVE 
// ??? REMOVE // |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ??? REMOVE 
void Jet::writeToFile( char* fileName ) const {
 jl->writeToFile( fileName );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Jet::writeToFile( FILE* filePtr ) const {
 jl->writeToFile( filePtr );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Jet::scaleBy( double y ) {
 PREPFORCHANGE(jl)
 jl->scaleBy( y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLterm* Jet::get() {
 PREPFORCHANGE(jl)
 return jl->get();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLterm Jet::firstTerm() const {
 return jl->firstTerm();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLterm Jet::lowTerm() const {
 return jl->lowTerm();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Jet::standardPart() const {
 return jl->standardPart();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Jet::clear() {
 PREPFORCHANGE(jl)
 jl->clear();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Jet::weightedDerivative( const int* ind ) const {
 return jl->weightedDerivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Jet::derivative( const int* ind ) const {
 return jl->derivative( ind );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet Jet::filter( const int& wgtLo, const int& wgtHi ) const { 
 Jet z;
 JL* zPtr = z.jl;
 JL* thisPtr = jl;
 dlist_iterator getNext( *(dlist*) thisPtr );
 JLterm* p;
 JLterm* q;
 int numTerms;
 int wgt, upperWgt;
 
 z.setEnvTo( *this );
 // ??? REMOVE:  z.jl->myEnv = jl->myEnv;

 if( ( wgtLo <= 0 ) && 
     ( wgtHi >= thisPtr->weight )   
   ) {
   z = *this;
   return z;
 } 

 numTerms = 0;
 upperWgt = 0;
 
 while((  p = (JLterm*) getNext()  )) {
   wgt = p->weight;
   if( ( wgt >= wgtLo ) && ( wgt <= wgtHi ) ) {
     q = new JLterm( p );
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

Jet Jet::filter( char (*f) ( const IntArray&, const double& ) ) const { 
 static Jet z;
 static JL* zPtr;
 static JLterm* p;
 static JLterm* q;

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

 while((  p = (JLterm*) getNext()  )) {
  oldIndex = p->index;

  if( (*f)( p->index, p->value ) ) {
   q = new JLterm( p );
   zPtr->append( q );
  }

  for( i = 0; i < nv; i++ ) 
   if( oldIndex(i) != p->index(i) ) {
    cerr <<  "\n"
         <<  "*** ERROR ***                                      \n" 
         <<  "*** ERROR *** Jet Jet::filter( char (*f) ( int*, double ) \n" 
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

Jet Jet::concat() const {
 static Jet v;
 dlist_iterator getNext( *(dlist*) jl );
 static JLterm* p;
 static int i;
 
 p = 0;
 i = 0;

 v.Reconstruct( (( jl->myEnv->JLmonomial )[0] )->myEnv );
 
 while((  p = (JLterm*) getNext()  )) {
   if( p->weight > jl->accuWgt ) break;
   for( i = 0; i < jl->myEnv->NumVar; i++ )
     jl->myEnv->exponent[i] = (p->index)(i);
   jl->myEnv->monoCode();
   v += ( p->value ) * ( jl->myEnv->JLmonomial[ jl->myEnv->monoRank() ] );
 }
 
 return v;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet 
Jet::operator() ( /* const */ JetVector& y ) const 
{
 static int i, n;   // ??? This should be rewritten 
 static Jet z;      // ??? so as to be self-contained.

 n = jl->myEnv->NumVar;
 Jet* u = new Jet [ n ];

 for( i = 0; i < n; i++ ) u[i] = y(i);
 z = operator()( u );

 delete [] u;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet Jet::operator() ( const Jet* y ) const { 
 Jet*     u = new Jet [ jl->myEnv->NumVar ];
 static Jet      z, term;
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
 	  << " *** ERROR *** Jet::operator()( Jet* )        \n"
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
 ( ( jl->myEnv->JLmonomial )[0] )->myEnv = y[0]->myEnv; // Needed by 
                                                        // Jet::concat
 jl->myEnv->JLmonomial[0] = 1.0;

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
     term = jl->myEnv->JLmonomial[ jl->myEnv->monoRank() ];
     ( jl->myEnv->exponent[i] )++;
     jl->myEnv->monoCode();
     jl->myEnv->JLmonomial[ jl->myEnv->monoRank() ] = term * u[i]; // ??? Is this OK???
 
   } // End while loop.
 
 // Monomials have been stored.
 // Now traverse the JL variable and evaluate.
 // Formerly done by using concat routine, which
 // I copy here
 {
  dlist_iterator getNext( *(dlist*) jl );
  static JLterm* p;
  static int i;
  
  p = 0;
  i = 0;
 
  z->myEnv = (( jl->myEnv->JLmonomial )[0] )->myEnv;
  
  while((  p = (JLterm*) getNext()  )) {
    if( p->weight > jl->accuWgt ) break;
    for( i = 0; i < jl->myEnv->NumVar; i++ )
      jl->myEnv->exponent[i] = (p->index)(i);
    jl->myEnv->monoCode();
    z += ( p->value ) * ( jl->myEnv->JLmonomial[ jl->myEnv->monoRank() ] );
  }
 }

 
 // Finish...
 delete [] u;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Jet::operator() ( const double* x ) const {
 return jl->operator()( x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Jet::operator() ( const Vector& x ) const {
 return jl->operator()( x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet Jet::D( const int* n ) const {
 // ??? REMOVE static char noTermAdded;
 static char doit;
 static int f, i, j, k, w;
 static Jet z;
 static JLterm* p;
 static JLterm* q;
 
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
     cerr <<  "\n*** JL ERROR: Cannot differentiate with negative index." 
          <<  "\n*******************************************************\n" 
          << endl;
     exit(0);
     }
   w += n[i];
 }
 
 if( w > jl->accuWgt ) {
   cerr <<  "\n*** JL ERROR: Differentiation request beyond accuracy allowed." 
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
 
 dlist_iterator getNext( *(dlist*) jl );

 while((  p = (JLterm*) getNext()  )) {
 
   q = new JLterm( p );
 
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
       cerr <<  "\n*** JL ERROR: Horrible, unexplainable error while differentiating!" 
            <<  "\n******************************************************************\n" 
            << endl;
       exit(0);
     }                           // Super fussbudget!! ( f "must" be positive )
   
     // -- Make final changes in private data of the JLterm and
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

Jet Jet::D( const IntArray& n ) const 
{

 if( n.Dim() != jl->myEnv->NumVar ) {
    cerr << "\n\n"
         << "*** ERROR ***                                   \n"
         << "*** ERROR *** Jet::D( IntArray )                \n"
         << "*** ERROR ***                                   \n"
         << "*** ERROR *** Inconsistent dimensions.          \n"
         << "*** ERROR ***                                   \n"
         << endl;
    exit(1);
 }

 // ??? REMOVE static char noTermAdded;
 static char doit;
 static int f, i, j, k, w;
 static Jet z;
 dlist_iterator getNext( *(dlist*) jl );
 static JLterm* p;
 static JLterm* q;
 
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
     cerr <<  "\n*** JL ERROR: Cannot differentiate with negative index." 
          <<  "\n*******************************************************\n" 
          << endl;
     exit(0);
     }
   w += n(i);
 }
 
 if( w > jl->accuWgt ) {
   cerr <<  "\n*** JL ERROR: Differentiation request beyond accuracy allowed." 
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
 
 while((  p = (JLterm*) getNext()  )) {
 
   q = new JLterm( p );
 
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
       cerr <<  "\n*** JL ERROR: Horrible, unexplainable error while differentiating!" 
            <<  "\n******************************************************************\n" 
            << endl;
       exit(0);
       }                           // Super fussbudget!! ( f "must" be positive )
   
     // -- Make final changes in private data of the JLterm and
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

// POSTPONED Jet Jet::Inverse() const {
// POSTPONED  static Jet      z;
// POSTPONED  static JLterm*  p;
// POSTPONED  static double   zero;
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
// POSTPONED           "*** ERROR ***  Jet Jet::Inverse()                 \n" 
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
// POSTPONED   z->addTerm( new JLterm( z->myEnv->AllZeroes, 0.0, z->myEnv ) );
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
// POSTPONED            "*** WARNING *** Jet::Inverse()                               \n"
// POSTPONED            "*** WARNING *** z.standardPart() = " << z.standardPart() << "\n"
// POSTPONED            "*** WARNING ***                                              \n"
// POSTPONED         << endl;
// POSTPONED   }
// POSTPONED   z->addTerm( new JLterm( z->myEnv->AllZeroes, 
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
// POSTPONED Jet Jet::EpsInverse() const {
// POSTPONED  static Jet        z, x, v, g;
// POSTPONED  static const int  d(1);
// POSTPONED  static int        i;
// POSTPONED  static Jet__environment*  newEnv;
// POSTPONED 
// POSTPONED  newEnv = new Jet__environment( jl->myEnv );
// POSTPONED  Jet::environments.append( newEnv );
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
