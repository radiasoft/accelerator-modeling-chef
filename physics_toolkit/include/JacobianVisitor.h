#ifndef JACOBIANVISITOR_H
#define JACOBIANVISITOR_H

#include "particleVisitor.h"

class beamline;
class MatrixD;

/// The JacobianVisitor class is a Visitor Pattern class which calculates
/// the Jacobian for each beamline and beamline element recursively. A
/// barnacle containing the jacobian matrix is placed on each element
/// and beamline. The user has the option to either calculate a Jacobian
/// of the specific element or a cumulative Jacobian from the start of the
/// beamline. In the visitBmlnElmnt method the progate method is called.

class JacobianVisitor : public JetParticleVisitor {
 public:
  enum JACOBIAN_TYPE {JACOBIAN_LOCAL, JACOBIAN_CUMULATIVE};
  JacobianVisitor();
  JacobianVisitor( beamline*, const JetParticle& );
  virtual ~JacobianVisitor();
  void visitBeamline(beamline* x);
  void visitBmlnElmnt(bmlnElmnt*);
  void clearBarnacles();
  void createJacobian(const JACOBIAN_TYPE&);
 private:
  beamline* theLine;
  char* whichJacobian;
  MatrixD cumulativeMatrix;
  JACOBIAN_TYPE jacobianType;
  JacobianVisitor(const JacobianVisitor&);
};

/// The label for this Barnacle is either "Local_Jacobian" or 
/// "Cumulative_Jacobian" depending on the type selected when the
/// createJacobian method was called.


class JacobianData : public BarnacleData {
public:
  MatrixD jac;
  JacobianData(const MatrixD& mat) {jac = mat;}
  JacobianData();
  JacobianData(const JacobianData& x) {jac = x.jac;}
  ~JacobianData() {}
  MatrixD RMatrix();
};


#endif    // JACOBIANVISITOR_H
