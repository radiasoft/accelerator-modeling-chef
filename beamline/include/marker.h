#ifndef MARKER_H
#define MARKER_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class marker : public bmlnElmnt
{
public:
  marker();                   // Data to be written to standard output
  marker( char* );            // Name identifier.
  marker( const marker& );
  marker( bmlnElmntData& );
  ~marker();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&   );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitMarker( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitMarker( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "marker") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new marker( *this ); }
} ;

#endif // MARKER_H
