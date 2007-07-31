/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      sectorPhysics.cc
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
****** REVISION HISTORY
******
****** July 2007  ostiguy@fnal.gov
******                            
****** - refactored and streamlined code: take advantage of 
******   Vector and Mapping operators.  
******                                    
******
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/sector.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

// this is the actual ***definition** of sector::defaultPropagate

sector::JET_Prop sector::defaultPropagate;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int sector::JET_Prop::operator()( bmlnElmnt* b, Particle& p ) {

 
 sector* s = dynamic_cast<sector*>(b);
 
 if( s->mapType_ ) {
   p.getState() =  s->myMap_   * p.getState(); // composition
 } 
 else {
   p.getState() = s->mapMatrix_* p.getState();
 } 

 // Longitudinal phase space kludge
 // in case there is a cavity within a sector ?
 // p.state[2] += DeltaT( p.state[5] );
 
 return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int sector::JET_Prop::operator()( bmlnElmnt* b, JetParticle& p ) {

 sector* s = dynamic_cast<sector*>(b);

 if( s->mapType_ ) { 
   p.getState() = s->myMap_ * p.getState();  // composition
 }
 else {
   // ??? REMOVE zero.fixReferenceAtStart( p.state );
   p.getState() = s->mapMatrix_ * p.getState();
 }

 // Longitudinal phase space kludge
 // outState = inState[2] + JetDeltaT( p.state(5) );
 //( p.state ).SetComponent( 2, outState );

 return 0;
}
