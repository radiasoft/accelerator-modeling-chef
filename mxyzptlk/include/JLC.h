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

#ifndef JLC_HXX
#define JLC_HXX

#ifdef __VISUAL_CPP__
#include <iostream>
#else
#include <stdio.h>
#include <iostream.h>
#endif

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
