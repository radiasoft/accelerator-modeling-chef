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

#ifndef LIEOPERATOR_HXX
#define LIEOPERATOR_HXX

#include "JetVector.h"

class LieOperator : public JetVector
{
public:

  // Friends   ??? Still necessary?
  friend class  Jet;
  friend struct JL;
  friend class  CLieOperator;

  // Constructors and destructors_____________________________________
  LieOperator( Jet__environment* = Jet::lastEnv );
  LieOperator( LieOperator& );
  LieOperator( const Jet& );     // Creates a Hamiltonian vector field.
  LieOperator( char*, 
               Jet__environment* = Jet::lastEnv ); // Creates the identity function.  
  ~LieOperator();

  // Member functions (public)
  void SetComponent(       int  i /* index     */,
                     const Jet& x /* component */ 
                   )
                   { setVariable( x, i ); }
  void setVariable( const Jet&, int );

  void setVariable( const double&, const int& ); // WARNING: alters the environment!!

  LieOperator& operator=( const JetVector& );

  void peekAt();
  void peekAt( char* );

  // Friendly operators ...........................
  friend ostream& operator<<(ostream&, LieOperator&);
  friend istream& operator>>(istream&, LieOperator&);

  // Lie operations ...................................
  Jet operator^( const Jet& ) const; 
                                // Action as a Lie operator
  Jet expMap( const double&, const Jet& );    
                                // Performs exponential map on
                                // the second argument.
  Jet expMap( const Jet&, const Jet& );
                                // The orbit parameter is here
                                // a Jet variable.
  JetVector expMap( const double&, const JetVector& );
                                // Performs exponential map 
                                // componenet wise.
  JetVector expMap( const Jet&, const JetVector& );

  // Lie bracket of two Lie operators
  friend LieOperator operator^( /* const */ LieOperator&,
                                /* const */ LieOperator& );
  // ??? const has to be completely fixed in MXYZPTLK

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
} ;

inline 
LieOperator& 
LieOperator::operator=( const JetVector& x )
{
  return (LieOperator&) JetVector::operator=( x );
}

#endif // LIEOPERATOR_HXX
