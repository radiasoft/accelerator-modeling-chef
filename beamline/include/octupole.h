/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      octupole.h
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******                                                                
****** REVISION HISTORY
****** 
****** Mar 2007           ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator scheme
******
**************************************************************************
*************************************************************************/
#ifndef OCTUPOLE_H
#define OCTUPOLE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/Particle.h>

class BmlVisitor;
class ConstBmlVisitor;

template<typename T> 
class TVector;


class octupole;
class thinOctupole;

typedef boost::shared_ptr<octupole>     OctupolePtr;
typedef boost::shared_ptr<thinOctupole> ThinOctupolePtr;

typedef boost::shared_ptr<octupole const >     ConstOctupolePtr;
typedef boost::shared_ptr<thinOctupole const>  ConstThinOctupolePtr;


class DLLEXPORT octupole : public bmlnElmnt {

  class Propagator;

public:

  typedef boost::shared_ptr<BasePropagator<octupole> > PropagatorPtr;   

  octupole();
  octupole( char const* name, double const& length,    double const& strength);

  octupole( octupole const& );
  octupole* Clone() const { return new octupole( *this ); }

  octupole& operator=( octupole const& rhs);

 ~octupole();

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  void setStrength( double const& );

  void setCurrent( double const& );

  void localPropagate(         Particle& p );
  void localPropagate(      JetParticle& p );
  void localPropagate(    ParticleBunch& b );
  void localPropagate( JetParticleBunch& b );

  const char* Type() const;
  bool isMagnet() const;

  void Split( double const&, ElmPtr&, ElmPtr& ) const;

 private:

  PropagatorPtr propagator_; 
};


class DLLEXPORT thinOctupole : public bmlnElmnt {

  class Propagator;

public:

  typedef boost::shared_ptr<BasePropagator<thinOctupole> > PropagatorPtr;   

  thinOctupole();
  thinOctupole( char const* name,  double const& strength );
  thinOctupole( thinOctupole const& );

  thinOctupole* Clone() const { return new thinOctupole( *this ); }

  thinOctupole& operator=( thinOctupole const& rhs);

 ~thinOctupole();

  void localPropagate(         Particle& p );
  void localPropagate(      JetParticle& p );
  void localPropagate(    ParticleBunch& b );
  void localPropagate( JetParticleBunch& b );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

 private:
  
  PropagatorPtr propagator_; 

};

#endif // OCTUPOLE_H
