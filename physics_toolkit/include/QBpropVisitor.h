#ifndef QBPROPVISITOR_H
#define QBPROPVISITOR_H

#include "particleVisitor.h"
#include "Slot.h"
#include "CF_rbend.h"

struct QBpropVisitor : public particleVisitor
{
  QBpropVisitor( const Particle& );
  QBpropVisitor( const QBpropVisitor& );
  ~QBpropVisitor();

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
};

#endif // QBPROPVISITOR_H
