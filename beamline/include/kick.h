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
	kick(char* /* name */ );
	kick(double, /* horizontal kick */ double /* vertical kick */);
	kick(char*, double, double );
	kick(const kick&);
	virtual ~kick();

	void propagate( ParticleBunch& x) {bmlnElmnt::propagate( x ); }
	void propagate( Particle& );
	void propagate( JetParticle& );

	void accept(BmlVisitor& v) { v.visitKick( this ); }
	double& horizontalStrength() { return horizontalKick; }
	double& verticalStrength() { return verticalKick; }

	char* Type() { return "kick"; }
	bmlnElmnt* Clone() { return new kick( *this ); }
};

#endif    // KICK_H
