#include "beamline.h"

#ifdef MADPHYSICS

void bmlnElmnt::localPropagate( Particle& p ) {

 static char firstCall = 1;
 if( firstCall ) {
  cout << "*** WARNING ***  bmlnElmnt::localPropagate( Particle& ): This\n"
       << "*** WARNING ***  routine was written using a linear\n"
       << "*** WARNING ***  approximation." << endl;
  firstCall = 0;
 }

 p.state[0] += length * p.state[3];
 p.state[1] += length * p.state[4];
 p.state[2] += length/p.Beta()/p.Gamma()/p.Gamma() * p.state[5];

}

void bmlnElmnt::localPropagate( JetParticle& p ) {

 static char firstCall = 1;
 if( firstCall ) {
  cout << "*** WARNING ***  bmlnElmnt::localPropagate( JetParticle& ): This\n"
       << "*** WARNING ***  routine was written using a linear\n"
       << "*** WARNING ***  approximation." << endl;
  firstCall = 0;
 }

 Jet  dummy;
 dummy = p.state[0] + length * p.state[3];
 ( p.state ).SetComponent( 0, dummy );
 dummy = p.state[1] + length * p.state[4];
 ( p.state ).SetComponent( 1, dummy );
 dummy = p.state[2] + length/p.Beta()/p.Gamma()/p.Gamma() * p.state[5];
 ( p.state ).SetComponent( 2, dummy );

}

void bmlnElmnt::localPropagate( ParticleBunch& b ) {
 slist_iterator getNext( (slist&) b );
 Particle* p;
 while( p = (Particle) getNext() ) this->localPropagate( *p );
}
 
#endif

/* ------------------------------------------------------ */

#ifndef MADPHYSICS

void bmlnElmnt::localPropagate( Particle& p ) {
 static double /* x, y, */ D, p3divpbar;
 static double xpr, ypr;

 // x = p.state[0];
 // y = p.state[1];
 p3divpbar = sqrt( ( 1.0 + p.state[5] ) * ( 1.0 + p.state[5] )
                 - p.state[3]*p.state[3] 
                 - p.state[4]*p.state[4] );


 xpr = p.state[3] / p3divpbar;
 ypr = p.state[4] / p3divpbar; 

 p.state[0] += length * xpr;
 p.state[1] += length * ypr;

 D = length*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 

 // D = sqrt( length*length 
 //         + (p.state[0] - x ) * (p.state[0] - x )
 //         + (p.state[1] - y ) * (p.state[1] - y ) ); 

 p.state[2] += ( D / p.Beta() ) - ( length / p.beta );
}

void bmlnElmnt::localPropagate( JetParticle& p ) {
 /* static ??? */ Jet /* x, y, */ D, p3divpbar;
 /* static ??? */ Jet dummy;
 /* static ??? */ Jet xpr, ypr;

 // x = p.state[0];
 // y = p.state[1];
 p3divpbar = sqrt( pow( 1.0 + p.state(5), 2 )
                 - p.state(3)*p.state(3) 
                 - p.state(4)*p.state(4) );

 xpr = p.state(3) / p3divpbar;
 ypr = p.state(4) / p3divpbar;

 dummy = p.state(0) + length * xpr;
 ( p.state ).SetComponent( 0, dummy );
 dummy = p.state(1) + length * ypr;
 ( p.state ).SetComponent( 1, dummy );

 D = length*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 

 // D = sqrt( length*length 
 //         + pow(p.state(0) - x, 2)
 //         + pow(p.state(1) - y, 2) ); 

 dummy = p.state(2) + ( D / p.Beta() ) - ( length / p.beta );
 ( p.state ).SetComponent( 2, dummy );
}

void bmlnElmnt::localPropagate( ParticleBunch& b ) {
 slist_iterator getNext( (slist&) b );
 Particle* p;
 while((  p = (Particle*) getNext()  )) this->localPropagate( *p );
}

#endif

