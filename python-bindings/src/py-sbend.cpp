#include <boost/python.hpp>
#include <sbend.h>
#include <iostream>


void wrap_sbend () {
  
using namespace boost::python;


class_<sbend, bases<bmlnElmnt> >("sbend", init<double, double,  double>())
  .def(init<char*,  double,  double,  double>() )
  .def(init<double, double,  double,  bmlnElmnt::PropFunc*>() )
  .def(init<double, double,  double>() )
  .def(init<char*,  double,  double,  double, bmlnElmnt::PropFunc*>() )
  .def(init<char*,  double,  double,  double>())
  .def("setAngle",  &sbend::setAngle)
  .def("getAngle",  &sbend::getAngle)
  .def("Type",      &sbend::Type);

}

#if 0
class sbend : public bmlnElmnt
{
private:
  // bmlnElmnt::strength -> magnetic field [T]
  double angle;      // bend angle  [ radians ]
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

public:

  class MAD_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "sbend::MAD_Prop"; }
  };
  static MAD_Prop LikeMAD;

  class Exact_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "sbend::Exact_Prop"; }
  };
  static Exact_Prop Exact;

  class Approx_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "sbend::Fast_Prop"; }
  private:
    double fastArcsin( double x     ) const;
    Jet    fastArcsin( const Jet& x ) const;
  };
  static Approx_Prop Approx;

  void P_Face ( Particle&,    const double& /* psi */  ) const;
  void J_Face ( JetParticle&, const double& /* psi */  ) const;
  
  sbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // angle   [ radians ]
         PropFunc*    = &sbend::Exact );

  sbend( char*,      // name
         double,     // length  [ meters ]
         double,     // magnetic field [T]
         double,     // angle   [ radians ]
         PropFunc*    = &sbend::Exact );

  sbend( const sbend& );
  ~sbend();

  double setAngle(double a)	{ return (angle = a); }
  double getAngle() { return angle; }

  void geomToEnd   ( BMLN_posInfo& );
  void geomToStart ( BMLN_posInfo& );
  void eliminate();

  void enterLocalFrame( Particle&    ) const;
  void enterLocalFrame( JetParticle& ) const;
  void leaveLocalFrame( Particle&    ) const;
  void leaveLocalFrame( JetParticle& ) const;

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  void accept( BmlVisitor& v ) { v.visitSbend( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitSbend( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "sbend") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new sbend( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** );

} ;

#endif // SBEND_H


