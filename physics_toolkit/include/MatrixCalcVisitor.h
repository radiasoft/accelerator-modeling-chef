#ifndef MATRIXCALCVISITOR_H
#define MATRIXCALCVISITOR_H

#include "Slot.h"
#include "CF_rbend.h"

class MatrixCalcVisitor : public BmlVisitor
{
 public:
  MatrixCalcVisitor( const Particle& );
  MatrixCalcVisitor( const MatrixCalcVisitor& );
  ~MatrixCalcVisitor();

  void visitBeamline( beamline* );
  void visitBmlnElmnt( bmlnElmnt* );
  void visitDrift( drift* );
  void visitRbend( rbend* );
  void visitSbend( sbend* );
  void visitSector( sector* );
  void visitQuadrupole( quadrupole* );
  void visitJetQuadrupole( JetQuadrupole* );
  void visitThinQuad( thinQuad* );
  void visitSlot( Slot* );
  void visitCF_rbend( CF_rbend* );
  void visitMarker( marker* );

  void getState( Vector& );
  void getState( Vector* );
  void getState( double* );
  Vector State();

  static int UNSTABLE;
  static int INTEGER_TUNE;
  static int PHASE_ERROR;
  static int DONE;

  const double* Beta_h();
  const double* Beta_v();
  const double* Alpha_h();
  const double* Alpha_v();
  const double* Psi_h();
  const double* Psi_v();
  const double* s();
  int           NumberOfElements();

 private: 
  char      _firstTime;
  char      _calcDone;
  int       _doCalc();
  Particle* _myParticle;
  beamline* _myBeamlinePtr;
  int       _numberOfElements;
  int       _counter;
  MatrixD** _linearModel_h;
  MatrixD** _linearModel_v;
  double*   _beta_h;
  double*   _beta_v;
  double*   _alpha_h;
  double*   _alpha_v;
  double*   _psi_h;
  double*   _psi_v;
  double*   _arcLength;
  MatrixD   _map_h;
  MatrixD   _map_v;
};

#include "MatrixCalcVisitor.icc"

#endif // MATRIXCALCVISITOR_H
