#ifndef DECAPOLE_H
#define DECAPOLE_H

#include "bmlnElmnt.h"

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

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinDecapole") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thinDecapole( *this ); }
} ;

#endif // DECAPOLE_H
