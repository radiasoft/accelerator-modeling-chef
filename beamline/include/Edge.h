/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Edge.h
******                                                                
******
**************************************************************************
*************************************************************************/
#ifndef EDGE_H
#define EDGE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;
class quadrupole;
class Edge;

typedef boost::shared_ptr<Edge>              EdgePtr; 
typedef boost::shared_ptr<Edge const>   ConstEdgePtr; 

class DLLEXPORT Edge : public bmlnElmnt {

  class Propagator;  

public:

  typedef boost::shared_ptr<BasePropagator<Edge> > PropagatorPtr;   

  Edge();
  Edge( char const* name, double const& strength );    
  Edge( Edge const& );

  Edge* Clone() const;

 ~Edge();

  void localPropagate(           Particle& p );
  void localPropagate(        JetParticle& p );
  void localPropagate(      ParticleBunch& b );
  void localPropagate(   JetParticleBunch& b );

  void accept( BmlVisitor& v );           
  void accept( ConstBmlVisitor& v ) const;

  bool        isMagnet()   const;
  char const* Type()       const;

 private:

  PropagatorPtr     propagator_;  

};

#endif // EDGE_H
