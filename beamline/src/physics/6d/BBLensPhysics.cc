/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      BBLensPhysics.cc
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
#include <beamline/BBLens.h>
#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/PhysicsConstants.h>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::localPropagate( Particle& p ) {

 if( strength == 0.0 ) return;

 double x, y;
 Vector p_beta(3);
 Vector E(3), K(3);

 Vector& state = p.getState();

 x = state[0];
 y = state[1];
 
 E = NormalizedEField( x, y );
 p_beta = p.VectorBeta();
 K  = Beta()^E;
 K  = p_beta^K;
 K += E; 
 
 double pn =  p.Beta()*p.Gamma();
 K *= - num*PH_MKS_rp / ( p.Beta()*pn );

 if( p.Charge()*num < 0.0 ) K = -K;  // ??? Check this!

 state[3] += K(0);
 state[4] += K(1);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::localPropagate( JetParticle& p ) {

 if( strength == 0.0 ) return;

 Jet x, y, s;
 JetVector p_beta(3);
 JetVector E(3), K(3);

 Mapping& state = p.getState();

 x = state(0);
 y = state(1);
 
 E = NormalizedEField( x, y );
 p_beta = p.VectorBeta();
 K  = Beta()^E;
 K  = p_beta^K;
 K += E; 

 Jet pn =  p.Beta()*p.Gamma();
 K *= - num*PH_MKS_rp / ( p.Beta()*pn );


 if( p.Charge()*num < 0.0 ) K = -K;  // ??? Check this!

 s = state(3) + K(0);
 state.SetComponent( 3, s );
 s = state(4) + K(1);
 state.SetComponent( 4, s );

}
