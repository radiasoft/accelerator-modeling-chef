/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      kickPhysics.cc
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <Particle.h>
#include <kick.h>

// =================================================
// vkick propagators
// =================================================

void vkick::localPropagate( Particle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    p.state[4] += strength;
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    p.state[4] += strength;
  }
}


void vkick::localPropagate( JetParticle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    ( p.state ).SetComponent( 4, p.state(4) + strength );
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    ( p.state ).SetComponent( 4, p.state(4) + strength );
  }
}


// =================================================
// hkick propagators
// =================================================

void hkick::localPropagate( Particle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    p.state[3] += strength;
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    p.state[3] += strength;
  }
}


void hkick::localPropagate( JetParticle& p ) {
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    ( p.state ).SetComponent( 3, p.state(3) + strength );
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    ( p.state ).SetComponent( 3, p.state(3) + strength );
  }
}

// =================================================
// kick propagators
// =================================================

void kick::localPropagate( Particle& p ) 
{
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    p.state[3] += horizontalKick;
    p.state[4] += verticalKick;
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    p.state[3] += horizontalKick;
    p.state[4] += verticalKick;
  }
}

void kick::localPropagate( JetParticle& p ) 
{
  static double realLength;
  if( length > 0.0 ) {
    realLength = length;
    length /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    (p.state).SetComponent( 3, p.state(3) + horizontalKick );
    (p.state).SetComponent( 4, p.state(4) + verticalKick   );
    bmlnElmnt::localPropagate( p );

    length = realLength;
  }
  else {
    (p.state).SetComponent( 3, p.state(3) + horizontalKick );
    (p.state).SetComponent( 4, p.state(4) + verticalKick   );
  }
}
