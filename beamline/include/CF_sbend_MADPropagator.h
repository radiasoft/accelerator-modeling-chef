/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      CF_sbend_MADPropagator.h
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
******  Author:    Leo Michelotti
******             Email: michelotti@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******  Nov 2013           michelotti@fnal.gov
******  - initial implementation of class CF_sbend_MADPropagator. This
******  is a prototype version, primarily intended for further
******  testing.  Use cautiously otherwise.  Planned future versions
******  will have a different interface (esp. by using templates to
******  expand the application beyond CF_sbend to other beamline
******  elements).
******
**************************************************************************
*************************************************************************/

#ifndef MADLIKEPROPAGATORS_H
#define MADLIKEPROPAGATORS_H

#include <beamline/CF_sbend.h>
#include <beamline/quadrupole.h>

class Particle;
class JetParticle;

class CF_sbend_MADPropagator;

typedef boost::shared_ptr<CF_sbend_MADPropagator>            CF_sbend_MADPropagatorPtr;
typedef boost::shared_ptr<CF_sbend_MADPropagator const> ConstCF_sbend_MADPropagatorPtr;

class CF_sbend_MADPropagator : public BasePropagator<CF_sbend>
{
 public:
  // ctors
  CF_sbend_MADPropagator();
  CF_sbend_MADPropagator( CF_sbend_MADPropagator const& ); // Should not be necessary

  ~CF_sbend_MADPropagator() {}

  inline CF_sbend_MADPropagator* Clone() const;

  void  setup( CF_sbend& elm );

  void  operator()( CF_sbend& elm,    Particle& p);
  void  operator()( CF_sbend& elm, JetParticle& p);

  // Bunch propagators not redefined; will use the default.


 public:
  static double const zero;
  static double const one;
  static double const two;
  static double const three;
  static double const four;
  static double const six;
  static double const nine;
  static double const twelve;
  static double const fifteen;
  static double const twty;
  static double const twty2;
  static double const twty4;
  static double const thty;
  static double const foty2;
  static double const fvty6;
  static double const svty2;
  static double const httwty;
  static double const c1;
  static double const c2;
  static double const c3;
  static double const c4;
  static double const s1;
  static double const s2;
  static double const s3;
  static double const s4;
  static double const cg0;
  static double const cg1;
  static double const cg2;
  static double const ch0;
  static double const ch1;
  static double const ch2;

 public:
  double ek_[7];
  double re_[7][7];
  double te_[7][7][7];

  static void tmfoc( double el, double sk1,
                     double& c, double& s, double& d, double& f );

  // DGN: Temporary kludge for diagnostic purpose only.
  static double rre[7][7];
  static double tte[7][7][7];
};


CF_sbend_MADPropagator* CF_sbend_MADPropagator::Clone() const
{ 
  return new CF_sbend_MADPropagator( *this ); 
}

#endif // MADLIKEPROPAGATORS_H
