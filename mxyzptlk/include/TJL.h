/*************************************************************************
**************************************************************************
**************************************************************************
******
******  MXYZPTLK:  A C++ implementation of differential algebra.
******
******  File:      TJL.h
******
******  Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******* Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
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
******  Feb 2005   Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******  
******              - Efficiency improvements
******              - Major memory management redesign. 
******  
******  Sept 2005   ostiguy@fnal.gov
******              - new code based on a single template parameter
******                instead of two. Mixed mode handled
******                using conversion operators.
******                 
******  
**************************************************************************
**************************************************************************
**************************************************************************
*************************************************************************/


#ifndef TJL_H
#define TJL_H

#include <dlist.h>
#include <IntArray.h>
#include <TMatrix.h>
#include <VectorD.h>
#include <TJetEnvironment.h>
#include <ext/hash_map>
#include <boost/functional/hash/hash.hpp>
#include <FastPODAllocator.h>
#include <vector>

// *******************************************************************

template<typename T>
struct TJLterm
{

  // Data
  IntArray _index;      //  An integer array giving the derivatives associated
                        //  with the JLterm.  For example, ( 1, 1, 0, 2 )
                        //  would correspond to D_1^1 D_2^1 D_4^2 .
  int      _weight;     //  The sum of the values in index.  For the above example,
                        //  this would be 4.
  T        _value;      //  The value associated with the JLterm.

  bool     _deleted;    //  true when the term has been deleted (but not yet de-allocated)

  // Constructors and destructors
  TJLterm();

  void Reconstruct( const IntArray&, const T& );

  //   NOTE: The void constructor is to be used ONLY in
  //   conjunction with Reconstruct. By default,
  //   _index is made 6 dimensional until Reconstruct
  //   is invoked.

  TJLterm( TJetEnvironment<T>* );
  TJLterm( const IntArray&, const T&, TJetEnvironment<T>* );

  //   If 0 is used for the environment pointer,
  //   it is assumed that the that the variable
  //   represents a constant function.

  TJLterm( const IntArray&, const T& );
  TJLterm( const TJLterm* );
  TJLterm( const TJLterm& );

  // Operators

  operator TJLterm <std::complex<double> > () const;
  operator TJLterm <double> () const;

  TJLterm& operator=( const TJLterm& );
  TJLterm  operator*( const TJLterm& );
  TJLterm  operator+( const TJLterm& );
  
  // Accessors
  IntArray& exponents()         { return _index; }
  IntArray  exponents()   const { return _index; }
  T&        coefficient()       { return _value; }
  T         coefficient() const { return _value; }
  T         coeff()       const { return _value; }  // old

  // JLterm array allocation functions

  static TJLterm<T>*       array_allocate(int n);
  static void              array_deallocate(TJLterm<T>* p);

  private:

  // ~TJLterm();           MUST NOT BE DEFINED ! 

  static boost::pool<>                                                       
                         _ordered_memPool;  // an ordered pool of TJLterms
  
  static __gnu_cxx::hash_map< TJLterm<T>*, unsigned int, boost::hash<TJLterm<T>*> > 
                         _array_sizes;      // the size of the allocated arrays, 
                                            // indexed by their pointers   

  // these declations are meant to prevent use of all forms of operator new[];

  static void* operator new[]( std::size_t size) throw (std::bad_alloc); 
  static void* operator new[]( std::size_t size, const std::nothrow_t&) throw();
  static void* operator new[]( std::size_t size, void*) throw();

  static void operator delete[](void* p) throw();
  static void operator delete[](void* p, const std::nothrow_t&) throw();
  // this form *cannot* be overloaded: static void operator delete[](void* p, void*) throw();

} ;


// *******************************************************************

