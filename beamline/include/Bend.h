/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
****** BEAMLINE:  C++ objects for design and analysis
******            of beamlines, storage rings, and   
******            synchrotrons.                      
******                                    
****** File:      Bend.h
******                                                                
****** REVISION HISTORY 
******
**************************************************************************
*************************************************************************/

#ifndef BEND_H
#define BEND_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class Bend;

typedef boost::shared_ptr<Bend>            BendPtr;
typedef boost::shared_ptr<Bend const> ConstBendPtr;


class DLLEXPORT Bend : public bmlnElmnt {

  class Propagator;
  class bend_core_access;

  friend class  bend_core_access; 
   
public:

  typedef boost::shared_ptr<BasePropagator<Bend> > PropagatorPtr;   

  enum BendType {type_sbend, type_rbend };

  Bend(  char const* name, double const& orbit_length_m, 
	                    double const& bfield_T, 
                            double const& bend_angle,
                            double const& usAngle,
	                    double const& dsAngle,
                            double const& usFaceAngle,
                            double const& dsFaceAngle, BendType bend_type );

  Bend(  Bend const& );

  Bend* Clone() const { return new Bend( *this ); }

 ~Bend();

  void localPropagate(         Particle&   p );
  void localPropagate(      JetParticle&   p ); 
  void localPropagate(    ParticleBunch&   b );
  void localPropagate( JetParticleBunch&   b ); 

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type() const;
  bool isMagnet() const;

  BendType getBendType() const;

  void setLength( double const& );

  void Split( double const&, ElmPtr&, ElmPtr& ) const;

private:

  BendType type_;           // the type of bend ( type_sbend or type_rbend) 
  double   angle_;          // [radians] bend angle

  double usAngle_;          // [radians] as defined in MAD for Bends.
  double dsAngle_;          

  double usFaceAngle_;      // [radians] entry (upstream) and exit (downstream) 
  double dsFaceAngle_;      // angles of the fiducial orbit referenced
                            // to the physical edge of the magnet. 

  PropagatorPtr  propagator_;

  std::ostream& writeTo(std::ostream&); 
  std::istream& readFrom(std::istream&); 

};

class Bend::bend_core_access {

 public:

  static double&                    get_angle( Bend& o )      { return o.angle_;       }
  static double&                    get_usAngle( Bend& o)     { return o.usAngle_;     } 
  static double&                    get_dsAngle( Bend& o)     { return o.dsAngle_;     }
  static double&                    get_usFaceAngle( Bend& o) { return o.usFaceAngle_; }
  static double&                    get_dsFaceAngle( Bend& o) { return o.dsFaceAngle_; }

}; 

#endif // BEND_H
