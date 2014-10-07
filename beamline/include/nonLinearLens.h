/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      nonLinearLens.h
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
******  Author:    Chong Shik Park
******             Email: cspark@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******
******  Aug 2014            cspark@fnal.gov
******  - implement non-linear lens first time
******
**************************************************************************
*************************************************************************/

#ifndef NONLINEARLENS_H
#define NONLINEARLENS_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>


class BmlVisitor;
class ConstBmlVisitor;
class nonLinearLens;

typedef boost::shared_ptr<nonLinearLens> NonLinearLensPtr; 
typedef boost::shared_ptr<nonLinearLens const> ConstNonLinearLensPtr; 


class DLLEXPORT nonLinearLens : public bmlnElmnt {

  class Propagator;  

public:

  typedef boost::shared_ptr<BasePropagator<nonLinearLens> > PropagatorPtr; 

  // length    in meters^-1
  // strength (B') in Tesla-meters^-1
  // knll: the integrated strength of the lens in meter. The strength is
  //       parameterized so that the quadrupol term of the multipole
  //       expansion is k1 = 2 * knll / cnll^2
  // cnll: the dimensional parameter of the lens in meter. The singularities
  //       of the potential are located at x=[-cnll, cnll] and y=0.

  // Ctors
  // -----
  nonLinearLens( char const* name, double const& knll, double const& cnll );
  nonLinearLens( char const* name, double const& knll, double const& cnll, double const& bcoeff, double const& dcoeff );
  nonLinearLens( nonLinearLens const& );

  nonLinearLens* Clone() const;

  nonLinearLens& operator=( nonLinearLens const& rhs );

  ~nonLinearLens();

  // Modifiers
  // ---------
  void set_knll( double const& x );
  void set_cnll( double const& x );
  void set_bcoeff( double const& x );
  void set_dcoeff( double const& x );

  double const& get_knll() { return knll_; }
  double const& get_cnll() { return cnll_; }
  double const& get_bcoeff() { return bcoeff_; }
  double const& get_dcoeff() { return dcoeff_; }

  // Propagators
  // -----------
  void localPropagate(         Particle& p );
  void localPropagate(      JetParticle& p );
  void localPropagate(    ParticleBunch& b ); 
  void localPropagate( JetParticleBunch& b ); 

  // Visitor Methods
  // ---------------
  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  // Queries
  // -------
  const char* Type() const;
  bool isMagnet() const;

  void usePropagator( PropagatorPtr& );

private:

  nonLinearLens();         // default constructor forbidden

  double knll_;
  double cnll_;
  double bcoeff_;
  double dcoeff_;

  PropagatorPtr            propagator_;

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

} ;


#endif // NONLINEARLENS_H
