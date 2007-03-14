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

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/pinger.h>

/* ********** Pinger propagators **********
   Note that these are also the propagators for HPinger and VPinger
   */

void Pinger::localPropagate( Particle& p ) {

  Vector& state = p.getState();

  if ( isArmed() ) {
    if( 0 == counter_ ) {
       state[3] += cos(kick_direction_)*strength_;
       state[4] += sin(kick_direction_)*strength_;
    }
    --counter_;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::localPropagate( JetParticle& p ) {


  Mapping& state = p.getState();

  if ( isArmed() ) {
    if( 0 == counter_ ) {
      Jet  dummy = p.State(3) + cos(kick_direction_)*strength_;
      state.SetComponent( 3, dummy );
      dummy = p.State(4) + sin(kick_direction_)*strength_;
      state.SetComponent( 4, dummy );
    }
    --counter_;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::localPropagate( ParticleBunch& b ) {


  if ( isArmed() ) {
    if( 0 == counter_ ) {

      for (  ParticleBunch::iterator it = b.begin(); it != b.end(); ++it )  
      {
        Vector& state = (*it)->getState();
        state[3] += cos(kick_direction_)*strength_;
        state[4] += sin(kick_direction_)*strength_;
      }
    }
    --counter_;
  }
}
 
