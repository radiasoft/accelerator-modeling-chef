/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJL.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
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
******  Revision History:
******
******  Feb 2005 - Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******  
******             - Efficiency improvements.
******             - New memory management.
******
******  Sept 2005   ostiguy@fnal.gov
******              - new code based on a single template parameter
******                instead of two. Mixed mode handled
******                using conversion operators.
******  
******                                                                
**************************************************************************
*************************************************************************/
#ifndef TJL_TCC
#define TJL_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <fstream>
#include <iosetup.h>

#include <GenericException.h>
#include <TJL.h>
#include <new>

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
using FNAL::pcerr;
using FNAL::pcout;

// ================================================================
//      Implementation of exceptions

template<typename T>
TJL<T>::BadDimension::BadDimension( int i, int j, 
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
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

template<typename T>
const char* TJL<T>::BadDimension::what() const throw()
{
  return errorString.c_str();
}

template<typename T>
TJL<T>::BadEnvironment::BadEnvironment( int a, int b, int c, 
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
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

template<typename T>
const char* TJL<T>::BadEnvironment::what() const throw()
{
  return errorString.c_str();
}

template<typename T>
TJL<T>::BadReference::BadReference( int ndx, double u,
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
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

template<typename T>
const char* TJL<T>::BadReference::what() const throw()
{
  return errorString.c_str();
}

template<typename T>
TJL<T>::HorribleException::HorribleException( int i, int j, int k, 
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
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

template<typename T>
const char* TJL<T>::HorribleException::what() const throw()
{
  return errorString.c_str();
}

template<typename T>
TJL<T>::HideousException::HideousException( int x, int y, 
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
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

template<typename T>
const char* TJL<T>::HideousException::what() const throw()
{
  return errorString.c_str();
}


// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Implementation of Class TJL

//    Constructors and destructors    |||||||||||||||||||||||||||


template<typename T>
void TJL<T>::initStore( int capacity) {

  _jltermStoreCapacity    = capacity;
  _jltermStore            =  TJLterm<T>::array_allocate( _jltermStoreCapacity ); 
  _jltermStoreCurrentPtr  = _jltermStore;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::initStore( ) {

  _jltermStoreCapacity    = 16;
  _jltermStore            = TJLterm<T>::array_allocate( _jltermStoreCapacity );
  _jltermStoreCurrentPtr  = _jltermStore;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL( TJetEnvironment<T>* pje ) 
{
 _rc      =  1;
 _weight  = -1;
 _count   =  0;

 initStore();   

 if( pje ) {
   _myEnv   = pje;
   _accuWgt = pje->_maxWeight;

 }
 else {
   _myEnv   = 0;
   _accuWgt = 100000;      // ??? Better way?
 }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>* TJL<T>::makeTJL( TJetEnvironment<T>* pje ) 
{

 if ( _thePool.empty() ) return ( new TJL<T>( pje ) );
 
  TJL<T>* p =  _thePool.back(); _thePool.pop_back();

  p->_rc      =  1;
  p->_weight  = -1;
  p->_count   =  0;

 if( pje ) {
   p->_myEnv   = pje;
   p->_accuWgt = pje->_maxWeight;

 }
 else {
   p->_myEnv   = 0;
   p->_accuWgt = 100000;      // ??? Better way?
 }

 return p;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL( const T& x, TJetEnvironment<T>* pje ):
_jltermStoreCapacity(0), _jltermStore(0), _jltermStoreCurrentPtr(0) 
{

 this->clear();
 _rc = 1;
 _weight = 0;
 _count = 0;
 _myEnv   = pje;


 initStore();

 if( pje ) {
   _accuWgt = pje->_maxWeight;
   insert( new( storePtr() ) TJLterm<T>( IntArray( pje->_numVar ), x, pje ) );

 }
 else {
   _accuWgt = 100000;    // ??? Better way?
   insert( new( storePtr() ) TJLterm<T>( IntArray(1), x, 0 ) );
 }

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>* TJL<T>::makeTJL( const T& x, TJetEnvironment<T>* pje )
{

 if (_thePool.empty() ) return (new TJL<T>(x,pje)); 
 
 TJL<T>* p = _thePool.back();  _thePool.pop_back();
 
  p->_rc     = 1;
  p->_weight = 0;
  p->_count  = 0;
  p->_myEnv  = pje;


 if( pje ) {
   p->_accuWgt = pje->_maxWeight;
   p->insert( new( p->storePtr() ) TJLterm<T>( IntArray( pje->_numVar ), x, pje ) );

 }
 else {
   p->_accuWgt = 100000;    // ??? Better way?
   p->insert( new( p->storePtr() ) TJLterm<T>( IntArray(1), x, 0 ) );
 }

 return p;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL( const IntArray& e, const T& x, TJetEnvironment<T>* pje ) :
_jltermStoreCapacity(0), _jltermStore(0), _jltermStoreCurrentPtr(0) 
{

  this->clear(); // ??? Why is this needed?
 _rc = 1;
 _weight = 0;
 _count = 0;
 _myEnv   = pje;


 initStore();

 if( pje ) {
   _accuWgt = pje->_maxWeight;
   insert( new( storePtr() ) TJLterm<T>( e, x, pje ) );
 }
 else {
   _accuWgt = 100000;    // ??? Better way?
   insert( new( storePtr() ) TJLterm<T>( IntArray(1), x, 0 ) );
 }


}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>* TJL<T>::makeTJL( const IntArray& e, const T& x, TJetEnvironment<T>* pje ) 
{

  if (_thePool.empty() ) return new TJL<T>(e,x,pje);

  TJL<T>* p = _thePool.back(); _thePool.pop_back();

  p->_rc = 1;
  p->_weight = 0;
  p->_count = 0;
  p->_myEnv   = pje;

 if( pje ) {
   p->_accuWgt = pje->_maxWeight;
   p->insert( new( p->storePtr() ) TJLterm<T>( e, x, pje ) );
 }
 else {
   p->_accuWgt = 100000;    // ??? Better way?
   p->insert( new( p->storePtr() ) TJLterm<T>( IntArray(1), x, 0 ) );
 }

 return p;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL( const TJL& x ):
_jltermStoreCapacity(0), _jltermStore(0), _jltermStoreCurrentPtr(0) 
 
{

 _count    = x._count;   // needed by append function
 _weight   = x._weight;  // needed by append function
 _accuWgt  = x._accuWgt;
 _myEnv    = x._myEnv;
 _rc       = 1;

 
 initStore( x._jltermStoreCapacity );
 memcpy( _jltermStore, x._jltermStore, (x._jltermStoreCurrentPtr-x._jltermStore)*sizeof(TJLterm<T>) );
  _jltermStoreCurrentPtr = _jltermStore + (x._jltermStoreCurrentPtr - x._jltermStore);


 dlist_iterator getNext( x._theList );
 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;

 while((  p = (TJLterm<T>*) getNext()  )) {
          q = _jltermStore + ( p - x._jltermStore); 
          append(q);
 }

 _count    = x._count;    // gets reset by append function
 _weight   = x._weight;   // may get reset by append function
 _accuWgt  = x._accuWgt;
 _myEnv    = x._myEnv;
 _rc       = 1;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>* TJL<T>::makeTJL( const TJL& x )
{

  if (_thePool.empty() ) return new TJL<T>(x);
 
  TJL<T>* p = _thePool.back(); _thePool.pop_back(); 
  
  if (p->_jltermStoreCapacity < x._jltermStoreCapacity)  
  { 
       TJLterm<T>::array_deallocate( p->_jltermStore );
       p->initStore(x._jltermStoreCapacity);   
  
  }

  p->_count    = x._count;   // needed by append function
  p->_weight   = x._weight;  // needed by append function
  p->_accuWgt  = x._accuWgt;
  p->_myEnv    = x._myEnv;
  p->_rc       = 1;

 
   memcpy( p->_jltermStore, x._jltermStore, (x._jltermStoreCurrentPtr-x._jltermStore)*sizeof(TJLterm<T>) );
   p->_jltermStoreCurrentPtr = p->_jltermStore + (x._jltermStoreCurrentPtr - x._jltermStore);


 dlist_iterator getNext( x._theList );
 
 TJLterm<T>* q = 0;
 TJLterm<T>* r = 0;

 while((  q = (TJLterm<T>*) getNext()  )) {
          r = p->_jltermStore + ( q - x._jltermStore); 
          p->append(r);
 }

 p->_count    = x._count;    // gets reset by append function
 p->_weight   = x._weight;   // may get reset by append function
 p->_accuWgt  = x._accuWgt;
 p->_myEnv    = x._myEnv;
 p->_rc       = 1;

 return p;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL( const TJL* x ) 
{


 dlist_iterator getNext( x->_theList );
 TJLterm<T>* p;
 TJLterm<T>* q;

 _count    = x->_count;
 _weight   = x->_weight;
 _accuWgt  = x->_accuWgt;
 _myEnv    = x->_myEnv;
 _rc       = 1;

 initStore( x->_jltermStoreCapacity );
 memcpy( _jltermStore, x->_jltermStore, (x->_jltermStoreCurrentPtr - x->_jltermStore)*sizeof(TJLterm<T>) );
 _jltermStoreCurrentPtr = _jltermStore + (x->_jltermStoreCurrentPtr - x->_jltermStore);

 while((  p = (TJLterm<T>*) getNext()  )) {
         q = _jltermStore + ( p - x->_jltermStore); 
         append(q); 
 }
 
 
 _count    = x->_count;
 _weight   = x->_weight;
 _accuWgt  = x->_accuWgt;
 _myEnv    = x->_myEnv;
 _rc       = 1;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>*  TJL<T>::makeTJL( const TJL* x ) 
{

 
 if (_thePool.empty() ) return new TJL<T>(x);
 
  TJL<T>* p = _thePool.back(); _thePool.pop_back(); 
  
  if (p->_jltermStoreCapacity < x->_jltermStoreCapacity)  
  { 
       TJLterm<T>::array_deallocate( p->_jltermStore );
       p->initStore(x->_jltermStoreCapacity);   
  };


  p->_count    = x->_count;
  p->_weight   = x->_weight;
  p->_accuWgt  = x->_accuWgt;
  p->_myEnv    = x->_myEnv;
  p->_rc       = 1;


  memcpy( p->_jltermStore, x->_jltermStore, (x->_jltermStoreCurrentPtr - x->_jltermStore)*sizeof(TJLterm<T>) );
  p->_jltermStoreCurrentPtr = p->_jltermStore + (x->_jltermStoreCurrentPtr - x->_jltermStore);

 dlist_iterator getNext( x->_theList );
 TJLterm<T>* q;
 TJLterm<T>* r;

 while((  q = (TJLterm<T>*) getNext()  )) {
          r = p->_jltermStore + ( q - x->_jltermStore); 
          p->append(r); 
 }
 
 
  p->_count    = x->_count;
  p->_weight   = x->_weight;
  p->_accuWgt  = x->_accuWgt;
  p->_myEnv    = x->_myEnv;
  p->_rc       = 1;

  return p;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::~TJL() 
{
 clear();
 TJLterm<T>::array_deallocate( _jltermStore );

 _jltermStore         = _jltermStoreCurrentPtr = 0;
 _jltermStoreCapacity = 0;
 

 // _myEnv is purposely NOT deleted, of course.
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::discardTJL( TJL<T>* p) 
{

   p->clear();
   _thePool.push_back(p);

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//     Member functions(public)  |||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>* TJL<T>::storePtr( ) {
 
TJLterm<T>* p                         = 0;
TJLterm<T>* q                         = 0;
TJLterm<T>* old_jltermStore           = 0;
TJLterm<T>* old_jltermStoreCurrentPtr = 0;
int             old_jltermStoreCapacity  = 0;

  if ( (_jltermStoreCurrentPtr-_jltermStore) < _jltermStoreCapacity ) 

     return _jltermStoreCurrentPtr++;

  else {

  // grow the allocation vector 

    old_jltermStore           = _jltermStore;
    old_jltermStoreCurrentPtr = _jltermStoreCurrentPtr;
    old_jltermStoreCapacity   = _jltermStoreCapacity;

    _jltermStoreCapacity *= 2;  // double the capacity
    _jltermStore = _jltermStoreCurrentPtr = TJLterm<T>::array_allocate( _jltermStoreCapacity );
 
  // We DO NOT do a memcpy here, so as to get rid of deleted jlterms. This is done
  // to prevent the size of the allocation vector to grow indefinitely.  


  // first copy the old list
    
     dlist oldList;
  
     dlist_iterator getNext( _theList );
     while((  p = (TJLterm<T>*) getNext()  )) {
          oldList.append(p);
    };

     _theList.clear();

     dlist_iterator getNext2( oldList );
     while((  p = (TJLterm<T>*) getNext2()  )) {
          q = new ( _jltermStoreCurrentPtr++ ) TJLterm<T>( *p );
          append(q);
    };

    // now, it is ok to zap the old store
 
    TJLterm<T>::array_deallocate( old_jltermStore );

    // finally, return a ptr to the next available block 

    return  _jltermStoreCurrentPtr++;
  }

}   


template<typename T>
void TJL<T>::addTerm( TJLterm<T>* a) 
{
 // If the value of *a is 0, don't bother with it unless
 // the _weight is also 0.
 if( ( a -> _value == T() ) && ( a -> _weight != 0 ) ) {
   a->_deleted =true; // NOTE: explicit deletion of an individual JLterm is NOT allowed. 
   return;
 }

 dlist_traversor getNext( _theList );
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
 
 if(     *a   <=   ( *(TJLterm<T>*) (p->info()) )   ) {
   if(   *a   %=   ( *(TJLterm<T>*) (p->info()) )   ) {
 
     (( (TJLterm<T>*) (p->info()) ) -> _value ) += ( a -> _value );
 
     if(
         (
           std::abs( ( (TJLterm<T>*) (p->info()) ) -> _value ) < MX_SMALL*std::abs( a -> _value )
         )
         // REMOVE ??? &&
         // REMOVE ??? (
         // REMOVE ???   p != ( dlist::lastPtr() ) -> nextPtr()
         // REMOVE ??? )
       )
 
       remove( p )->_deleted = true; // NOTE: explicit deletion of an individual JLterm is not allowed 
 
     a->_deleted = true; // NOTE: explicit deletion of an individual JLterm is not allowed 
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
 
   if(     *a   <=   ( *(TJLterm<T>*) (p->info()) )   ) {
     if(   *a   %=   ( *(TJLterm<T>*) (p->info()) )   ) {
 
       (( (TJLterm<T>*) (p->info()) ) -> _value ) += ( a -> _value );
 
       if(
	   (
	     std::abs( ( (TJLterm<T>*) (p->info()) ) -> _value ) < MX_SMALL*std::abs( a -> _value )
	   )
	   // REMOVE ??? &&
	   // REMOVE ??? (
	   // REMOVE ???   p != ( dlist::lastPtr() ) -> nextPtr()
	   // REMOVE ??? )
	 )
 
         //delete remove( p );
         remove( p )->_deleted = true;
         a->_deleted = true; // NOTE: explicit deletion of an individual JLterm is not allowed 
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

template<typename T>
void TJL<T>::addTerm( TJLterm<T>* a, dlist_traversor& getNext ) 
{
 // JUST A TEST: THIS IS NOT WRITTEN CORRECTLY.
 // IT SHOULD NOT BE USED.

 throw( GenericException( __FILE__, __LINE__, 
   "void TJL<T>::addTerm( TJLterm<T>* a, dlist_traversor& getNext )", 
   "This method is broken.") );


 // If the value of *a is 0, don't bother with it unless
 // the _weight is also 0.
 if( ( a -> _value == 0.0 ) && ( a -> _weight != 0 ) ) {
   a->_deleted = true; // NOTE: explicit deletion of an individual JLterm is not allowed
   return;
 }

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
 
 if(     *a   <=   ( *(TJLterm<T>*) (p->info()) )   ) {
   if(   *a   %=   ( *(TJLterm<T>*) (p->info()) )   ) {
 
     (( (TJLterm<T>*) (p->info()) ) -> _value ) += ( a -> _value );
 
     if( std::abs( ( (TJLterm<T>*) (p->info()) ) -> _value ) 
           < MX_SMALL*std::abs( a -> _value ) ) {
      remove( p )->_deleted = true; // NOTE: explicit deletion of an individual JLterm is not allowed 
     } 
     a->_deleted = true; // NOTE: explicit deletion of an individual JLterm is not allowed
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
 while((  p = getNext()  ))
 {
   if(     *a   <=   ( *(TJLterm<T>*) (p->info()) )   ) {
     if(   *a   %=   ( *(TJLterm<T>*) (p->info()) )   ) {
 
       (( (TJLterm<T>*) (p->info()) ) -> _value ) += ( a -> _value );
 
       if(
	   (
	     std::abs( ( (TJLterm<T>*) (p->info()) ) -> _value ) < MX_SMALL*std::abs( a -> _value )
	   )
	   // REMOVE ??? &&
	   // REMOVE ??? (
	   // REMOVE ???   p != ( dlist::lastPtr() ) -> nextPtr()
	   // REMOVE ??? )
	 )
 
         remove(p)->_deleted = true; // NOTE: explicit deletion of an individual JLterm is not allowed
 
       a->_deleted = true; // NOTE: explicit deletion of an individual JLterm is not allowed
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

template<typename T>
void TJL<T>::getReference( T* r ) const 
{
 int i;
 for( i = 0; i < _myEnv->_numVar; i++ ) r[i] = _myEnv->_refPoint[i];
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TJL<T>::isNilpotent() const 
{
 dlist_iterator getNext( _theList );
 TJLterm<T>* p = (TJLterm<T>*) getNext();
 if( p->_weight == 0 && std::abs(p->_value) > MX_SMALL ) 
   { return false; }
 else
   { return true; }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::writeToFile( ofstream& outStr ) const 
{
 dlist_traversor getNext( _theList );
 int i;
 TJLterm<T>* p;
 dlink* q;
 
 outStr << "\n_Count  = " << _count
        << ", Weight = " << _weight
        << ", Max accurate weight = " << _accuWgt
        << endl;
 outStr << "Reference point:\n";
 for( i = 0; i < _myEnv->_numVar; i++ ) {
   outStr << _myEnv->_refPoint[i];
 }
 outStr << endl;
 while((  q = getNext()  )) {
   p = (TJLterm<T>*) q->info();
   outStr << "Weight: " << p -> _weight
          << "   Value: " << p -> _value
          << " || ";
   outStr << "Addresses: " 
          <<  ((int) q->prevPtr())
          <<  ((int) q)
          <<  ((int) q->nextPtr())
          << " : " 
          <<  ((int) p)
          << endl;
   outStr << "Index:  ";
   for( i = 0; i < _myEnv->_numVar; i++ ) {
     outStr << ((p->_index)(i)) << "  ";
   }
   outStr << endl;
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::scaleBy( T y ) 
{
 TJLterm<T>* p;
 dlist_iterator getNext( _theList );
 
 if ( this->_count < 1 ) {
   (*pcout) << "\n\n*** WARNING::TJL<T>::scaleBy: function invoked \n"
        <<     "    by null JL variable with address " << (int) this 
        << endl;
   return;
   }
 
 for (p = _jltermStore; p < _jltermStoreCurrentPtr; p++) {
   if ( !(p->_deleted) ) p->_value *= y;               
 }

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::setVariable( const T& x, 
                              const int& j, 
                              TJetEnvironment<T>* theEnv )
{

 if( theEnv != 0 ) _myEnv = theEnv;

 if( _myEnv == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const T&, const int&, TJetEnvironment<T>* )", 
     "Private data _myEnv is null: object has no environment assigned.") );
 }

 clear();

 if( ( j < 0 ) || ( j >= _myEnv->_numVar ) ) return;

 IntArray ndx( _myEnv->_numVar );  // Assumes: initialized with zeroes
 insert( new(storePtr()) TJLterm<T>( ndx, x, _myEnv ) );
 ndx(j) = 1;
 append( new(storePtr()) TJLterm<T>( ndx, 1.0, _myEnv ) );
 
 _myEnv->_refPoint[j] = x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::setVariable( const int& j, 
                              TJetEnvironment<T>* theEnv ) 
{
 if( theEnv != 0 ) _myEnv = theEnv;

 if( _myEnv == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const int&, TJetEnvironment<T>* )", 
     "Private data _myEnv is null: object has no environment assigned.") );
 }

 clear();
 if( ( j < 0 ) || ( j >= _myEnv->_numVar ) ) return;
 
 T x = theEnv->_refPoint[j];
 IntArray ndx( _myEnv->_numVar );
 insert( new( storePtr() ) TJLterm<T>( ndx, x, theEnv ) );
 ndx(j) = 1;
 append ( new( storePtr() ) TJLterm<T>( ndx, 1.0, theEnv ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::insert( TJLterm<T>* a) 
{
 _theList.insert(a);
 _count++;
 if( _weight < (a->_weight) ) _weight = a->_weight;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
inline
TJLterm<T>* TJL<T>::get() 
{
 TJLterm<T>* p;
 if( _count <= 0 ) return 0;
 p = ((TJLterm<T>*) _theList.get());
 if( --_count == 0 ) _weight = -1;
 return p;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJL<T>::lowTerm() const 
{
 dlist_iterator getNext( _theList );
 TJLterm<T>* p      = 0;
 TJLterm<T>* result = 0;

 p = (TJLterm<T>*) getNext();
 result = 0;
   
 while ( (result == 0) && (p!=0) ) 
 {
   if( p->_value != 0.0 ) result = p;
   p = (TJLterm<T>*) getNext();
 }

 if( result == 0 ) return TJLterm<T>( this->_myEnv->_allZeroes, 0.0, this->_myEnv );
 else              return TJLterm<T>( *result );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJL<T>::firstTerm() const 
{
 TJLterm<T>* p = 0;
 p = ( TJLterm<T>* ) const_cast<dlist&>(_theList).firstInfoPtr() ;
 if( 0 != p ) {
   return TJLterm<T>( *p );
 }
 else {
   return TJLterm<T>( this->_myEnv->_allZeroes, 0.0, this->_myEnv );
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::append( TJLterm<T>* a) 
{
 _theList.append(a);
 _count++;
 if( _weight < (a->_weight) ) _weight = a->_weight;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>* TJL<T>::remove( dlink* w ) 
{
 TJLterm<T>* a;
 dlink* predecessor;
 dlink* successor;
 int predWeight;
 int succWeight;
 
 predecessor = w->prevPtr();
 successor   = w->nextPtr();
 
 a = (TJLterm<T>*) _theList.remove( w );
 _count--;
 
 if( _count == 0 ) {
   _weight = -1;
   return a;
   }
 
 if ( _weight > a->_weight ) return a;
 
 predWeight = ( (TJLterm<T>*) ( predecessor->info() ) ) -> _weight;
 succWeight = ( (TJLterm<T>*) ( successor->info() ) ) -> _weight;
 _weight = predWeight;
 if( succWeight > _weight ) _weight = succWeight;
 return a;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL<T>::standardPart() const 
{
 TJLterm<T>* p; 
 if( _count < 1 ) { return 0.0; }
 dlist_iterator g( _theList );
 p = (TJLterm<T>*) g();
 if( p->_weight  == 0 ) { return p->_value; }
 return 0.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::clear() 
{


 _theList.clear();
 _count      = 0;
 _weight     =  -1;
 if( _myEnv ) _accuWgt = _myEnv->_maxWeight;
 else         _accuWgt = 100000;   // ??? Better way?

 _jltermStoreCurrentPtr  = _jltermStore ; // do not delete the store here !

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL<T>::weightedDerivative( const int* ind ) const 
{
 int i;
 TJLterm<T>* p;
 char theOne;
 
 if( _count < 1 ) return 0.0;
 
 dlist_iterator getNext( _theList );

 while((  p = (TJLterm<T>*) getNext()  )) {
   theOne = 1;
   for ( i = 0; i < _myEnv->_numVar; i++ )
     theOne = theOne && ( (p->_index)(i) == ind[i] );
   if( theOne ) return p->_value;
 }
 
 return 0.0;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL<T>::derivative( const int* ind ) const 
{
 double n;
 double multiplier;
 T d;
 int i;
 
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

template<typename T>
T TJL<T>::operator()( const T* x )  const 
{
 // This routine is linked to double TJL<T>::operator()( const Vector& x ) const
 // Any change made to this must be reflected in the other.

 TJLterm<T>* p = 0;
 T v    = T();
 int w   = 0;
 int i   = 0;
 T term = T();
 T* u   = 0;
 dlist_iterator getNext( _theList );
 
 u = new T[ _myEnv->_numVar ];
 
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
     // REMOVE: _myEnv->_monoCode();
     // REMOVE: term = _myEnv->_monomial[ _myEnv->_monoRank() ];
     term = _myEnv->_monomial[ _myEnv->_offset.index( _myEnv->_exponent ) ];
     ( _myEnv->_exponent[i] )++;
     // REMOVE: _myEnv->_monoCode();
     // REMOVE: _myEnv->_monomial[ _myEnv->_monoRank() ] = term * u[i];
     _myEnv->_monomial[ _myEnv->_offset.index( _myEnv->_exponent ) ] = term * u[i];
 
   }
 
 // Monomials have been now stored at this point.
 // Now traverse the JL variable and evaluate.
 
 v = 0.0;
 
 while((  p = (TJLterm<T>*) getNext()  )) {
   // REMOVE; for( i = 0; i < _myEnv->_numVar; i++ ) _myEnv->_exponent[i] = (p->_index)(i);
   // REMOVE: _myEnv->_monoCode();
   // REMOVE: v += ( p->_value ) * ( _myEnv->_monomial[ _myEnv->_monoRank() ] );
   v += ( p->_value ) * ( _myEnv->_monomial[ _myEnv->_offset.index( p->_index ) ] );
 }
 
 delete [] u;
 
 return v;
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL<T>::operator()( const Vector& x ) const 
{
  int n = x.Dim();
  T newarg [n];
  for( int i = 0; i < n; i++ ) {
    newarg[i] = x(i);
  }
  return this->operator()( newarg );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      The overloaded operators for class JL
//

template<typename T>
istream& operator>>( istream& is,  TJL<T>& x ) 
{  // ??? This function shouldn't
   // ??? be here.
  char buf[100];
  int i,j;
  int count;
  T value;
  TJLterm<T>* q;

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
    q = new(x.storePtr()) TJLterm<T>( ndx, value, x._myEnv );
    x.append( q );
  }

 return is;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
ostream& operator<<( ostream& os, const TJL<T>& x ) 
{
 dlist_traversor getNext( x._theList );

 TJLterm<T>* p;
 dlink* q;

 os << "\n Count = " << x._count << " , Weight = " << x._weight;
 os << " , Max accurate weight = " << x._accuWgt << endl;
 os << "Begin Environment: \n"
    << *(x._myEnv)
    << "\nEnd Environment: " << endl;

 while((  q = getNext()  )) {
   p = (TJLterm<T>*) q->info();
   os << "Index: ";
   os << p->_index << " ";
   os << "   Value: " << setprecision(30) << p -> _value << endl;
   // os << "   Value: " << p -> _value << endl;
 }
 return os << "\n" << endl;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const TJL<T>& x, const TJL<T>& y ) 
{
 if( ( x._count   != y._count )   ||
     ( x._weight  != y._weight )  ||
     ( x._accuWgt != y._accuWgt )
   ) return false;
 
 if( x._myEnv != y._myEnv ) return false;

 dlist_iterator getNextX( x._theList );
 dlist_iterator getNextY( y._theList );
 TJLterm<T>* p;
 TJLterm<T>* q;
 
 while((  p = (TJLterm<T>*) getNextX()  )) {
   if( !( q = (TJLterm<T>*) getNextY() ) ) return false;
   if( !( *p == *q ) ) return false;
   }
 
 if( (TJLterm<T>*) getNextY() ) {
   printf( "\n*** ERROR: Inconsistency in JL operator==\n" );
     // This point should never be reached, since that would imply that
     // x._count != y._count
   return false;
   }
 
 return true;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const TJL<T>& x, const T& y ) 
{
 bool result = true;
 TJLterm<T>* p;

 if( x._count < 1 ) {
  if( y == 0.0 ) return true;
  else           return false;
 }

 dlist_iterator getNext( x._theList );

 while((  p = (TJLterm<T>*) getNext()  )) 
  result = result && ( p->_weight == 0 ? p->_value == y : 
                                        p->_value == 0.0 );

 return result;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const T& y, const TJL<T>& x )
{
 return x == y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TJL<T>& x, const TJL<T>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TJL<T>& x, const T& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const T& x, const TJL<T>& y ) 
{
 return !( x == y );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>& TJL<T>::operator=( const T& x ) 
{
 
 clear();
 _weight = 0;   // _rc is purposely left untouched!!
 _count  = 0;
 if( _myEnv ) _accuWgt = _myEnv->_maxWeight;
 else {
   throw( GenericException( __FILE__, __LINE__, 
     "TJL<T>& TJL<T>::operator=( const T& x ) {", 
     "Private data _myEnv is null: object has no environment assigned.") );
 }

 insert( new(storePtr()) TJLterm<T>( IntArray( _myEnv->_numVar ), x, _myEnv ) );

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>& TJL<T>::operator=( const TJL<T>& x ) 
{
 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;
 TJLterm<T>* old_jltermStore = 0;

 if( this == &x ) return *this;
 
 clear(); 

 // fast copy 

 if ( _jltermStoreCapacity < x._jltermStoreCapacity )  {
   old_jltermStore = _jltermStore;
      _jltermStore = _jltermStoreCurrentPtr =  TJLterm<T>::array_allocate(x._jltermStoreCapacity);
      _jltermStoreCapacity = x._jltermStoreCapacity;
 };

 
 memcpy( _jltermStore, x._jltermStore, (x._jltermStoreCurrentPtr-x._jltermStore)*sizeof(TJLterm<T>) );
 _jltermStoreCurrentPtr = _jltermStore + (x._jltermStoreCurrentPtr-x._jltermStore);

 dlist_iterator getNext( x._theList );

 while((  p = (TJLterm<T>*) getNext()  )) {
          q = _jltermStore + ( p - x._jltermStore); 
          append(q);  
 }

 if (old_jltermStore) {
    TJLterm<T>::array_deallocate( old_jltermStore );
 }
 _count   = x._count;     // _rc is purposely left untouched
 _weight  = x._weight;
 _accuWgt = x._accuWgt;
 _myEnv   = x._myEnv;

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJL<T>& TJL<T>::operator+=( const T& x ) {   

 static char firstNull = 1;
 int i;

 TJLterm<T>* p;                               
 TJLterm<T>* q;

 if( p = (TJLterm<T>*) _theList.firstInfoPtr() )
 {
   if ( p->_weight == 0 ) { 
    p->_value += x;
    if( std::abs( p->_value ) <  MX_SMALL*std::abs(x) ) p->_value = 0.0;
   }
 
   else {
     q = new(storePtr()) TJLterm<T>( _myEnv );
     for( i = 0; i < _myEnv->_numVar; i++ ) q->_index(i) = 0;
     q->_weight = 0;
     q->_value = x;
     insert( q );
   }
 }
 else
 {
   if( firstNull ) {
     (*pcerr) << "*** WARNING ***                                 \n"
             "*** WARNING *** TJL<T>& TJL<T>::operator+=( const T& )\n"
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

template<typename T>
TJLterm<T> TJLterm<T>::operator*( const TJLterm<T>& y ) 
{
 TJLterm<T> z(*this);
 int n = 0;

 if((  ( n = this->_index.Dim() ) != y._index.Dim()  )) {
   throw( typename TJL<T>::BadDimension( this->_index.Dim(), y._index.Dim(),
                            __FILE__, __LINE__, 
                            "TJLterm<T> TJLterm<T>::operator*( const TJLterm<T>& y ) ",
                            "Inconsistent number of coordinates." ) );
 }

 z._weight = this->_weight + y._weight;
 for( int i = 0; i < n; i++ ) {  z._index(i) = this->_index(i) + y._index(i);  }
 z._value = this->_value * y._value;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJLterm<T>::operator+( const TJLterm<T>& y ) 
{
 if( this->_index != y._index ) {
   throw( typename TJL<T>::BadDimension( this->_index.Dim(), y._index.Dim(),
                            __FILE__, __LINE__, 
                            "TJLterm<T> TJLterm<T>::operator*( const TJLterm<T>& y ) ",
                            "Inconsistent number of coordinates." ) );
 }
 TJLterm<T> z(*this);
 z._value += y._value;
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Implementation of Class TJLterm<T>

template<typename T>
TJLterm<T>::TJLterm() 
: _index(6), _weight(0), _value(T()), _deleted(false) /// index argument necessary ???
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm( TJetEnvironment<T>* pje ) 
: _index( pje->_numVar ), _weight(0), _value(T()), _deleted(false) 
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm( const IntArray& l, 
                const T& x, 
                TJetEnvironment<T>* pje ) 
:   _index( l ), _weight(l.Sum()), _value(x), _deleted(false)
{ }
//------------------------------------------------------------------------------------------------------
// The code below has been commented out for efficiency. It performs sanity checks and can be re-enabled
// for debugging pruposes.
// Remember: TJLterm<>::TJLterm() is called on a massive scale ! 
//------------------------------------------------------------------------------------------------------
#if 0
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
     throw( GenericException( __FILE__, __LINE__, 
            "TJLterm<T>::TJLterm<T>( const IntArray&, const T&, TJetEnvironment<T>*",
            "Dimensions are wrong.") );
   }
  
   // ??? These checks could be removed for speed.
   dpt = 0;
   for( i = 0; i < n; i++ ) {
     if( (l(i) < 0) ) {
       throw( GenericException( __FILE__, __LINE__, 
              "TJLterm<T>::TJLterm<T>( const IntArray&, const T&, TJetEnvironment<T>*",
              "Bad index in JLTerm.") );
     }
     dpt += l(i);
   }
  
   if( dpt > pje->_maxWeight ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJLterm<T>::TJLterm<T>( const IntArray&, const T&, TJetEnvironment<T>*",
            "Attempt to load a JLTerm with too large a weight.") );
   }
   
   // ??? REMOVE: _index = l;
   _weight = dpt;
   _value = x;
 }

 else {
   if( l.Dim() != 1 ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJLterm<T>::TJLterm<T>( const IntArray&, const T&, TJetEnvironment<T>*",
            "Inconsistency between l and pje") );
   }
   if( l(0) != 0 ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJLterm<T>::TJLterm<T>( const IntArray&, const T&, TJetEnvironment<T>*",
            "Bad value of the index when pje = 0.") );
   }

   _index = l;
   _weight = l.Sum();
   _value = x;
 }
}
#endif
//--------------------------------------------------------------------------------------------------------
// end of commented out code
//---------------------------------------------------------------------------------------------------------


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm( const IntArray& l, const T& x )
: _index(l), _weight(l.Sum()), _value(x),_deleted(false)
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJLterm<T>::TJLterm( const TJLterm<T>* x ) 
: _index( x->_index ), _weight(x->_weight), _value(x->_value), _deleted(x->_deleted)
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>::TJLterm( const TJLterm<T>& x ) 
: _index( x._index ), _weight(x._weight), _value(x._value), _deleted(x._deleted)
{
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJLterm<T>::Reconstruct( const IntArray& e, const T& x )
{
  _index.Reconstruct(e);
  _weight   = e.Sum();
  _value    = x;
  _deleted  = false;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// This destructor should **never** be called 
// doing so will break the specialized allocation.

#if  0
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
template<typename T>
TJLterm<T>::~TJLterm<T>() 
{
 

}
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif 

template<typename T>
TJLterm<T>* TJLterm<T>::array_allocate(int n) {

    TJLterm<T>* p = 
       static_cast<TJLterm<T>*>(_ordered_memPool.ordered_malloc( n ));
    _array_sizes[ p ] = n;
    return p;
}

template<typename T>
void TJLterm<T>::array_deallocate(TJLterm<T>* p) {

    _ordered_memPool.ordered_free( p, _array_sizes[p] ); 
    _array_sizes.erase(p );
}


//*** Static member variable instantiations for the TJLterm class
//
// Notes:
// 
//  __gnu_cxx::hash_map is an extension to the STL. 
//  There is equivalent functionality in tr1 ...
//  boost::hash<TJLterm<T>*> is used to get a portable
//  pointer hash fnct. 

template <typename T>
boost::pool<> TJLterm<T>::_ordered_memPool( sizeof(TJLterm<T>), 2048 );

template <typename T>
__gnu_cxx::hash_map< TJLterm<T>*, unsigned int, boost::hash<TJLterm<T>*> > TJLterm<T>::_array_sizes;


// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Overloaded operators for class TJLterm<T>

template<typename T>
TJLterm<T>& TJLterm<T>::operator=( const TJLterm<T>& x ) 
{
 _deleted = x._deleted;
 _weight  = x._weight;
 _value   = x._value;
 _index   = x._index;
 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator==( const TJLterm<T>& a, const TJLterm<T>& b ) 
{
 if( a._weight != b._weight ) return false;
 if( a._value  != b._value  ) return false;
 if( a._index  != b._index  ) return false;
 return true;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator!=( const TJLterm<T>& a, const TJLterm<T>& b ) 
{
 return !( a == b );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator<=( const TJLterm<T>& a, const TJLterm<T>& b ) 
{
 int i;
 if( a._index.Dim() != b._index.Dim() ) {
   throw( typename TJL<T>::BadDimension( a._index.Dim(), b._index.Dim(), 
          __FILE__, __LINE__, 
          "char operator<=( const TJLterm<T>&, const TJLterm<T>& )",
          "Dimensions don't match.") );
 }

 if( a._weight != b._weight ) { return ( a._weight < b._weight ); }
 
// the code below should work, but at the moment it does not for some reason ... . 
// 
#if 0
 return (a._index <= b._index) ;
#endif

// do this instead ... 

#if 1

 // for( i = 0; i < a._index.Dim(); i++ ) {
 //   if( a._index(i) == b._index(i) ) continue;
 //   return ( a._index(i) < b._index(i) );
 // }

 i = a._index.Dim();
 
 int k,l;

 IntArrayReverseIterator getNext_a(a._index); // this should go away. Use 
 IntArrayReverseIterator getNext_b(b._index); // IntArray operator <= instead

 while( 0 < i-- ) {
   k = getNext_a(); l = getNext_b();
   if( k  != l ) {
     return (  k < l );
   }
 }

// while( 0 < i ) {
//   i--;
//   if( a._index(i) != b._index(i) ) {
//     return ( a._index(i) < b._index(i) );
//   }
// }

 return true;  // when all indices are the same.
#endif
//////////////////////////////////////////////////////////////////////////////
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool operator%=( const TJLterm<T>& a, const TJLterm<T>& b ) 
{
 if( a._weight != b._weight ) return false;
 return a._index == b._index;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


//the static variable _thePool is defined here. 

template <typename T>
std::vector<TJL<T>* > TJL<T>::_thePool; 


#endif // TJL_TCC
