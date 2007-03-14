/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      decapolePhysics.cc
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

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/decapole.h>

void thinDecapole::localPropagate( Particle& p ) {
 double x, y, k;
 double xx, xy, yy;
 
 Vector& state = p.getState();

  if(strength_ != 0) {
    k = strength_/p.ReferenceBRho();
    x = state[0];
    y = state[1];
 
    xx = x*x;  xy = x*y;  yy = y*y;
 
    state[3] -=       k * ( xx*xx - 6.0*xy*xy + yy*yy );
    state[4] += 4.0 * k * xy * ( xx - yy );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinDecapole::localPropagate( JetParticle& p ) {

  Mapping& state = p.getState();

  if(strength_ != 0) {
    Jet     k, x, y, s;;
    Jet     xx, xy, yy;
 
    k = strength_/p.ReferenceBRho();
    x = state(0);
    y = state(1);
 
    xx = x*x;  xy = x*y;  yy = y*y;
 
    s = state(3) - ( k * ( xx*xx - 6.0*xy*xy + yy*yy ) );
    state.SetComponent( 3, s );
    s = state(4) + ( 4.0 * k * xy * ( xx - yy ) );
    state.SetComponent( 4, s );
  }
}
