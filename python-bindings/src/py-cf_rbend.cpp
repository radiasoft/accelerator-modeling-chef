#include <boost/python.hpp>
#include <CF_rbend.h>
#include <iostream>

void wrap_cf_rbend () {
  

using namespace boost::python;


 class_<CF_rbend, bases<bmlnElmnt> >("CF_rbend", init<double, double, double>())
  .def(init<const char*, double, double, double>())
  .def(init<double, double, double, int>())
  .def(init<const char*, double, double, double, int>())
  .def(init<double, double, double, const Vector&, int>() );
 
}


#if 0

class Proton;
class JetProton;

class CF_rbend : public bmlnElmnt
{
  public:

  CF_rbend( double,     // length  [ meters ]
            double,     // field   [ tesla ]
            double,     // Entry angle [ radians ]
            int = 1 );
 

  CF_rbend( const char*,// name
            double,     // length  [ meters ]
            double,     // field   [ tesla ]
            double,     // Entry angle [ radians ]
            int = 1 );


  CF_rbend( double,         // length     [ meter    ]
            double,         // field      [ tesla    ]
            double,         // angle      [ radians  ]
            const Vector&,  // multipoles [ meter^-n ]
            // Normal and skew multipoles alternate.
            // xmlt(0)       = b_0    xmlt(1)     = a_0
            // xmlt(2)       = b_1    xmlt(3)     = a_1
            // xmlt(4)       = b_2    xmlt(5)     = a_2
            // ...
            // xmlt(2*m)     = b_[m]  xmlt(2*m+1) = a_[m]
            int = 1 );


  CF_rbend( const char*,    // name
            double,         // length     [ meter    ]
            double,         // field      [ tesla    ]
            double,         // angle      [ radians  ]
            const Vector&,  // multipoles [ meter^-n ]
            int = 1 );


  CF_rbend( const CF_rbend& );

  ~CF_rbend();
  void eliminate();


  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  void localPropagate( ParticleBunch& x ) 
    { bmlnElmnt::localPropagate( x ); }

  void accept( BmlVisitor& v ) { v.visitCF_rbend( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitCF_rbend( this ); }
  void acceptInner( BmlVisitor& v );
  void acceptInner( ConstBmlVisitor& v );
  
  void peekAt( double& s, Particle* = 0 );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "CF_rbend") != 0 )
    return bmlnElmnt::isType(c); else return 1; }
  
  bmlnElmnt* Clone() const 
    { return new CF_rbend( *this ); }
  double OrbitLength( const Particle& );
  void Split( double, bmlnElmnt**, bmlnElmnt** );

  double PoleFaceAngle()    
    { return _poleFaceAngle; }
  double getPoleFaceAngle() 
    { return _poleFaceAngle; }
  double getEntryAngle() 
    { return _poleFaceAngle; }

  double AdjustPosition( const Proton& );
  double AdjustPosition( const JetProton& );


  int setQuadrupole ( double );  
  int setSextupole  ( double );  
  int setOctupole   ( double );  
  // The argument is integrated multipole strength
  // i.e., .setQuadrupole ( B'l   )
  //       .setSextupole  ( B''l/2 )
  //       .setOctupole   ( B'''l/6 )
  // 
  // Returns 0 if multipole is set correctly.
  //         1 if there are no multipoles of required type.
  //           (this should never happen)

  double getQuadrupole() const;
  double getSextupole()  const;
  double getOctupole()   const;
  // Returns integrated multipole strengths
  // i.e., .getQuadrupole() returns B'l
  //       .getSextupole()  returns B"l/2


 private:

  bmlnElmnt** _u;
  bmlnElmnt** _v;

  double _poleFaceAngle;

  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);
};


#endif // CF_RBEND
