/*************************************************************************
**************************************************************************
**************************************************************************
******
******  MXYZPTLK:  A C++ implementation of differential algebra.
******  Version:   4.3
******
******  File:      JL.h
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


// Private memory allocators
// J.F. Ostiguy - Feb 11 1999
//#define __PRIVATE_ALLOCATOR__

#ifndef JL_HXX
#define JL_HXX


#include <iostream>

#include "dlist.h"
#include "IntArray.h"
#include "Matrix.h"
#include "VectorD.h"
// ??? REMOVE: #include "Jet.h"    // Needed for Jet::scratchFile
#include "Jet__environment.h"

#ifdef __PRIVATE_ALLOCATOR__
#include <vmalloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

class  LieOperator;
struct JLCterm;
struct JLC;

// *******************************************************************

struct JLterm {

#ifdef __PRIVATE_ALLOCATOR__
  private:

  static int _init;
  static Vmalloc_t* _vmem;
  static void meminit(size_t size);

  public:

  void* operator new(size_t size);
  void  operator delete(void* obj, size_t size);
#endif

  // Data
  IntArray index; // An integer array giving the derivatives associated
                  //   with the JLterm.  For example, ( 1, 1, 0, 2 )
                  //   would correspond to D_1^1 D_2^1 D_4^2 .
  int weight;     // The sum of the values in index.  For the above example,
                  //   this would be 4.
  double value;   // The value associated with the JLterm.

  // Constructors and destructors
  JLterm( const Jet__environment* );
  JLterm( const IntArray&, const double&, const Jet__environment*  );
  JLterm( const JLterm* );
  JLterm( const JLterm& );
  JLterm( const JLCterm& );
  ~JLterm();

  // Operators
  void operator=( const JLterm& );
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

  // Accessors
  IntArray& exponents()         { return index; }
  IntArray  exponents()   const { return index; }
  double&   coefficient()       { return value; }
  double    coefficient() const { return value; }
  double    coeff()       const { return value; }  // old
} ;


// *******************************************************************
struct JL : public dlist {

  int count;        // The number of JL terms in the variable
  int weight;       // The maximum weight of the terms
  int accuWgt;      // Highest weight computed accurately

  Jet__environment* myEnv;
                    // Environment of the jet.

  int rc;           // Reference counting for garbage collection

  void insert( JLterm* );
  void append( JLterm* );
  JLterm* remove( dlink* );

  static FILE  *scratchFile;

  // Constructors and destructors_____________________________________
  JL( const Jet__environment* = 0 );
  JL( const double&, Jet__environment* );
  JL( const JL& );
  JL( const JLC&, Jet__environment* );
  JL( JL* );
  ~JL();

  // Public member functions__________________________________________
  JLterm* get();                // Pops the top term, which should be the
                                // one of lowest weight.
  JLterm  firstTerm() const;    // Returns a JLterm equivalent to the top term,
                                // which should be the one of lowest weight.
  JLterm  lowTerm()   const;    // Returns a JLterm equivalent to the
                                // non-zero term of lowest weight.
  void addTerm( JLterm* );      // Public only for diagnostic purposes.

  char isNilpotent() const;
  void writeToFile( char*   /* Name of unopened file */ ) const;
  void writeToFile( FILE* ) const;

  void getReference( double* ) const;
  void scaleBy( double );

  void setVariable( const double&,
                    const int&,
                          Jet__environment* = 0 );
                  // WARNING: This routine alters the environment in
                  // WARNING: the third argument.

  void setVariable( const int&,
                          Jet__environment* = 0 );

  double standardPart() const;
  void clear();
  double weightedDerivative( const int* ) const;
  double derivative( const int* ) const;

  double operator()( const Vector& ) const;
  double operator()( const double* ) const;
                       // Performs a multinomial evaluation of
                       // the JL variable.  Essentially acts as a
                       // power series expansion.
  JL& operator()( const JL* ) const;
                       // Self explanatory ...

  // ??? REMOVE JL& operator()( LieOperator& );  // Self explanatory ...

  // ??? REMOVE JL& D( const int* )const ;		   // Performs differentiation of a JL variable.

  JL& operator=( const JL& );
  JL& operator=( const double& );
  JL& operator+=( const double& );


  // Exception subclasses____________________________________________
  struct GenericException : public std::exception
  {
    GenericException( std::string, int, const char* = "", const char* = "" );
    // Miscellaneous errors
    // 1st argument: name of file in which exception is thrown
    // 2nd         : line from which exception is thrown
    // 3rd         : identifies function containing throw
    // 4th         : identifies type of error
    ~GenericException() throw() {}
    const char* what() const throw();
    std::string errorString;
  };

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
    // reference point into a Jet__environment.
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

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
} ;

void operator+=( JL&, const JL& );   // ??? Why are these void operators?
void operator-=( JL&, const JL& );
void operator*=( JL&, const JL& );
void operator/=( JL&, const JL& );
char operator!=( const JL&, const JL& );
char operator!=( const JL&, const double& );
char operator!=( const double&, const JL& );


// *******************************************************************
//
//     Setup routines, etc.
//

// ??? REMOVE extern void JLFixReference( double* );   // ??? Still exists???
// ??? REMOVE extern void JLFixReference( JL& );
// ??? REMOVE extern void JLFixReferenceAtStart( const LieOperator& );
// ??? REMOVE extern void JLFixReferenceAtEnd( LieOperator& );

extern ostream& operator<<(ostream&, const JL&);
extern istream& operator>>(istream&,       JL&);
extern char operator==( const JLterm&, const JLterm& );
extern char operator==( const JL&,     const JL& );
extern char operator==( const JL&,     const double& );
extern char operator==( const double&, const JL& );
extern char operator<=( const JLterm&, const JLterm& );
extern char operator%=( const JLterm&, const JLterm& );
                                       // Acts like == but compares
                                       // indices only.
extern JLterm operator*( const JLterm&, const JLterm& );

#endif // JL_HXX
