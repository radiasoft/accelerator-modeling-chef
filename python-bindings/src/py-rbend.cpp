#include <boost/python.hpp>

#include <rbend.h>
#include <Particle.h>

void wrap_rbend () {
  
using namespace boost::python;


  void   (rbend::* setEntryAngle_overload_1)( const Particle&)    = &rbend::setEntryAngle;
  void   (rbend::* setEntryAngle_overload_2)( const JetParticle&) = &rbend::setEntryAngle;
  void   (rbend::* setEntryAngle_overload_3)( double)             = &rbend::setEntryAngle;

  double   (rbend::* setPoleFaceAngle_overload_1)( const Particle&)     = &rbend::setPoleFaceAngle;
  double   (rbend::* setPoleFaceAngle_overload_2)( const JetParticle&)  = &rbend::setPoleFaceAngle;


class_<rbend, bases<bmlnElmnt> >("rbend", init<double,double>() )
  .def( init<double, double, double>() )
  .def( init<char*, double, double>() )
  .def( init<char*, double, double, double>() )
  .def( "getReferenceTime",        &rbend::getReferenceTime)
  .def( "PoleFaceAngle",           &rbend::PoleFaceAngle)
  .def( "getPoleFaceAngle" ,       &rbend::getPoleFaceAngle)   
  .def( "getTanPoleFaceAngle",     &rbend::getTanPoleFaceAngle)
  .def( "getEntryAngle",           &rbend::getEntryAngle)
  .def( "setPoleFaceAngle",        setPoleFaceAngle_overload_1)
  .def( "setPoleFaceAngle",        setPoleFaceAngle_overload_2)
  .def( "setEntryAngle",           setEntryAngle_overload_1)
  .def( "setEntryAngle",           setEntryAngle_overload_2)
  .def( "setEntryAngle",           setEntryAngle_overload_3)
  .def( "setAngle",                &rbend::setAngle)
  .def( "Type",                    &rbend::Type);

}


#if 0

class rbend : public bmlnElmnt
{
private:
  // ??? REMOVE: static void P_Face    ( const bmlnElmnt*, Particle& );
  // ??? REMOVE: static void J_Face    ( const bmlnElmnt*, JetParticle& );
  double poleFaceAngle;
  double tanPFAngle;
  double _CT;   // Reference time of traversal, in meters.

  // bmlnElmnt::strength -> magnetic field [T]
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

public:
  class MAD_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::MAD_Prop"; }
  };
  static MAD_Prop LikeMAD;

  class NoEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::NoEdge_Prop"; }

    NoEdge_Prop();
    ~NoEdge_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
  private:
    double _fastArcsin( double x     ) const;
    Jet    _fastArcsin( const Jet& x ) const;
    char   _approx;
  };
  static NoEdge_Prop NoEdge;

  class Exact_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::Exact_Prop"; }

    Exact_Prop();
    ~Exact_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static Exact_Prop Exact;

  class InEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::InEdge_Prop"; }

    InEdge_Prop();
    ~InEdge_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static InEdge_Prop InEdge;

  class OutEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::OutEdge_Prop"; }

    OutEdge_Prop();
    ~OutEdge_Prop();
    char isApproximate();
    void makeApproximate();
    void makeExact();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static OutEdge_Prop OutEdge;

  class Real_Exact_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::Real_Exact_Prop"; }

    Real_Exact_Prop();
    ~Real_Exact_Prop();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static Real_Exact_Prop RealExact;

  class Real_InEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::Real_InEdge_Prop"; }

    Real_InEdge_Prop();
    ~Real_InEdge_Prop();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static Real_InEdge_Prop RealInEdge;

  class Real_OutEdge_Prop : public bmlnElmnt::PropFunc
  {
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "rbend::Real_OutEdge_Prop"; }

    Real_OutEdge_Prop();
    ~Real_OutEdge_Prop();
    void setPropagator( NoEdge_Prop* );
  private:
    NoEdge_Prop* _myPropagator;
  };
  static Real_OutEdge_Prop RealOutEdge;

  // ------------------------------------------------------

  rbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
         PropFunc*    = &rbend::RealExact );

  rbend( double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // Pole face angle [ radians ]
         PropFunc*    = &rbend::Exact );

  rbend( char*,      // name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
         PropFunc*    = &rbend::RealExact );

  rbend( char*,      // name
         double,     // length  [ meters ]
         double,     // field   [ tesla ]
         double,     // Pole face angle [ radians ]
         PropFunc*    = &rbend::Exact );

  rbend( const rbend& );
  ~rbend();

  void eliminate();

  void accept( BmlVisitor& v ) { v.visitRbend( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitRbend( this ); }

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p ) { (*Propagator)( this, p ); }
  void localPropagate( JetParticle& p ) { (*Propagator)( this, p ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "rbend") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new rbend( *this ); }
  double OrbitLength( const Particle& );
  void Split( double, bmlnElmnt**, bmlnElmnt** );


  double getReferenceTime()    const {return _CT;}
  double PoleFaceAngle()       const {return poleFaceAngle;} 
  double getPoleFaceAngle()    const {return poleFaceAngle;} 
  double getTanPoleFaceAngle() const {return tanPFAngle;}    
  double getEntryAngle()       const {return poleFaceAngle;} 

  double setReferenceTime( const Particle& );  // returns _CT
  double setReferenceTime( double );           // returns previous _CT
  double setPoleFaceAngle( const Particle& );
  double setPoleFaceAngle( const JetParticle& );
  void   setEntryAngle( const Particle& x )
  {
    this->setPoleFaceAngle( x );
  }
  void setEntryAngle( const JetParticle& x )
  {
    this->setPoleFaceAngle( x );
  }
  void setEntryAngle( double a )
  { 
     poleFaceAngle = a;
     tanPFAngle    = tan( poleFaceAngle );
  }
  double setAngle( double a )
  { 
     this->setEntryAngle(a);
     return poleFaceAngle;
  }
};

#endif
