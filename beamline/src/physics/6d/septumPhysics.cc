/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      septumPhysics.cc
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

#include <beamline/septum.h>

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

void thinSeptum::localPropagate( Particle& p ) {

  Vector& state = p.State();

  double kick = ( state[0] > xWire_ ) ? strengthPos_/(1.0 + state[5]) :  strengthNeg_/(1.0 + state[5]); 
  
  state[3] += kick;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinSeptum::localPropagate( JetParticle& p ) { 
// This is taken from the thinSeptaPhysics.cc file. AJB 2/20/98

  Mapping& state = p.State();

  Jet kick = ( state[0].standardPart() > xWire_ ) ? 
                  strengthPos_/(1.0 + state[5]) :  strengthNeg_/(1.0 + state[5] ); 

  state[3] += kick;

}

