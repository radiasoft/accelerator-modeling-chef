/*************************************************************************
**************************************************************************
**************************************************************************
******
******  MXYZPTLK:  A C++ implementation of differential algebra.
******
******  File:      TJL.h
******  Version:   1.0
******
******  Copyright (c) Universities Research Association, Inc.
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
******  Revision History:
******
******  Feb 2005 - Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******  
******  Efficiency improvements
******  - Major memory management redesign. 
******  
****** 
******  Usage, modification, and redistribution are subject to terms          
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

template<typename T1, typename T2>
struct TJLterm: public gms::FastPODAllocator<TJLterm<T1,T2> >  
{

  // Data
  IntArray _index; // An integer array giving the derivatives associated
                   //   with the JLterm.  For example, ( 1, 1, 0, 2 )
                   //   would correspond to D_1^1 D_2^1 D_4^2 .
  int _weight;     // The sum of the values in index.  For the above example,
                   //   this would be 4.
  T1 _value;       // The value associated with the JLterm.

  bool _deleted;   // true if the term has been deleted

  // Constructors and destructors
  TJLterm();
  void Reconstruct( const IntArray&, const T1& );
  // NOTE: The void constructor is to be used ONLY in
  //   conjunction with Reconstruct. By default,
  //   _index is made 6 dimensional until Reconstruct
  //   is invoked.
  TJLterm( TJetEnvironment<T1,T2>* );
  TJLterm( const IntArray&, const T1&, TJetEnvironment<T1,T2>* );
  // If 0 is used for the environment pointer,
  //   it is assumed that the that the variable
  //   represents a constant function.
  TJLterm( const IntArray&, const T1& );
  TJLterm( const TJLterm* );
  TJLterm( const TJLterm& );
  TJLterm( const TJLterm<T2,T1>& );
  TJLterm( const TJLterm<T2,T1>* );

  // ~TJLterm();  NOT NEEDED ! 

  // Operators
  TJLterm& operator=( const TJLterm& );
  TJLterm  operator*( const TJLterm& );
  TJLterm  operator+( const TJLterm& );
  
  // Accessors
  IntArray& exponents()         { return _index; }
  IntArray  exponents()   const { return _index; }
  T1&       coefficient()       { return _value; }
  T1        coefficient() const { return _value; }
  T1        coeff()       const { return _value; }  // old

  static TJLterm<T1,T2>*   array_allocate(int n);
  static void              array_deallocate(TJLterm<T1,T2>* p);

  private:

  static boost::pool<>                                                       
                         _ordered_memPool; // an ordered pool of TJLterms
  
  static __gnu_cxx::hash_map< TJLterm<T1,T2>*, unsigned int, boost::hash<TJLterm<T1,T2>*> > 
                         _array_sizes;     // the size of the allocated arrays, 
                                           // indexed by their pointers   

  // prevent use of all forms of operator new[];

  static void* operator new[]( std::size_t size) throw (std::bad_alloc); 
  static void* operator new[]( std::size_t size, const std::nothrow_t&) throw();
  static void* operator new[]( std::size_t size, void*) throw();

  static void operator delete[](void* p) throw();
  static void operator delete[](void* p, const std::nothrow_t&) throw();
  // this form *cannot* be overloaded: static void operator delete[](void* p, void*) throw();

} ;


// *******************************************************************

template<typename T1, typename T2> struct TJL
{

  dlist _theList;    // Data structure to hold all the information.

  int _count;        // The number of JL terms in the variable
  int _weight;       // The maximum weight of the terms
  int _accuWgt;      // Highest weight computed accurately

  TJetEnvironment<T1,T2>*  _myEnv;
                    // Environment of the jet.

  int _rc;           // Reference counting for garbage collection
  
  TJLterm<T1,T2>*                     _jltermStore;
  TJLterm<T1,T2>*                     _jltermStoreCurrentPtr;
  int                                 _jltermStoreCapacity;

  void initStore();                // setup and initialize the jlterm store using default capacity 
  void initStore(int capacity);    // setup and initialize the jlterm store 

  void insert( TJLterm<T1,T2>* );
  void append( TJLterm<T1,T2>* );
  TJLterm<T1,T2>* remove( dlink* );

  // Constructors and destructors_____________________________________

 private:

  TJL( TJetEnvironment<T1,T2>* = 0 );
  TJL( const T1&, TJetEnvironment<T1,T2>* );
  TJL( const IntArray&, const T1&, TJetEnvironment<T1,T2>*  );
  TJL( const TJL& );
  TJL( const TJL* );
  TJL( const TJL<T2,T1>&, TJetEnvironment<T1,T2>* );
  ~TJL();

  static std::vector<TJL<T1,T2>* >  _thePool;

 public:
 
  // factory functions 

  static TJL<T1,T2>* makeTJL( TJetEnvironment<T1,T2>* = 0 );
  static TJL<T1,T2>* makeTJL( const T1&, TJetEnvironment<T1,T2>* );
  static TJL<T1,T2>* makeTJL( const IntArray&, const T1&, TJetEnvironment<T1,T2>*  );
  static TJL<T1,T2>* makeTJL( const TJL& );
  static TJL<T1,T2>* makeTJL( const TJL* );
  static TJL<T1,T2>* makeTJL( const TJL<T2,T1>&, TJetEnvironment<T1,T2>* );

  static void discardTJL( TJL<T1,T2>* p);  

  // Public member functions__________________________________________

  TJLterm<T1,T2>* get();        // Pops the top term, which should be the
                                // one of lowest weight.
  TJLterm<T1,T2>  firstTerm() const;    
                                // Returns a TJLterm<T1,T2> equivalent 
                                // to the top term,
                                // which should be the one of lowest weight.
  TJLterm<T1,T2>  lowTerm()   const;    
                                // Returns a TJLterm<T1,T2> equivalent to the
                                // non-zero term of lowest weight.
  void addTerm( TJLterm<T1,T2>* ); // Public only for diagnostic purposes.
  void addTerm( TJLterm<T1,T2>*, dlist_traversor& ); 
                                   // Public only for diagnostic purposes.
 
  TJLterm<T1,T2>* storePtr();   // return a ptr to the next available block in the JLterm store;

  bool isNilpotent() const;
  void writeToFile( std::ofstream& ) const;

  void getReference( T1* ) const;
  void scaleBy( T1 );

  void setVariable( const T1&,
                    const int&,
                    TJetEnvironment<T1,T2>* = 0 );
                  // WARNING: This routine alters the environment in
                  // WARNING: the third argument.

  void setVariable( const int&,
                    TJetEnvironment<T1,T2>* = 0 );

  T1 standardPart() const;
  void clear();
  T1 weightedDerivative( const int* ) const;
  T1 derivative( const int* ) const;

  T1 operator()( const Vector& ) const;
  T1 operator()( const T1* ) const;
                       // Performs a multinomial evaluation of
                       // the TJL variable.  Essentially acts as a
                       // power series expansion.
  TJL& operator()( const TJL* ) const;
                       // Self explanatory ...

  // Arithmetic operators
  TJL& operator=( const TJL& );
  TJL& operator=( const T1& );
  TJL& operator+=( const TJL& );
  TJL& operator+=( const T1& );
  TJL& operator-=( const TJL& );
  TJL& operator-=( const T1& );
  TJL& operator*=( const TJL& );
  TJL& operator*=( const T1& );
  TJL& operator/=( const TJL& );
  TJL& operator/=( const T1& );


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
    // reference point into a TJetEnvironment<T1,T2>.
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


#endif // TJL_H
