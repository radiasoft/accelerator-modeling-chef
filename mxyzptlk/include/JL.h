/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                ****
******  MXYZPTLK:  A C++ implementation of differential algebra.      ****
******                                                                ****
******  Copyright (c) 1990  Universities Research Association, Inc.   ****
******                All Rights Reserved                             ****
******                                                                ****
******  Author:    Leo Michelotti                                     ****
******                                                                ****
******             Fermilab                                           ****
******             P.O.Box 500                                        ****
******             Mail Stop 345                                      ****
******             Batavia, IL   60510                                ****
******                                                                ****
******             Phone: (708) 840 4956                              ****
******             Email: michelot@hazel.fnal.gov                     ****
******                    michelotti@adcalc.fnal.gov                  ****
******                    almond::michelotti                          ****
******                                                                ****
******  Release    Version 1.0 : January   31, 1990                   ****
******  Dates:              .1 : October   11, 1990                   ****
******                      .2 : November   6, 1990                   ****
******                      .3 : October    2, 1992                   ****
******                      .4 : July      20, 1993                   ****
******                     2.0   August    12, 1993                   ****
******                     3.0   May       19, 1994                   ****
******                      .1   September 14, 1994                   ****
******                                                                ****
**************************************************************************
**************************************************************************
******                                                                ****
******  This material resulted from work developed under a            ****
******  Government Contract and is subject to the following           ****
******  license: The Government retains a paid-up, nonexclusive,      ****
******  irrevocable worldwide license to reproduce, prepare           ****
******  derivative works, perform publicly and display publicly       ****
******  by or for the Government, including the right to distribute   ****
******  to other Government contractors.  Neither the United          ****
******  States nor the United States Department of Energy, nor        ****
******  any of their employees, makes any warranty, express or        ****
******  implied, or assumes any legal liability or responsibility     ****
******  for the accuracy, completeness or usefulness of any           ****
******  information, apparatus, product, or process disclosed, or     ****
******  represents that its use would not infringe privately owned    ****
******  rights.                                                       ****
******                                                                ****
******  These files are made avaliable for use by specific            ****
******  individuals or a specific group.  They are not to be          ****
******  reproduced or redistributed.                                  ****
******                                                                ****
******  These files are provided "as is" with no warranties of any    ****
******  kind including the warranties of design,                      ****
******  merchantibility and fitness for a particular purpose,         ****
******  or arising from a course of dealing, usage or trade           ****
******  practice.                                                     ****
******                                                                ****
******  These files are provided with no support or obligation on     ****
******  the part of U.R.A. or Fermilab to assist in their use,        ****
******  correction, modification or enhancement.                      ****
******                                                                ****
******  Neither U.R.A. nor Fermilab shall be liable with              ****
******  respect to the infringement of copyrights, trade              ****
******  secrets or any patents by these files or any part             ****
******  thereof.                                                      ****
******                                                                ****
******  In no event will U.R.A., Fermilab, or the author(s) be        ****
******  liable for any lost revenue or profits or other               ****
******  special, indirect and consequential damages, even if          ****
******  they have been advised of the possibility of such             ****
******  damages.                                                      ****
******                                                                ****
**************************************************************************
*************************************************************************/

#ifndef JL_HXX
#define JL_HXX
#ifdef __VISUAL_CPP__
#include <iostream>
#else
#include <iostream.h>
#include <stdio.h>
#endif

#include "dlist.h"
#include "IntArray.h"
#include "Matrix.h"
#include "VectorD.h"
#include "Jet.h"    // Needed for Jet::scratchFile

class LieOperator;
struct JLCterm;
struct JLC;

// *******************************************************************

struct JLterm {    

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
  double coeff() const { return value; }
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
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

  // Constructors and destructors_____________________________________
  JL( Jet__environment* = 0 );
  JL( const double&, Jet__environment* );
  JL( const JL& );
  JL( const JLC&, Jet__environment* );
  JL( JL* );
  ~JL();

  // Public member functions__________________________________________
  JLterm* get();            // Pops the top term, which should be the 
                            // one of lowest weight.
  void addTerm( JLterm* );  // Public only for diagnostic purposes.

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
