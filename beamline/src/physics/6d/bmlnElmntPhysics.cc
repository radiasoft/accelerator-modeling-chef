/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      bmlnElmntPhysics.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                          
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/bmlnElmnt.h>
#include <iostream>

using FNAL::pcerr;
using FNAL::pcout;


#ifdef MADPHYSICS

void bmlnElmnt::localPropagate( Particle& p ) {

 static char firstCall = 1;
 if( firstCall ) {
  (*pcout) << "*** WARNING ***  bmlnElmnt::localPropagate( Particle& ): This\n"
            << "*** WARNING ***  routine was written using a linear\n"
            << "*** WARNING ***  approximation." << std::endl;
  firstCall = 0;
 }

 p.state[0] += length * p.state[3];
 p.state[1] += length * p.state[4];
 p.state[2] += length/p.Beta()/p.Gamma()/p.Gamma() * p.state[5];

}

void bmlnElmnt::localPropagate( JetParticle& p ) {

 static char firstCall = 1;
 if( firstCall ) {
  (*pcout) << "*** WARNING ***  bmlnElmnt::localPropagate( JetParticle& ): This\n"
            << "*** WARNING ***  routine was written using a linear\n"
            << "*** WARNING ***  approximation." << std::endl;
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
  Particle* p;
  ParticleBunch::Iterator get( b );
  while((  p = (Particle*) get.next()  )) this->localPropagate( *p );
}
 
#endif

/* ------------------------------------------------------ */

#ifndef MADPHYSICS

void bmlnElmnt::localPropagate( Particle& p ) {

 double /* x, y, */ D, p3divpbar;
 double xpr, ypr;

 Vector& state = p.getState();

 p3divpbar = sqrt( ( 1.0 + state[5] ) * ( 1.0 + state[5] )
                 - state[3]*state[3] 
                 - state[4]*state[4] );

 xpr = state[3] / p3divpbar;
 ypr = state[4] / p3divpbar; 

 state[0] += length * xpr;
 state[1] += length * ypr;

 D = length*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 

 state[2] += ( D / p.Beta() ) - _ctRef;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::localPropagate( JetParticle& p ) {

 Jet /* x, y, */ D, p3divpbar;
 Jet dummy;
 Jet xpr, ypr;

 Mapping& state = p.getState();

 p3divpbar = sqrt( ( 1.0 + state(5) ) * ( 1.0 + state(5) )
                 - state(3)*state(3) 
                 - state(4)*state(4) );

 xpr = state(3) / p3divpbar;
 ypr = state(4) / p3divpbar;

 dummy = state(0) + length * xpr;
 state.SetComponent( 0, dummy );

 dummy = state(1) + length * ypr;
 state.SetComponent( 1, dummy );

 D = length*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 

 dummy = state(2) + ( D / p.Beta() ) - _ctRef;
 state.SetComponent( 2, dummy );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::localPropagate( ParticleBunch& b ) {
 Particle* p;
 ParticleBunch::Iterator get( b );
 while((  p = (Particle*) get.next()  )) { this->localPropagate( *p ); }
}

#endif

