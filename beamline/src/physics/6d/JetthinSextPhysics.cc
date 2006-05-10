/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******
******  File:      JetthinSextPhysics.cc
******  Version:   2.0                    
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <Particle.h>
#include <sextupole.h>
#include <FDPolarity.h>

void JetthinSext::localPropagate( Particle& p ) {
 double k;
 double    inState  [BMLN_dynDim];
 double    outState  [BMLN_dynDim];
 int i;

 FDPolarity* pol = (FDPolarity*)dataHook.find("Polarity");
 if(pol != 0)
   k = pol->polarity * strength / p.bRho;
 else
   k = strength / p.bRho;

  p.getState( inState );

  outState[0] = inState[0];
  outState[1] = inState[1];
  outState[2] = inState[2];
  outState[3] = inState[3];
  outState[4] = inState[4];
  outState[3] += k * ( inState[0]*inState[0] - inState[1]*inState[1] );
  outState[4] -= 2.0 * k * inState[0]*inState[1];
  outState[5] = inState[5];


  p.setState( outState );

}

void JetthinSext::localPropagate( JetParticle& p ) {
  Jet k;
  Jet    zero;
  Jet    inState  [BMLN_dynDim];
  Jet    outState  [BMLN_dynDim];
  int i;
  
  zero = 0.0;
  // ??? REMOVE zero.fixReferenceAtStart( p.state );

  k = K2L / p.ReferenceBRho();

  p.getState( inState );

  outState[0] = inState[0];
  outState[1] = inState[1];
  outState[2] = inState[2];
  outState[3] = inState[3] + k * ( inState[0]*inState[0] - 
				  inState[1]*inState[1] );
  outState[4] = inState[4] - 2.0 * k * inState[0]*inState[1];
  outState[5] = inState[5];

  for( i = 0; i < BMLN_dynDim; i++  ) {
    ( p.state ).SetComponent( i, outState[i] );
  }
}
