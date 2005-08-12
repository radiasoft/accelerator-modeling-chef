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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "Particle.h"
#include "octupole.h"

void octupole::localPropagate( Particle& p ) {
  p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - _ctRef );
}

void octupole::localPropagate( JetParticle& p ) {
  p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - _ctRef );
}


void thinOctupole::localPropagate( Particle& p ) {
 static double x, y, k;
 static double xx, yy;
 
  if(strength != 0) {
    k = strength / p.ReferenceBRho();
    x = p.state[0];
    y = p.state[1];
 
    xx = x*x;  
    yy = y*y;
 
    p.state[3] -= k * x * ( xx - 3.0*yy );
    p.state[4] -= k * y * ( yy - 3.0*xx );
  }
}

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
