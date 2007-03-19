/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******
******  File:      JetParticleVisitor.cc
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******  
******  REVISION HISTORY
******
******  Mar 2007        ostiguy@fnal.gov
****** - Contents separated from ParticleVisitor.cc 
****** - use stack variables fro local Particles/JetParticles
****** - visitor interface takes advantage of (reference) dynamic type
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <physics_toolkit/JetParticleVisitor.h>
#include <beamline/Particle.h>


JetParticleVisitor::JetParticleVisitor(JetParticle const& x) 
 : BmlVisitor(), particle_(x) {

  x_     = Particle::xIndex();
  xp_    = Particle::npxIndex();
  y_     = Particle::yIndex();
  yp_    = Particle::npyIndex();
  cdt_   = Particle::cdtIndex();
  dpop_  = Particle::ndpIndex();

  Mapping temp = particle_.getState();
  state_ = particle_.getState();
}

JetParticleVisitor::JetParticleVisitor(JetParticleVisitor const& x)
  : BmlVisitor(), particle_(x.particle_) {

  state_ = particle_.getState(); 

  x_     = Particle::xIndex();
  xp_    = Particle::npxIndex();
  y_     = Particle::yIndex();
  yp_    = Particle::npyIndex();
  cdt_   = Particle::cdtIndex();
  dpop_  = Particle::ndpIndex();

}


JetParticleVisitor::~JetParticleVisitor() {
}

void JetParticleVisitor::setParticle(JetParticle const& x) {

  particle_ = x;
  state_ = particle_.getState();

}
void JetParticleVisitor::setState(Mapping const& x) {
  state_ = particle_.getState();	
 
}

const JetParticle& JetParticleVisitor::getParticle() {
  particle_.setState(state_);
  return particle_;
}
