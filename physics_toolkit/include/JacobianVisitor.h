/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      JacobianVisitor.h
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab     
******                All Rights Reserved                             
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
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - visitor takes advantage of dynamic typing
******                                                                
**************************************************************************
*************************************************************************/


#ifndef JACOBIANVISITOR_H
#define JACOBIANVISITOR_H

#include <physics_toolkit/JetParticleVisitor.h>

class beamline;

/// The JacobianVisitor class is a Visitor Pattern class which calculates
/// the Jacobian for each beamline and beamline element recursively. A
/// barnacle containing the jacobian matrix is placed on each element
/// and beamline. The user has the option to either calculate a Jacobian
/// of the specific element or a cumulative Jacobian from the start of the
/// beamline. In the visitBmlnElmnt method the progate method is called.

class JacobianVisitor : public JetParticleVisitor {

 public:
  enum JACOBIAN_TYPE {JACOBIAN_LOCAL, JACOBIAN_CUMULATIVE};

  JacobianVisitor( beamline*, JetParticle const& );

  virtual ~JacobianVisitor();

  void visit(beamline& x);
  void visit(bmlnElmnt& x);
  void clearBarnacles();
  void createJacobian(JACOBIAN_TYPE const&);

 private:

  beamline*           theLine_;
  char const*         whichJacobian_;
  MatrixD             cumulativeMatrix_;
  JACOBIAN_TYPE       jacobianType_;

  JacobianVisitor(JacobianVisitor const&);
};

/// The label for this Barnacle is either "Local_Jacobian" or 
/// "Cumulative_Jacobian" depending on the type selected when the
/// createJacobian method was called.


struct JacobianData {

  JacobianData();
  JacobianData(MatrixD      const& mat)    {jac = mat;}
  JacobianData(JacobianData const& x)      {jac = x.jac;}

  MatrixD jac;
  MatrixD RMatrix();
};


#endif    // JACOBIANVISITOR_H
