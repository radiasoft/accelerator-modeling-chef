#if HAVE_CONFIG_H
#include <config.h>
#endif
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include "beamline.h"
#include "Particle.h"
#include "particleVisitor.h"


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
  _x = Particle::_x();
  _xp = Particle::_xp();
  _y = Particle::_y();
  _yp = Particle::_yp();
  _cdt = Particle::_cdt();
  _dpop = Particle::_dpop();
  state = new double[BMLN_dynDim];
  particle->getState(state);
}

particleVisitor::particleVisitor(const particleVisitor& x) {
  if(x.particle != 0) {
    particle = x.particle->Clone();
    state = new double[BMLN_dynDim];
    particle->getState(state);
  } else {
    particle = 0;
    state = 0;
  }
  _x = Particle::_x();
  _xp = Particle::_xp();
  _y = Particle::_y();
  _yp = Particle::_yp();
  _cdt = Particle::_cdt();
  _dpop = Particle::_dpop();
}

particleVisitor::~particleVisitor() {
  if(particle != 0)
    delete particle;
  if(state != 0)
    delete [] state;
}

void particleVisitor::setParticle(const Particle& x) {
  if(particle != 0) {
    delete particle;
  } else {
    state = new double[BMLN_dynDim];
  }
  particle = x.Clone();
  particle->getState(state);

}
void particleVisitor::setState(double* x) {
  if(particle != 0) {
    particle->setState(x);
    particle->getState(state);
  }
}
const Particle& particleVisitor::getParticle() {
  particle->setState(state);
  return *particle;
}

JetParticleVisitor::JetParticleVisitor() : BmlVisitor() {
  particle = 0;
  state = 0;
  dim = 0;
  _x = 0;
  _xp = 0;
  _y = 0;
  _yp = 0;
  _cdt = 0;
  _dpop = 0;
}

JetParticleVisitor::JetParticleVisitor(const JetParticle& x) : BmlVisitor() {
  particle = x.Clone();
  _x = Particle::_x();
  _xp = Particle::_xp();
  _y = Particle::_y();
  _yp = Particle::_yp();
  _cdt = Particle::_cdt();
  _dpop = Particle::_dpop();
  Mapping temp;
  particle->getState(temp);
  dim = temp.Dim();
  state = new Jet[dim];
  particle->getState(state);
}

JetParticleVisitor::JetParticleVisitor(const JetParticleVisitor& x) {
  if(x.particle != 0) {
    particle = x.particle->Clone();
    Mapping temp;
    particle->getState(temp);
    dim = temp.Dim();
    state = new Jet[dim];
    particle->getState(state);
  } else {
    particle = 0;
    state = 0;
  }
  _x = Particle::_x();
  _xp = Particle::_xp();
  _y = Particle::_y();
  _yp = Particle::_yp();
  _cdt = Particle::_cdt();
  _dpop = Particle::_dpop();
}

JetParticleVisitor::~JetParticleVisitor() {
  if(particle != 0)
    delete particle;
  if(state != 0)
    delete [] state;
}

void JetParticleVisitor::setParticle(const JetParticle& x) {
  if(particle != 0) {
    delete particle;
    delete state;
  }
  particle = x.Clone();
  Mapping temp;
  particle->getState(temp);
  dim = temp.Dim();
  state = new Jet[dim];
  particle->getState(state);

}
void JetParticleVisitor::setState(Jet* x) {
  if(particle != 0) {		// This method is not robust.
    Mapping temp(dim,x);
    particle->setState(temp);	// The user could have changed
    particle->getState(state);	// dimensions of the state vector.
  }
}
const JetParticle& JetParticleVisitor::getParticle() {
  Mapping temp(dim,state);
  particle->setState(temp);
  return *particle;
}
