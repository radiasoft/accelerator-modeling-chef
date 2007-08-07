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

void octupole::localPropagate( Particle& p ) 
{
  p_bml_->propagate( p );
  p.set_cdt( p.get_cdt() - ctRef_ );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void octupole::localPropagate( JetParticle& p ) 
{
  p_bml_->propagate( p );
  p.set_cdt( p.get_cdt() - ctRef_ );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinOctupole::localPropagate( Particle& p ) {

  Vector& state = p.State(); 

  if(strength_ != 0) {
    
    double k = strength_ / p.ReferenceBRho();
    double x = state[0];
    double y = state[1];
 
    double xx = x*x;  
    double yy = y*y;
 
    state[3] -= k * x * ( xx - 3.0*yy );
    state[4] -= k * y * ( yy - 3.0*xx );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinOctupole::localPropagate( JetParticle& p ) {

  Mapping& state = p.State(); 

  if(strength_ != 0) {
 
    Jet     x( state[0] );
    Jet     y( state[1] );
    Jet     xx( x*x );
    Jet     yy( y*y );
 
    double k = strength_ / p.ReferenceBRho();
    
    state[3] -= k * x * ( xx - 3.0*yy );
    state[4] -= k * y * ( yy - 3.0*xx );
  }
}
