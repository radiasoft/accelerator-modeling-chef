/*************************************************************************
**************************************************************************
**************************************************************************
******
******  MXYZPTLK:  A C++ implementation of differential algebra.
******
******  File:      TJL.h
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


#ifndef TJL_H
#define TJL_H

#include "dlist.h"
#include "IntArray.h"
#include "TMatrix.h"
#include "VectorD.h"
#include "TJetEnvironment.h"

// Private memory allocators
// J.F. Ostiguy - Feb 11 1999
//#define __PRIVATE_ALLOCATOR__
#ifdef __PRIVATE_ALLOCATOR__
#include <vmalloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

// template<typename T1, typename T2> class TLieOperator;
// template<typename T1, typename T2> struct TJL;


// *******************************************************************

template<typename T1, typename T2>
struct TJLterm 
{
// Private memory allocators
// J.F. Ostiguy - Feb 11 1999
#ifdef __PRIVATE_ALLOCATOR__
  static int _init;
  static Vmalloc_t* _vmem;
  static void meminit(size_t size);

  void* operator new(size_t size);
  void  operator delete(void* obj, size_t size);
#endif

  // Data
  IntArray _index; // An integer array giving the derivatives associated
                   //   with the JLterm.  For example, ( 1, 1, 0, 2 )
                   //   would correspond to D_1^1 D_2^1 D_4^2 .
  int _weight;     // The sum of the values in index.  For the above example,
                   //   this would be 4.
  T1 _value;       // The value associated with the JLterm.

  // Constructors and destructors
  TJLterm( const TJetEnvironment<T1,T2>* );
  TJLterm( const IntArray&, const T1&, const TJetEnvironment<T1,T2>*  );
  TJLterm( const TJLterm* );
  TJLterm( const TJLterm& );
  TJLterm( const TJLterm<T2,T1>& );
  TJLterm( const TJLterm<T2,T1>* );
  ~TJLterm();

  // Operators
  TJLterm& operator=( const TJLterm& );
  TJLterm& operator*( const TJLterm& );


  // Accessors
  IntArray& exponents()         { return _index; }
  IntArray  exponents()   const { return _index; }
  T1&       coefficient()       { return _value; }
  T1        coefficient() const { return _value; }
  T1        coeff()       const { return _value; }  // old
} ;


// *******************************************************************
template<typename T1, typename T2>
struct TJL : public dlist 
{
  int _count;        // The number of JL terms in the variable
  int _weight;       // The maximum weight of the terms
  int _accuWgt;      // Highest weight computed accurately

  TJetEnvironment<T1,T2>*  _myEnv;
                    // Environment of the jet.

  int _rc;           // Reference counting for garbage collection

  void insert( TJLterm<T1,T2>* );
  void append( TJLterm<T1,T2>* );
  TJLterm<T1,T2>* remove( dlink* );

  static FILE  *scratchFile;

  // Constructors and destructors_____________________________________
  TJL( const TJetEnvironment<T1,T2>* = 0 );
  TJL( const T1&, TJetEnvironment<T1,T2>* );
  TJL( const TJL& );
  TJL( const TJL* );
  TJL( const TJL<T2,T1>&, TJetEnvironment<T1,T2>* );
  ~TJL();

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

  bool isNilpotent() const;
  void writeToFile( char* /* Name of unopened file */ ) const;
  void writeToFile( FILE* ) const;

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

template<typename T1,typename T2> bool operator!=( const TJL<T1,T2>&, const TJL<T1,T2>& );
template<typename T1,typename T2> bool operator!=( const TJL<T1,T2>&, const T1& );
template<typename T1,typename T2> bool operator!=( const T1&, const TJL<T1,T2>& );
template<typename T1,typename T2> std::ostream& operator<<(std::ostream&, const TJL<T1,T2>&);
template<typename T1,typename T2> std::istream& operator>>(std::istream&,       TJL<T1,T2>&);
template<typename T1,typename T2> bool operator==( const TJLterm<T1,T2>&, const TJLterm<T1,T2>& );
template<typename T1,typename T2> bool operator==( const TJL<T1,T2>&,     const TJL<T1,T2>& );
template<typename T1,typename T2> bool operator==( const TJL<T1,T2>&,     const double& );
template<typename T1,typename T2> bool operator==( const double&, const TJL<T1,T2>& );
template<typename T1,typename T2> bool operator<=( const TJLterm<T1,T2>&, const TJLterm<T1,T2>& );
template<typename T1,typename T2> bool operator%=( const TJLterm<T1,T2>&, const TJLterm<T1,T2>& );
                                // Acts like == but compares
                                // indices only.

typedef TJL<double,FNAL::Complex> JL;
typedef TJL<FNAL::Complex,double> JLC;

#endif // TJL_H
