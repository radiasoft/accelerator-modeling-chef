#ifndef KICK_H
#define KICK_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class hkick : public bmlnElmnt
{
public:
  hkick();
  hkick( double );   // kick size in radians
  hkick( char* );    // name; assumes zero kick
  hkick( char*,      // name
         double );   // kick size in radians
  hkick( char*,      // name
         double,     // length
         double );   // kick size in radians
  hkick( const hkick& );
  hkick( bmlnElmntData& );

  ~hkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "hkick") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new hkick( *this ); }

  void accept( BmlVisitor& v ) { v.visitHkick( this ); }
};


class vkick : public bmlnElmnt
{
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

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "vkick") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new vkick( *this ); }
};


class kick : public bmlnElmnt {
private:
        double horizontalKick;
        double verticalKick;
        istream& readFrom(istream&);
        ostream& writeTo(ostream&);
public:
        kick();
        kick( const char* /* name */ );
        kick( double, /* horizontal kick */ double /* vertical kick */ );
        kick( const char*, double, double );
        kick( double, // length
              double, // horizontal kick 
              double  // vertical kick 
            );
        kick( const char*, // name 
              double,      // length
              double,      // horizontal kick 
              double       // vertical kick
            );
        kick( const kick& );
        virtual ~kick();

        void localPropagate( ParticleBunch& x) {bmlnElmnt::localPropagate( x ); }
        void localPropagate( Particle& );
        void localPropagate( JetParticle& );

        void accept(BmlVisitor& v) { v.visitKick( this ); }
        double& horizontalStrength() { return horizontalKick; }
        double& verticalStrength() { return verticalKick; }

        const char* Type() const;
        bmlnElmnt* Clone() const { return new kick( *this ); }
};

#endif    // KICK_H
