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


#include <math.h>
#include "JLC.h"

#ifdef __VISUAL_CPP__
#include <iomanip>
using std::cout;
using std::setprecision;
#else
#include <stdlib.h>
#include <iomanip.h>
#endif

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
                            //   removal of a JLCterm.
#endif  // MX_SMALL

#ifndef MX_MAXITER
#define MX_MAXITER  100     // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER

#ifdef OBJECT_DEBUG
int JLC::objectCount = 0;
int JLCterm::objectCount = 0;
#endif

//***************************************************************
//***************************************************************
//***************************************************************
//
//      Implementation of Class JLC

//    Constructors and destructors    |||||||||||||||||||||||||||

JLC::JLC( JetC__environment* pje ) {
 rc      = 1;
 weight  = -1;
 count   = 0;

 if( pje ) {
   myEnv   = pje;
   accuWgt = pje->MaxWeight;
 }
 else {
   myEnv   = 0;
   accuWgt = 100000;    // ??? Better way?
 }
 
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLC::JLC( const Complex& x, JetC__environment* pje ) {

 
 clear();
 rc = 1;
 weight = 0;
 count = 0;
 myEnv   = pje;

 if( pje ) {
   accuWgt = pje->MaxWeight;
   insert( new JLCterm( IntArray( pje->NumVar ), x, pje ) );
 }
 else {
   accuWgt = 100000;    // ??? Better way?
   insert( new JLCterm( IntArray(1), x, 0 ) );
 }
 
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLC::JLC( const JLC& x ) {
 dlist_iterator getNext( (dlist&) x );
 JLCterm* p;
 JLCterm* q;

 
 static char firstCall = 1;
 if( firstCall ) {
  firstCall = 0;
  cout << "\n*** WARNING *** The JLC copy constructor was called for \n"
       <<   "*** WARNING *** some inexplicable reason.\n"
       <<   "*** WARNING *** You must have an incredible error somewhere.\n"
       <<   "*** WARNING *** Go figure!!\n\n";
  cout.flush();
  cerr << "\n*** WARNING *** The JLC copy constructor was called for \n"
       <<   "*** WARNING *** some inexplicable reason.\n"
       <<   "*** WARNING *** You must have an incredible error somewhere.\n"
       <<   "*** WARNING *** Go figure!!\n\n";
  cerr.flush();
 }

 while((  p = (JLCterm*) getNext()  )) {
   q = new JLCterm( p );
   append( q );
 }

 count    = x.count;
 weight   = x.weight;
 accuWgt  = x.accuWgt;
 myEnv    = x.myEnv;
 rc       = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLC::JLC( JLC* x ) {
 dlist_iterator getNext( *(dlist*) x );
 JLCterm* p;
 JLCterm* q;

 
 static char firstCall = 1;
 if( firstCall ) {
  firstCall = 0;
  cout << "\n*** WARNING *** The JLC pointer constructor was called for \n"
       <<   "*** WARNING *** some inexplicable reason.\n"
       <<   "*** WARNING *** You must have an incredible error somewhere.\n"
       <<   "*** WARNING *** Go figure!!\n\n";
  cout.flush();
 }

 while((  p = (JLCterm*) getNext()  )) {
   q = new JLCterm( p );
   append( q );
 }

 count    = x->count;
 weight   = x->weight;
 accuWgt  = x->accuWgt;
 myEnv    = x->myEnv;
 rc       = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLC::~JLC() {
 clear();
 // myEnv is purposely NOT deleted, of course.

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//     Member functions(public)  |||||||||||||||||||||||||||||||

void JLC::addTerm( JLCterm* a) {
 dlist_traversor getNext( *(dlist*) this );
 dlink* p;
 
 // If the value of *a is 0, don't bother with it unless
 // the weight is also 0.
 if( ( a -> value == 0.0 ) && ( a -> weight != 0 ) ) {
   delete a;
   return;
 }
 
 //
 // In the event that the candidate is lighter than the first
 // element in the JLC dlist ... or the list is empty
 //
 p = getNext();
 
 if( p == 0 ) {
   insert( a );
   return;
   }
 
 if(     *a   <=   ( *(JLCterm*) (p->info()) )   ) {
   if(   *a   %=   ( *(JLCterm*) (p->info()) )   ) {
 
     (( (JLCterm*) (p->info()) ) -> value ) += ( a -> value );
 
     if(
         (
           abs( ( (JLCterm*) (p->info()) ) -> value ) < MX_SMALL*abs( a -> value )
         )
         // REMOVE ??? &&
         // REMOVE ??? (
         // REMOVE ???   p != ( dlist::lastPtr() ) -> nextPtr()
         // REMOVE ??? )
       )
 
         delete remove( p );
 
     delete a;
     return;
     }
 
   else {
     insert( a );
     return;
     }
 
   }
 
 //
 // Otherwise, search through the list
 //
 while((  p = getNext()  )){
 
   if(     *a   <=   ( *(JLCterm*) (p->info()) )   ) {
     if(   *a   %=   ( *(JLCterm*) (p->info()) )   ) {
 
       (( (JLCterm*) (p->info()) ) -> value ) += ( a -> value );
 
       if(
	   (
	     abs( ( (JLCterm*) (p->info()) ) -> value ) < MX_SMALL*abs( a -> value )
	   )
	   // REMOVE ??? &&
	   // REMOVE ??? (
	   // REMOVE ???   p != ( dlist::lastPtr() ) -> nextPtr()
	   // REMOVE ??? )
	 )
 
         delete remove( p );
 
       delete a;
       return;
       }
 
     else {
       p -> putAbove( new dlink( a, 0, 0 ) );
       (this->count)++;
       if( (this->weight) < (a->weight) ) this->weight = a->weight;
       return;
       }
 
     }
   }
 
 //
 //  The new entry is heavier than existing ones.
 //
 append( a );
 return;
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JLC::getReference( Complex* r ) {
 int i;
 for( i = 0; i < myEnv->NumVar; i++ ) r[i] = myEnv->refPoint[i];
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JLC::isNilpotent() {
 dlist_iterator getNext( *(dlist*) this );
 JLCterm* p = (JLCterm*) getNext();
 if( p->weight == 0 && p->value != 0.0 ) return 0;
 else                                    return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JLC::writeToFile( char* fileName ) {
 dlist_traversor getNext( *(dlist*) this );
 int i;
 JLCterm* p;
 dlink* q;
 
 JetC::scratchFile = fopen( fileName, "w" );
 
 fprintf( JetC::scratchFile,
          "\nCount  = %d, Weight = %d, Max accurate weight = %d\n",
          count, weight, accuWgt );
 fprintf( JetC::scratchFile, "Reference point:  \n" );
 for( i = 0; i < myEnv->NumVar; i++ )
   fprintf( JetC::scratchFile, "%e  ", myEnv->refPoint[i] );
 fprintf( JetC::scratchFile, "\n" );
 while((  q = getNext()  )) {
   p = (JLCterm*) q->info();
   fprintf( JetC::scratchFile, "Weight: %d   Value: %e  || ",
           p -> weight, p -> value );
   fprintf( JetC::scratchFile, "Addresses: %d %d %d : %d\n",
                  q->prevPtr(), q, q->nextPtr(), p );
   fprintf( JetC::scratchFile, "Index:  ");
   for( i = 0; i < myEnv->NumVar; i++ )
     fprintf( JetC::scratchFile, "%d  ", (p -> index)(i) );
   fprintf( JetC::scratchFile, "\n\n");
   }
 
 fclose( JetC::scratchFile );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JLC::writeToFile( FILE* filePtr ) {
 dlist_traversor getNext( *(dlist*) this );
 int i;
 JLCterm* p;
 dlink* q;
 
 fprintf( filePtr,
          "\nCount  = %d, Weight = %d, Max accurate weight = %d\n",
          count, weight, accuWgt );
 fprintf( filePtr, "Reference point:  \n" );
 for( i = 0; i < myEnv->NumVar; i++ )
   fprintf( filePtr, "%e  ", myEnv->refPoint[i] );
 fprintf( filePtr, "\n" );
 while((  q = getNext()  )) {
   p = (JLCterm*) q->info();
   fprintf( filePtr, "Weight: %d   Value: %e  || ",
           p -> weight, p -> value );
   fprintf( filePtr, "Addresses: %d %d %d : %d\n",
                  q->prevPtr(), q, q->nextPtr(), p );
   fprintf( filePtr, "Index:  ");
   for( i = 0; i < myEnv->NumVar; i++ )
     fprintf( filePtr, "%d  ", (p -> index)(i) );
   fprintf( filePtr, "\n\n");
   }
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JLC::scaleBy( Complex y ) {
 JLCterm* p;
 dlist_iterator getNext( *(dlist*) this );
 
 if ( this->count < 1 ) {
   cout << "\n\n*** WARNING::JLC::scaleBy: function invoked \n"
        <<     "    by null JLC variable with address " << (int) this 
        << endl;
   return;
   }
 
 while((  p = (JLCterm*) getNext()  ))   p->value *= y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JLC::setVariable( const Complex& x, 
                      const int& j, 
                            JetC__environment* theEnv )
{
 if( theEnv != 0 ) myEnv = theEnv;

 if( myEnv == 0 ) {
   cerr << "\n\n"
        << "*** ERROR ***                                      \n"
        << "*** ERROR *** JLC::setVariable                      \n"
        << "*** ERROR ***                                      \n"
        << "*** ERROR *** myEnv = 0                            \n"
        << "*** ERROR ***                                      \n"
        << endl;
   exit(1);
 }

 clear();

 if( ( j < 0 ) || ( j >= myEnv->NumVar ) ) return;

 IntArray ndx( myEnv->NumVar );  // Assumes: initialized with zeroes
 insert( new JLCterm( ndx, x, myEnv ) );
 ndx(j) = 1;
 append( new JLCterm( ndx, complex_1, myEnv ) );
 
 myEnv->refPoint[j] = x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JLC::setVariable( const int& j, 
                            JetC__environment* theEnv ) 
{
 if( theEnv != 0 ) myEnv = theEnv;

 if( myEnv == 0 ) {
   cerr << "\n\n"
        << "*** ERROR ***                                      \n"
        << "*** ERROR *** JLC::setVariable                      \n"
        << "*** ERROR ***                                      \n"
        << "*** ERROR *** myEnv = 0                            \n"
        << "*** ERROR ***                                      \n"
        << endl;
   exit(1);
 }

 static Complex x;
 
 clear();
 if( ( j < 0 ) || ( j >= myEnv->NumVar ) ) return;
 
 IntArray ndx( myEnv->NumVar );
 insert( new JLCterm( ndx, x, theEnv ) );
 ndx(j) = 1;
 append ( new JLCterm( ndx, complex_1, theEnv ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JLC::insert( JLCterm* a) {
 dlist::insert(a);
 count++;
 if( weight < (a->weight) ) weight = a->weight;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLCterm* JLC::get() {
 JLCterm* p;
 if( count <= 0 ) return 0;
 p = ((JLCterm*) dlist::get());
 if( --count == 0 ) weight = -1;
 return p;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JLC::append( JLCterm* a) {
 dlist::append(a);
 count++;
 if( weight < (a->weight) ) weight = a->weight;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLCterm* JLC::remove( dlink* w ) {
 JLCterm* a;
 dlink* predecessor;
 dlink* successor;
 int predWeight;
 int succWeight;
 
 predecessor = w->prevPtr();
 successor   = w->nextPtr();
 
 a = (JLCterm*) dlist::remove( w );
 count--;
 
 if( count == 0 ) {
   weight = -1;
   return a;
   }
 
 if ( weight > a->weight ) return a;
 
 predWeight = ( (JLCterm*) ( predecessor->info() ) ) -> weight;
 succWeight = ( (JLCterm*) ( successor->info() ) ) -> weight;
 weight = predWeight;
 if( succWeight > weight ) weight = succWeight;
 return a;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Complex JLC::standardPart() {
 dlist_iterator g( (dlist&) *this );
 JLCterm* p; 
 if( count < 1 )       return 0.0;
 p = (JLCterm*) g();
 if( p->weight  == 0 ) return p->value;
                       return 0.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JLC::clear() {
 JLCterm* p;
 while((  p = get()  ))  
   delete p;
 dlist::clear();
 count   = 0;
 weight  = -1;
 if( myEnv ) accuWgt = myEnv->MaxWeight;
 else        accuWgt = 100000;   // ??? Better way?
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Complex JLC::weightedDerivative( int* ind ) {
 int i;
 JLCterm* p;
 char theOne;
 
 if( count < 1 ) return 0.0;
 
 dlist_iterator getNext( (dlist&) *this );

 while((  p = (JLCterm*) getNext()  )) {
   theOne = 1;
   for ( i = 0; i < myEnv->NumVar; i++ )
     theOne = theOne && ( (p->index)(i) == ind[i] );
   if( theOne ) return p->value;
 }
 
 return 0.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Complex JLC::derivative( int* ind ) {
 static double n;
 static double multiplier;
 static Complex d;
 static int i;
 
 d = weightedDerivative( ind );
 if ( d == 0.0 ) return 0.0;
 
 multiplier = 1.0;
 for ( i = 0; i < myEnv->NumVar; i++ ) {
   n = ind[i];
   while(n)  multiplier *= n--;
   }
 
 return multiplier*d;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Complex JLC::operator()( Complex* x ) {
 JLCterm* p;
 Complex v;
 int w;
 int i;
 Complex term;
 Complex* u;
 dlist_iterator getNext( *(dlist*) this );
 
 u = new Complex[ myEnv->NumVar ];
 
 // Subtract off the reference point.
 for( i = 0; i < myEnv->NumVar; i++ ) u[i] = x[i] - myEnv->refPoint[i];
 
 // Evaluate and store monomials.
 
 // The zeroth one.
 myEnv->monomial[0] = 1.0;
 
 // For all higher weights ...
 for( w = 1; w <= weight; w++ )
 
   // Get the next set of exponents of weight w.
   while( nexcom( w, myEnv->NumVar, myEnv->exponent ) ) {
 
     // Find the first non-zero exponent.
     i = 0;
     while( !( myEnv->exponent[i++] ) ) ;
     i--;
 
     // The value of the monomial associated with this composition
     // is obtained by multiplying a factor into a previously
     // computed monomial.
     ( myEnv->exponent[i] )--;
     myEnv->monoCode();
     term = myEnv->monomial[ myEnv->monoRank() ];
     ( myEnv->exponent[i] )++;
     myEnv->monoCode();
     myEnv->monomial[ myEnv->monoRank() ] = term * u[i];
 
   }
 
 // Monomials have been now stored at this point.
 // Now traverse the JLC variable and evaluate.
 
 v = 0.0;
 
 while((  p = (JLCterm*) getNext()  )) {
   for( i = 0; i < myEnv->NumVar; i++ ) myEnv->exponent[i] = (p->index)(i);
   myEnv->monoCode();
   v += ( p->value ) * ( myEnv->monomial[ myEnv->monoRank() ] );
 }
 
 delete [] u;
 
 return v;
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



//***************************************************************
//***************************************************************
//***************************************************************
//
//      The overloaded operators for class JLC
//

istream& operator>>( istream& is,  JLC& x ) 
{  // ??? This function shouldn't
   // ??? be here.
  char buf[100];
  int i,j;
  int count;
  Complex value;
  JLCterm* q;

  x.clear();

  is >> buf;
  is >> buf;
  is >> count;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> x.weight;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> x.accuWgt;
  is >> buf;
  is >> buf;
  is >> x.myEnv;
  is >> buf;
  is >> buf;
  IntArray ndx( x.myEnv->NumVar );
  for( j = 0; j < count; j++) {
    is >> buf;
    for( i = 0; i < x.myEnv->NumVar; i++ )  {
     is >> ndx(i);
    }
    is >> buf;
    is >> value;
    q = new JLCterm( ndx, value, x.myEnv );
    x.append( q );
  }

 return is;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<( ostream& os, JLC& x ) {
 dlist_traversor getNext( (dlist&) x );
 int i;
 JLCterm* p;
 dlink* q;

 os << "\n Count = " << x.count << " , Weight = " << x.weight;
 os << " , Max accurate weight = " << x.accuWgt << endl;
 os << "Begin Environment: \n"
    << x.myEnv
    << "End Environment: " << endl;

 while((  q = getNext()  )) {
   p = (JLCterm*) q->info();
   os << "Index: ";
   for( i = 0; i < x.myEnv->NumVar; i++ )
     os << p -> index(i) << " ";
  os << "   Value: " << setprecision(30) << p -> value << endl;
 }
 return os << "\n" << endl;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const JLC& x, const JLC& y ) {
 dlist_iterator getNextX( (dlist&) x );
 dlist_iterator getNextY( (dlist&) y );
 JLCterm* p;
 JLCterm* q;

 
 if( ( x.count   != y.count )   ||
     ( x.weight  != y.weight )  ||
     ( x.accuWgt != y.accuWgt )
   ) return 0;
 
 if( x.myEnv != y.myEnv ) return 0;

 while((  p = (JLCterm*) getNextX()  )) {
   if( !( q = (JLCterm*) getNextY() ) ) return 0;
   if( !( *p == *q ) ) return 0;
   }
 
 if( (JLCterm*) getNextY() ) {
   printf( "\n*** ERROR: Inconsistency in JLC operator==\n" );
     // This point should never be reached, since that would imply that
     // x.count != y.count
   return 0;
   }
 
 return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const JLC& x, const Complex& y ) {
 char result = 1;
 dlist_iterator getNext( (dlist&) x );
 JLCterm* p;

 if( x.count < 1 ) {
  if( real(y) == 0.0 && imag(y) == 0.0 )  return 1;
  else                                    return 0;
 }

 while((  p = (JLCterm*) getNext()  )) 
  result = result && ( p->weight == 0 ? p->value == y : 
                                        p->value == 0.0 );

  // REMOVE ??? result = result && ( ( ( p->weight == 0 ) && ( p->value == y   ) )
  // REMOVE ???                      ||
  // REMOVE ???                      ( ( p->weight != 0 ) && ( p->value == 0.0 ) )
  // REMOVE ???                    )
  // REMOVE ??? 
 return result;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const Complex& y, const JLC& x ){
 return x == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const JLC& x, const JLC& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const JLC& x, const Complex& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const Complex& x, const JLC& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLC& JLC::operator=( const Complex& x ) {
 static int* ndx;
 
 clear();
 weight = 0;   // rc is purposely left untouched!!
 count  = 0;

 if( myEnv ) accuWgt = myEnv->MaxWeight;
 else {
   cerr << "\n\n"
        << "*** ERROR ***                                       \n"
        << "*** ERROR ***  JLC::operator=( Complex )              \n"
        << "*** ERROR ***                                       \n"
        << "*** ERROR ***  myEnv must be set ahead of time.     \n"
        << "*** ERROR ***                                       \n"
        << endl;
   exit(1);
 }

 insert( new JLCterm( IntArray( myEnv->NumVar ), x, myEnv ) );

 delete [] ndx;
 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLC& JLC::operator=( const JLC& x ) {
 dlist_iterator getNext( (dlist&) x );
 static JLCterm* p;
 static JLCterm* q;

 if( this == &x ) return *this;
 
 clear();
 while((  p = (JLCterm*) getNext()  )) {
   q = new JLCterm( p );
   append( q );
 }

 count   = x.count;     // rc is purposely left untouched
 weight  = x.weight;
 accuWgt = x.accuWgt;
 myEnv   = x.myEnv;

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLC& JLC::operator+=( const Complex& x ) {   // ??? Untested!!
 static char firstNull = 1;
 int i;
 // ??? REMOVE dlist_iterator getNext( *(dlist*) this );
 JLCterm* p;                               
 JLCterm* q;

 // ??? REMOVE if( p = (JLCterm*) getNext() )
 if( p = (JLCterm*) firstInfoPtr() )
 {
   if ( p->weight == 0 ) { 
    p->value += x;
    if( abs( p->value ) <  MX_SMALL*abs(x) ) p->value = 0.0;
   }
 
   else {
     q = new JLCterm( myEnv );
     for( i = 0; i < myEnv->NumVar; i++ ) q->index(i) = 0;
     q->weight = 0;
     q->value = x;
     insert( q );
   }
 }
 else
 {
   if( firstNull ) {
     cerr << "*** WARNING ***                                 \n"
             "*** WARNING *** JLC& JLC::operator+=( const Complex& )\n"
             "*** WARNING *** Operator invoked by a null      \n"
             "*** WARNING *** JLC.  This warning will not be   \n"
             "*** WARNING *** repeated.                       \n"
             "*** WARNING ***                                 \n"
          << endl;
     firstNull = 0;
   }
   operator=( x );
 }

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLCterm operator*( JLCterm& x, JLCterm& y ) {
 JLCterm z(x);
 static int i, n;

 if((  ( n = x.index.Dim() ) != y.index.Dim()  )) {
   cerr << "\n\n"
        << "*** ERROR ***                                      \n"
        << "*** ERROR *** JLCterm operator*                     \n"
        << "*** ERROR ***                                      \n"
        << "*** ERROR *** Dimensions don't match.              \n"
        << "*** ERROR *** First argument:  " << x.index.Dim() << " \n"
        << "*** ERROR *** Second argument: " << y.index.Dim() << " \n"
        << "*** ERROR ***                                      \n"
        << endl;
   exit(1);
 }

 z.weight = x.weight + y.weight;
 for( i = 0; i < n; i++ ) 
   z.index(i) = x.index(i) + y.index(i);
 z.value = x.value * y.value;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



//***************************************************************
//***************************************************************
//***************************************************************
//
//      Implementation of Class JLCterm

JLCterm::JLCterm( const JetC__environment* pje ) 
: index( pje->NumVar )
{
 weight = 0;
 value = 0.0;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLCterm::JLCterm( const IntArray& l, 
                const Complex& x, 
                const JetC__environment* pje ) 
: index( l.Dim() )
{
 int i, dpt;

 if( pje ) {
   int n = l.Dim();
   if( n != pje->NumVar ) {
     cerr << "\n*** JLC ERROR *** Dimensions are wrong." << endl;
     exit(1);
   }
  
   // ??? These checks could be removed for speed.
   dpt = 0;
   for( i = 0; i < n; i++ ) {
     if( (l(i) < 0) ) {
       printf( "\n*** JLC ERROR *** Bad index in JLCTerm.\n" );
       exit(0);
     }
     dpt += l(i);
   }
  
   if( dpt > pje->MaxWeight ) {
     printf("\n*** JLC ERROR *** Attempt to load a JLCTerm with too large a weight.\n");
     exit(0);
   }
   
   index = l;
   weight = dpt;
   value = x;
 }

 else {
   if( l.Dim() != 1 ) {
     cerr << "\n\n"
          << "*** ERROR ***                                        \n"
          << "*** ERROR *** JLCTerm::JLCTerm                         \n"
          << "*** ERROR ***                                        \n"
          << "*** ERROR *** Inconsistency between l and pje        \n"
          << "*** ERROR ***                                        \n"
          << endl;
     exit(1);
   }
   if( l(0) != 0 ) {
     cerr << "\n\n"
          << "*** ERROR ***                                        \n"
          << "*** ERROR *** JLCTerm::JLCTerm                         \n"
          << "*** ERROR ***                                        \n"
          << "*** ERROR *** Bad value of the index when pje = 0.   \n"
          << "*** ERROR ***                                        \n"
          << endl;
     exit(1);
   }
   index = l;
   weight = l.Sum();
   value = x;
 }

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLCterm::JLCterm( const JLCterm* x ) 
: index( x->index.Dim() )
{

 
 weight = x->weight;
 value  = x->value;
 index  = x->index;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLCterm::JLCterm( const JLCterm& x ) 
: index( x.index.Dim() )
{

 
 weight = x.weight;
 value  = x.value;
 index  = x.index;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLCterm::~JLCterm() {
#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}



//***************************************************************
//***************************************************************
//***************************************************************
//
//      Overloaded operators for class JLCterm

void JLCterm::operator=( const JLCterm& x ) {

 weight = x.weight;
 value  = x.value;
 index  = x.index;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const JLCterm& a, const JLCterm& b ) {
 if( a.weight != b.weight ) return 0;
 if( a.value  != b.value  ) return 0;
 if( a.index  != b.index  ) return 0;
 return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const JLCterm& a, const JLCterm& b ) {
 return !( a == b );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator<=( JLCterm& a, JLCterm& b ) {
 int i;
 if( a.index.Dim() != b.index.Dim() ) {
   cerr << "\n\n"
        << "*** ERROR ***                                      \n"
        << "*** ERROR *** char operator<=( JLCterm, JLCterm )    \n"
        << "*** ERROR ***                                      \n"
        << "*** ERROR *** Dimensions don't match.              \n"
        << "*** ERROR *** First argument:  " << a.index.Dim() << " \n"
        << "*** ERROR *** Second argument: " << b.index.Dim() << " \n"
        << "*** ERROR ***                                      \n"
        << endl;
   exit(1);
 }
 if( a.weight != b.weight ) return ( a.weight < b.weight );
 for( i = 0; i < a.index.Dim(); i++ ) {
   if( a.index(i) == b.index(i) ) continue;
   return ( a.index(i) < b.index(i) );
 }
 return 1;  // when all indices are the same.
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator%=( JLCterm& a, JLCterm& b ) {
 if( a.weight != b.weight ) return 0;
 return a.index == b.index;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
