#include <boost/python.hpp>

#include <octupole.h>

void wrap_octupole () {
  

using namespace boost::python;


class_<octupole,bases<bmlnElmnt> >("octupole", init<double,double>())
  .def(init< char*, double, double>() );
 
}


#if 0

class octupole : public bmlnElmnt
{
public:
  octupole( double, /* length   */
            double  /* strength */ );
  octupole( char*,  /* name     */
            double, /* length   */
            double  /* strength */ );
  octupole( const octupole& );
  octupole( bmlnElmntData& );
  ~octupole();

  void accept( BmlVisitor& v ) { v.visitOctupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitOctupole( this ); }

  void setStrength( double );
  void setStrength( double, int );

  void setCurrent( double );

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "octupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new octupole( *this ); }
  void Split( double, bmlnElmnt**, bmlnElmnt** );
};



class thinOctupole : public bmlnElmnt
{
public:
  thinOctupole( double /* strength */ );
  thinOctupole( char*  /* name */,
                double /* strength */ );
  thinOctupole( bmlnElmntData& );
  thinOctupole( const thinOctupole& );
  ~thinOctupole();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& p );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitThinOctupole( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinOctupole( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinOctupole") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  
  bmlnElmnt* Clone() const { return new thinOctupole( *this ); }
} ;

#endif
