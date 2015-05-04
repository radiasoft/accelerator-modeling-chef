/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******
******  File:      normalFormSage.h
******
******  Copyright (c) Fermi Research Alliance
******                Universities Research Association, Inc.
******                Fermilab
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contracts No. DE-AC02-76CH03000
******  and No. DE-AC02-07CH11359.
******
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******
******  Authors:   Leo Michelotti     michelotti@fnal.gov
******             Eric Stern         egstern@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******  Sep 2001        michelotti@fnal.gov
******  - original version of this code was written as a graphical
******    demonstration of normal form coordinates at any order
******    in two-dimensional longitudinal phase space.
******  - It used the already existing procedures in
******    physics_toolkit/src/normalForm.cc to generate the
******    operators, which were then used to create the normal
******    form coordinates.
******  - the original demo was called "macdemo.cc," because it was
******    written for Jim Maclachlan. A modified version still survives
******    as widget_toolkit/tests/lpsDemo.cc.
******
******  Sep 2010        michelotti@fnal.gov
******  - procedures in macdemo are encapsulated into a new class 
******    called EquilibriumSage, because its intent was to
******    generate equilibrium distributions using normal form coordinates.
******  - only prototype version written.
******
******  Jan - Feb 2011  michelotti@fnal.gov
******  - prototype EquilbriumSage is generalized to higher dimensions.
******  - name change to normalFormSage and passed on for testing.
******
******  Feb - Apr 2011  egstern@fnal.gov
******  - changed signature of constructor to accept JetProton
******  - transformations to canonical coordinates (x, p_x, y, p_y, -t, E )
******  - tests added to the code. 
******  - utility functions added, such as checkLinearNormalForm
******  - changed normalFormSage class so it does not create jet environment
******  
******  Jan - Feb 2012  egstern@fnal.gov
******  - added signatures to checkLinearNormalForm
******  - repressed warnings about imaginary coordinates
******
******  Jun 2014        michelotti@fnal.gov
******  - changed signature of constructor to eliminate usage of
******    both a JetProton and a Mapping.
******  - modified code in anticipation of need to extend to
******    usage with offset closed orbits (for Alex Macridin).
******
******  May 2015        michelotti@fnal.gov
******  - this version is (or should be) functionally equivalent to
******    the file given to Alex Macridin in June, 2014. All differences
******    are (or should be) cosmetic only.
******
**************************************************************************
*************************************************************************/


#ifndef NORMALFORMSAGE_H
#define NORMALFORMSAGE_H

#include <iostream>
#include <stdlib.h>

#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/mxyzptlk.h>

class JetParticle;

class normalFormSage
{
 public:
  normalFormSage( JetParticle const&, int theOrder = 1 );
  ~normalFormSage();

  // The following variables are public just for convenience.
  // They should only be modified by class EquilibriumSage!

  MatrixC          E_;    // matrix of eigenvectors
  MatrixC          invE_; // and its inverse
  CLieOperator*    XG_;   // it turns out that _N is predefined
  CLieOperator*    T_;    // by compiler
  MappingC*        f_;
  MappingC*        g_;

  void           setState( const Vector& );
  void           cnvDataToNormalForm(   Vector  const&, VectorC& );
  void           cnvDataFromNormalForm( VectorC const&, Vector&  );
  MatrixC const& getNormalEigenvectors(void);
  bool           checkLinearNormalForm();
  bool           checkLinearNormalForm(double toler);
  Mapping        getCanonMap();
  Mapping        CanonToChef;
  Mapping        ChefToCanon;

  std::vector<double> stationaryActions( double sx, double sy, double sz );

  #if 0
  // placeholder for future development
  Vector         tolerance_mask;
  #endif

  // private:
  int              dim_;
  Vector           closed_orbit_;

  int              order_;
  Jet__environment_ptr p_je_;

  Mapping          canonMap_;    // the canonical map used for normal form calc
};

#endif // NORMALFORMSAGE_H
