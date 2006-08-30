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

void thinSeptum::localPropagate( Particle& p ) {
  static int i;
  static double kick;
  static double TrState[6], dummy[6];
  
  for( i = 0; i < 6; i++  ) TrState[i] = p.state[i];
  
  if (TrState[0] > xWire ) kick = strengthPos/(1.0 + TrState[5]);
  else kick = strengthNeg/(1.0 + TrState[5]);
  
  TrState[3] += kick;

  p.state[3] = TrState[3];
  p.state[4] = TrState[4];
}



void thinSeptum::localPropagate( JetParticle& p ) { 
// This is taken from the thinSeptaPhysics.cc file. AJB 2/20/98

  static int i;
  Jet TrState[6], dummy[6];
  Jet JetxWire;
  Jet kick;

  for( i = 0; i < 6; i++  ) TrState[i] = p.state(i);

  JetxWire = xWire;
  if (TrState[0].standardPart() > JetxWire.standardPart() ) kick = strengthPos/(1.0 + TrState[5]);
  else kick = strengthNeg/(1.0 + TrState[5]);
  
 TrState[3] += kick;
  
  ( p.state ).SetComponent( 3, TrState[3] );
  ( p.state ).SetComponent( 4, TrState[4] );
}

