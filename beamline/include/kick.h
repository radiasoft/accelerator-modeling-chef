#ifndef KICK_H
#define KICK_H

#include "mxyzptlk.h"
#include "beamline.inc"

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

        char* Type() const;
        bmlnElmnt* Clone() const { return new kick( *this ); }
};

#endif    // KICK_H
