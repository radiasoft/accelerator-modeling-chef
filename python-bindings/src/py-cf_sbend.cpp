#include <boost/python.hpp>
#include <bmlnElmnt.h>
#include <CF_sbend.h>

using namespace boost::python;

void wrap_cf_sbend () {

  class_<CF_sbend, bases<bmlnElmnt> >("CF_sbend", init<double, double, double>() )

  .def(init<const char*, double, double, double>() )
  .def(init<double, double, double, int>() )
  .def(init<const char*, double, double, double, int>() )
  .def(init<double, double, double, const Vector&,int >() )
  .def(init<const char*, double, double, double, const Vector&, int>() )
  .def("eliminate",        &CF_sbend::eliminate)
  .def("setQuadrupole",    &CF_sbend::setQuadrupole)
  .def("setSextupole",     &CF_sbend::setSextupole)
  .def("setOctupole",      &CF_sbend::setOctupole) 
  .def("setDipoleField",   &CF_sbend::setDipoleField)
  .def("getQuadrupole",    &CF_sbend::getQuadrupole) 
  .def("getSextupole",     &CF_sbend::getSextupole)  
  .def("getOctupole",      &CF_sbend::getOctupole)
  .def("getDipoleField",   &CF_sbend::getDipoleField) 
  .def("getAngle",         &CF_sbend::getAngle)
  .def("Angle",            &CF_sbend::Angle);

}



#if 0

class CF_sbend : public bmlnElmnt
{
  public:

  CF_sbend( double,     // length  [ meters ]
            double,     // field   [ tesla ]
            double,     // angle   [ radians ]
            int = 4 );
 

  CF_sbend( const char*,// name
            double,     // length  [ meters ]
            double,     // field   [ tesla ]
            double,     // angle   [ radians ]
            int = 4 );


  CF_sbend( double,         // length     [ meter    ]
            double,         // field      [ tesla    ]
            double,         // angle      [ radians  ]
            const Vector&,  // multipoles [ meter^-n ]
            // Normal and skew multipoles alternate.
            // xmlt(0)       = b_0    xmlt(1)     = a_0
            // xmlt(2)       = b_1    xmlt(3)     = a_1
            // xmlt(4)       = b_2    xmlt(5)     = a_2
            // ...
            // xmlt(2*m)     = b_[m]  xmlt(2*m+1) = a_[m]
            int = 4 );


  CF_sbend( const char*,    // name
            double,         // length     [ meter    ]
            double,         // field      [ tesla    ]
            double,         // angle      [ radians  ]
            const Vector&,  // multipoles [ meter^-n ]
            int = 4 );


  CF_sbend( const CF_sbend& );

  ~CF_sbend();
  void eliminate();


  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x ) 
    { bmlnElmnt::localPropagate( x ); }

  void accept( BmlVisitor& v ) { v.visitCF_sbend( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitCF_sbend( this ); }
  void acceptInner( BmlVisitor& v );
  void acceptInner( ConstBmlVisitor& v );
  
  void peekAt( double& s, Particle* = 0 );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "CF_sbend") != 0 )
    return bmlnElmnt::isType(c); else return 1; }
  
  bmlnElmnt* Clone() const 
    { return new CF_sbend( *this ); }
  double OrbitLength( const Particle& );
  void Split( double, bmlnElmnt**, bmlnElmnt** );


  int setQuadrupole  ( double );  
  int setSextupole   ( double );  
  int setOctupole    ( double );  
  // The argument is integrated multipole strength
  // i.e., .setQuadrupole ( B'l   )
  //       .setSextupole  ( B''l/2 )
  //       .setOctupole   ( B'''l/6 )
  // 
  // Returns 0 if multipole is set correctly.
  //         1 if there are no multipoles of required type.
  //           (this should never happen)

  int setDipoleField ( double );  
  // Here the argument is the dipole field, 
  // NOT the integrated dipole field.

  double getQuadrupole() const;
  double getSextupole()  const;
  double getOctupole()   const;
  // Returns integrated multipole strengths
  // i.e., .getQuadrupole() returns B'l
  //       .getSextupole()  returns B''l/2
  //       .getOctupole()   returns B'''l/6

  double getDipoleField() const;
  // Returns the dipole field,
  // NOT the integrated dipole field.

  double getAngle() const;
  double Angle()    const;

 private:

  bmlnElmnt** _u;
  bmlnElmnt** _v;

  double _angle; // total bend angle  [ radians ]

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
};


#endif // CF_SBEND
