#include <boost/python.hpp>

#include <decapole.h>


void wrap_decapole () {
  

using namespace boost::python;


class_<thinDecapole, bases<bmlnElmnt> >("thinDecapole", init<double>())
  .def( init<char*, double>() )
  .def( "Type",    &thinDecapole::Type);


}
 
#if 0

class thinDecapole : public bmlnElmnt
{
public:
  thinDecapole( double /* strength */ );
  thinDecapole( char*  /* name */,
                double /* strength */ );
  thinDecapole( bmlnElmntData& );
  thinDecapole( const thinDecapole& );
  ~thinDecapole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinDecapole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinDecapole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinDecapole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thinDecapole( *this ); }
} ;

#endif
