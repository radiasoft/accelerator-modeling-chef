/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
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
  Complex value;   // The value associated with the JLCterm.

  // Constructors and destructors
  JLCterm( const JetC__environment* ); 
  JLCterm( const IntArray&, const Complex&, const JetC__environment*  );
  JLCterm( const JLCterm* );
  JLCterm( const JLCterm& );
  JLCterm( const JLterm& );
  JLCterm( const JLterm* );
  ~JLCterm();

  // Operators
  void operator=( const JLCterm& );
  Complex coeff() const { return value; }
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
  JLC( const Complex&, JetC__environment* );
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

  void getReference( Complex* ) const;
  void scaleBy( Complex );

  void setVariable( const Complex&, 
                    const int&, 
                          JetC__environment* = 0 );
                  // WARNING: This routine alters the environment in
                  // WARNING: the third argument.

  void setVariable( const int&, 
                          JetC__environment* = 0 );

  Complex standardPart() const;
  void clear();
  Complex weightedDerivative( const int* ) const;
  Complex derivative( const int* ) const;
  Complex operator()( const Complex* ) const;
             // Performs a multinomial evaluation of 
             // the JLC variable.  Essentially acts as a 
             // power series expansion.
  JLC& operator()( const JLC* ) const;
             // Self explanatory ...
  // ??? REMOVE JLC& D( const int* ) const;
  // ??? REMOVE            // Performs differentiation of a JLC variable.

  JLC& operator=( const JLC& );
  JLC& operator=( const Complex& );
  JLC& operator+=( const Complex& );

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
} ;

void operator+=( JLC&, const JLC& );   // ??? Why are these void operators?
void operator-=( JLC&, const JLC& );
void operator*=( JLC&, const JLC& );
void operator/=( JLC&, const JLC& );
char operator!=( const JLC&, const JLC& );
char operator!=( const JLC&, const Complex& );
char operator!=( const Complex&, const JLC& );


// *******************************************************************
//
//     Setup routines, etc.
//

extern ostream& operator<<(ostream&, const JLC&);
extern istream& operator>>(istream&,       JLC&);
extern char operator==( const JLCterm&, const JLCterm& );
extern char operator==( const JLC&,     const JLC& );
extern char operator==( const JLC&,     const Complex& );
extern char operator==( const Complex&, const JLC& );
extern char operator<=( const JLCterm&, const JLCterm& );
extern char operator%=( const JLCterm&, const JLCterm& );   // Acts like == but compares
                                       // indices only.
extern JLCterm operator*( const JLCterm&, const JLCterm& );

#endif
