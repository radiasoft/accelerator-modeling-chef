#ifndef SROT_H
#define SROT_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class srot : public bmlnElmnt
{
public:
  srot();
  srot( double /* strength = rotation angle in radians */ );
  srot( char * /* name */ );
  srot( char * /* name */, double /* strength */ );
  srot( const srot& );
  ~srot();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitSrot( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitSrot( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "srot") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new srot( *this ); }
};

#endif // SROT_H
