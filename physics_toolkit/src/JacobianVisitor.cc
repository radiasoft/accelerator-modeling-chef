/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      JacobianVisitor.cc
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
****** REVSION HISTORY
****** 
****** Mar 2007     ostiguy@fnal.gov
******
****** - local particles allocated on the stack
****** - visitor interface takes advantage of (reference) dynamic type
****** - new-style type safe Barnacles
******
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/beamline.h>
#include <mxyzptlk/Mapping.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <physics_toolkit/JacobianVisitor.h>

const MatrixD identityMatrix = Matrix::Imatrix(6);

char const* LOCAL_JACOBIAN_STRING      = "Local_Jacobian";
char const* CUMULATIVE_JACOBIAN_STRING = "Cumulative_Jacobian";

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JacobianData::JacobianData() : jac(6,6,0.0) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixD JacobianData::RMatrix() {
  MatrixD retMat(6,6,0.0);

  retMat[0][0] = jac[0][0];
  retMat[0][1] = jac[0][3];
  retMat[0][2] = jac[0][1];
  retMat[0][3] = jac[0][4];
  retMat[0][4] = jac[0][2];
  retMat[0][5] = jac[0][5];

  retMat[1][0] = jac[3][0];
  retMat[1][1] = jac[3][3];
  retMat[1][2] = jac[3][1];
  retMat[1][3] = jac[3][4];
  retMat[1][4] = jac[3][2];
  retMat[1][5] = jac[3][5];

  retMat[2][0] = jac[1][0];
  retMat[2][1] = jac[1][3];
  retMat[2][2] = jac[1][1];
  retMat[2][3] = jac[1][4];
  retMat[2][4] = jac[1][2];
  retMat[2][5] = jac[1][5];

  retMat[3][0] = jac[4][0];
  retMat[3][1] = jac[4][3];
  retMat[3][2] = jac[4][1];
  retMat[3][3] = jac[4][4];
  retMat[3][4] = jac[4][2];
  retMat[3][5] = jac[4][5];

  retMat[4][0] = jac[2][0];
  retMat[4][1] = jac[2][3];
  retMat[4][2] = jac[2][1];
  retMat[4][3] = jac[2][4];
  retMat[4][4] = jac[2][2];
  retMat[4][5] = jac[2][5];

  retMat[5][0] = jac[5][0];
  retMat[5][1] = jac[5][3];
  retMat[5][2] = jac[5][1];
  retMat[5][3] = jac[5][4];
  retMat[5][4] = jac[5][2];
  retMat[5][5] = jac[5][5];

  return retMat;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JacobianVisitor::JacobianVisitor(beamline* mach, JetParticle const& p)
: JetParticleVisitor(p) 
{

  theLine_      = mach;
  Mapping temp = particle_.State();
  state_       = particle_.State();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JacobianVisitor::~JacobianVisitor() {
				// The destructor does not delete the
				// beamline passed in from the constructor.
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JacobianVisitor::visit(beamline& x) {

  BmlVisitor::visit(x);
  if(jacobianType_ == JACOBIAN_CUMULATIVE) {

    x.dataHook.append( Barnacle(whichJacobian_, JacobianData(cumulativeMatrix_) ));

  } else {

    BarnacleList::iterator it = x.dataHook.find( whichJacobian_ );
    JacobianData jacInfo = boost::any_cast<JacobianData>(it->info);
    x.dataHook.erase(it); 

    jacInfo.jac = jacInfo.jac * cumulativeMatrix_.inverse();

    x.dataHook.append(  Barnacle(whichJacobian_, jacInfo) );

  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JacobianVisitor::visit(bmlnElmnt& be) {

  if(jacobianType_ == JACOBIAN_CUMULATIVE) {

    be.propagate(particle_);

    Mapping map = particle_.State();
    cumulativeMatrix_ = map.jacobian();
    JacobianData matBarn(cumulativeMatrix_);
    be.dataHook.append( Barnacle(whichJacobian_,  JacobianData(cumulativeMatrix_) ) );

  } else {


    BarnacleList::iterator it = be.dataHook.find( whichJacobian_ );
    JacobianData jacInfo = boost::any_cast<JacobianData>(it->info);
    be.dataHook.erase( it);
    
    MatrixD tmpMat = jacInfo.jac;
    jacInfo.jac = tmpMat * cumulativeMatrix_.inverse();
    be.dataHook.append( Barnacle(whichJacobian_, jacInfo) );
    cumulativeMatrix_ = tmpMat;
  }
    
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JacobianVisitor::clearBarnacles() {

  for (beamline::iterator it = theLine_->begin();  it != theLine_->end(); ++it) {
    (*it)->dataHook.eraseAll("Jacobian");
  }
  cumulativeMatrix_ = identityMatrix;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JacobianVisitor::createJacobian(const JACOBIAN_TYPE& jacType) {
  jacobianType_ = jacType;
  JACOBIAN_TYPE tmpType = jacobianType_;
				// If the local Jacobian is desired then
				// we must go through the beamline twice.
				// The first time the cumulative Jacobian
				// is calculated then the second time it
				// is changed to the local Jacobian.
  if(jacobianType_ == JACOBIAN_LOCAL) {
    jacobianType_  = JACOBIAN_CUMULATIVE;
    whichJacobian_ = LOCAL_JACOBIAN_STRING;
  } else
    whichJacobian_ = CUMULATIVE_JACOBIAN_STRING;

  cumulativeMatrix_ = identityMatrix;
  theLine_->accept(*this);
				// Now go around and calculate the local
				// Jacobian.
  if(tmpType == JACOBIAN_LOCAL)
    jacobianType_ = JACOBIAN_LOCAL;
				// Reset the particle.
  Mapping tmpMap(state_);
  particle_.State() = tmpMap;
  cumulativeMatrix_    = identityMatrix;
  theLine_->accept(*this);
				// We have to do something special for the
				// highest level beamline otherwise one gets
				// the identity matrix.

  if(tmpType == JACOBIAN_LOCAL){


    BarnacleList::iterator it = theLine_->dataHook.find( whichJacobian_ );
    JacobianData jacInfo = boost::any_cast<JacobianData>(it->info);
    theLine_->dataHook.erase( it);
    jacInfo.jac = cumulativeMatrix_;
    theLine_->dataHook.append( Barnacle( whichJacobian_,jacInfo ) );
  }
}

