/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJL.cc
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


#include "TJL.h"

#include <iomanip>

#ifdef  __PRIVATE_ALLOCATOR__
#include <vmalloc.h>
       int TJLterm<T1,T2>::_init=0;
Vmalloc_t* TJLterm<T1,T2>::_vmem=0;
#endif

template<typename T1, typename T2>
FILE  *TJL<T1,T2>::scratchFile;


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


// ================================================================
//      Implementation of exceptions

template<typename T1, typename T2>
TJL<T1,T2>::BadDimension::BadDimension( int i, int j, 
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

template<typename T1, typename T2>
const char* TJL<T1,T2>::BadDimension::what() const throw()
{
  return errorString.c_str();
}

template<typename T1, typename T2>
TJL<T1,T2>::BadEnvironment::BadEnvironment( int a, int b, int c, 
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

template<typename T1, typename T2>
const char* TJL<T1,T2>::BadEnvironment::what() const throw()
{
  return errorString.c_str();
}

template<typename T1, typename T2>
TJL<T1,T2>::BadReference::BadReference( int ndx, double u,
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
      << "\n*** ERROR *** The imaginary part of _refPoint[ " << i << " ]"
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

template<typename T1, typename T2>
const char* TJL<T1,T2>::BadReference::what() const throw()
{
  return errorString.c_str();
}

template<typename T1, typename T2>
TJL<T1,T2>::HorribleException::HorribleException( int i, int j, int k, 
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
         "\n*** ERROR *** _currentIndex:      " << curIndex
      << "\n*** ERROR *** _newCoords.size() : " << coordSize
      << "\n*** ERROR *** _newValues.size() : " << valueSize
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

template<typename T1, typename T2>
const char* TJL<T1,T2>::HorribleException::what() const throw()
{
  return errorString.c_str();
}

template<typename T1, typename T2>
TJL<T1,T2>::HideousException::HideousException( int x, int y, 
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

template<typename T1, typename T2>
const char* TJL<T1,T2>::HideousException::what() const throw()
{
  return errorString.c_str();
}


// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Implementation of Class TJL

//    Constructors and destructors    |||||||||||||||||||||||||||

template<typename T1, typename T2>
TJL<T1,T2>::TJL( const TJetEnvironment<T1,T2>* pje ) 
{
 _rc      = 1;
 _weight  = -1;
 _count   = 0;

 if( pje ) {
   _myEnv   = (TJetEnvironment<T1,T2>*) pje;
   _accuWgt = pje->_maxWeight;
 }
 else {
   _myEnv   = 0;
   _accuWgt = 100000;    // ??? Better way?
 }
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJL<T1,T2>::TJL( const T1& x, TJetEnvironment<T1,T2>* pje ) 
{
 this->clear();
 _rc = 1;
 _weight = 0;
 _count = 0;
 _myEnv   = pje;

 if( pje ) {
   _accuWgt = pje->_maxWeight;
   insert( new TJLterm<T1,T2>( IntArray( pje->_numVar ), x, pje ) );
 }
 else {
   _accuWgt = 100000;    // ??? Better way?
   insert( new TJLterm<T1,T2>( IntArray(1), x, 0 ) );
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJL<T1,T2>::TJL( const TJL& x ) 
{
 dlist_iterator getNext( (dlist&) x );
 TJLterm<T1,T2>* p;
 TJLterm<T1,T2>* q;

 _count    = x._count;   // needed by append function
 _weight   = x._weight;  // needed by append function
 _accuWgt  = x._accuWgt;
 _myEnv    = x._myEnv;
 _rc       = 1;

 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   q = new TJLterm<T1,T2>( p );
   append( q );
 }

 _count    = x._count;    // gets reset by append function
 _weight   = x._weight;   // may get reset by append function
 _accuWgt  = x._accuWgt;
 _myEnv    = x._myEnv;
 _rc       = 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJL<T1,T2>::TJL( const TJL* x ) 
{
 dlist_iterator getNext( *(dlist*) x );
 TJLterm<T1,T2>* p;
 TJLterm<T1,T2>* q;

 _count    = x->_count;
 _weight   = x->_weight;
 _accuWgt  = x->_accuWgt;
 _myEnv    = x->_myEnv;
 _rc       = 1;

 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   q = new TJLterm<T1,T2>( p );
   append( q );
 }

 _count    = x->_count;
 _weight   = x->_weight;
 _accuWgt  = x->_accuWgt;
 _myEnv    = x->_myEnv;
 _rc       = 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJL<T1,T2>::~TJL() 
{
 clear();
 // _myEnv is purposely NOT deleted, of course.
}
/*
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//     Member functions(public)  |||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJL<T1,T2>::addTerm( TJLterm<T1,T2>* a) 
{
 // If the value of *a is 0, don't bother with it unless
 // the _weight is also 0.
 if( ( a -> value == 0.0 ) && ( a -> _weight != 0 ) ) {
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
 
 if(     *a   <=   ( *(TJLterm<T1,T2>*) (p->info()) )   ) {
   if(   *a   %=   ( *(TJLterm<T1,T2>*) (p->info()) )   ) {
 
     (( (TJLterm<T1,T2>*) (p->info()) ) -> value ) += ( a -> value );
 
     if(
         (
           std::abs( ( (TJLterm<T1,T2>*) (p->info()) ) -> value ) < MX_SMALL*std::abs( a -> value )
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
 
   if(     *a   <=   ( *(TJLterm<T1,T2>*) (p->info()) )   ) {
     if(   *a   %=   ( *(TJLterm<T1,T2>*) (p->info()) )   ) {
 
       (( (TJLterm<T1,T2>*) (p->info()) ) -> value ) += ( a -> value );
 
       if(
	   (
	     std::abs( ( (TJLterm<T1,T2>*) (p->info()) ) -> value ) < MX_SMALL*std::abs( a -> value )
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
       (this->_count)++;
       if( (this->_weight) < (a->_weight) ) this->_weight = a->_weight;
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

template<typename T1, typename T2>
void TJL<T1,T2>::getReference( double* r ) const 
{
 int i;
 for( i = 0; i < _myEnv->_numVar; i++ ) r[i] = _myEnv->_refPoint[i];
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
char TJL<T1,T2>::isNilpotent() const 
{
 dlist_iterator getNext( *(dlist*) this );
 TJLterm<T1,T2>* p = (TJLterm<T1,T2>*) getNext();
 if( p->_weight == 0 && std::abs(p->value) > MX_SMALL ) 
   { return 0; }
 else
   { return 1; }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJL<T1,T2>::writeToFile( char* fileName ) const 
{
 dlist_traversor getNext( *(dlist*) this );
 int i;
 TJLterm<T1,T2>* p;
 dlink* q;
 
 TJL<T1,T2>::scratchFile = fopen( fileName, "w" );
 
 fprintf( TJL<T1,T2>::scratchFile,
          "\n_Count  = %d, Weight = %d, Max accurate weight = %d\n",
          _count, _weight, _accuWgt );
 fprintf( TJL<T1,T2>::scratchFile, "Reference point:  \n" );
 for( i = 0; i < _myEnv->_numVar; i++ )
   fprintf( TJL<T1,T2>::scratchFile, "%e  ", _myEnv->_refPoint[i] );
 fprintf( TJL<T1,T2>::scratchFile, "\n" );
 while((  q = getNext()  )) {
   p = (TJLterm<T1,T2>*) q->info();
   fprintf( TJL<T1,T2>::scratchFile, "Weight: %d   Value: %e  || ",
           p -> _weight, p -> value );
   fprintf( TJL<T1,T2>::scratchFile, "Addresses: %d %d %d : %d\n",
                  q->prevPtr(), q, q->nextPtr(), p );
   fprintf( TJL<T1,T2>::scratchFile, "Index:  ");
   for( i = 0; i < _myEnv->_numVar; i++ )
     fprintf( TJL<T1,T2>::scratchFile, "%d  ", (p -> index)(i) );
   fprintf( TJL<T1,T2>::scratchFile, "\n\n");
   }
 
 fclose( TJL<T1,T2>::scratchFile );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJL<T1,T2>::writeToFile( FILE* filePtr ) const 
{
 dlist_traversor getNext( *(dlist*) this );
 int i;
 TJLterm<T1,T2>* p;
 dlink* q;
 
 fprintf( filePtr,
          "\n_Count  = %d, Weight = %d, Max accurate weight = %d\n",
          _count, _weight, _accuWgt );
 fprintf( filePtr, "Reference point:  \n" );
 for( i = 0; i < _myEnv->_numVar; i++ )
   fprintf( filePtr, "%e  ", _myEnv->_refPoint[i] );
 fprintf( filePtr, "\n" );
 while((  q = getNext()  )) {
   p = (TJLterm<T1,T2>*) q->info();
   fprintf( filePtr, "Weight: %d   Value: %e  || ",
           p -> _weight, p -> value );
   fprintf( filePtr, "Addresses: %d %d %d : %d\n",
                  q->prevPtr(), q, q->nextPtr(), p );
   fprintf( filePtr, "Index:  ");
   for( i = 0; i < _myEnv->_numVar; i++ )
     fprintf( filePtr, "%d  ", (p -> index)(i) );
   fprintf( filePtr, "\n\n");
   }
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJL<T1,T2>::_scaleBy( double y ) 
{
 TJLterm<T1,T2>* p;
 dlist_iterator getNext( *(dlist*) this );
 
 if ( this->_count < 1 ) {
   cout << "\n\n*** WARNING::TJL<T1,T2>::_scaleBy: function invoked \n"
        <<     "    by null JL variable with address " << (int) this 
        << endl;
   return;
   }
 
 while((  p = (TJLterm<T1,T2>*) getNext()  ))   p->value *= y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJL<T1,T2>::setVariable( const double& x, 
                      const int& j, 
                            TJetEnvironment<T1,T2>* theEnv )
{
 if( theEnv != 0 ) _myEnv = theEnv;

 if( _myEnv == 0 ) {
   throw( TJL<T1,T2>::GenericException( __FILE__, __LINE__, 
     "void TJL<T1,T2>::setVariable( const double&, const int&, TJetEnvironment<T1,T2>* )", 
     "Private data _myEnv is null: object has no environment assigned.") );
 }

 clear();

 if( ( j < 0 ) || ( j >= _myEnv->_numVar ) ) return;

 IntArray ndx( _myEnv->_numVar );  // Assumes: initialized with zeroes
 insert( new TJLterm<T1,T2>( ndx, x, _myEnv ) );
 ndx(j) = 1;
 append( new TJLterm<T1,T2>( ndx, 1.0, _myEnv ) );
 
 _myEnv->_refPoint[j] = x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJL<T1,T2>::setVariable( const int& j, 
                            TJetEnvironment<T1,T2>* theEnv ) 
{
 if( theEnv != 0 ) _myEnv = theEnv;

 if( _myEnv == 0 ) {
   throw( TJL<T1,T2>::GenericException( __FILE__, __LINE__, 
     "void TJL<T1,T2>::setVariable( const int&, TJetEnvironment<T1,T2>* )", 
     "Private data _myEnv is null: object has no environment assigned.") );
 }

 clear();
 if( ( j < 0 ) || ( j >= _myEnv->_numVar ) ) return;
 
 double x = theEnv->_refPoint[j];
 IntArray ndx( _myEnv->_numVar );
 insert( new TJLterm<T1,T2>( ndx, x, theEnv ) );
 ndx(j) = 1;
 append ( new TJLterm<T1,T2>( ndx, 1.0, theEnv ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJL<T1,T2>::insert( TJLterm<T1,T2>* a) 
{
 dlist::insert(a);
 _count++;
 if( _weight < (a->_weight) ) _weight = a->_weight;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJLterm<T1,T2>* TJL<T1,T2>::get() 
{
 TJLterm<T1,T2>* p;
 if( _count <= 0 ) return 0;
 p = ((TJLterm<T1,T2>*) dlist::get());
 if( --_count == 0 ) _weight = -1;
 return p;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJLterm<T1,T2> TJL<T1,T2>::lowTerm() const 
{
 dlist_iterator getNext( *(dlist*) this );
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* result;

 p = (TJLterm<T1,T2>*) getNext();
 result = 0;
   
 while ( (result == 0) && (p!=0) ) 
 {
   if( p->value != 0.0 ) result = p;
   p = (TJLterm<T1,T2>*) getNext();
 }

 if( result == 0 ) return TJLterm<T1,T2>( this->_myEnv->_allZeroes, 0.0, this->_myEnv );
 else              return TJLterm<T1,T2>( *result );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJLterm<T1,T2> TJL<T1,T2>::firstTerm() const 
{
 static TJLterm<T1,T2>* p;
 p = (TJLterm<T1,T2>*) ( ((dlist*) this)->firstInfoPtr() );
 if( 0 != p ) {
   return TJLterm<T1,T2>( *p );
 }
 else {
   return TJLterm<T1,T2>( this->_myEnv->_allZeroes, 0.0, this->_myEnv );
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJL<T1,T2>::append( TJLterm<T1,T2>* a) 
{
 dlist::append(a);
 _count++;
 if( _weight < (a->_weight) ) _weight = a->_weight;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJLterm<T1,T2>* TJL<T1,T2>::remove( dlink* w ) 
{
 TJLterm<T1,T2>* a;
 dlink* predecessor;
 dlink* successor;
 int predWeight;
 int succWeight;
 
 predecessor = w->prevPtr();
 successor   = w->nextPtr();
 
 a = (TJLterm<T1,T2>*) dlist::remove( w );
 _count--;
 
 if( _count == 0 ) {
   _weight = -1;
   return a;
   }
 
 if ( _weight > a->_weight ) return a;
 
 predWeight = ( (TJLterm<T1,T2>*) ( predecessor->info() ) ) -> _weight;
 succWeight = ( (TJLterm<T1,T2>*) ( successor->info() ) ) -> _weight;
 _weight = predWeight;
 if( succWeight > _weight ) _weight = succWeight;
 return a;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
double TJL<T1,T2>::standardPart() const 
{
 TJLterm<T1,T2>* p; 
 if( _count < 1 )       return 0.0;
 dlist_iterator g( (dlist&) *this );
 p = (TJLterm<T1,T2>*) g();
 if( p->_weight  == 0 ) return p->value;
                       return 0.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void TJL<T1,T2>::clear() 
{
 TJLterm<T1,T2>* p;
 while((  p = get()  ))  
   delete p;
 dlist::clear();
 _count   = 0;
 _weight  = -1;
 if( _myEnv ) _accuWgt = _myEnv->_maxWeight;
 else        _accuWgt = 100000;   // ??? Better way?
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
double TJL<T1,T2>::weightedDerivative( const int* ind ) const 
{
 int i;
 TJLterm<T1,T2>* p;
 char theOne;
 
 if( _count < 1 ) return 0.0;
 
 dlist_iterator getNext( (dlist&) *this );

 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   theOne = 1;
   for ( i = 0; i < _myEnv->_numVar; i++ )
     theOne = theOne && ( (p->index)(i) == ind[i] );
   if( theOne ) return p->value;
 }
 
 return 0.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
double TJL<T1,T2>::derivative( const int* ind ) const 
{
 static double n;
 static double multiplier;
 static double d;
 static int i;
 
 d = weightedDerivative( ind );
 if ( d == 0.0 ) return 0.0;
 
 multiplier = 1.0;
 for ( i = 0; i < _myEnv->_numVar; i++ ) {
   n = ind[i];
   while(n)  multiplier *= n--;
   }
 
 return multiplier*d;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
double TJL<T1,T2>::operator()( const double* x )  const 
{
 // This routine is linked to double TJL<T1,T2>::operator()( const Vector& x ) const
 // Any change made to this must be reflected in the other.
 TJLterm<T1,T2>* p;
 double v;
 int w;
 int i;
 double term;
 double* u;
 dlist_iterator getNext( *(dlist*) this );
 
 u = new double[ _myEnv->_numVar ];
 
 // Subtract off the reference point.
 for( i = 0; i < _myEnv->_numVar; i++ ) u[i] = x[i] - _myEnv->_refPoint[i];
 
 // Evaluate and store _monomials.
 
 // The zeroth one.
 _myEnv->_monomial[0] = 1.0;
 
 // For all higher weights ...
 for( w = 1; w <= _weight; w++ )
 
   // Get the next set of _exponents of weight w.
   while( nexcom( w, _myEnv->_numVar, _myEnv->_exponent ) ) {
 
     // Find the first non-zero _exponent.
     i = 0;
     while( !( _myEnv->_exponent[i++] ) ) ;
     i--;
 
     // The value of the _monomial associated with this composition
     // is obtained by multiplying a factor into a previously
     // computed _monomial.
     ( _myEnv->_exponent[i] )--;
     _myEnv->_monoCode();
     term = _myEnv->_monomial[ _myEnv->_monoRank() ];
     ( _myEnv->_exponent[i] )++;
     _myEnv->_monoCode();
     _myEnv->_monomial[ _myEnv->_monoRank() ] = term * u[i];
 
   }
 
 // Monomials have been now stored at this point.
 // Now traverse the JL variable and evaluate.
 
 v = 0.0;
 
 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   for( i = 0; i < _myEnv->_numVar; i++ ) _myEnv->_exponent[i] = (p->index)(i);
   _myEnv->_monoCode();
   v += ( p->value ) * ( _myEnv->_monomial[ _myEnv->_monoRank() ] );
 }
 
 delete [] u;
 
 return v;
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
double TJL<T1,T2>::operator()( const Vector& x ) const 
{
 // This routine is linked to double TJL<T1,T2>::operator()( double* x ) 
 // Any change made to this must be reflected in the other.
 TJLterm<T1,T2>* p;
 double v;
 int w;
 int i;
 double term;
 double* u;
 dlist_iterator getNext( *(dlist*) this );
 
 u = new double[ _myEnv->_numVar ];
 
 // Subtract off the reference point.
 for( i = 0; i < _myEnv->_numVar; i++ ) u[i] = x(i) - _myEnv->_refPoint[i];
 
 // Evaluate and store _monomials.
 
 // The zeroth one.
 _myEnv->_monomial[0] = 1.0;
 
 // For all higher weights ...
 for( w = 1; w <= _weight; w++ )
 
   // Get the next set of _exponents of weight w.
   while( nexcom( w, _myEnv->_numVar, _myEnv->_exponent ) ) {
 
     // Find the first non-zero _exponent.
     i = 0;
     while( !( _myEnv->_exponent[i++] ) ) ;
     i--;
 
     // The value of the _monomial associated with this composition
     // is obtained by multiplying a factor into a previously
     // computed _monomial.
     ( _myEnv->_exponent[i] )--;
     _myEnv->_monoCode();
     term = _myEnv->_monomial[ _myEnv->_monoRank() ];
     ( _myEnv->_exponent[i] )++;
     _myEnv->_monoCode();
     _myEnv->_monomial[ _myEnv->_monoRank() ] = term * u[i];
 
   }
 
 // Monomials have been now stored at this point.
 // Now traverse the JL variable and evaluate.
 
 v = 0.0;
 
 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   for( i = 0; i < _myEnv->_numVar; i++ ) _myEnv->_exponent[i] = (p->index)(i);
   _myEnv->_monoCode();
   v += ( p->value ) * ( _myEnv->_monomial[ _myEnv->_monoRank() ] );
 }
 
 delete [] u;
 
 return v;
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      The overloaded operators for class JL
//

template<typename T1, typename T2>
istream& operator>>( istream& is,  JL& x ) 
{  // ??? This function shouldn't
   // ??? be here.
  char buf[100];
  int i,j;
  int count;
  double value;
  TJLterm<T1,T2>* q;

  x.clear();

  is >> buf;
  is >> buf;
  is >> count;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> x._weight;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> buf;
  is >> x._accuWgt;
  is >> buf;
  is >> buf;
  streamIn( is, &(x._myEnv) );
  is >> buf;
  is >> buf;
  IntArray ndx( x._myEnv->_numVar );
  for( j = 0; j < count; j++) {
    is >> buf;
    for( i = 0; i < x._myEnv->_numVar; i++ )  {
     is >> ndx(i);
    }
    is >> buf;
    is >> value;
    q = new TJLterm<T1,T2>( ndx, value, x._myEnv );
    x.append( q );
  }

 return is;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
ostream& operator<<( ostream& os, const JL& x ) 
{
 dlist_traversor getNext( (dlist&) x );
 int i;
 TJLterm<T1,T2>* p;
 dlink* q;

 os << "\n Count = " << x._count << " , Weight = " << x._weight;
 os << " , Max accurate weight = " << x._accuWgt << endl;
 os << "Begin Environment: \n"
    << *(x._myEnv)
    << "\nEnd Environment: " << endl;

 while((  q = getNext()  )) {
   p = (TJLterm<T1,T2>*) q->info();
   os << "Index: ";
   for( i = 0; i < x._myEnv->_numVar; i++ )
     os << p -> index(i) << " ";
   os << "   Value: " << setprecision(30) << p -> value << endl;
   // os << "   Value: " << p -> value << endl;
 }
 return os << "\n" << endl;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
char operator==( const JL& x, const JL& y ) 
{
 if( ( x._count   != y._count )   ||
     ( x._weight  != y._weight )  ||
     ( x._accuWgt != y._accuWgt )
   ) return 0;
 
 if( x._myEnv != y._myEnv ) return 0;

 dlist_iterator getNextX( (dlist&) x );
 dlist_iterator getNextY( (dlist&) y );
 TJLterm<T1,T2>* p;
 TJLterm<T1,T2>* q;
 
 while((  p = (TJLterm<T1,T2>*) getNextX()  )) {
   if( !( q = (TJLterm<T1,T2>*) getNextY() ) ) return 0;
   if( !( *p == *q ) ) return 0;
   }
 
 if( (TJLterm<T1,T2>*) getNextY() ) {
   printf( "\n*** ERROR: Inconsistency in JL operator==\n" );
     // This point should never be reached, since that would imply that
     // x._count != y._count
   return 0;
   }
 
 return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
char operator==( const JL& x, const double& y ) 
{
 char result = 1;
 TJLterm<T1,T2>* p;

 if( x._count < 1 ) {
  if( y == 0.0 ) return 1;
  else           return 0;
 }

 dlist_iterator getNext( (dlist&) x );

 while((  p = (TJLterm<T1,T2>*) getNext()  )) 
  result = result && ( p->_weight == 0 ? p->value == y : 
                                        p->value == 0.0 );

  // REMOVE ??? result = result && ( ( ( p->_weight == 0 ) && ( p->value == y   ) )
  // REMOVE ???                      ||
  // REMOVE ???                      ( ( p->_weight != 0 ) && ( p->value == 0.0 ) )
  // REMOVE ???                    )
  // REMOVE ??? 
 return result;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
char operator==( const double& y, const JL& x )
{
 return x == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
char operator!=( const JL& x, const JL& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
char operator!=( const JL& x, const double& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
char operator!=( const double& x, const JL& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
JL& TJL<T1,T2>::operator=( const double& x ) 
{
 static int* ndx;
 
 clear();
 _weight = 0;   // _rc is purposely left untouched!!
 _count  = 0;
 if( _myEnv ) _accuWgt = _myEnv->_maxWeight;
 else {
   throw( TJL<T1,T2>::GenericException( __FILE__, __LINE__, 
     "JL& TJL<T1,T2>::operator=( const double& x ) {", 
     "Private data _myEnv is null: object has no environment assigned.") );
 }

 insert( new TJLterm<T1,T2>( IntArray( _myEnv->_numVar ), x, _myEnv ) );

 delete [] ndx;
 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
JL& TJL<T1,T2>::operator=( const JL& x ) 
{
 static TJLterm<T1,T2>* p;
 static TJLterm<T1,T2>* q;

 if( this == &x ) return *this;
 
 clear();

 dlist_iterator getNext( (dlist&) x );

 while((  p = (TJLterm<T1,T2>*) getNext()  )) {
   q = new TJLterm<T1,T2>( p );
   append( q );
 }

 _count   = x._count;     // _rc is purposely left untouched
 _weight  = x._weight;
 _accuWgt = x._accuWgt;
 _myEnv   = x._myEnv;

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
JL& TJL<T1,T2>::operator+=( const double& x ) {   // ??? Untested!!
 static char firstNull = 1;
 int i;
 // ??? REMOVE dlist_iterator getNext( *(dlist*) this );
 TJLterm<T1,T2>* p;                               
 TJLterm<T1,T2>* q;

 // ??? REMOVE if( p = (TJLterm<T1,T2>*) getNext() )
 if( p = (TJLterm<T1,T2>*) firstInfoPtr() )
 {
   if ( p->_weight == 0 ) { 
    p->value += x;
    if( std::abs( p->value ) <  MX_SMALL*std::abs(x) ) p->value = 0.0;
   }
 
   else {
     q = new TJLterm<T1,T2>( _myEnv );
     for( i = 0; i < _myEnv->_numVar; i++ ) q->index(i) = 0;
     q->_weight = 0;
     q->value = x;
     insert( q );
   }
 }
 else
 {
   if( firstNull ) {
     cerr << "*** WARNING ***                                 \n"
             "*** WARNING *** JL& TJL<T1,T2>::operator+=( const double& )\n"
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

template<typename T1, typename T2>
TJLterm<T1,T2> operator*( const TJLterm<T1,T2>& x, const TJLterm<T1,T2>& y ) 
{
 TJLterm<T1,T2> z(x);
 static int i, n;

 if((  ( n = x.index.Dim() ) != y.index.Dim()  )) {
   throw( TJL<T1,T2>::BadDimension( x.index.Dim(), y.index.Dim(),
                            __FILE__, __LINE__, 
                            "TJLterm<T1,T2> operator*( const TJLterm<T1,T2>& x, const TJLterm<T1,T2>& y )",
                            "Inconsistent number of coordinates." ) );
 }

 z._weight = x._weight + y._weight;
 for( i = 0; i < n; i++ ) 
   z.index(i) = x.index(i) + y.index(i);
 z.value = x.value * y.value;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Implementation of Class TJLterm<T1,T2>

template<typename T1, typename T2>
TJLterm<T1,T2>::TJLterm<T1,T2>( const TJetEnvironment<T1,T2>* pje ) 
: index( pje->_numVar ), _weight(0), value(0.0)
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJLterm<T1,T2>::TJLterm<T1,T2>( const IntArray& l, 
                const double& x, 
                const TJetEnvironment<T1,T2>* pje ) 
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
   if( n != pje->_numVar ) {
     throw( TJL<T1,T2>::GenericException( __FILE__, __LINE__, 
            "TJLterm<T1,T2>::TJLterm<T1,T2>( const IntArray&, const double&, const TJetEnvironment<T1,T2>*",
            "Dimensions are wrong.") );
   }
  
   // ??? These checks could be removed for speed.
   dpt = 0;
   for( i = 0; i < n; i++ ) {
     if( (l(i) < 0) ) {
       throw( TJL<T1,T2>::GenericException( __FILE__, __LINE__, 
              "TJLterm<T1,T2>::TJLterm<T1,T2>( const IntArray&, const double&, const TJetEnvironment<T1,T2>*",
              "Bad index in JLTerm.") );
     }
     dpt += l(i);
   }
  
   if( dpt > pje->_maxWeight ) {
     throw( TJL<T1,T2>::GenericException( __FILE__, __LINE__, 
            "TJLterm<T1,T2>::TJLterm<T1,T2>( const IntArray&, const double&, const TJetEnvironment<T1,T2>*",
            "Attempt to load a JLTerm with too large a weight.") );
   }
   
   // ??? REMOVE: index = l;
   _weight = dpt;
   value = x;
 }

 else {
   if( l.Dim() != 1 ) {
     throw( TJL<T1,T2>::GenericException( __FILE__, __LINE__, 
            "TJLterm<T1,T2>::TJLterm<T1,T2>( const IntArray&, const double&, const TJetEnvironment<T1,T2>*",
            "Inconsistency between l and pje") );
   }
   if( l(0) != 0 ) {
     throw( TJL<T1,T2>::GenericException( __FILE__, __LINE__, 
            "TJLterm<T1,T2>::TJLterm<T1,T2>( const IntArray&, const double&, const TJetEnvironment<T1,T2>*",
            "Bad value of the index when pje = 0.") );
   }
   index = l;
   _weight = l.Sum();
   value = x;
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#ifdef __PRIVATE_ALLOCATOR__

template<typename T1, typename T2>
void TJLterm<T1,T2>::meminit(size_t size) 
{

  _vmem = vmopen(Vmdcsbrk,Vmpool,0);
  //_vmem = vmopen(Vmdcsbrk,Vmpool,VM_TRACE);
  //int fd =  creat("vmalloc.log", 666);
  //vmtrace(fd);
  vmalloc(_vmem,size);
  _init = 1;
};

template<typename T1, typename T2>
void* TJLterm<T1,T2>::operator new(size_t size)
{
 if (_init ==0) meminit(size);
 return vmalloc(_vmem,size);
};

template<typename T1, typename T2>
void TJLterm<T1,T2>::operator delete(void* obj, size_t size)
{
  vmfree(_vmem,obj);
};
#endif


template<typename T1, typename T2>
TJLterm<T1,T2>::TJLterm<T1,T2>( const TJLterm<T1,T2>* x ) 
: index( x->index ), _weight(x->_weight), value(x->value)
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJLterm<T1,T2>::TJLterm<T1,T2>( const TJLterm<T1,T2>& x ) 
: index( x.index ), _weight(x._weight), value(x.value)
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TJLterm<T1,T2>::~TJLterm<T1,T2>() 
{
}



// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Overloaded operators for class TJLterm<T1,T2>

template<typename T1, typename T2>
void TJLterm<T1,T2>::operator=( const TJLterm<T1,T2>& x ) 
{

 _weight = x._weight;
 value  = x.value;
 index  = x.index;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
char operator==( const TJLterm<T1,T2>& a, const TJLterm<T1,T2>& b ) 
{
 if( a._weight != b._weight ) return 0;
 if( a.value  != b.value  ) return 0;
 if( a.index  != b.index  ) return 0;
 return 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
char operator!=( const TJLterm<T1,T2>& a, const TJLterm<T1,T2>& b ) 
{
 return !( a == b );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
char operator<=( const TJLterm<T1,T2>& a, const TJLterm<T1,T2>& b ) 
{
 int i;
 if( a.index.Dim() != b.index.Dim() ) {
   throw( TJL<T1,T2>::BadDimension( a.index.Dim(), b.index.Dim(), 
          __FILE__, __LINE__, 
          "char operator<=( const TJLterm<T1,T2>&, const TJLterm<T1,T2>& )",
          "Dimensions don't match.") );
 }
 if( a._weight != b._weight ) { return ( a._weight < b._weight ); }
 for( i = 0; i < a.index.Dim(); i++ ) {
   if( a.index(i) == b.index(i) ) continue;
   return ( a.index(i) < b.index(i) );
 }
 return 1;  // when all indices are the same.
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
char operator%=( const TJLterm<T1,T2>& a, const TJLterm<T1,T2>& b ) 
{
 if( a._weight != b._weight ) return 0;
 return a.index == b.index;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
*/


template TJL<double,FNAL::Complex>;
template TJL<FNAL::Complex,double>;
