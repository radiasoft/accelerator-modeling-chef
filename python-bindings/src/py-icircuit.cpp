#include <boost/python.hpp>

#include <kick.h>

#include <iostream>

void wrap_kick () {
  

using namespace boost::python;


class_<hkick>("hkick")
  .def(init<double>())
  .def(init<const char*>())
  .def(init<const char*,double>())
  .def(init<const char*,double, double>())
  .def( "Type", &vkick::Type);


class_<vkick>("vkick")
  .def(init<double>())
  .def(init<const char*>())
  .def(init<const char*,double>())
  .def(init<const char*,double, double>())
  .def( "Type", &vkick::Type);


class_<kick>("kick")
  .def(init<double,double>())
  .def(init<const char*>())
  .def(init<const char*,double, double, double>())
  .def(init<const char*,double, double, double>())
  .def( "Type", &vkick::Type);
}


#if 0

public:
  vkick();            // Assumes zero kick
  vkick( double );    // kick size in radians
  vkick( char* );     // name; assumes zero kick
  vkick( char*,       // name
         double  );   // kick size in radians
  vkick( char*,       // name
         double,      // length
         double  );   // kick size in radians
  vkick( const vkick& );
  vkick( bmlnElmntData& );
  ~vkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitVkick( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitVkick( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "vkick") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new vkick( *this ); }
};

#endif
