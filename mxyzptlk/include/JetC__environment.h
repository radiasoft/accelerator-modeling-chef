#ifndef JETCENV_H
#define JETCENV_H

#include "Matrix.h"
#include "IntArray.h"
#include "slist.h"

struct Jet__environment;
class JetC;

struct JetC__environment {
 int       NumVar;        // Number of scalar variables associated with 
                          //   the JLC variable.
 int       SpaceDim;      // This is, if you will, the dimension of phase space.
 int       dof;           // The number of degrees of freedom = SpaceDim / 2.
 Complex*  refPoint;      // Reference point in phase space about which 
                          //   derivatives are taken.
 double*   scale;         // An array containing numbers which scale the
                          //   problem's variables.  Each entry is a "typical
                          //   size" for the variable of the same index.
                          // 
 int       MaxWeight;     // Maximum acceptable weight of a JLC variable,
                          //   equivalent to the maximum order of derivatives
                          //   to be carried through calculations.
                          // ----------------------------------------------
                          // 
 Complex*  monomial;      // Storage area for monomials used in multinomial
                          //   evaluation.  The area itself is defined by 
                          //   JLCSetup.
 JetC*     JLCmonomial;   // Storage area for JLC monomials used in concatenation.
                          //   The area itself is defined by JLCSetup.
 int*      exponent;      // Used by nexcom (as called by JLC::operator()
                          //   when storing monomials. The array exponent 
                          //   is allocated by JLCSetup.
 char*     expCode;       // The Wilf code of a monomial. 
 char      PBOK;          // Taking Poisson brackets is OK: the dimension of 
                          //   phase space is even.
 MatrixI*  numPaths;      // An array counting the number of paths to a vertex 
                          //   in the Wilf graph.  JLCSetup arranges that 
                          //   numPaths[w][n] = number of compositions of
                          //   a weight  w  into  n  parts.
 IntArray  AllZeroes;     // An integer array containing zeroes.  Used to 
                          //   filter the standard part of a JLC variable.
 slist     myCoords;      // Coordinates of this environment.

 // Member functions -------------------------------------
 JetC__environment();
 JetC__environment( JetC__environment& );
 JetC__environment( const Jet__environment& );
 ~JetC__environment();

 JetC__environment& operator=( const JetC__environment& );
 JetC__environment& operator=( const Jet__environment& );
 char operator==( const JetC__environment& ) const;
 char operator!=( const JetC__environment& ) const;

 // Monomial ranking routines based on Wilf's algorithm
 void monoCode();     // Computes the code of a monomial from its
                      // exponents.
 void monoDecode();   // The inverse operation to monoCode.
 int  monoRank();     // Computes the rank of a monomial.

 // Streams
 friend ostream& operator<<( ostream&, const JetC__environment& );
 friend istream& operator>>( istream&, JetC__environment* );

 // A global
 static char SkipEnvEqTest;
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};


#endif
