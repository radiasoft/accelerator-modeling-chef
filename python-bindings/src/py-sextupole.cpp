#include <boost/python.hpp>
#include <sextupole.h>

void wrap_sextupole () {
  

using namespace boost::python;


class_<sextupole, bases<bmlnElmnt> >("sextupole", init<double,double>() )
  .def(init<char*, double, double>() )
  .def("setCurrent",         &sextupole::setCurrent)
  .def("Type",               &sextupole::Type);

class_<thinSextupole, bases<bmlnElmnt> >("thinSextupole", init<double>() )
  .def( init<char*, double>() )
  .def("Type",  &thinSextupole::Type);
 
}

#if 0

class sextupole : public bmlnElmnt
{
public:
  sextupole( double, /* length   */
             double  /* strength */ );
  sextupole( char*,  /* name     */
             double, /* length   */
             double  /* strength */ );
  sextupole( bmlnElmntData& );
  sextupole( const sextupole& );
  ~sextupole();

  void setStrength( double );
  void setStrength( double, int );

  void setCurrent( double );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitSextupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitSextupole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "sextupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new sextupole( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** );
} ;



class thinSextupole : public bmlnElmnt
{
public:
  thinSextupole( double /* strength */ );
  thinSextupole( char*  /* name */,
                 double /* strength */ );
  thinSextupole( bmlnElmntData& );
  thinSextupole( const thinSextupole& );
  ~thinSextupole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinSextupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinSextupole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinSextupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thinSextupole( *this ); }
} ;

class JetthinSext : public bmlnElmnt
{
private:
  Jet K2L;
  int strengthIndex;
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

public:
  JetthinSext( double, int );
  JetthinSext( char*,       // name
	       double,      // B-prime L in Tesla; + = horizontally focussing
	       int );       // index of focal length parameter (> 6)
  JetthinSext( bmlnElmntData& );
  JetthinSext( const JetthinSext& );
  ~JetthinSext();
  
  void setStrength( double );
  void setStrength( double, int );
  
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );
  
  void accept( BmlVisitor& v ) { v.visitJetthinSext( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitJetthinSext( this ); }

  void eliminate();
  
  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "JetthinSext") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new JetthinSext( *this ); }
  
} ;
 
#endif
