/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      JL.cc
******  Version:   4.3
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


#include "JL.h"

#include <iomanip>

#ifdef  __PRIVATE_ALLOCATOR__
#include <vmalloc.h>
       int JLterm::_init=0;
Vmalloc_t* JLterm::_vmem=0;
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


using namespace std;

// ================================================================

#ifndef MX_SMALL
#define MX_SMALL    1.0e-12 // Used by JL::addTerm to decide 
                            //   removal of a JLterm.
#endif  // MX_SMALL

#ifndef MX_MAXITER
#define MX_MAXITER  100     // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER

#ifdef OBJECT_DEBUG
int JL::objectCount = 0;
int JLterm::objectCount = 0;
#endif


// ================================================================
//      Implementation of exceptions

JL::GenericException::GenericException( string fileName, int lineNumber, 
                                        const char* fcn, 
                                        const char* msg )
{
  ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    cerr << errorString;
    cerr << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

const char* JL::GenericException::what() const throw()
{
  return errorString.c_str();
}

JL::BadDimension::BadDimension( int i, int j, 
                                string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: xdim(i), ydim(j)
{
  ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** First  argument has " << xdim << " coordinates."
      << "\n*** ERROR *** Second argument has " << ydim << " coordinates."
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    cerr << errorString;
    cerr << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

const char* JL::BadDimension::what() const throw()
{
  return errorString.c_str();
}

JL::BadEnvironment::BadEnvironment( int a, int b, int c, 
                                string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: i(a), env_1(b), env_2(c)
{
  ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** comp[" << i << "].Env = " << env_1
      << "\n*** ERROR *** this->myEnv = " << env_2
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    cerr << errorString;
    cerr << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

const char* JL::BadEnvironment::what() const throw()
{
  return errorString.c_str();
}

JL::BadReference::BadReference( int ndx, double u,
                                string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: i(ndx), im(u)
{
  ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** The imaginary part of refPoint[ " << i << " ]"
      << "\n*** ERROR *** is equal to " << im <<  ", which is not zero."
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    cerr << errorString;
    cerr << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

const char* JL::BadReference::what() const throw()
{
  return errorString.c_str();
}

JL::HorribleException::HorribleException( int i, int j, int k, 
                                string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: curIndex(i), coordSize(j), valueSize(k)
{
  ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** A horrible, inexplicable error    "
         "\n*** ERROR *** has occurred. This is beyond      "
         "\n*** ERROR *** the realm of human understanding. "
         "\n*** ERROR *** Please consult an exorcist.       "
         "\n*** ERROR *** currentIndex:      " << curIndex
      << "\n*** ERROR *** newCoords.size() : " << coordSize
      << "\n*** ERROR *** newValues.size() : " << valueSize
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    cerr << errorString;
    cerr << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

const char* JL::HorribleException::what() const throw()
{
  return errorString.c_str();
}

JL::HideousException::HideousException( int x, int y, 
                                string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: i(x), n(y)
{
  ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** An unbelievably hideous error     "
      << "\n*** ERROR *** has occurred.                     "
      << "\n*** ERROR *** " << i << " != " << n << "        "
      << "\n*** ERROR ***               This is beyond      "
      << "\n*** ERROR *** the realm of human understanding. "
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    cerr << errorString;
    cerr << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

const char* JL::HideousException::what() const throw()
{
  return errorString.c_str();
}



//***************************************************************
//***************************************************************
//***************************************************************
//
//      Implementation of Class JL

//    Constructors and destructors    |||||||||||||||||||||||||||

JL::JL( const Jet__environment* pje ) {
 rc      = 1;
 weight  = -1;
 count   = 0;

 if( pje ) {
   myEnv   = (Jet__environment*) pje;
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

JL::JL( const double& x, Jet__environment* pje ) {
 clear();
 rc = 1;
 weight = 0;
 count = 0;
 myEnv   = pje;

 if( pje ) {
   accuWgt = pje->MaxWeight;
   insert( new JLterm( IntArray( pje->NumVar ), x, pje ) );
 }
 else {
   accuWgt = 100000;    // ??? Better way?
   insert( new JLterm( IntArray(1), x, 0 ) );
 }
 
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JL::JL( const JL& x ) {
 dlist_iterator getNext( (dlist&) x );
 JLterm* p;
 JLterm* q;

 count    = x.count;   // needed by append function
 weight   = x.weight;  // needed by append function
 accuWgt  = x.accuWgt;
 myEnv    = x.myEnv;
 rc       = 1;

 while((  p = (JLterm*) getNext()  )) {
   q = new JLterm( p );
   append( q );
 }

 count    = x.count;    // gets reset by append function
 weight   = x.weight;   // may get reset by append function
 accuWgt  = x.accuWgt;
 myEnv    = x.myEnv;
 rc       = 1;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JL::JL( JL* x ) {
 dlist_iterator getNext( *(dlist*) x );
 JLterm* p;
 JLterm* q;

 count    = x->count;
 weight   = x->weight;
 accuWgt  = x->accuWgt;
 myEnv    = x->myEnv;
 rc       = 1;

 while((  p = (JLterm*) getNext()  )) {
   q = new JLterm( p );
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

JL::~JL() {
 clear();
 // myEnv is purposely NOT deleted, of course.

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//     Member functions(public)  |||||||||||||||||||||||||||||||

void JL::addTerm( JLterm* a) {
 // If the value of *a is 0, don't bother with it unless
 // the weight is also 0.
 if( ( a -> value == 0.0 ) && ( a -> weight != 0 ) ) {
   delete a;
   return;
 }

 dlist_traversor getNext( *(dlist*) this );
 dlink* p;

 //
 // In the event that the candidate is lighter than the first
 // element in the JL dlist ... or the list is empty
 //
 p = getNext();
 
 if( p == 0 ) {
   insert( a );
   return;
   }
 
 if(     *a   <=   ( *(JLterm*) (p->info()) )   ) {
   if(   *a   %=   ( *(JLterm*) (p->info()) )   ) {
 
     (( (JLterm*) (p->info()) ) -> value ) += ( a -> value );
 
     if(
         (
           fabs( ( (JLterm*) (p->info()) ) -> value ) < MX_SMALL*fabs( a -> value )
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
 
   if(     *a   <=   ( *(JLterm*) (p->info()) )   ) {
     if(   *a   %=   ( *(JLterm*) (p->info()) )   ) {
 
       (( (JLterm*) (p->info()) ) -> value ) += ( a -> value );
 
       if(
	   (
	     fabs( ( (JLterm*) (p->info()) ) -> value ) < MX_SMALL*fabs( a -> value )
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

void JL::getReference( double* r ) const {
 int i;
 for( i = 0; i < myEnv->NumVar; i++ ) r[i] = myEnv->refPoint[i];
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char JL::isNilpotent() const {
 dlist_iterator getNext( *(dlist*) this );
 JLterm* p = (JLterm*) getNext();
 if( p->weight == 0 && fabs(p->value) > MX_SMALL ) 
   { return 0; }
 else
   { return 1; }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JL::writeToFile( char* fileName ) const {
 dlist_traversor getNext( *(dlist*) this );
 int i;
 JLterm* p;
 dlink* q;
 
 JL::scratchFile = fopen( fileName, "w" );
 
 fprintf( JL::scratchFile,
          "\nCount  = %d, Weight = %d, Max accurate weight = %d\n",
          count, weight, accuWgt );
 fprintf( JL::scratchFile, "Reference point:  \n" );
 for( i = 0; i < myEnv->NumVar; i++ )
   fprintf( JL::scratchFile, "%e  ", myEnv->refPoint[i] );
 fprintf( JL::scratchFile, "\n" );
 while((  q = getNext()  )) {
   p = (JLterm*) q->info();
   fprintf( JL::scratchFile, "Weight: %d   Value: %e  || ",
           p -> weight, p -> value );
   fprintf( JL::scratchFile, "Addresses: %d %d %d : %d\n",
                  q->prevPtr(), q, q->nextPtr(), p );
   fprintf( JL::scratchFile, "Index:  ");
   for( i = 0; i < myEnv->NumVar; i++ )
     fprintf( JL::scratchFile, "%d  ", (p -> index)(i) );
   fprintf( JL::scratchFile, "\n\n");
   }
 
 fclose( JL::scratchFile );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JL::writeToFile( FILE* filePtr ) const {
 dlist_traversor getNext( *(dlist*) this );
 int i;
 JLterm* p;
 dlink* q;
 
 fprintf( filePtr,
          "\nCount  = %d, Weight = %d, Max accurate weight = %d\n",
          count, weight, accuWgt );
 fprintf( filePtr, "Reference point:  \n" );
 for( i = 0; i < myEnv->NumVar; i++ )
   fprintf( filePtr, "%e  ", myEnv->refPoint[i] );
 fprintf( filePtr, "\n" );
 while((  q = getNext()  )) {
   p = (JLterm*) q->info();
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

void JL::scaleBy( double y ) {
 JLterm* p;
 dlist_iterator getNext( *(dlist*) this );
 
 if ( this->count < 1 ) {
   cout << "\n\n*** WARNING::JL::scaleBy: function invoked \n"
        <<     "    by null JL variable with address " << (int) this 
        << endl;
   return;
   }
 
 while((  p = (JLterm*) getNext()  ))   p->value *= y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JL::setVariable( const double& x, 
                      const int& j, 
                            Jet__environment* theEnv )
{
 if( theEnv != 0 ) myEnv = theEnv;

 if( myEnv == 0 ) {
   throw( JL::GenericException( __FILE__, __LINE__, 
     "void JL::setVariable( const double&, const int&, Jet__environment* )", 
     "Private data myEnv is null: object has no environment assigned.") );
 }

 clear();

 if( ( j < 0 ) || ( j >= myEnv->NumVar ) ) return;

 IntArray ndx( myEnv->NumVar );  // Assumes: initialized with zeroes
 insert( new JLterm( ndx, x, myEnv ) );
 ndx(j) = 1;
 append( new JLterm( ndx, 1.0, myEnv ) );
 
 myEnv->refPoint[j] = x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JL::setVariable( const int& j, 
                            Jet__environment* theEnv ) 
{
 if( theEnv != 0 ) myEnv = theEnv;

 if( myEnv == 0 ) {
   throw( JL::GenericException( __FILE__, __LINE__, 
     "void JL::setVariable( const int&, Jet__environment* )", 
     "Private data myEnv is null: object has no environment assigned.") );
 }

 clear();
 if( ( j < 0 ) || ( j >= myEnv->NumVar ) ) return;
 
 double x = theEnv->refPoint[j];
 IntArray ndx( myEnv->NumVar );
 insert( new JLterm( ndx, x, theEnv ) );
 ndx(j) = 1;
 append ( new JLterm( ndx, 1.0, theEnv ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JL::insert( JLterm* a) {
 dlist::insert(a);
 count++;
 if( weight < (a->weight) ) weight = a->weight;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLterm* JL::get() {
 JLterm* p;
 if( count <= 0 ) return 0;
 p = ((JLterm*) dlist::get());
 if( --count == 0 ) weight = -1;
 return p;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLterm JL::lowTerm() const {
 dlist_iterator getNext( *(dlist*) this );
 static JLterm* p;
 static JLterm* result;

 p = (JLterm*) getNext();
 result = 0;
   
 while ( (result == 0) && (p!=0) ) 
 {
   if( p->value != 0.0 ) result = p;
   p = (JLterm*) getNext();
 }

 if( result == 0 ) return JLterm( this->myEnv->AllZeroes, 0.0, this->myEnv );
 else              return JLterm( *result );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLterm JL::firstTerm() const {
 static JLterm* p;
 p = (JLterm*) ( ((dlist*) this)->firstInfoPtr() );
 if( 0 != p ) {
   return JLterm( *p );
 }
 else {
   return JLterm( this->myEnv->AllZeroes, 0.0, this->myEnv );
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JL::append( JLterm* a) {
 dlist::append(a);
 count++;
 if( weight < (a->weight) ) weight = a->weight;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLterm* JL::remove( dlink* w ) {
 JLterm* a;
 dlink* predecessor;
 dlink* successor;
 int predWeight;
 int succWeight;
 
 predecessor = w->prevPtr();
 successor   = w->nextPtr();
 
 a = (JLterm*) dlist::remove( w );
 count--;
 
 if( count == 0 ) {
   weight = -1;
   return a;
   }
 
 if ( weight > a->weight ) return a;
 
 predWeight = ( (JLterm*) ( predecessor->info() ) ) -> weight;
 succWeight = ( (JLterm*) ( successor->info() ) ) -> weight;
 weight = predWeight;
 if( succWeight > weight ) weight = succWeight;
 return a;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double JL::standardPart() const {
 JLterm* p; 
 if( count < 1 )       return 0.0;
 dlist_iterator g( (dlist&) *this );
 p = (JLterm*) g();
 if( p->weight  == 0 ) return p->value;
                       return 0.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JL::clear() {
 JLterm* p;
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

double JL::weightedDerivative( const int* ind ) const {
 int i;
 JLterm* p;
 char theOne;
 
 if( count < 1 ) return 0.0;
 
 dlist_iterator getNext( (dlist&) *this );

 while((  p = (JLterm*) getNext()  )) {
   theOne = 1;
   for ( i = 0; i < myEnv->NumVar; i++ )
     theOne = theOne && ( (p->index)(i) == ind[i] );
   if( theOne ) return p->value;
 }
 
 return 0.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double JL::derivative( const int* ind ) const {
 static double n;
 static double multiplier;
 static double d;
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

double JL::operator()( const double* x )  const {
 // This routine is linked to double JL::operator()( const Vector& x ) const
 // Any change made to this must be reflected in the other.
 JLterm* p;
 double v;
 int w;
 int i;
 double term;
 double* u;
 dlist_iterator getNext( *(dlist*) this );
 
 u = new double[ myEnv->NumVar ];
 
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
 // Now traverse the JL variable and evaluate.
 
 v = 0.0;
 
 while((  p = (JLterm*) getNext()  )) {
   for( i = 0; i < myEnv->NumVar; i++ ) myEnv->exponent[i] = (p->index)(i);
   myEnv->monoCode();
   v += ( p->value ) * ( myEnv->monomial[ myEnv->monoRank() ] );
 }
 
 delete [] u;
 
 return v;
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double JL::operator()( const Vector& x ) const {
 // This routine is linked to double JL::operator()( double* x ) 
 // Any change made to this must be reflected in the other.
 JLterm* p;
 double v;
 int w;
 int i;
 double term;
 double* u;
 dlist_iterator getNext( *(dlist*) this );
 
 u = new double[ myEnv->NumVar ];
 
 // Subtract off the reference point.
 for( i = 0; i < myEnv->NumVar; i++ ) u[i] = x(i) - myEnv->refPoint[i];
 
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
 // Now traverse the JL variable and evaluate.
 
 v = 0.0;
 
 while((  p = (JLterm*) getNext()  )) {
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
//      The overloaded operators for class JL
//

istream& operator>>( istream& is,  JL& x ) 
{  // ??? This function shouldn't
   // ??? be here.
  char buf[100];
  int i,j;
  int count;
  double value;
  JLterm* q;

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
  streamIn( is, &(x.myEnv) );
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
    q = new JLterm( ndx, value, x.myEnv );
    x.append( q );
  }

 return is;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<( ostream& os, const JL& x ) {
 dlist_traversor getNext( (dlist&) x );
 int i;
 JLterm* p;
 dlink* q;

 os << "\n Count = " << x.count << " , Weight = " << x.weight;
 os << " , Max accurate weight = " << x.accuWgt << endl;
 os << "Begin Environment: \n"
    << *(x.myEnv)
    << "\nEnd Environment: " << endl;

 while((  q = getNext()  )) {
   p = (JLterm*) q->info();
   os << "Index: ";
   for( i = 0; i < x.myEnv->NumVar; i++ )
     os << p -> index(i) << " ";
   os << "   Value: " << setprecision(30) << p -> value << endl;
   // os << "   Value: " << p -> value << endl;
 }
 return os << "\n" << endl;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const JL& x, const JL& y ) {
 if( ( x.count   != y.count )   ||
     ( x.weight  != y.weight )  ||
     ( x.accuWgt != y.accuWgt )
   ) return 0;
 
 if( x.myEnv != y.myEnv ) return 0;

 dlist_iterator getNextX( (dlist&) x );
 dlist_iterator getNextY( (dlist&) y );
 JLterm* p;
 JLterm* q;
 
 while((  p = (JLterm*) getNextX()  )) {
   if( !( q = (JLterm*) getNextY() ) ) return 0;
   if( !( *p == *q ) ) return 0;
   }
 
 if( (JLterm*) getNextY() ) {
   printf( "\n*** ERROR: Inconsistency in JL operator==\n" );
     // This point should never be reached, since that would imply that
     // x.count != y.count
   return 0;
   }
 
 return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const JL& x, const double& y ) {
 char result = 1;
 JLterm* p;

 if( x.count < 1 ) {
  if( y == 0.0 ) return 1;
  else           return 0;
 }

 dlist_iterator getNext( (dlist&) x );

 while((  p = (JLterm*) getNext()  )) 
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

char operator==( const double& y, const JL& x ){
 return x == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const JL& x, const JL& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const JL& x, const double& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const double& x, const JL& y ) {
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JL& JL::operator=( const double& x ) {
 static int* ndx;
 
 clear();
 weight = 0;   // rc is purposely left untouched!!
 count  = 0;
 if( myEnv ) accuWgt = myEnv->MaxWeight;
 else {
   throw( JL::GenericException( __FILE__, __LINE__, 
     "JL& JL::operator=( const double& x ) {", 
     "Private data myEnv is null: object has no environment assigned.") );
 }

 insert( new JLterm( IntArray( myEnv->NumVar ), x, myEnv ) );

 delete [] ndx;
 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JL& JL::operator=( const JL& x ) {
 static JLterm* p;
 static JLterm* q;

 if( this == &x ) return *this;
 
 clear();

 dlist_iterator getNext( (dlist&) x );

 while((  p = (JLterm*) getNext()  )) {
   q = new JLterm( p );
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

JL& JL::operator+=( const double& x ) {   // ??? Untested!!
 static char firstNull = 1;
 int i;
 // ??? REMOVE dlist_iterator getNext( *(dlist*) this );
 JLterm* p;                               
 JLterm* q;

 // ??? REMOVE if( p = (JLterm*) getNext() )
 if( p = (JLterm*) firstInfoPtr() )
 {
   if ( p->weight == 0 ) { 
    p->value += x;
    if( fabs( p->value ) <  MX_SMALL*fabs(x) ) p->value = 0.0;
   }
 
   else {
     q = new JLterm( myEnv );
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
             "*** WARNING *** JL& JL::operator+=( const double& )\n"
             "*** WARNING *** Operator invoked by a null      \n"
             "*** WARNING *** JL.  This warning will not be   \n"
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

JLterm operator*( const JLterm& x, const JLterm& y ) {
 JLterm z(x);
 static int i, n;

 if((  ( n = x.index.Dim() ) != y.index.Dim()  )) {
   throw( JL::BadDimension( x.index.Dim(), y.index.Dim(),
                            __FILE__, __LINE__, 
                            "JLterm operator*( const JLterm& x, const JLterm& y )",
                            "Inconsistent number of coordinates." ) );
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
//      Implementation of Class JLterm

JLterm::JLterm( const Jet__environment* pje ) 
: index( pje->NumVar ), weight(0), value(0.0)
{
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLterm::JLterm( const IntArray& l, 
                const double& x, 
                const Jet__environment* pje ) 
: index( l )
{
 // I think that it is not necessary to use a try block
 // with this constructor, even though it throws exceptions,
 // because the destructor does no subsidiary garbage collection.
 // - Leo Michelotti
 //   Thursday, January 22, 2004
 int i, dpt;

 if( pje ) {
   int n = l.Dim();
   if( n != pje->NumVar ) {
     throw( JL::GenericException( __FILE__, __LINE__, 
            "JLterm::JLterm( const IntArray&, const double&, const Jet__environment*",
            "Dimensions are wrong.") );
   }
  
   // ??? These checks could be removed for speed.
   dpt = 0;
   for( i = 0; i < n; i++ ) {
     if( (l(i) < 0) ) {
       throw( JL::GenericException( __FILE__, __LINE__, 
              "JLterm::JLterm( const IntArray&, const double&, const Jet__environment*",
              "Bad index in JLTerm.") );
     }
     dpt += l(i);
   }
  
   if( dpt > pje->MaxWeight ) {
     throw( JL::GenericException( __FILE__, __LINE__, 
            "JLterm::JLterm( const IntArray&, const double&, const Jet__environment*",
            "Attempt to load a JLTerm with too large a weight.") );
   }
   
   // ??? REMOVE: index = l;
   weight = dpt;
   value = x;
 }

 else {
   if( l.Dim() != 1 ) {
     throw( JL::GenericException( __FILE__, __LINE__, 
            "JLterm::JLterm( const IntArray&, const double&, const Jet__environment*",
            "Inconsistency between l and pje") );
   }
   if( l(0) != 0 ) {
     throw( JL::GenericException( __FILE__, __LINE__, 
            "JLterm::JLterm( const IntArray&, const double&, const Jet__environment*",
            "Bad value of the index when pje = 0.") );
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

#ifdef __PRIVATE_ALLOCATOR__

void JLterm::meminit(size_t size) {

  _vmem = vmopen(Vmdcsbrk,Vmpool,0);
  //_vmem = vmopen(Vmdcsbrk,Vmpool,VM_TRACE);
  //int fd =  creat("vmalloc.log", 666);
  //vmtrace(fd);
  vmalloc(_vmem,size);
  _init = 1;
};

void* JLterm::operator new(size_t size){
 if (_init ==0) meminit(size);
 return vmalloc(_vmem,size);
};

void JLterm::operator delete(void* obj, size_t size){
  vmfree(_vmem,obj);
};
#endif


JLterm::JLterm( const JLterm* x ) 
: index( x->index ), weight(x->weight), value(x->value)
{

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLterm::JLterm( const JLterm& x ) 
: index( x.index ), weight(x.weight), value(x.value)
{
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JLterm::~JLterm() {
#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}



//***************************************************************
//***************************************************************
//***************************************************************
//
//      Overloaded operators for class JLterm

void JLterm::operator=( const JLterm& x ) {

 weight = x.weight;
 value  = x.value;
 index  = x.index;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator==( const JLterm& a, const JLterm& b ) {
 if( a.weight != b.weight ) return 0;
 if( a.value  != b.value  ) return 0;
 if( a.index  != b.index  ) return 0;
 return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator!=( const JLterm& a, const JLterm& b ) {
 return !( a == b );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator<=( const JLterm& a, const JLterm& b ) {
 int i;
 if( a.index.Dim() != b.index.Dim() ) {
   throw( JL::BadDimension( a.index.Dim(), b.index.Dim(), 
          __FILE__, __LINE__, 
          "char operator<=( const JLterm&, const JLterm& )",
          "Dimensions don't match.") );
 }
 if( a.weight != b.weight ) { return ( a.weight < b.weight ); }
 for( i = 0; i < a.index.Dim(); i++ ) {
   if( a.index(i) == b.index(i) ) continue;
   return ( a.index(i) < b.index(i) );
 }
 return 1;  // when all indices are the same.
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char operator%=( const JLterm& a, const JLterm& b ) {
 if( a.weight != b.weight ) return 0;
 return a.index == b.index;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
