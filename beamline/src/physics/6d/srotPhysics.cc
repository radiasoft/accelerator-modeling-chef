#include "beamline.inc"

void srot::propagate(Particle& p) {
double result[4];

result[0] = p.state[0] * cos(strength) + p.state[1] * sin(strength);
result[1] = p.state[1] * cos(strength) - p.state[0] * sin(strength);
p.state[0] = result[0];
p.state[1] = result[1];
result[2] = p.state[3] * cos(strength) + p.state[4] * sin(strength);
result[3] = p.state[4] * cos(strength) - p.state[3] * sin(strength);
p.state[3] = result[2];
p.state[4] = result[3];
}

void srot::propagate(JetParticle& p) {
Jet result[4];

result[0] = p.state(0) * cos(strength) + p.state(1) * sin(strength);
result[1] = p.state(1) * cos(strength) - p.state(0) * sin(strength);
result[2] = p.state(3) * cos(strength) + p.state(4) * sin(strength);
result[3]  = p.state(4) * cos(strength) - p.state(3) * sin(strength);
( p.state ).SetComponent( 0, result[0] );
( p.state ).SetComponent( 1, result[1] );
( p.state ).SetComponent( 3, result[2] );
( p.state ).SetComponent( 4, result[3] );
}
