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


#include "BBLens.h"
#include "VectorD.h"
#include "PhysicsConstants.h"

void BBLens::localPropagate( Particle& p ) {

 if( strength == 0.0 ) return;

 double x, y;
 Vector p_beta(3);
 Vector E(3), K(3);

 x = p.state[0];
 y = p.state[1];
 
 E = NormalizedEField( x, y );
 p_beta = p.VectorBeta();
 K  = Beta()^E;
 K  = p_beta^K;
 K += E; 
 K *= - num*PH_MKS_rp / ( p.Beta()*p.pn );

 if( p.q*num < 0.0 ) K = -K;  // ??? Check this!

 p.state[3] += K(0);
 p.state[4] += K(1);
}

void BBLens::localPropagate( JetParticle& p ) {

 if( strength == 0.0 ) return;

 Jet x, y, s;
 JetVector p_beta(3);
 JetVector E(3), K(3);

 x = p.state(0);
 y = p.state(1);
 
 E = NormalizedEField( x, y );
 p_beta = p.VectorBeta();
 K  = Beta()^E;
 K  = p_beta^K;
 K += E; 
 K *= - num*PH_MKS_rp / ( p.Beta()*p.pn );


 if( p.q*num < 0.0 ) K = -K;  // ??? Check this!

 s = p.state(3) + K(0);
 ( p.state ).SetComponent( 3, s );
 s = p.state(4) + K(1);
 ( p.state ).SetComponent( 4, s );

}
