#include "beamline.inc"
#include "kick.h"

void kick::localPropagate( Particle& p ) {
	p.state[3] += horizontalKick;
	p.state[4] += verticalKick;
}

void kick::localPropagate( JetParticle& p ) {
	(p.state).SetComponent(3,p.state(3) + horizontalKick );
	(p.state).SetComponent(4,p.state(4) + verticalKick );
}
