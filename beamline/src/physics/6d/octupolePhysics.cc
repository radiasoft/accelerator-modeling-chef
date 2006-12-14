/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      octupolePhysics.cc
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
#include <beamline/beamline.h>
#include <beamline/octupole.h>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void octupole::localPropagate( Particle& p ) {
  p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - _ctRef );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void octupole::localPropagate( JetParticle& p ) {
  p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - _ctRef );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinOctupole::localPropagate( Particle& p ) {

 double x, y, k;
 double xx, yy;
 
  Vector& state = p.getState(); 

  if(strength != 0) {
    k = strength / p.ReferenceBRho();
    x = state[0];
    y = state[1];
 
    xx = x*x;  
    yy = y*y;
 
    state[3] -= k * x * ( xx - 3.0*yy );
    state[4] -= k * y * ( yy - 3.0*xx );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinOctupole::localPropagate( JetParticle& p ) {


  if(strength != 0) {
    double  k;
    Jet     x( p.State(0) );
    Jet     y( p.State(1) );
    Jet     xx( x.Env() );
    Jet     yy( y.Env() );
 
    k = strength / p.ReferenceBRho();
    
    xx = x*x;  
    yy = y*y;

    p.State(3) -= k * x * ( xx - 3.0*yy );
    p.State(4) -= k * y * ( yy - 3.0*xx );
  }
}
