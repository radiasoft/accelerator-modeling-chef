/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.3
******                                    
******  File:      JLC.h
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


#ifndef JLC_HXX
#define JLC_HXX

#include <iostream>

#include "dlist.h"
#include "IntArray.h"
#include "Matrix.h"
// ??? REMOVE: #include "JetC.h"    // Needed for JetC::scratchFile
#include "complexAddon.h"
#include "JetC__environment.h"

class LieOperator;
struct JLterm;
struct JL;

// *******************************************************************

struct JLCterm {    

  // Data
  IntArray index; // An integer array giving the derivatives associated
                  //   with the JLCterm.  For example, ( 1, 1, 0, 2 )
                  //   would correspond to D_1^1 D_2^1 D_4^2 .
  int weight;     // The sum of the values in index.  For the above example,
                  //   this would be 4.
  FNAL::Complex value;   // The value associated with the JLCterm.

  // Constructors and destructors
  JLCterm( const JetC__environment* ); 
  JLCterm( const IntArray&, const FNAL::Complex&, const JetC__environment*  );
  JLCterm( const JLCterm* );
  JLCterm( const JLCterm& );
  JLCterm( const JLterm& );
  JLCterm( const JLterm* );
  ~JLCterm();

  // Operators
  void operator=( const JLCterm& );
  FNAL::Complex coeff() const { return value; }
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
} ;


// *******************************************************************
struct JLC : public dlist {  

  int count;        // The number of JLC terms in the variable
  int weight;       // The maximum weight of the terms
  int accuWgt;      // Highest weight computed accurately

  JetC__environment* myEnv;  
                    // Environment of the jet.

  int rc;           // Reference counting for garbage collection

  void insert( JLCterm* );
  void append( JLCterm* );
  JLCterm* remove( dlink* );

  static FILE  *scratchFile;

  // Constructors and destructors_____________________________________
  JLC( const JetC__environment* = 0 );
  JLC( const FNAL::Complex&, JetC__environment* );
  JLC( const JLC& );
  JLC( const JL&, JetC__environment* );
  JLC( JLC* );
  ~JLC();

  // Public member functions__________________________________________
  JLCterm* get();               // Pops the top term, which should be the 
                                // one of lowest weight.
  JLCterm  firstTerm() const;   // Returns a JLterm equivalent to the top term,
                                // which should be the one of lowest weight.
  JLCterm  lowTerm()   const;   // Returns a JLCterm equivalent to the 
                                // non-zero term of lowest weight.
  void addTerm( JLCterm* );     // Public only for diagnostic purposes.

  char isNilpotent() const;
  void writeToFile( char*   /* Name of unopened file */ ) const;
  void writeToFile( FILE* ) const;

  void getReference( FNAL::Complex* ) const;
  void scaleBy( FNAL::Complex );

  void setVariable( const FNAL::Complex&, 
                    const int&, 
                          JetC__environment* = 0 );
                  // WARNING: This routine alters the environment in
                  // WARNING: the third argument.

  void setVariable( const int&, 
                          JetC__environment* = 0 );

  FNAL::Complex standardPart() const;
  void clear();
  FNAL::Complex weightedDerivative( const int* ) const;
  FNAL::Complex derivative( const int* ) const;
  FNAL::Complex operator()( const FNAL::Complex* ) const;
             // Performs a multinomial evaluation of 
             // the JLC variable.  Essentially acts as a 
             // power series expansion.
  JLC& operator()( const JLC* ) const;
             // Self explanatory ...
  // ??? REMOVE JLC& D( const int* ) const;
  // ??? REMOVE            // Performs differentiation of a JLC variable.

  JLC& operator=( const JLC& );
  JLC& operator=( const FNAL::Complex& );
  JLC& operator+=( const FNAL::Complex& );


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

void operator+=( JLC&, const JLC& );   // ??? Why are these void operators?
void operator-=( JLC&, const JLC& );
void operator*=( JLC&, const JLC& );
void operator/=( JLC&, const JLC& );
char operator!=( const JLC&, const JLC& );
char operator!=( const JLC&, const FNAL::Complex& );
char operator!=( const FNAL::Complex&, const JLC& );


// *******************************************************************
//
//     Setup routines, etc.
//

extern ostream& operator<<(ostream&, const JLC&);
extern istream& operator>>(istream&,       JLC&);
extern char operator==( const JLCterm&, const JLCterm& );
extern char operator==( const JLC&,     const JLC& );
extern char operator==( const JLC&,     const FNAL::Complex& );
extern char operator==( const FNAL::Complex&, const JLC& );
extern char operator<=( const JLCterm&, const JLCterm& );
extern char operator%=( const JLCterm&, const JLCterm& );   // Acts like == but compares
                                       // indices only.
extern JLCterm operator*( const JLCterm&, const JLCterm& );

#endif
