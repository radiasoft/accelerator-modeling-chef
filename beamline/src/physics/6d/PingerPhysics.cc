/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
******                                    
******  File:      PingerPhysics.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <Particle.h>
#include <ParticleBunch.h>
#include <pinger.h>

/* ********** Pinger propagators **********
   Note that these are also the propagators for HPinger and VPinger
   */

void Pinger::localPropagate( Particle& p ) {
  if ( isArmed() ) {
    if( 0 == _counter ) {
      p.state[3] += cos(_kick_direction)*strength;
      p.state[4] += sin(_kick_direction)*strength;
    }
    _counter--;
  }
}

void Pinger::localPropagate( JetParticle& p ) {
  if ( isArmed() ) {
    if( 0 == _counter ) {
      Jet  dummy;
      dummy = p.State(3) + cos(_kick_direction)*strength;
      ( p.state ).SetComponent( 3, dummy );
      dummy = p.State(4) + sin(_kick_direction)*strength;
      ( p.state ).SetComponent( 4, dummy );
    }
    _counter--;
  }
}

void Pinger::localPropagate( ParticleBunch& b ) {
  if ( isArmed() ) {
    if( 0 == _counter ) {
      Particle* p;
      ParticleBunch::Iterator get( b );
      while((  p = (Particle*) get.next()  )) {
        p->state[3] += cos(_kick_direction)*strength;
        p->state[4] += sin(_kick_direction)*strength;
      }
    }
    _counter--;
  }
}
 
