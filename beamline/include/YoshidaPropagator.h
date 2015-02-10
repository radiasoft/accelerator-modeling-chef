/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      YoshidaPropagator.h
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
******  Jun 2013           michelotti@fnal.gov
******  - initial implementation. This is a prototype version, primarily 
******    intended for further testing.  Use cautiously otherwise.
******    Planned future versions will have a different interface
******    (esp. by using templates to expand the application beyond
******    quadrupole to other beamline elements).
******
******  Feb 2015           michelotti@fnal.gov
******  - removed BmlPtr my_beamline_ptr_ from YoshidaPropagator's
******    private data. Combined with lack of virtual destructors
******    for base propagators, it resulted in a memory leak.
******
**************************************************************************
*************************************************************************/

#ifndef YOSHIDAPROPAGATORS_H
#define YOSHIDAPROPAGATORS_H

#include <beamline/quadrupole.h>

class Particle;
class JetParticle;

class YoshidaPropagator;

typedef boost::shared_ptr<YoshidaPropagator>            YoshidaPropagatorPtr;
typedef boost::shared_ptr<YoshidaPropagator const> ConstYoshidaPropagatorPtr;

class YoshidaPropagator : public BasePropagator<quadrupole>
{

 public:

  // ctors
  YoshidaPropagator( int order = 3, int steps = 1 );  // Number of thin kicks will be 3^order
                                                      // e.g. by default, 3^3 = 27 thin kicks
  YoshidaPropagator( YoshidaPropagator const& ); // Should not be necessary

  ~YoshidaPropagator() {}

  inline YoshidaPropagator* Clone() const;

  void  setup( quadrupole& elm );

  void  operator()( quadrupole& elm,    Particle& p);
  void  operator()( quadrupole& elm, JetParticle& p);

  // Bunch propagators not redefined; will use the default.

 private:

  int    order_;
  int    steps_;
  double accum_az_;
  double accum_kick_;
  double prev_az_;

  void DriftMap_( double length );
  void KickMap_ ( BmlPtr&, double length, double strength );
  void generate_map_( BmlPtr&, int n, double t, double strength );

};


YoshidaPropagator* YoshidaPropagator::Clone() const
{ 
  return new YoshidaPropagator( *this ); 
}

#endif // YOSHIDAPROPAGATORS_H
