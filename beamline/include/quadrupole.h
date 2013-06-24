/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      quadrupole.h
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
******  Apr 2008           michelotti@fnal.gov
******  - added quadrupole::setLength(..) method to override
******    the base class implementation.
******
******  Mar 2007           ostiguy@fnal.gov
******  - covariant return types
******  - support for reference counted elements
******
******  Dec 2007            ostiguy@fnal.gov
******  - new typesafe propagator scheme
******
******  Jun 2013            michelotti@fnal.gov
******  - restored ability to change the number of thin
******    quadrupole kicks within the (tandem) propagator.
******
******  Jun 2013            michelotti@fnal.gov
******  - added method quadrupole::usePropagator, in conjunction
******    with creation of new class YoshidaPropagator. Putting
******    this here should be temporary. It logically belongs
******    in class bmlnElmnt.
******
**************************************************************************
*************************************************************************/

#ifndef QUADRUPOLE_H
#define QUADRUPOLE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>


class BmlVisitor;
class ConstBmlVisitor;
class quadrupole;
class thinQuad;

typedef boost::shared_ptr<quadrupole> QuadrupolePtr; 
typedef boost::shared_ptr<thinQuad>   ThinQuadPtr; 

typedef boost::shared_ptr<quadrupole const> ConstQuadrupolePtr; 
typedef boost::shared_ptr<thinQuad const>   ConstThinQuadPtr; 


class DLLEXPORT quadrupole : public bmlnElmnt {

  class Propagator;  

public:

  typedef boost::shared_ptr<BasePropagator<quadrupole> > PropagatorPtr; 

  // length    in meters^-1
  // strength (B') in Tesla-meters^-1


  quadrupole();
  quadrupole( char const* name, double const& length, double const& strength );
  quadrupole( quadrupole const& );

  quadrupole* Clone() const;

  ~quadrupole();

  void setStrength( double const& new_strength );
  void setLength( double const& new_length );

  void setNumberOfKicks( int n ); // Changes number of thin quad kicks
                                  // in the propagator.
  int numberOfKicks() const;      // Returns number of thin quad kicks
                                  // in the propagator.

  void localPropagate(         Particle& b );   
  void localPropagate(      JetParticle& b );   
  void localPropagate(    ParticleBunch& b ); 
  void localPropagate( JetParticleBunch& b ); 

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

  void Split( double const&, ElmPtr&, ElmPtr& ) const;

  void usePropagator( PropagatorPtr& );

private:

  PropagatorPtr            propagator_;

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

} ;


//-------------------------------------------------------------------------------

class DLLEXPORT thinQuad : public bmlnElmnt
{

  class Propagator;  

public:

  // integrated_strength (B'L) in Tesla; + = horizontally focussing

  typedef boost::shared_ptr<BasePropagator<thinQuad> > PropagatorPtr; 

  thinQuad();
  thinQuad( char const* name, double const& integrated_strength );    // B'L in Tesla; + = horizontally focussing
  thinQuad( thinQuad const& );

  thinQuad* Clone() const;

 ~thinQuad();

  void localPropagate(         Particle& b );   
  void localPropagate(      JetParticle& b );   
  void localPropagate(    ParticleBunch& b ); 
  void localPropagate( JetParticleBunch& b ); 

  void accept( BmlVisitor& v );           
  void accept( ConstBmlVisitor& v ) const;

  bool        isMagnet()   const;
  char const* Type()       const;

 private:

  PropagatorPtr propagator_;  
};

#endif // QUADRUPOLE_H
