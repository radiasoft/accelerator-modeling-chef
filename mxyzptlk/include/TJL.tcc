/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
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
******             Batavia, IL   60510                                
******             Email: michelotti@fnal.gov                         
******
******  Revision History:
******
******  Feb 2005 - Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******  
******  - Efficiency improvements.
******  - new memory management scheme
******
******  Sep-Dec 2005  ostiguy@fnal.gov
******
****** - refactored code to use a single class template parameter
******   instead of two. Mixed mode operations now handled using 
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - centralized TJetEnvironment management
****** - all implementation details now completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for implicit instantiations, define MXYZPTLK_IMPLICIT_TEMPLATES 
******  
**************************************************************************
*************************************************************************/
#ifndef TJL_TCC
#define TJL_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <complex>
#include <iomanip>
#include <fstream>
#include <iosetup.h>

#include <utils.h> // misc utils: nexcom(), bcfRec(), nearestInteger() ...  
#include <GenericException.h>
#include <TJL.h>

#define MX_SMALL    1.0e-12  // Used by JL::addTerm to decide 
                             //   removal of a JLterm.
#define MX_MAXITER  100      // Maximum number of iterations allowed
                             //   in iterative routines


using FNAL::pcerr;
using FNAL::pcout;

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//  
// ******  the static class variable _thePool is a container for ****
//         discarded Jets. 

template <typename T> std::vector<TJL<T>* > TJL<T>::_thePool; 



// ================================================================
//      Implementation of exceptions

