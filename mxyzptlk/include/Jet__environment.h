#ifndef JETENV_H
#define JETENV_H

#include "Matrix.h"
#include "IntArray.h"
#include "slist.h"

struct JetC__environment;
class Jet;

struct Jet__environment {
 int     NumVar;        // Number of scalar variables associated with 
			//   the JL variable.
 int     SpaceDim;      // This is, if you will, the dimension of phase space.
 int     dof;           // The number of degrees of freedom = SpaceDim / 2.
 double* refPoint;      // Reference point in phase space about which 
			//   derivatives are taken.
 double* scale;         // An array containing numbers which scale the
                        //   problem's variables.  Each entry is a "typical
                        //   size" for the variable of the same index.
                        // 
 int     MaxWeight;     // Maximum acceptable weight of a JL variable,
			//   equivalent to the maximum order of derivatives
			//   to be carried through calculations.
                        // ----------------------------------------------
                        // 
 double* monomial;      // Storage area for monomials used in multinomial
			//   evaluation.  The area itself is defined by 
			//   JLSetup.
 Jet*    JLmonomial;    // Storage area for JL monomials used in concatenation.
			//   The area itself is defined by JLSetup.
 int*    exponent;      // Used by nexcom (as called by JL::operator()
			//   when storing monomials. The array exponent 
			//   is allocated by JLSetup.
 char*   expCode;       // The Wilf code of a monomial. 
 short   PBOK;          // Taking Poisson brackets is OK: the dimension of 
			//   phase space is even.
 MatrixI* numPaths;     // An array counting the number of paths to a vertex 
			//   in the Wilf graph.  JLSetup arranges that 
			//   numPaths[w][n] = number of compositions of
			//   a weight  w  into  n  parts.
 IntArray AllZeroes;    // An integer array containing zeroes.  Used to 
			//   filter the standard part of a JL variable.
 slist   myCoords;      // Coordinates of this environment.

 // Member functions -------------------------------------
 Jet__environment();
 Jet__environment( Jet__environment& );
 Jet__environment( const JetC__environment& );
 ~Jet__environment();

 Jet__environment& operator=( const Jet__environment& );
 Jet__environment& operator=( const JetC__environment& );
 char operator==( const Jet__environment& ) const;
 char operator!=( const Jet__environment& ) const;

 // Monomial ranking routines based on Wilf's algorithm
 void monoCode();     // Computes the code of a monomial from its
                      // exponents.
 void monoDecode();   // The inverse operation to monoCode.
 int  monoRank();     // Computes the rank of a monomial.

 // Streams
 friend ostream& operator<<( ostream&, const Jet__environment& );
 friend istream& streamIn( istream&, Jet__environment** );
 // friend ostream& operator>>( ostream&, const Jet__environment* );
 // will not work properly.

 // A global
 static char SkipEnvEqTest;

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};


#endif
