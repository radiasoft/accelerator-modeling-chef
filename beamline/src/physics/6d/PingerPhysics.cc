#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      PingerPhysics.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include "pinger.h"

/* ********** Pinger propagators **********
   Note that these are also the propagators for HPinger and VPinger
   */

void Pinger::localPropagate( Particle& p ) {
  if ( isArmed() ) {
    p.state[3] += cos(_kick_direction)*strength;
    p.state[4] += sin(_kick_direction)*strength;
    disarm();
  }
}

void Pinger::localPropagate( JetParticle& p ) {
  if ( isArmed() ) {
    Jet  dummy;
    dummy = p.State(3) + cos(_kick_direction)*strength;
    ( p.state ).SetComponent( 3, dummy );
    dummy = p.State(4) + sin(_kick_direction)*strength;
    ( p.state ).SetComponent( 4, dummy );
    disarm();
  }
}