template<typename T>
TJL<T>::BadDimension::BadDimension( int i, int j, 
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: xdim(i), ydim(j)
{
  std::ostringstream uic;
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
         << std::endl;
    firstTime = false;
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TJL<T>::BadDimension::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::BadEnvironment::BadEnvironment( int a, int b, int c, 
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: i(a), env_1(b), env_2(c)
{
  std::ostringstream uic;
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
         << std::endl;
    firstTime = false;
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TJL<T>::BadEnvironment::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::BadReference::BadReference( int ndx, double u,
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: i(ndx), im(u)
{
  std::ostringstream uic;
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
         << std::endl;
    firstTime = false;
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TJL<T>::BadReference::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::HorribleException::HorribleException( int i, int j, int k, 
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: curIndex(i), coordSize(j), valueSize(k)
{
  std::ostringstream uic;
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
         << std::endl;
    firstTime = false;
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const char* TJL<T>::HorribleException::what() const throw()
{
  return errorString.c_str();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::HideousException::HideousException( int x, int y, 
                                std::string fileName, int lineNumber, 
                                const char* fcn, 
                                const char* msg )
: i(x), n(y)
{
  std::ostringstream uic;
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
         << std::endl;
    firstTime = false;
  }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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
// 
// 
//    Constructors and destructors    |||||||||||||||||||||||||||


template<typename T>
void TJL<T>::initStore( int capacity) {

  _jltermStoreCapacity    = capacity;
  _jltermStore            =  TJLterm<T>::array_allocate( _jltermStoreCapacity ); 
  _jltermStoreCurrentPtr  = _jltermStore;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::initStore( ) {

  _jltermStoreCapacity    = 4;
  _jltermStore            = TJLterm<T>::array_allocate( _jltermStoreCapacity );
  _jltermStoreCurrentPtr  = _jltermStore;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL(EnvPtr<T> const& pje, T x): 
_count(0),
_weight(0),                        
_accuWgt( pje->maxWeight() ),
_myEnv( pje ),  
_constIterPtr(0),
_iterPtr(0)
{

   initStore();
   // NOTE: only non-zero terms are stored
   if ( x != T() ) {
     insert( TJLterm<T>( IntArray( pje->numVar() ), x, pje ) );
   }
 }

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::makeTJL( EnvPtr<T> const& pje, T x )
{

  if (!pje) return 0; // this form is called by TCoord and Tparam

  if (_thePool.empty() ) return (  JLPtr<T>(new  TJL<T>(pje, x ) )); 
 
  TJL<T>* p    = _thePool.back();  _thePool.pop_back();
  p->_count    = 0;
  p->_weight   = 0;
  p->_accuWgt  = pje->maxWeight();
  p->_myEnv    = pje; 

   // NOTE: only non-zero terms are stored

  if ( x != T() ) {
    p->insert( TJLterm<T>( IntArray( pje->numVar() ), x, pje ) );
  }

  p->_constIterPtr = 0,
  p->_iterPtr      = 0;

 return  JLPtr<T>(p);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL( const IntArray& e, const T& x, EnvPtr<T> const& pje ) :
 _count(0),
 _weight(0),
 _accuWgt(pje->maxWeight() ),
 _myEnv(pje), 
_jltermStoreCapacity(0), 
_jltermStore(0), 
_jltermStoreCurrentPtr(0), 
_constIterPtr(0),
_iterPtr(0)
{

 initStore();
 
 // NOTE: only non-zero terms are stored

 if ( x != T() ) { 
   insert( TJLterm<T>( e, x, pje ) );
 } 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
 JLPtr<T> TJL<T>::makeTJL( const IntArray& e, const T& x, EnvPtr<T> const& pje ) 
{

  if (!pje) return 0;  // cannot create a TJL without a properly constructed environemnt 

  if (_thePool.empty() ) return  JLPtr<T>( new TJL<T>(e,x,pje) );

  TJL<T>* p = _thePool.back(); _thePool.pop_back();

  p->_count     = 0;
  p->_weight    = 0;
  p->_accuWgt   = pje->maxWeight();
  p->_myEnv     = pje; 

 // NOTE: only non-zero terms are stored

 if ( x != T() ) { 
    p->insert( TJLterm<T>( e, x, pje ) );
 } 

  p->_constIterPtr = 0,
  p->_iterPtr      = 0;

  return JLPtr<T>(p);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>::TJL( const TJL<T>& x ): 
 _count(0),                     
 _weight(x._weight),     
 _accuWgt(x._accuWgt),
 _myEnv(x._myEnv),
_jltermStoreCapacity(0), 
_jltermStore(0), 
_jltermStoreCurrentPtr(0), 
_constIterPtr(0), 
_iterPtr(0)
{
 
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

 //****************************************************************************************
 // set the JLterm iterators properly in case they are in use. 
 // 
 // THIS TERRIBLE KLUDGE MUST GO AWAY ! THE TERM ITERATORS DO NOT BELONG 
 // IN THE TJL CLASS ! 
 // FIX ME !!! 
 // 
 //****************************************************************************************

 // this messy code is necessary because there is no way (nor should not there be any !) 
 // to sitectly access the internals of an iterator.    


 getNext.Reset(); 
 
 if ( x._constIterPtr) {

    _constIterPtr = new dlist_iterator( _theList );
    q = ( TJLterm<T>* ) x._constIterPtr->current();
  
    if (q) { 
       while(  p = ( TJLterm<T>*) getNext( ) ) {
         if ( *p   ==   *q ) break;

       } 
    };

    *_constIterPtr = getNext;

 };  

 getNext.Reset(); 
 
 if (x._iterPtr ) {

   _iterPtr = new dlist_iterator( _theList );

   q = (TJLterm<T>*) x._iterPtr->current();
   
   if (q ) {   
       while(  p = ( TJLterm<T>*) getNext( ) ) {
         if ( *p  ==  *q ) break;
       }   
   };

   *_iterPtr = getNext;

 }
 

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
template<typename U>
JLPtr<T>  TJL<T>::makeTJL( const TJL<U>& x )
{

  if (_thePool.empty() ) 
     return  JLPtr<T>(new TJL<T>(x));
 
  TJL<T>* p = _thePool.back(); _thePool.pop_back(); 
  
  if (p->_jltermStoreCapacity < x._jltermStoreCapacity)  
  { 
       TJLterm<T>::array_deallocate( p->_jltermStore );
       p->initStore(x._jltermStoreCapacity);   
  
  }

  p->_count    = 0;          // needed by append function
  p->_weight   = x._weight;  // needed by append function
  p->_accuWgt  = x._accuWgt;
  p->_myEnv    = x._myEnv;
 
   memcpy( p->_jltermStore, x._jltermStore, (x._jltermStoreCurrentPtr-x._jltermStore)*sizeof(TJLterm<T>) );
   p->_jltermStoreCurrentPtr = p->_jltermStore + (x._jltermStoreCurrentPtr - x._jltermStore);


 dlist_iterator getNext( x._theList );
 
 TJLterm<U>* q = 0;
 TJLterm<T>* r = 0;

 while((  q = (TJLterm<U>*) getNext()  )) {
          r = p->_jltermStore + ( q - x._jltermStore); 
          p->append(r);
 }


 //****************************************************************************************
 // set the JLterm iterators properly in case they are in use. 
 // 
 // THIS TERRIBLE KLUDGE MUST GO AWAY ! THE TERM ITERATORS DO NOT BELONG 
 // IN THE TJL CLASS ! 
 // FIX ME !!! 
 // 
 //****************************************************************************************

 // this messy code is necessary because there is no way (nor should there be any !) 
 // to access the internals of an iterator.    

 getNext.Reset(); 
 
 if ( x._constIterPtr) {

    p->_constIterPtr = new dlist_iterator( p->_theList );
    q = (TJLterm<U>*) x._constIterPtr->current();

    if( q ) {

       while(  r = (TJLterm<U>*) getNext( ) ) {

///////FIXME        if ( *r   ==   *q ) break;
       } 
     };
 };  

 getNext.Reset(); 
 
 if (x._iterPtr ) {

   p->_iterPtr  = new dlist_iterator( p->_theList );

   if (q) {
     q = (TJLterm<T>*) x._iterPtr->current();
     while(  r = ( TJLterm<U>*) getNext( ) ) {
/////FIXME        if ( *r  ==  *q ) break;
     }   
   };

   *(p->_iterPtr) = getNext;

 }

 return JLPtr<T>(p);

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
 
 if( _iterPtr)        delete _iterPtr;
 if( _constIterPtr )  delete _constIterPtr;

 // _myEnv is purposely NOT deleted, of course.
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::discardTJL( TJL<T>* p) 
{
  
   EnvPtr<T> nullEnv;

   p->clear();

   p->_myEnv = nullEnv; // nullify the environment. 

   if( p->_iterPtr)        { delete p->_iterPtr;       p->_iterPtr      = 0; }
   if( p->_constIterPtr )  { delete p->_constIterPtr;  p->_constIterPtr = 0; }

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
           append(q); // note: pointer form 
    };

    // now, it is ok to zap the old store
 
    TJLterm<T>::array_deallocate( old_jltermStore );

    // finally, return a ptr to the next available block 

    return  _jltermStoreCurrentPtr++;
  }

}   


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
 TJLterm<T>* TJL<T>::removeTerm( TJLterm<T>* a) 
{

 dlist_traversor getNext( _theList );
 dlink* p;
 
  _weight = 0; 

  while((  p = getNext()  )){
 
       if ( (TJLterm<T>*) p == a ) {
         remove( p );
          --_count;
         continue;  // skip the next statement if a term
                    // has been removed
       }
        
  // readjust the max weight to reflect the removed term.

  _weight  = std::max(_weight,  ((TJLterm<T>*) p)->_weight);

  }
  
 return(a);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::addTerm( const TJLterm<T> & a )
{

 dlist_traversor getNext( _theList );
 dlink* p;


 // In the event that the candidate is lighter than the first
 // element in the JL dlist ... or the list is empty
 //
 p = getNext();
 
 if( p == 0 ) {
   insert( a ); // inserting a term into a list does not change it 
   return;
   }
 
 if(     a   <=   ( *(TJLterm<T>*) (p->info()) )   ) {
   if(   a   %=   ( *(TJLterm<T>*) (p->info()) )   ) {
 
     (( (TJLterm<T>*) (p->info()) ) -> _value ) += ( a._value );
 
     //  NOTE: the older versions of this method would remove and delete zero terms. 
     return;
     }
 
   else {
     insert( a); // inserting a term into a list does not change it  
     return;
     }
 
   }
 
 //
 // Otherwise, search through the list
 //
 while((  p = getNext()  )){
 
   if(      a   <=   ( *(TJLterm<T>*) (p->info()) )   ) { // compares weights only 
     if(    a   %=   ( *(TJLterm<T>*) (p->info()) )   ) { // equality implies equak weights and indentical exponents
 
       (( (TJLterm<T>*) (p->info()) ) -> _value ) += ( a._value );
 
       //  NOTE: the older versions of this method would remove and delete zero terms. 
       return;
       }
 
     else {
       TJLterm<T>* q = new( this->storePtr() ) TJLterm<T>(a);
       p -> putAbove( new dlink( const_cast<TJLterm<T>*>(q), 0, 0 ) );
       ++(this->_count);
       this->_weight = std::max(this->_weight, q->_weight);
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
 for( i = 0; i < _myEnv->numVar(); ++i ) r[i] = _myEnv->refPoint()[i];
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
void TJL<T>::writeToFile( std::ofstream& outStr ) const 
{
 dlist_traversor getNext( _theList );
 int i;
 TJLterm<T>* p;
 dlink* q;
 
 outStr << "\n_Count  = " << _count
        << ", Weight = " << _weight
        << ", Max accurate weight = " << _accuWgt
        << std::endl;
 outStr << "Reference point:\n";
 for( i = 0; i < _myEnv->numVar(); ++i ) {
   outStr << _myEnv->refPoint()[i];
 }
 outStr << std::endl;
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
          << std::endl;
   outStr << "Index:  ";
   for( i = 0; i < _myEnv->numVar(); ++i ) {
     outStr << ((p->_index)(i)) << "  ";
   }
   outStr << std::endl;
   }
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::peekAt() const {

 dlist_traversor getNext( _theList );
 int i;
 TJLterm<T>* p;
 dlink* q;
 (*pcout) << "\nCount  = "
      << _count
      << " Weight = "
      << _weight
      << " Max accurate weight = "
      << _accuWgt
      << std::endl;
 (*pcout) << "Reference point: ";
 for( i = 0; i < getEnv()->numVar(); i++ ) 
    (*pcout) << std::setprecision(12) << getEnv()->getRefPoint()[i] << "  ";
 (*pcout) << std::endl;
 while((  q = getNext()  )) {
   p = (TJLterm<T>*) q->info();
   (*pcout) << "Weight: "
        << p -> _weight
        << "   Value: "
        << p -> _value
        << " || ";
   (*pcout) << "Addresses: "
        << (int) q->prevPtr() << "  "
        << (int) q            << "  "
        << (int) q->nextPtr() 
        << " : "
        << (int) p
        << std::endl;
   (*pcout) << "Index:  " << (p -> _index) << std::endl;

   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::scaleBy( T y ) 
{ 

 TJLterm<T>* p = 0;

 if ( std::abs(y) < MX_SMALL ) { // *** multiplication by zero  -- remove all the terms !

   _theList.clear();                       // note: the terms are not deleted    
   _jltermStoreCurrentPtr = _jltermStore;  // reset the store ptr to the begining   
   _count    = 0;
   _weight   = 0; 
   _accuWgt  = _myEnv->maxWeight();  
   return; 
 } 
    
 // y is not zero, so just scale every term

 for (p = _jltermStore; p < _jltermStoreCurrentPtr; ++p) {
   if ( !(p->_deleted) ) p->_value *= y;               
 }

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TJL<T>& TJL<T>::Negate( ) {

 for(  TJLterm<T>* p = _jltermStore; p< _jltermStoreCurrentPtr; ++p  ) {
   if (!p->_deleted)  p->_value = -p->_value;
 }
 return *this;

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::setVariable(  const T& x, const int& j, EnvPtr<T> const& pje)
{
 
// this member function is meant to be called **ONLY** when a coordinate is instantiated

 if( ( j < 0 ) || ( j >= _myEnv->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const T&, const int&, TJetEnvironment<T>* )", 
     "Index out of range") );
 }
 
  clear(); 
 _myEnv   = pje;
 _accuWgt = _myEnv->maxWeight();
 _weight  = 0;
  
 IntArray ndx( _myEnv->numVar() );  
 insert( TJLterm<T>( ndx,  x,     _myEnv ) );
 ndx(j) = 1;
 append( TJLterm<T>( ndx, (T)1.0, _myEnv ) );
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
void TJL<T>::setVariable(  const T& x, const int& j )
{
 
// this member function is meant to be called **ONLY** when a coordinate is instantiated

 if( ( j < 0 ) || ( j >= _myEnv->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const T&, const int&, TJetEnvironment<T>* )", 
     "Index out of range") );
 }
 
 clear(); 
 _accuWgt =  _myEnv->maxWeight();
 _weight = 0;

 IntArray ndx( _myEnv->numVar() );  
 insert( TJLterm<T>( ndx,  x,     _myEnv ) );
 ndx(j) = 1;
 append( TJLterm<T>( ndx, (T)1.0, _myEnv ) );
 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 
template<typename T>
void TJL<T>::setVariable( const int& j, 
                              EnvPtr<T> const& theEnv ) 
{

 if( _myEnv == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const int&, TJetEnvironment<T>* )", 
     "Private data _myEnv is null: object has no environment assigned.") );
 }

 if( ( j < 0 ) || ( j >= _myEnv->numVar() ) ) {
   throw( GenericException( __FILE__, __LINE__, 
     "void TJL<T>::setVariable( const int&, TJetEnvironment<T>* )", 
     "Index out of range") );

 }

 _myEnv = theEnv;
 
 clear();
 _accuWgt =  theEnv->maxWeight();
 _weight = 0;

 T x = theEnv->refPoint()[j];
 IntArray ndx( _myEnv->numVar() );
 insert( TJLterm<T>( ndx, x, theEnv ) );
 ndx(j) = 1;
 append ( TJLterm<T>( ndx, 1.0, theEnv ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::insert( const TJLterm<T>* a) 
{
 _theList.insert(const_cast<TJLterm<T>*>(a));
 ++_count;
 _weight = std::max( _weight, a->_weight );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::insert( const TJLterm<T>& a) 
{

 TJLterm<T>* p = new (storePtr())TJLterm<T>(a);
  _theList.insert(const_cast<TJLterm<T>*>(p));
 ++_count;
 _weight = std::max( _weight, p->_weight );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>* TJL<T>::get() 
{
 TJLterm<T>* p;
 if( _count <= 0 ) return 0;
 p = ((TJLterm<T>*) _theList.get());
 if( --_count == 0 ) _weight = 0;
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
   if( p->_value != T() ) result = p;
   p = (TJLterm<T>*) getNext();
 }

 if( result == 0 ) return TJLterm<T>( IntArray(this->_myEnv->numVar()), T(), this->_myEnv );
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
   return TJLterm<T>( IntArray(this->_myEnv->numVar()), T(), this->_myEnv );
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::append( TJLterm<T> const& a) 
{
 TJLterm<T>* p = new( this->storePtr() ) TJLterm<T>(a); 
 _theList.append(p);
 ++_count;
 _weight  = std::max(_weight, p->_weight);

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::append( const TJLterm<T>* a) 
{
 _theList.append( const_cast<TJLterm<T>*>(a) );
 ++_count;
 _weight  = std::max(_weight, a->_weight);

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
   _weight = 0;
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
 if( _count < 1 ) { return T(); }

 dlist_iterator g( _theList );
 p = (TJLterm<T>*) g();
 
 if( p->_weight  == 0 ) { return p->_value; } 
 
 return T(); // return 0 if the order of the lowest term is > 0 
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::clear() 
{


 _theList.clear();
 _count      =    0;
 _weight     =    0; 
 _accuWgt    =    0;  

 _jltermStoreCurrentPtr  = _jltermStore ; // do not delete the store here !

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T TJL<T>::weightedDerivative( const int* ind ) const 
{
 int i;
 TJLterm<T>* p;
 bool theOne;
 
 if( _count < 1 ) return T();
 
 dlist_iterator getNext( _theList );

 while((  p = (TJLterm<T>*) getNext()  )) {
   theOne = true;
   for ( i = 0; i < _myEnv->numVar(); ++i )
     theOne = theOne && ( (p->_index)(i) == ind[i] );
   if( theOne ) return p->_value;
 }
 
 return T();
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
 if ( d == T() ) return 0.0;
 
 multiplier = 1.0;
 for ( i = 0; i < _myEnv->numVar(); ++i ) {
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
   T v   = T();
 int w   = 0;
 int i   = 0;
 T term  = T();
 T* u    = 0;
 dlist_iterator getNext( _theList );
 
 u = new T[ _myEnv->numVar() ];
 
 // Subtract off the reference point.
 for( i = 0; i < _myEnv->numVar(); ++i ) u[i] = x[i] - _myEnv->refPoint()[i];
 
 // Evaluate and store _monomials.
 
 // The zeroth one.

 _myEnv->monomial()[0] = 1.0;

 int* exponent = _myEnv->exponent();
 
 // For all higher weights ...
 for( w = 1; w <= _weight; w++ )
 
   // Get the next set of _exponents of weight w.
   while( nexcom( w, _myEnv->numVar(), exponent ) ) {
 
     // Find the first non-zero _exponent.
     i = 0;
     while( !exponent[i++]  ) ;
     i--;
 
     // The value of the _monomial associated with this composition
     // is obtained by multiplying a factor into a previously
     // computed _monomial.

     exponent[i]--;

     term = (_myEnv->monomial())[ _myEnv->offsetIndex( exponent ) ];

     exponent[i]++;

     _myEnv->monomial()[ _myEnv->offsetIndex( exponent ) ] = term * u[i];
 
   }
 
 // Monomials have been now stored at this point.
 // Now traverse the JL variable and evaluate.
 
 v = T();
 
 while((  p = (TJLterm<T>*) getNext()  )) {
   v += ( p->_value ) * ( _myEnv->monomial()[ _myEnv->offsetIndex( p->_index ) ] );
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
  for( int i = 0; i < n; ++i ) {
    newarg[i] = x(i);
  }
  return this->operator()( newarg );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::filter( const int& wgtLo, const int& wgtHi ) const 
{ 
 //----------------------------------------------------------------------------
 // Filters the terms with weights included in the closed interval [wgtLo,wgtHi]
 //----------------------------------------------------------------------------

 // Trivial case: nothing to filter ... return a *deep copy* of the current jl.
 
 if( ( wgtLo <= 0 ) && ( wgtHi >= _weight ) ) {
   return JLPtr<T>( TJL<T>::makeTJL( *this ));
 } 

 // Begin filtering ... 

 JLPtr<T> z( TJL<T>::makeTJL( _myEnv) );

 dlist_iterator getNext( _theList );
 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;
 int numTerms;
 int wgt, upperWgt;
 
 numTerms = 0;
 upperWgt = 0;
 
 while((  p = (TJLterm<T>*) getNext()  )) {
   wgt = p->_weight;
   if( ( wgt >= wgtLo ) && ( wgt <= wgtHi ) ) {
     z->append( TJLterm<T>(*p) );
     if( wgt > upperWgt ) upperWgt = wgt;
     numTerms++;
     }
   }
 
 z->_count   = numTerms;             // ??? Is this necessary?
 z->_weight  = upperWgt;             // ??? Where is the copy of the re
 z->_accuWgt = _accuWgt;             // ??? reference point???

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::filter( bool (*f) ( const IntArray&, const T& ) ) const 
{ 

 JLPtr<T> z( TJL<T>::makeTJL(_myEnv) );


 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;

 dlist_iterator getNext( _theList );

 int i  = 0;
 int nv = 0;
 IntArray oldIndex;

 nv = _myEnv->numVar();
 oldIndex.Reconstruct( nv );

 while((  p = (TJLterm<T>*) getNext()  )) {
  oldIndex = p->_index;

  if( (*f)( p->_index, p->_value ) ) {
     z->append( TJLterm<T>(*p) );

  }

  for( i = 0; i < nv; i++ ) 
   if( oldIndex(i) != p->_index(i) ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::filter( bool (*f) ( const IntArray&, const T& ) ) const",
           "Test routine changes value of _index array." ) );
   }
 }

 return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::truncMult( JLPtr<T> const& v, const int& wl ) const 
{ 
 //
 // Truncated multiplication: used only by the division operator.
 // No checks are made on the environments.
 //

 // If one of the arguments is void, return a *deep copy* of it ..

 if ( _count < 1 )     return  JLPtr<T>( TJL<T>::makeTJL(*this) );   
 if ( v->_count < 1 )  return  JLPtr<T>( TJL<T>::makeTJL(*v) );        

 // .. otherwise continue normal operations.

 // Initializations

 JLPtr<T> z( TJL<T>::makeTJL( _myEnv) );

 TJLterm<T>* tjlmml =  _myEnv->TJLmml(); 

 T  dummy   = T();
 T  product = T();

 int indy = 0;

 for (  TJLterm<T>* p = v->_jltermStore; p <v->_jltermStoreCurrentPtr; p++ ) {

    if (p->_deleted) continue;

    for ( TJLterm<T>* q = _jltermStore; q < _jltermStoreCurrentPtr; q++ ) {
  
      if (q->_deleted) continue;

      if( ( p->_weight + q->_weight ) > wl ) continue;

      indy = _myEnv->offsetIndex( p->_index + q->_index );

   // Will work even when the exponents are all zero.

      product = p->_value * q->_value;
      dummy = tjlmml[indy]._value + product;

      if( std::abs( dummy ) < MX_SMALL*std::abs( product ) ) {
        tjlmml[indy]._value = T();
      } 
      else {
        tjlmml[indy]._value = dummy;
      }
    }
 }
 
 // Transfer answer from the scratchpad.

 TJLterm<T>*  zed      = tjlmml;
 TJLterm<T>*  upperzed = tjlmml + _myEnv->maxTerms();

 while( zed < upperzed ) {
   if( zed->_value !=  T() ) {
     z->append( TJLterm<T>( *zed ) );
     zed->_value = T();
   }
   zed++;
 }

 // Determine the maximum weight computed accurately.
 // ??? Is this step necessary???
 
 z->_accuWgt = std::min(_accuWgt, v->_accuWgt);
 
 return  z;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// ***************************************************************
// ***************************************************************
// ***************************************************************
//
//      Operators for class JL
//------------------------------------------------------------------

template<typename T>
std::ostream& operator<<( std::ostream& os, const TJL<T>& x ) 
{
 dlist_traversor getNext( x._theList );

 TJLterm<T>* p;
 dlink* q;

 os << "\n Count = " << x._count << " , Weight = " << x._weight;
 os << " , Max accurate weight = " << x._accuWgt << std::endl;
 os << "Begin Environment: \n"
    << *(x._myEnv)
    << "\nEnd Environment: " << std::endl;

 while((  q = getNext()  )) {
   p = (TJLterm<T>*) q->info();
   os << "Index: ";
   os << p->_index << " ";
   os << "   Value: " << std::setprecision(30) << p -> _value << std::endl;
   // os << "   Value: " << p -> _value << std::endl;
 }
 return os << "\n" << std::endl;
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
  if( y == T() ) return true;
  else           return false;
 }

 dlist_iterator getNext( x._theList );

 while((  p = (TJLterm<T>*) getNext()  )) 
  result = result && ( p->_weight == 0 ? p->_value == y : 
                                        p->_value == T() );

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
 if( _myEnv ) _accuWgt = _myEnv->maxWeight();
 else {
   throw( GenericException( __FILE__, __LINE__, 
     "TJL<T>& TJL<T>::operator=( const T& x ) {", 
     "Private data _myEnv is null: object has no environment assigned.") );
 }

 insert( TJLterm<T>( IntArray( _myEnv->numVar() ), x, _myEnv ) );

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


 _count   = x._count;    
 _weight  = x._weight;
 _accuWgt = x._accuWgt;
 _myEnv   = x._myEnv;
 _constIterPtr = 0; //  **not copied**
 _iterPtr      = 0; //  **not copied**

 return *this;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJL<T>& TJL<T>::operator+=( const T& x ) {   

 static bool firstNull = true;

 TJLterm<T>* p = 0;                               
 TJLterm<T>* q = 0;

 if( p = (TJLterm<T>*) _theList.firstInfoPtr() )
 {
   if ( p->_weight == 0 ) { 
      p->_value += x;
      if( std::abs( p->_value ) <  MX_SMALL*std::abs(x) ) p->_value = T();
   }
 
   else {
     q = new(storePtr()) TJLterm<T>( _myEnv );

     for( int i = 0; i < _myEnv->numVar(); ++i ) {
        q->_index(i) = 0;
     }

     q->_weight = 0;
     q->_value  = x;
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
          << std::endl;
     firstNull = false;
   }
   operator=( x );
 }

 return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::sin() const
{ 

 JLPtr<T> epsilon( TJL<T>::makeTJL( *this) ); // deep copy 

 if( epsilon->_count == 0 ) return epsilon; // argument is null 
 
 dlist_iterator getNext( epsilon->_theList );
 TJLterm<T>* p = (TJLterm<T>*) getNext();

 if( ( p -> _weight ) == 0 ) {                // jet has non-zero standard part
   T cs = std::cos( p -> _value );
   T sn = std::sin( p -> _value );
   epsilon->get()->_deleted=true;             // pop out the standard part and delete it;

   // ---------------------------------------------
   // at this point, epsilon is a pure differential    
   // ---------------------------------------------

     return  ( _epsCos(epsilon)*sn + _epsSin(epsilon)*cs);
  
 }

 else {                               

   return _epsSin( epsilon ); // argument is already a pure infinitesimal

 }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::cos() const
{ 

 JLPtr<T> epsilon( makeTJL( *this) ); // deep copy 

 if( _count == 0 ) {
    epsilon->addTerm( TJLterm<T>( _myEnv->allZeroes(), (T)(1.0), epsilon->_myEnv ) ); // cos(0.0) = 1.0
    return epsilon;
 }
 
 
 dlist_iterator getNext( epsilon->_theList );
 TJLterm<T>* p = (TJLterm<T>*) getNext();

 if( ( p -> _weight ) == 0 ) {                // jet has non-zero standard part
   T cs = std::cos( p -> _value );
   T sn = std::sin( p -> _value );
   epsilon->get()->_deleted=true;             // pop out the standard part and delete it;

   // ---------------------------------------------
   // at this point, epsilon is a pure differential    
   // ---------------------------------------------
   
     return  (_epsCos( epsilon )*cs - _epsSin( epsilon )*sn );
 }

 else {                               

   return _epsCos( epsilon ); // x is pure infinitesimal

   }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::_epsSin( JLPtr<T> const& epsilon ) 
{ 
 
 JLPtr<T> epsq;
 epsq = epsilon*epsilon;
 epsq->Negate();                                              //   epsq = -epsilon*epsilon 
 JLPtr<T> z( epsilon->clone() );               //    z = epsilon -- deep copy 



 JLPtr<T> term =  epsilon*epsq;                // term = 1/6 * epsilon*epsq
 term->scaleBy(1.0/6.0);
 
 double n = 3.0;
 double n1,n2;

  int nsteps = 0;
  while( term->_count > 0 ) {
   z    += term;                                // z += term;
   term *= epsq;                                // term->multiply(epsq);
   n1 = ++n; n2 = ++n;
   term->scaleBy( 1.0/static_cast<T>(n1*n2) );  // term = ( ( term*epsq ) / ++n ) / ++n;
   

   if (++nsteps > epsilon->_accuWgt) break;                            // expansion terminates after at most  epsilon->_accuWgt steps


 }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::_epsCos( JLPtr<T> const& epsilon ) 
{ 
 

 JLPtr<T> z( makeTJL( epsilon->_myEnv, ((T) 1.0) ));             // z   = 1    

 JLPtr<T> epsq = (epsilon*epsilon);
 epsq->Negate();                                         // epsq = -epsilon*epsilon

 JLPtr<T> term(epsq->clone());
 term->scaleBy(0.5);                                     // term = epsq/2.0

 double n = 2.0;
 double n1, n2;
 
  int nsteps =0;
  while( term->_count > 0 ) {
   z += term;                                   // z += term;
   term *= epsq;                                // term->multiply(epsq);
   n1 = ++n; n2 = ++n;
   term->scaleBy( 1.0/static_cast<T>(n1*n2) );  // term = ( ( term*epsq ) / ++n ) / ++n;  
   if (++nsteps > epsilon->_accuWgt) break;     // expansion terminates after at most  epsilon->_accuWgt steps
   }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::sqrt() const 
{

 if( _count == 0 ) {

   // * the code below should be replaced by a more permanent 
   // * solution; possibly a function that clears the scratchpads for all
   // * existing environments. 
   // * The scratchpads should be cleared when *any* exception is thrown
   // * within mxyzptlk because the TJLterm _value fields may contain  
   // * "Nan"s. 
   // *DO NOT REMOVE OR COMMENT OUT UNLESS YOU HAVE IMPLEMENTED A BETTER SOLUTION !* 
   //   
   // -JFO 

   //--------------------------------------------------------------------------

   (*pcout) << "Resetting Environment Scratchpad" << std::endl;
   for (int i=0; i< _myEnv->maxTerms(); i++ ) {
         _myEnv->TJLmml()[i]._value = 0.0;
   }          
   
   //--------------------------------------------------------------------------

   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> sqrt() { ",
          "Argument is zero." ) );
 }
 

 TJLterm<T>* p = 0;

 T factor     = T();
 T stdpart    = T();

 dlist_iterator getNext(_theList );
 p = (TJLterm<T>*) getNext();

 if( ( ( p -> _weight      ) ==  0   ) &&
     ( ( stdpart = p -> _value ) !=  T() )
   )                                  // non-zero standard part
 {
 factor = std::sqrt( stdpart );
   if( _count == 1 )           // ... operand may have no derivatives
     {
     return JLPtr<T>( makeTJL(_myEnv, factor)); 
     }
   else                       // ... normal case
     {
     JLPtr<T> epsilon( makeTJL(*this) ); // deep copy 
     epsilon->get()->_deleted = true;                   // pop out and delete the standard part off epsilon
     epsilon->scaleBy( 1.0/stdpart );
     return ( _epsSqrt(epsilon)*factor );               // NOT EFFICIENT !!!
     }
 }
 else                                 // nilpotent argument
 {
   
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> sqrt() ",
          "Argument's standard part vanishes; it is nilpotent." ) );
  
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::_epsSqrt( JLPtr<T> const& epsilon ) 
{  

 // This function is identical to epsPow
 // with the substitution  s = 1/2

 JLPtr<T>      z(makeTJL(epsilon->_myEnv,(T) 1.0)); // z = 1.0 
 JLPtr<T>      term(makeTJL(*epsilon));             // deep copy;  term = epsilon 
 
 double f    = 1.0 / 2.0;
 double n    = 1.0;

 term->scaleBy(f);                      // term = f*epsilon;
 
 int nsteps = 0;
 while( term->_count > 0 ) {

   z    += term;                                     // z += term; in place add
   term *= epsilon;                                  // term->multiply(epsilon);
   term->scaleBy( ((T)--f)/static_cast<T>(++n) );    // term *= ( ((T) (--f)) * epsilon ) / ++n;

   if (++nsteps > epsilon->_accuWgt) break;                            // expansion terminates after at most  epsilon->_accuWgt steps
                                                                       // NOTE: "term->count" may not reach zero due to round-off
 }

 z->_accuWgt = epsilon->_accuWgt;

 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 template<typename T>
 JLPtr<T> TJL<T>::exp() const
 {
  
 JLPtr<T> epsilon( makeTJL(*this) ); //deep copy
 
 T factor = T();
 TJLterm<T>* p = 0;

 dlist_iterator getNext( _theList );

 if( _count == 0 ) {
   if( epsilon->_count != 0  ) {
     std::ostringstream uic;
     uic  << "Horrible, inexplicable error: epsilon->_count = " 
          << epsilon->_count;
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T> exp( const TJet<T>& ) ",
            uic.str().c_str() ) );
   }
   epsilon->addTerm( TJLterm<T>( _myEnv->allZeroes(), ((T) 1.0), _myEnv ) );
   return epsilon;
 }

 p = (TJLterm<T>*) getNext();
 if( ( p -> _weight ) == 0 ) {         // x has non-zero standard part
   factor = std::exp( p -> _value );
   epsilon->get()->_deleted = true;   // pop out and delete the standard part off epsilon
   return ( _epsExp( epsilon )*factor );
   }
 else {                               // x is pure infinitesimal
   return _epsExp( epsilon );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
JLPtr<T>  TJL<T>::_epsExp( JLPtr<T> const& epsilon ) 
{ 

 JLPtr<T> z( makeTJL(epsilon->_myEnv, ((T) 1.0)  ));   // z    = 1.0;
 JLPtr<T> term( epsilon->clone() );                     // term = epsilon

 double n = 1.0;
 
 int nsteps = 0;
 while( term->_count > 0 ) {
   z += term;                                             // z += term;
   term *= epsilon;  
   term->scaleBy(1.0/static_cast<T>(++n));                // term = ( term*epsilon ) / ++n;
   if (++nsteps > epsilon->_accuWgt) break;               // this should not be necessary
                                                          // unless the scratchpad is poluted.   
 }
  
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::pow(const double& s )  const
{

 if( _count == 0 ) return JLPtr<T>( makeTJL(*this) ); // deepcopy  pow(0, s) = 0  
 
 int u = 0;
 dlist_iterator getNext(_theList );
 T std    = T();

 TJLterm<T>* p = (TJLterm<T>*) getNext();
 if( ( ( p -> _weight      ) ==  0   ) &&
     ( ( std = p -> _value ) !=  0.0 ) )  // x has non-zero standard part
 {

   if( _count == 1 ) {             // x may have no derivatives
     return JLPtr<T>( makeTJL(_myEnv, std::pow(std,s) ));
   }

   JLPtr<T> z( makeTJL(*this) );   //   deep copy

   p = z->get();
   std = p->_value;  
   p->_deleted = true;   //   pop out and delete the standard part off zilon

   z->scaleBy( 1.0/std );
   z = _epsPow( z, s );
   z->scaleBy( std::pow( std, s ) );
   return z;
   }
 else                                 // x is pure infinitesimal
   {
   u = nearestInteger( s );
   if( s != (T) u ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJet<T> pow( const TJet<T>&, const double& )",
            "Cannot use infinitesimal as base with non-integer _exponent." ) );
   }

   JLPtr<T> z(makeTJL(_myEnv, ((T) 1.0))); // z = 1.0;

   if ( u == 0 ) {
     return z;
   }
   if ( u > 0 ) {
     JLPtr<T> x(makeTJL(*this));      // x, deepcopy;
     while( u-- > 0 )   z *= x;
     return z;
     }
   else {
     JLPtr<T> x(makeTJL(*this));               //  x, deepcopy;
     while( u++ < 0 )   z *= x;
     JLPtr<T> tmp(makeTJL(_myEnv, ((T) 1.0))); // tmp = 1.0;
     z = tmp/z;                                //  z = 1.0/z;
     return z;
     }
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::pow(int n ) const
{

  if( n == 0 ) 
     return JLPtr<T>(makeTJL( _myEnv, ((T) 1.0)));    // z = 1
  else if( n > 0 ) {
     JLPtr<T> z(makeTJL( *this));              // z =  x
     JLPtr<T> x(makeTJL( *this));              // z =  x
     for( int i = 2; i <= n; ++i ) z *= x;
     return z;
  }
  else { // exponent is negative

    JLPtr<T> xr( makeTJL( _myEnv, ((T) 1.0) ));
    JLPtr<T> x(  makeTJL(*this) );               // deep copy of argument; this step is not really necessary
                                                                // and should be eliminated
    xr =  xr/x;                                                 // xr  = 1/x 
    x  = xr;                                                    // deep copy 
    for( int i = -2; i >= n; i-- ) x *= xr;
    return x;
  }

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::_epsPow( JLPtr<T> const& epsilon, const double& s ) 
{ 
 JLPtr<T> z( makeTJL(epsilon->_myEnv, ((T) 1.0)) ); // z = 1.0;
 JLPtr<T> term;

 double f = s;
 double n = 1.0;

 term  = epsilon;
 term->scaleBy(s);                   //term = s*epsilon;
 
 z->_myEnv = epsilon->_myEnv;
 
 int ncount = 0;
 while( term->_count > 0 ) {
   z    += term;                                     
   term *= epsilon;                                  // term = ( ((T) (--f)) * term * epsilon ) / ++n;
   term->scaleBy(((T) (--f))/ static_cast<T>(++n) );
   if (++ncount > epsilon->_accuWgt ) break;
   }
 
 z->_accuWgt = epsilon->_accuWgt;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::log() const
{

 if( _count == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> log () { ",
          "Argument is zero." ) );
 }
 
 dlist_iterator  getNext( _theList );
 TJLterm<T>*  p = (TJLterm<T>*) getNext();

 if( ( (p->_weight) ==  0   ) &&
     ( (p->_value)  !=  T() ) ) // x has non-zero standard part
 {
   if( _count == 1 ) {             // x may have no derivatives
     return JLPtr<T>( makeTJL( _myEnv, std::log( p->_value )));

   }

   JLPtr<T> z( makeTJL(*this)   );
   JLPtr<T> u( makeTJL(_myEnv) );
   JLPtr<T> w( makeTJL(_myEnv) );
   
   p = z->get();
   T std   = p->_value;
   p->_deleted = true;         //   pop the standard part and delete it

   z->scaleBy( - 1.0/std );

   w = z;
   u = z;                                         // We use here the formulae

   double n = 1.0;                                // ln( s + e )   = ln s + ln( 1 + e/s ), and
   while( u->_count > 0 ) {                       // ln( 1 + e/s ) = ln( 1 - ( -e/s )) 
     u *= z;                                      //  = - sum ( - e/s )^n / n
     w += (u / static_cast<T>(++n));              // w += ( u / ++n );
   }
   
   JLPtr<T> tmp( makeTJL(_myEnv, std::log(std) ));
   return ( tmp - w );     // std::log(std) - w
 
   }
 else                                 // operand has zero standard part
   {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T> log() ",
          "operand's standard part vanishes; it is nilpotent." ) );
   }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::compose( JLPtr<T> const y[ ]) const 
{

 //-------------------------------------------------
 //  **** Composition operator ****
 //-------------------------------------------------

 JLPtr<T>  u[ _myEnv->numVar() ];

 //------------------------------------------------
 // Check consistency of reference points and
 // subtract reference point prior to concatenation.
 //-------------------------------------------------

 for( int i= 0; i< _myEnv->numVar(); ++i ) {
 
  if( y[i]->_myEnv !=  y[0]->_myEnv ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJL<T>::compose( JLPtr<T> const y[] ) const ",
            "Inconsistent environments." ) );
   }
 
   u[i] =  y[i] + JLPtr<T>( makeTJL( y[0]->_myEnv, -_myEnv->getRefPoint()[i])) ;  // u[i] = y[i] - _myEnv->getRefPoint()[i];
 }


 JLPtr<T> term( makeTJL(_myEnv) ) ; // Initializing term should not be necessary. 

 JLPtr<T>* tjlmonomial = _myEnv->TJLmonomial();
 int*                     exponent    = _myEnv->exponent();

 // -----------------------------
 // Evaluate and store monomials.
 // -----------------------------

 // The zeroth one ...

 tjlmonomial[0] = JLPtr<T>(makeTJL( u[0]->_myEnv, ((T) 1.0) ));

 // For all higher weights ...

 int  i = 0;
 int  w = 1;

 for( w=1; w <= _accuWgt; w++ ) {

  // Get the next set of _exponents of weight w.

   while( nexcom( w, _myEnv->numVar(), exponent ) ) {
 
     // Find the first non-zero _exponent.
     i = 0;
     
     while( !(exponent[i++]) ) ;
     i--;
 
     // The value of the monomial associated with this composition
     // is obtained by multiplying a factor into a previously
     // computed monomial.

     (exponent[i])--;
     
     term = tjlmonomial[ _myEnv->offsetIndex( exponent ) ];

     exponent[i]++;

     tjlmonomial[ _myEnv->offsetIndex( exponent )] = u[i]*term; 
 

   } // End while loop.
  }  // End for loop
 
 // -----------------------------------------------
 // Monomials have been stored.
 // Now traverse the TJL<T> variable and evaluate.
 // Formerly done by using concat routine, which
 // is reproduced here
 //------------------------------------------------
 
 JLPtr<T> z( makeTJL(u[0]->_myEnv) ); 

 dlist_iterator getNext( _theList );
 
 TJLterm<T>* p = 0;
 while( p = (TJLterm<T>*) getNext()  ) {

    if( p->_weight > _accuWgt ) break;
    z += ( tjlmonomial[ _myEnv->offsetIndex(p->_index) ] *  (p->_value) );
 } 
 
 // Finish...
  
 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
std::istream& operator>>( std::istream& is,  TJL<T>& x ) 
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
  streamIn( is, x._myEnv);
  is >> buf;
  is >> buf;
  IntArray ndx( x.getEnv()->numVar() );
  for( j = 0; j < count; ++j) {
    is >> buf;
    for( i = 0; i < x._myEnv->numVar(); ++i )  {
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
JLPtr<T> TJL<T>::asin() const
{ 

 const T zero = T(); 	 
 EnvPtr<T>  pje(_myEnv);
  	 
 // Initial Newton's step 	 
 
 JLPtr<T> z(makeTJL(*this));        // deep copy	        
 JLPtr<T> dz; 
 JLPtr<T> x( makeTJL(*this) );

 dz = ( z->sin() - x ) / z->cos(); 	 

 // Setting up the iteration 	 
 int upperBound = 8; 	 
 
 int iter = 0;	   
 int indy = 0; 	 

 double compValue; 	 
 bool repeat = true; 	 
 
 TJLterm<T>*     pz = 0; 	 
 TJLterm<T>* tjlmml = pje->TJLmml(); 	 
 	 
 // Iterated Newton's steps
 
 while( repeat && (iter < MX_MAXITER) ) 	 
 { 	 
     while( iter++ < upperBound ) { 	 
      // These two lines are the heart of the calculation: 	 
      z  -= dz; 	 
      dz  =  (z->sin() - x ) / z->cos();	      
     }
	 
     // The rest is just determining when to stop. 	  
     //   This procedure could be improved, but it's better 	 
     //   than the previous one, which was just comparing 	 
     //   dz to zero. 	 
     repeat = false; 	 
 	 
     // Load the current answer into a scratchpad 	   

     z->resetIterator(); 	 

     pz = z->stepIterator(); 	 
     while( 0 != pz ) { 	 
       indy = pje->offsetIndex(pz->_index); 	 
       tjlmml[indy]._value = pz->_value; 	 
       pz = z->stepIterator(); 	 
     } 	 
 	 
     // Compare to the increment, one coefficient at a time 	   
     dz->resetIterator(); 	   
     pz = dz->stepIterator(); 	 
     while( 0 != pz ) { 	 
       indy = pje->offsetIndex(pz->_index); 	 
       compValue = std::abs( tjlmml[indy]._value ); 	 
       if( compValue < MX_SMALL ) { 	 
         if( std::abs(pz->_value) > MX_SMALL ) { 	 
           repeat = true; 	 
           break; 	 
         } 	 
       } 	 
       else { 	 
         if( std::abs(pz->_value) > MX_SMALL*compValue ) { 	 
           repeat = true; 	 
           break; 	 
         } 	 
       } 	 
       pz = dz->stepIterator(); 	 
     } 	 
 	 
     // Clean the scratchpad. 	 
     z->resetIterator(); 	 
     pz = z->stepIterator(); 	 
     while( 0 != pz ) { 	 
       indy = pje->offsetIndex(pz->_index); 	 
       tjlmml[indy]._value = zero; 	 
       pz = z->stepIterator(); 	 
     }
 	 
     // And continue 	 
     upperBound += 8; 	 
 } 	 
 
 // If stopped because too many iterations,
 // print a warning message


 if( iter >= MX_MAXITER ) {
  (*pcerr) << "*** WARNING ***                             \n";
  (*pcerr) << "*** WARNING *** JLPtr<T>& asin()      \n";
  (*pcerr) << "*** WARNING *** Over " 
       << MX_MAXITER 
       << " iterations used;    \n";
  (*pcerr) << "*** WARNING *** result may be incorrect.    \n";
  (*pcerr) << "*** WARNING ***                             \n";
  (*pcerr) << "*** WARNING *** Initial value:              \n";
  (*pcerr) << "*** WARNING ***                             \n"
       << std::endl;

   this->printCoeffs();
  (*pcerr) << "*** WARNING *** A few iterations:             " << std::endl;
  for( iter = 0; iter < 4; ++iter ) {
   (*pcerr) << "*** WARNING *** Iteration No. " << ( iter+1 ) << std::endl;
   z -= dz;
   dz = ( z->sin() - x ) / z->cos();
   (*pcerr) << "*** WARNING *** dz = " << std::endl;
   dz->printCoeffs();
   (*pcerr) << "*** WARNING *** z = " << std::endl;
   z->printCoeffs();
  }
 }


 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::atan() const
{   
 
 JLPtr<T> z( makeTJL(*this));      // deep copy
 JLPtr<T> c;
 JLPtr<T> dz;

 JLPtr<T> x( makeTJL(*this)); 

 int iter = 0;

 c = z->cos();                                                    // c = cos( z );
 dz = c*( z->sin() - x*c );                                       // in-place multiply would help 

 while( ( dz->standardPart() != T() ) && ( iter++ < MX_MAXITER ) ) { 
  z -= dz;
  c = z->cos();                                                   // c = cos( z );
  dz = c*( z->sin() - x*c );
 }

 if( iter >= MX_MAXITER ) {
  (*pcerr) <<  "*** WARNING ***                         \n" 
       <<  "*** WARNING *** JLPtr<T>& atan()            \n" 
       <<  "*** WARNING *** Over " 
                << MX_MAXITER  << " iterations used;    \n"
       <<  "*** WARNING *** result may be incorrect.    \n" 
       <<  "*** WARNING ***                             \n" 
       << std::endl;
 }

 return z;
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::printCoeffs() const {

 dlist_iterator getNext( _theList );
 int i;
 TJLterm<T>* p;
 

 std::cout << "\nCount  = " << _count 
      << ", Weight = " << _weight 
      << ", Max accurate weight = " << _accuWgt << std::endl;
 std::cout << "Reference point: " 
      << _myEnv->getRefPoint()[0];
 for( i = 1; i < _myEnv->numVar(); i++ ) {
   std::cout << ", ";
   std::cout << _myEnv->getRefPoint()[i];
 }
 std::cout << std::endl;

 while((  p = (TJLterm<T>*) getNext()  )) {
   if( p->_weight > _accuWgt ) break;
   std::cout << "Index:  " 
        << p->_index
        << "   Value: "
        << p->_value
        << std::endl;
 }

 std::cout << "\n" << std::endl;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
JLPtr<T> TJL<T>::D( const int* n ) const 
{

 //----------------------------- 
 // Differentiation operator
 //-----------------------------

 bool doit = false;

 dlist_iterator getNext( _theList );
 
 // --- Preliminary check of _index set.
 
 int w = 0;
 
 for( int i = 0; i < _myEnv->numVar(); ++i ) {
   if( n[i] < 0 ) {
     throw( GenericException( __FILE__, __LINE__, 
            "TJL<T>::D( const int* ) const",
            "Cannot differentiate with negative _index." ) );
     }
   w += n[i];
 }
 
 if( w > _accuWgt ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::D( const int* ) const",
          "Differentiation request beyond accuracy allowed." ) );
 }
 
 if( w == 0 ) return JLPtr<T>(makeTJL(_myEnv));

 JLPtr<T> z(makeTJL(_myEnv)); 

 int f = 0;
 int j = 0;

 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;

 // --- Construct the derivative one link at a time.
 // --- ( See note Obs.4 )
 
 while((  p = (TJLterm<T>*) getNext()  )) {
 
   q = new( z->storePtr() ) TJLterm<T>( *p );
 
   doit = true;
   // -- Reset the _index.
   for( int i=0; i< _myEnv->numVar(); ++i) 
     doit = doit && ( ( q->_index(i) -= n[i] ) >= 0 );
 
   if( doit ) {
     // -- Build factorial multiplier.
     f = 1;
     for( int k = 0; k < _myEnv->numVar(); ++k ) {
       j = q->_index(k);
       for( int i = 0; i < n[k]; i++ ) f *= ++j;
     }
     if( f <= 0 ) {
       throw( GenericException( __FILE__, __LINE__, 
              "TJet<T> TJet<T>::D( const int* ) const",
              "Horrible, unexplainable error while differentiating!" ) );
     }                           // Super fussbudget!! ( f "must" be positive )
   
     // -- Make final changes in private data of the TJLterm<T> and
     //    absorb it into the answer.
     (q->_value)   *= f;              // OK. So I'm a fussbudget with parentheses again ..
     (q->_weight ) -= w;
   
     z->append( q );
   } 

   else {
    q->_deleted = true;            // delete q.  Is this correct?   
                                   // ??? It wasn't here before!
   }
 }
 
 // --- Finally, adjust _accuWgt and return value
 
 z->_accuWgt = _accuWgt - w;       // ??? Is this correct ???

 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
//    **** Iteration routines **** 
//------------------------------------------------------------ 


template<typename T>
void TJL<T>::resetConstIterator() 
{
  if (_constIterPtr) delete _constIterPtr;

  _constIterPtr = new dlist_iterator( _theList );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T> TJL<T>::stepConstIterator()  const
{
  if( _constIterPtr ) {
    return TJLterm<T>( *( (TJLterm<T>*)(_constIterPtr->operator()())) );
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "TJLterm<T> TJL<T>::stepConstIterator()  const",
           "You must first resetConstIterator." ) );
  }
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const TJLterm<T>& TJL<T>::stepConstIteratorRef()  const
{
  if( _constIterPtr ) {
    return *( (TJLterm<T>*)( _constIterPtr->operator()() ) );
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "const TJLterm<T>& TJL<T>::stepConstIterator()  const",
           "You must first resetConstIterator." ) );
  }
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
const TJLterm<T>* TJL<T>::stepConstIteratorPtr()  const
{
  if( _constIterPtr ) {
    return ( (const TJLterm<T>*) ( _constIterPtr->operator()() ) );
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "const TJLterm<T>& TJL<T>::stepConstIterator()  const",
           "You must first resetConstIterator." ) );
  }
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TJL<T>::resetIterator()
{
  if( _iterPtr != 0 ) {  
    delete _iterPtr;
    _iterPtr = 0;
  }

  _iterPtr = new dlist_iterator( _theList );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TJLterm<T>* TJL<T>::stepIterator()
{

  if( _iterPtr ) {
    return (TJLterm<T>*) (_iterPtr->operator()());
  }
  else {
    throw( GenericException( __FILE__, __LINE__, 
           "TJLterm<T>* TJet<T>::stepIterator()",
           "You must first resetIterator." ) );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// *** friend functions ******

template <typename T>
JLPtr<T>   operator+(JLPtr<T> const & x, JLPtr<T> const& y  ){  


// Check for consistency and set reference point of the sum.

  if( x->_myEnv != y->_myEnv ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::operator+(JLPtr<T> const& x, JLPtr<T> const& y)"
           "Inconsistent environments." ) );
  }

 if (x->_count < 1) return JLPtr<T>( y->clone() ); // x=0; return a copy of y
 if (y->_count < 1) return JLPtr<T>( x->clone() ); // y=0; return a copy of x

 //  -----------------------------------------------------------------
 //  Loop over the terms and accumulate monomials in the scrach pad.
 //  Use direct sequential access to access terms since order is
 //  irrelevant in this context.
 //  ------------------------------------------------------------------ 
 
 TJLterm<T>* tjlmml =  x->_myEnv->TJLmml(); // the environment scratchpad
 
 TJLterm<T>* p = 0;
 int indy      = 0;
 T result      = T();

 //------------------------------------------------------------
 // accumulate terms of the lhs argument (i.e. x) into the scratchpad
 //------------------------------------------------------------
 for(  p = x->_jltermStore; p < x->_jltermStoreCurrentPtr; ++p  ) {

   if ( p->_deleted) continue; // ignore deleted terms ! 
   indy = x->_myEnv->offsetIndex( p->_index);
   tjlmml[indy]._value = p->_value;
 }

 //-------------------------------------------------------------------
 // accumulate terms of the rhs argument (i.e. y)  into the scratchpad
 // ------------------------------------------------------------------

 for(  p = y->_jltermStore; p< y->_jltermStoreCurrentPtr; ++p  ) {

   if ( p->_deleted) continue;   // ignore deleted terms ! 
   indy   = x->_myEnv->offsetIndex( p->_index);
   tjlmml[indy]._value +=  p->_value;

 }
 //------------------------------------------------
 // At this point, the result is in the scratchpad!
 //------------------------------------------------


 // Transfer result from the scratchpad. 
 // Append only monomials with non-zero coefficients

  JLPtr<T> z( TJL<T>::makeTJL(x->_myEnv) ); 

  TJLterm<T>* zed      = tjlmml;
  TJLterm<T>* upperzed = tjlmml + x->_myEnv->maxTerms();

 // collect the terms from the scratchpad and return 

 for( p=zed;  p<upperzed; ++p) 
 {
    if (  p->_value != T() )  {
      z->append( TJLterm<T>( *p ) ); 
      p->_value = T();
    }
 
 };

  z->_accuWgt = std::min(x->_accuWgt, y->_accuWgt);

 return z;
}

#if  0
===============================================================================================================
old LIST based operator+

  // Check for consistency and set reference point of the sum.

  if( x->_myEnv != y->_myEnv ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJet<T> operator+( const TJet<T>& x, const TJet<T>& )",
           "Inconsistent environments." ) );
  }


  EnvPtr<T> pje(x->_myEnv);

                                                          // ??? This should be modified so that
  JLPtr<T> z( TJL<T>::makeTJL(pje));                // ??? terms beyond the accurate weight of
  TJLterm<T>*    p = 0;                                   // ??? x or y are not computed and carried
  TJLterm<T>*    q = 0;                                   // ??? into the answer.


  // If one of the arguments is void, then return *a deep copy* of the other ..
  if( x->_count < 1 ) {           
    return JLPtr<T>( TJL<T>::makeTJL( *y );             
  }
  if( y->_count < 1 ) {
    return JLPtr<T>( TJL<T>::makeTJL( *x );
  }
                                 
  // .. otherwise, continue normal operations.

  dlist_iterator getp( x->_theList );
  dlist_iterator getq( y->_theList );

  p = (TJLterm<T>*) getp();
  q = (TJLterm<T>*) getq();

  T result; 

  while( q && p ) {

    if( *p <= *q ) {   
      if( *p %= *q ) { 

  
        result = p->_value + q->_value;

        // if (abs(result) > MX_SMALL)  // is this test needed ????
        z->append( TJLterm<T>( p->_index, result ) );
       
        p = (TJLterm<T>*) getp();
        q = (TJLterm<T>*) getq();

      }
      else {
        z->append( TJLterm<T>( p->_index, p->_value ) );
        p = (TJLterm<T>*) getp();
      }
    }
    else {
         z->append( TJLterm<T>( q->_index, q->_value ) );
       q = (TJLterm<T>*) getq();
    }
  }

  while(p) {
     z->append( TJLterm<T>( p->_index, p->_value ) );
    p = (TJLterm<T>*) getp();
  }

  while(q) {
     z->append( TJLterm<T>( q->_index, q->_value ) );
    q = (TJLterm<T>*) getq();

  }


  // .....determine the maximum weight computed accurately.

  z->_accuWgt = std::min(x->_accuWgt, y->_accuWgt);
  
  return z;
}
===========================================================================================================================
#endif


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>&  operator+=(JLPtr<T>& x,      JLPtr<T> const& y  ) {

 //---------------
 // In place add()
 //---------------

// Check for consistency and set reference point of the sum.

  if( y->_myEnv != x->_myEnv ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::add(JLPtr<T> const& y)"
           "Inconsistent environments." ) );
  }

 if (y->_count < 1) return x; // rhs is zero; do nothing

 //  -----------------------------------------------------------------
 //  Loop over the terms and accumulate monomials in the scrach pad.
 //  Use direct sequential access to access terms since order is
 //  irrelevant in this context.
 //  ------------------------------------------------------------------ 
 
 TJLterm<T>* tjlmml =  x->_myEnv->TJLmml(); // the environment scratchpad
 
 TJLterm<T>* p = 0;
 int indy      = 0;
 T result      = T();

 //------------------------------------------------------------
 // accumulate terms of the current object into the scratchpad
 //------------------------------------------------------------
 for(  p = x->_jltermStore; p < x->_jltermStoreCurrentPtr; ++p  ) {

   if ( p->_deleted) continue; // ignore deleted terms ! 
   indy = x->_myEnv->offsetIndex( p->_index);
   tjlmml[indy]._value = p->_value;
 }

 //-------------------------------------------------------------------
 // accumulate terms of the rhs argument (i.e. y)  into the scratchpad
 // ------------------------------------------------------------------

 for(  p = y->_jltermStore; p< y->_jltermStoreCurrentPtr; ++p  ) {

   if ( p->_deleted) continue;   // ignore deleted terms ! 
   indy   =  x->_myEnv->offsetIndex( p->_index);
   result =  tjlmml[indy]._value +  p->_value;

   if (std::abs(result) > MX_SMALL) 
        tjlmml[indy]._value = result; // should not be needed !
   else 
        tjlmml[indy]._value = T(); 
 }
 

 //------------------------------------------------
 // At this point, the result is in the scratchpad!
 //------------------------------------------------

 // -----------------------------
 // zero out  the current object 
 // -----------------------------

 int accuWgt = x->_accuWgt; // save lhs accurate weight value before clearing it ...   
 
 x->clear();
 x->_weight     =  0;                      // reset
 x->_accuWgt    =  x->_myEnv->maxWeight(); // reset

 // Transfer result from the scratchpad. 
 // Append only monomials with non-zero coefficients

 TJLterm<T>* zed      = tjlmml;
 TJLterm<T>* upperzed = tjlmml + x->_myEnv->maxTerms();

 // collect the terms from the scratchpad and return 

 for( p=zed;  p<upperzed; ++p) 
 {
    if (  p->_value != T() )  {
      x->append( TJLterm<T>( *p ) ); 
      p->_value = T();
    }
 
 };

  x->_accuWgt = std::min(accuWgt, y->_accuWgt);


 return x;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>&  operator-=(JLPtr<T>& x,      JLPtr<T> const& y  ) {

 //---------------
 // In place add()
 //---------------

// Check for consistency and set reference point of the sum.

  if( y->_myEnv != x->_myEnv ) {

   throw( GenericException( __FILE__, __LINE__, 
           "TJL<T>::add(JLPtr<T> const& y)"
           "Inconsistent environments." ) );
  }

 if (y->_count < 1) return x; // rhs is zero; do nothing

 //  -----------------------------------------------------------------
 //  Loop over the terms and accumulate monomials in the scrach pad.
 //  Use direct sequential access to access terms since order is
 //  irrelevant in this context.
 //  ------------------------------------------------------------------ 
 
 TJLterm<T>* tjlmml =  x->_myEnv->TJLmml(); // the environment scratchpad
 
 TJLterm<T>* p = 0;
 int indy      = 0;
 T result      = T();

 //------------------------------------------------------------
 // accumulate terms of the current object into the scratchpad
 //------------------------------------------------------------
 for(  p = x->_jltermStore; p < x->_jltermStoreCurrentPtr; ++p  ) {

   if ( p->_deleted) continue; // ignore deleted terms ! 
   indy = x->_myEnv->offsetIndex( p->_index);
   tjlmml[indy]._value = p->_value;
 }

 //-------------------------------------------------------------------
 // accumulate terms of the rhs argument (i.e. y)  into the scratchpad
 // ------------------------------------------------------------------

 for(  p = y->_jltermStore; p< y->_jltermStoreCurrentPtr; ++p  ) {

   if ( p->_deleted) continue;   // ignore deleted terms ! 
   indy   =  x->_myEnv->offsetIndex( p->_index);
   result =  tjlmml[indy]._value -  p->_value;

   if (std::abs(result) > MX_SMALL) 
        tjlmml[indy]._value = result; // should not be needed !
   else 
        tjlmml[indy]._value = T(); 
 }
 

 //------------------------------------------------
 // At this point, the result is in the scratchpad!
 //------------------------------------------------

 // -----------------------------
 // zero out  the current object 
 // -----------------------------

 int accuWgt = x->_accuWgt; // save lhs accurate weight value before clearing it ...   
 
 x->clear();
 x->_weight     =  0;                      // reset
 x->_accuWgt    =  x->_myEnv->maxWeight(); // reset

 // Transfer result from the scratchpad. 
 // Append only monomials with non-zero coefficients

 TJLterm<T>* zed      = tjlmml;
 TJLterm<T>* upperzed = tjlmml + x->_myEnv->maxTerms();

 // collect the terms from the scratchpad and return 

 for( p=zed;  p<upperzed; ++p) 
 {
    if (  p->_value != T() )  {
      x->append( TJLterm<T>( *p ) ); 
      p->_value = T();
    }
 
 };

  x->_accuWgt = std::min(accuWgt, y->_accuWgt);


 return x;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator-(JLPtr<T> const &x) {

 JLPtr<T> z( x->clone() );
 z->Negate();
 return z;

}
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator-(JLPtr<T> const & x,  JLPtr<T> const& y  ){
  
  return (x+(-y));

}
  

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator*(JLPtr<T> const & x,  T const& y  ){  

  JLPtr<T> z (x->clone() );
  z->scaleBy( y );  
  return z;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator*( T const & x,             JLPtr<T> const& y  ){  

  return y*x;

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator*(JLPtr<T> const & x,  JLPtr<T> const& y  ){  

//----------------------------
// Multiplication of two jets
//----------------------------

 // Consistency check 

 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJLPtr<T> operator*( const JLPtr<T>&, const JLPtr<T>& ) ",
          "Inconsistent environments." ) );
 };

 // if one of the arguments is zero, return a *deep copy*  (required!). 

 if( x->_count < 1 ) return JLPtr<T>(  x->clone() ); 
 if( y->_count < 1 ) return JLPtr<T>(  y->clone() );


 EnvPtr<T> pje(x->_myEnv);
 JLPtr<T> z( TJL<T>::makeTJL( pje ) );

 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;

 int  indy                = 0;
 TJLterm<T>* zed          = 0;
 TJLterm<T>* upperzed     = 0;
 T  dummy                 = T();
 T  product               = T();


  // -------------------------------------------------
 // Determine the maximum weight computed accurately.
 // -------------------------------------------------

 TJLterm<T> x_1stTerm( x->lowTerm() ); 
 TJLterm<T> y_1stTerm( y->lowTerm() ); 

 int testWeight    = 0;
 int trialWeight  =  0;
 
 testWeight = x->_accuWgt;
 if( ( y_1stTerm._weight != 0 ) && ( y_1stTerm._value !=  T() ) )
 {
   testWeight += y_1stTerm._weight;
 }

 trialWeight = y->_accuWgt;
 if( ( x_1stTerm._weight != 0 ) && ( x_1stTerm._value !=  T() ) )
 {
   trialWeight += x_1stTerm._weight;
 }

 z->_accuWgt = std::min(testWeight, trialWeight);
 z->_accuWgt = std::min(z->_accuWgt,  pje->maxWeight() ); 
 
 testWeight = z->_accuWgt;

 //  -----------------------------------------------------------------
 //  Loop over the terms and accumulate monomials in the scrach pad.
 //  Use direct sequential access to access terms since order is
 //  irrelevant in this context.
 //  ------------------------------------------------------------------ 
 
 TJLterm<T>* tjlmml =  pje->TJLmml(); 
 
 for(  p = (y->_jltermStore); p < (y->_jltermStoreCurrentPtr); ++p  ) {

   if ( p->_deleted) continue; // ignore deleted terms ! 

   for(  q = (x->_jltermStore); q < (x->_jltermStoreCurrentPtr); ++q  ) {

     if ( q->_deleted) continue;

     if( ( p->_weight + q->_weight ) > testWeight ) continue;   

     indy = pje->offsetIndex( p->_index + q->_index );

     // Will work even when the exponents are all zero.

     product = p->_value * q->_value;

     dummy = tjlmml[indy]._value + product;

     if( (std::abs( dummy ) < MX_SMALL*std::abs( product )) || ( std::abs( dummy ) < MX_ABS_SMALL) ) {
       tjlmml[indy]._value = T();
     } 
     else {
       tjlmml[indy]._value = dummy;
     }
 }}

 // -----------------------------------------------------------------------------------------------
 // At this point, the terms are ordered in the scratchpad!
 // clear the scratchpad and setup the JLterm* dlist 
 // this could be optimized further by using use another mean 
 // to clear scratch pad  
 //-------------------------------------------------------------------------------------------------

 // Transfer answer from the scratchpad and append only monomials with non-zero coefficients

 zed      = tjlmml;
 upperzed = tjlmml + pje->maxTerms();

 for( p = zed; p < upperzed; ++p) 
 {
    if (  p->_value != T() )  {
      z->append( TJLterm<T>( *p ) ); 
      p->_value = T();  // erase the scratchpad entry
    }
 
 };

 // IMPORTANT NOTE:
 // The result of a multiplication operation can be an empty Jet. 
 // This happens when the product of x and y would yield only high 
 // order monomial terms (i.e.) beyond the max weight allowed for
 // the environment.   

 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>&  operator*=(JLPtr<T> & x,     JLPtr<T> const& y  )  
{

//------------------------------------
// In place Multiplication of two jets
//------------------------------------

 // Consistency check 

 if( x->_myEnv != y->_myEnv ) {
   throw( GenericException( __FILE__, __LINE__, 
          "multiply(JLPtr<T> const& ) ",
          "Inconsistent environments." ) );
 };

 // if one of the arguments is zero, clear x before returning 

 if( x->_count < 1 ) return x;
 if( y->_count < 1 ) { 

   x->clear();
   x->_accuWgt = x->_myEnv->maxWeight();
   x->_weight  = 0; 
   return x;
 }

 TJLterm<T>* p = 0;
 TJLterm<T>* q = 0;

 int  indy                = 0;
 TJLterm<T>* zed          = 0;
 TJLterm<T>* upperzed     = 0;
 T  dummy                 = T();
 T  product               = T();


  // -------------------------------------------------
 // Determine the maximum weight computed accurately.
 // -------------------------------------------------

 TJLterm<T> x_1stTerm( x->lowTerm() ); 
 TJLterm<T> y_1stTerm( y->lowTerm() ); 

 int testWeight    = 0;
 int trialWeight  =  0;
 
 testWeight = x->_accuWgt;
 if( ( y_1stTerm._weight != 0 ) && ( y_1stTerm._value !=  T() ) )
 {
   testWeight += y_1stTerm._weight;
 }

 trialWeight = y->_accuWgt;
 if( ( x_1stTerm._weight != 0 ) && ( x_1stTerm._value !=  T() ) )
 {
   trialWeight += x_1stTerm._weight;
 }

 int accuWgt = std::min(testWeight, trialWeight);
     accuWgt = std::min(accuWgt,    x->_myEnv->maxWeight() ); 
 
 testWeight = accuWgt;

 //  -----------------------------------------------------------------
 //  Loop over the terms and accumulate monomials in the scrach pad.
 //  Use direct sequential access to access terms since order is
 //  irrelevant in this context.
 //  ------------------------------------------------------------------ 
 
 TJLterm<T>* tjlmml =  x->_myEnv->TJLmml(); 
 
 for(  p = (y->_jltermStore); p < (y->_jltermStoreCurrentPtr); ++p  ) {

   if ( p->_deleted) continue; // ignore deleted terms ! 

   for(  q = x->_jltermStore; q < x->_jltermStoreCurrentPtr; ++q  ) {

     if ( q->_deleted) continue;

     if( ( p->_weight + q->_weight ) > testWeight ) continue;   

     indy = x->_myEnv->offsetIndex( p->_index + q->_index );

     // Will work even when the exponents are all zero.

     product = p->_value * q->_value;

     dummy = tjlmml[indy]._value + product;

     if( (std::abs( dummy ) < MX_SMALL*std::abs( product )) || ( std::abs( dummy ) < MX_ABS_SMALL) ) {
       tjlmml[indy]._value = T();
     } 
     else {
       tjlmml[indy]._value = dummy;
     }
 }}

 // -----------------------------------------------------------------------------------------------
 // At this point, the terms are ordered in the scratchpad!
 // clear the scratchpad and setup the JLterm* dlist 
 // this could be optimized further by using use another mean 
 // to clear scratch pad  
 //-------------------------------------------------------------------------------------------------

 x->clear();
 x->_accuWgt = accuWgt;
 x->_weight = 0;

 // Transfer answer from the scratchpad and append only monomials with non-zero coefficients

 zed      = tjlmml;
 upperzed = tjlmml + x->_myEnv->maxTerms();

 for( p = zed; p < upperzed; ++p) 
 {
    if (  std::abs(p->_value) > 0.0)  {
      x->append( TJLterm<T>( *p ) ); 
      p->_value = T( );  // erase the scratchpad entry
    }
 
 };

 // IMPORTANT NOTE:
 // The result of a multiplication operation can be an empty Jet. 
 // This happens when the product of x and y would yield only high 
 // order monomial terms (i.e.) beyond the max weight computed
 // accurately 


 return x;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename T>
JLPtr<T>  operator/(JLPtr<T> const& wArg,  JLPtr<T> const& uArg  ){ 

 if ( (wArg->_myEnv) != (uArg->_myEnv) ) {

   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/ ( TJL<T> const& wArg, TJL<T> const& uArg )",
          "Inconsistent environments." ) );
 }

 if ( wArg->_count < 1 ) return JLPtr<T>( TJL<T>::makeTJL( wArg->_myEnv ) );  // numerator is zero
 

 // Check for void operators ..

 if ( uArg->_count < 1 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/ ( TJL<T> const& wArg, TJL<T> const& uArg )",
          "Attempt to divide by a void TJL<T> variable." ) );
 }
 
 // Initialize local variables and set the environment of the answer.
 // (These steps are not necessary, but they enforce a discipline.)

 JLPtr<T>  v( TJL<T>::makeTJL(wArg->_myEnv) );
 JLPtr<T> vn( TJL<T>::makeTJL(wArg->_myEnv) );
 JLPtr<T>  w( TJL<T>::makeTJL(wArg->_myEnv) );
 JLPtr<T>  u( TJL<T>::makeTJL(wArg->_myEnv) );

 TJLterm<T>* qu = 0;
 TJLterm<T>* qw = 0;
 T u0 = T();

 int wgt = 0;
 int wl  = 0;
 int mw  =  wArg->_myEnv->maxWeight();
 

 dlist_looper gu( const_cast<dlist&>(uArg->_theList) );
 dlist_looper gw( const_cast<dlist&>(wArg->_theList) );

 // Normalize the denominator
 if( ( qu = (TJLterm<T>*) gu() ) == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/( JLPtr<T> const& wArg, JLPtr<T> const& uArg )",
          "Division algorithm called with uninitialized JL." ) );
   }
 if( ( wgt = qu->_weight ) != 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/( JLPtr<T> const& wArg, JLPtr<T> const& uArg )",
          "Attempt to divide by nilpotent element." ) );
   }
 if(  (u0 = qu->_value ) == 0.0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "TJL<T>::operator/( JLPtr<T> const& wArg, JLPtr<T> const& uArg )",
          "Attempt to divide by zero." ) );
   }


 u = uArg * ( (T) 1.0 )/ u0 ; 

 //  -------------------
 //  Recursive algorithm
 //  ------------------- 

 qw = (TJLterm<T>*) gw();

 if( qw->_weight == 0 ) 
   { (*v) = ( qw->_value ); }
 else                   
   { (*v) = T(); }
 
 wl = 0;
 while ( wl < mw ) {

   wl++;
   vn  = u->truncMult( v, wl );
   vn  = vn->filter( wl, wl );
    w  = wArg->filter( wl, wl );
     
  // compute v = v + w - vn 

   v += (w-vn);      // v + w- vn                       
 
   }                                     
 
 // Correct normalization of the answer ..

 v->scaleBy( ((T) 1.0)/ u0); 
 
 // Determine the maximum weight computed accurately.

 v->_accuWgt = std::min(  wArg->_accuWgt, uArg->_accuWgt);

 // .. and return _value.

 return v;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
JLPtr<T>   operator/( JLPtr<T> const & x,  T const& y  ){  

  JLPtr<T> z (x->clone() );
  z->scaleBy( ((T) 1.0) / y );  
  return z;

}


#endif // TJL_TCC
