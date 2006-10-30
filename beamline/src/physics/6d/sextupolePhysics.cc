/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      sextupolePhysics.cc
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

#include <beamline/beamline.h>
#include <beamline/sextupole.h>
#include <beamline/Particle.h>

void sextupole::localPropagate( Particle& p ) {
  p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - _ctRef );
}

void sextupole::localPropagate( JetParticle& p ) {
  p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - _ctRef );
}


void thinSextupole::localPropagate( Particle& p ) {
 double x, y, k;
 double TrState[6], dummy[6];
 int    i;

 if( strength != 0.0 ) 
 {
   for( i = 0; i < 6; i++  ) {
     TrState[i] = p.state[i];
   }

   k = strength / p.ReferenceBRho();
   x = TrState[0];
   y = TrState[1];
 
   TrState[3] -= k * ( x*x - y*y );
   TrState[4] += 2.0 * k * x*y;

   p.state[3] = TrState[3];
   p.state[4] = TrState[4];
 }
}


void thinSextupole::localPropagate( JetParticle& p ) {
 static Jet TrState[6];
 double k;
 static Jet x, y;
 static int i;
 
 if( strength != 0.0 ) 
 {
   for( i = 0; i < 6; i++  ) {
     TrState[i] = p.state(i);
   }

   k = strength / p.ReferenceBRho();
   x = TrState[0];
   y = TrState[1];
 
   TrState[3] -= k * ( x*x - y*y );
   TrState[4] += 2.0 * k * x*y ;

   ( p.state ).SetComponent( 3, TrState[3] );
   ( p.state ).SetComponent( 4, TrState[4] );
 }
}
