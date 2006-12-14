/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      particleVisitor.cc
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <physics_toolkit/particleVisitor.h>


particleVisitor::particleVisitor() : BmlVisitor() {
  particle = 0;
  _x = 0;
  _xp = 0;
  _y = 0;
  _yp = 0;
  _cdt = 0;
  _dpop = 0;
}

particleVisitor::particleVisitor(const Particle& x) : BmlVisitor() {
  particle = x.Clone();
  _x     = Particle::xIndex();
  _xp    = Particle::npxIndex();
  _y    = Particle:: yIndex();
  _yp   = Particle:: npyIndex();
  _cdt  = Particle:: cdtIndex();
  _dpop = Particle:: ndpIndex();
  state = particle->getState();
}

particleVisitor::particleVisitor(const particleVisitor& x) {
  if(x.particle != 0) {
    particle = x.particle->Clone();
    state = particle->getState();
  } else {
    particle = 0;
    state = 0;
  }
  _x     = Particle::xIndex();
  _xp    = Particle::npxIndex();
  _y    = Particle:: yIndex();
  _yp   = Particle:: npyIndex();
  _cdt  = Particle:: cdtIndex();
  _dpop = Particle:: ndpIndex();
}

particleVisitor::~particleVisitor() {

  if(particle) delete particle;
}

void particleVisitor::setParticle(const Particle& x) {

  if(particle) delete particle;
  particle = x.Clone();
  state = particle->getState();

}
void particleVisitor::setState(Vector const& x) {
  if(!particle) return;
  particle->setState(x);
  state = particle->getState();
 
}
const Particle& particleVisitor::getParticle() {
  particle->setState(state);
  return *particle;
}

JetParticleVisitor::JetParticleVisitor() : BmlVisitor() {
  particle = 0;
  state = 0;
  dim   = 0;
  _x    = 0;
  _xp   = 0;
  _y    = 0;
  _yp   = 0;
  _cdt  = 0;
  _dpop = 0;
}

JetParticleVisitor::JetParticleVisitor(const JetParticle& x) : BmlVisitor() {

  particle = x.Clone();
  _x     = Particle::xIndex();
  _xp    = Particle::npxIndex();
  _y     = Particle::yIndex();
  _yp    = Particle::npyIndex();
  _cdt   = Particle::cdtIndex();
  _dpop  = Particle::ndpIndex();

  Mapping temp= particle->getState();
  dim = temp.Dim();
  state = particle->getState();
}

JetParticleVisitor::JetParticleVisitor(JetParticleVisitor const& x) {

  _x     = Particle::xIndex();
  _xp    = Particle::npxIndex();
  _y     = Particle::yIndex();
  _yp    = Particle::npyIndex();
  _cdt   = Particle::cdtIndex();
  _dpop  = Particle::ndpIndex();

  if( !x.particle) { 
      particle = 0; 
      return;
  }
 
 particle = x.particle->Clone();
  state = particle->getState();


}

JetParticleVisitor::~JetParticleVisitor() {
  if(particle != 0)
    delete particle;
}

void JetParticleVisitor::setParticle(const JetParticle& x) {
  if(particle != 0) {
    delete particle;
  }
  particle = x.Clone();
  state = particle->getState();

}
void JetParticleVisitor::setState(Mapping const& x) {
  if( !particle ) return;
  state = particle->getState();	
 
}
const JetParticle& JetParticleVisitor::getParticle() {
  particle->setState(state);
  return *particle;
}
