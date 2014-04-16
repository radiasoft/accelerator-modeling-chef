/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      srotPhysics.cc
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



#include <beamline/srot.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

void srot::localPropagate(Particle& p) {

 double result[4];
 
 Vector& state = p.State();

 result[0] = state[0] * cos(strength_) + state[1] * sin(strength_);
 result[1] = state[1] * cos(strength_) - state[0] * sin(strength_);

 state[0]  = result[0];
 state[1]  = result[1];

 result[2] = state[3] * cos(strength_) + state[4] * sin(strength_);
 result[3] = state[4] * cos(strength_) - state[3] * sin(strength_);

 state[3]  = result[2];
 state[4]  = result[3];

}

void srot::localPropagate(JetParticle& p) {

Jet result[4];

 Mapping& state = p.State();

 result[0] = state[0] * cos(strength_) + state[1] * sin(strength_);
 result[1] = state[1] * cos(strength_) - state[0] * sin(strength_);

 state[0]  = result[0];
 state[1]  = result[1];

 result[2] = state[3] * cos(strength_) + state[4] * sin(strength_);
 result[3] = state[4] * cos(strength_) - state[3] * sin(strength_);

 state[3]  = result[2];
 state[4]  = result[3];

}