template<typename T> class TJL
{

 
 public:
 
  dlist _theList;    // Data structure to hold all the information.

  int   _count;      // The number of JL terms in the variable
  int   _weight;     // The maximum weight of the terms
  int   _accuWgt;    // Highest weight computed accurately

  TJetEnvironment<T>*  _myEnv;
                     // Environment of the jet.

  int _rc;           // Reference counting for garbage collection
  
  TJLterm<T>*                         _jltermStore;
  TJLterm<T>*                         _jltermStoreCurrentPtr;
  int                                 _jltermStoreCapacity;

  void initStore();                // setup and initialize the jlterm store using default capacity 
  void initStore(int capacity);    // setup and initialize the jlterm store 

  void insert( TJLterm<T>* );
  void append( TJLterm<T>* );
  TJLterm<T>* remove( dlink* );

  // Constructors and destructors_____________________________________

 public:

  TJL( TJetEnvironment<T>* = 0 );
  TJL( const T&, TJetEnvironment<T>* );
  TJL( const IntArray&, const T&, TJetEnvironment<T>*  );
  TJL( const TJL& );
  TJL( const TJL* );
  ~TJL();

  static std::vector<TJL<T>* >  _thePool;  // declared public because the TJet
                                           // conversion function need access FIXME !


 public:
 

  // factory functions 

  static TJL<T>* makeTJL( TJetEnvironment<T>* = 0 );
  static TJL<T>* makeTJL( const T&, TJetEnvironment<T>* );
  static TJL<T>* makeTJL( const IntArray&, const T&, TJetEnvironment<T>*  );
  static TJL<T>* makeTJL( const TJL& );
  static TJL<T>* makeTJL( const TJL* );
  static void discardTJL( TJL<T>* p);  
  
  // converters

  // operator TJL<double> () const;
  // operator TJL<std::complex<double> > () const;


  // Public member functions__________________________________________

  TJLterm<T>* get();                      // Pops the top term, which should be the
                                          // one of lowest weight.
  TJLterm<T>  firstTerm() const;    
                                          // Returns a TJLterm<T> equivalent 
                                          // to the top term,
                                          // which should be the one of lowest weight.
  TJLterm<T>  lowTerm()   const;    
                                          // Returns a TJLterm<T> equivalent to the
                                          // non-zero term of lowest weight.
  void addTerm( TJLterm<T>* );            // Public only for diagnostic purposes.
  void addTerm( TJLterm<T>*, dlist_traversor& ); 
                                           // Public only for diagnostic purposes.
 
  TJLterm<T>* storePtr();                  // returns a ptr to the next available block in the JLterm store;

  bool isNilpotent() const;
  void writeToFile( std::ofstream& ) const;

  void getReference( T* ) const;
  void scaleBy( T );

  void setVariable( const T&,
                    const int&,
                    TJetEnvironment<T>* = 0 );
                  // WARNING: This routine alters the environment in
                  // WARNING: the third argument.

  void setVariable( const int&,
                    TJetEnvironment<T>* = 0 );

  T standardPart() const;
  void clear();
  T weightedDerivative( const int* ) const;
  T derivative( const int* ) const;

  T operator()( const Vector& ) const;
  T operator()( const T* ) const;
                       // Performs a multinomial evaluation of
                       // the TJL variable.  Essentially acts as a
                       // power series expansion.
  TJL& operator()( const TJL* ) const;
                       // Self explanatory ...

  // Arithmetic operators
  TJL& operator=( const TJL& );
  TJL& operator=( const T& );
  TJL& operator+=( const TJL& );
  TJL& operator+=( const T& );
  TJL& operator-=( const TJL& );
  TJL& operator-=( const T& );
  TJL& operator*=( const TJL& );
  TJL& operator*=( const T& );
  TJL& operator/=( const TJL& );
  TJL& operator/=( const T& );


  // Exception subclasses____________________________________________
  struct BadDimension: public std::exception
  {
    BadDimension( int, int, std::string, int, const char* = "", const char* = "" );
    // A binary operator has detected mismatch between its two arguments 
    // in number of Jet coordinates.
    // 1st argument: IntArray dimension of operator's first argument
    // 2nd         : IntArray dimension of operator's second argument
    // 3rd         : name of file in which exception is thrown
    // 4th         : line from which exception is thrown
    // 5th         : identifies function containing throw
    // 6th         : identifies type of error
    ~BadDimension() throw() {}
    const char* what() const throw();
    std::string errorString;
    int xdim, ydim;
  };

  struct BadEnvironment: public std::exception
  {
    BadEnvironment( int, int, int, std::string, int, const char* = "", const char* = "" );
    // Thrown by JetVector copy constructor: environment mismatch
    // in a component. THIS SHOULD NEVER HAPPEN.
    // 1st argument: component index
    // 2nd         : address of component's environment
    // 3rd         : address of JetVector environment
    // 4th         : name of file in which exception is thrown
    // 5th         : line from which exception is thrown
    // 6th         : identifies function containing throw
    // 7th         : identifies type of error
    ~BadEnvironment() throw() {}
    const char* what() const throw();
    std::string errorString;
    int i, env_1, env_2;
  };

  struct BadReference: public std::exception
  {
    BadReference( int, double, std::string, int, const char* = "", const char* = "" );
    // Attempt to convert a JetC__environment with non-real
    // reference point into a TJetEnvironment<T>.
    // 1st argument: reference point component index
    // 2nd         : imaginary part of component
    // 3rd         : name of file in which exception is thrown
    // 4th         : line from which exception is thrown
    // 5th         : identifies function containing throw
    // 6th         : identifies type of error
    ~BadReference() throw() {}
    const char* what() const throw();
    std::string errorString;
    int i;
    double im;
  };

  struct HorribleException : public std::exception
  {
    HorribleException( int, int, int, 
                       std::string, int, const char* = "", const char* = "" );
    // Thrown from Jet::EndEnvironment if index numbers don't align correctly.
    // This should be impossible.
    // 1st argument: currentIndex
    // 2nd         : newCoords.size()
    // 3rd         : newValues.size()
    // 4th         : name of file in which exception is thrown
    // 5th	   : line from which exception is thrown
    // 6th         : identifies function containing throw
    // 7th         : identifies type of error
    ~HorribleException() throw() {}
    const char* what() const throw();
    std::string errorString;
    int curIndex, coordSize, valueSize;
  };

  struct HideousException : public std::exception
  {
    HideousException( int, int, 
                       std::string, int, const char* = "", const char* = "" );
    // Thrown from Jet::EndEnvironment if index numbers don't align correctly.
    // This should be impossible.
    // 1st argument: currentIndex
    // 2nd         : newCoords.size()
    // 3rd         : newValues.size()
    // 4th         : name of file in which exception is thrown
    // 5th	   : line from which exception is thrown
    // 6th         : identifies function containing throw
    // 7th         : identifies type of error
    ~HideousException() throw() {}
    const char* what() const throw();
    std::string errorString;
    int i, n;
  };
};

#ifdef  MXYZPTLK_IMPLICIT_TEMPLATES
#include <TJL.tcc>
#endif

#endif // TJL_H
