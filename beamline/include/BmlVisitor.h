/*
**
** File: BmlVisitor.h
** 
** Header file for the visitor class BmlVisitor.
** This class is to be used as the base class for
** all actual beamline visitor objects.
** 
** --- Leo Michelotti
** --- August 21, 1997
**
*/

#ifndef BMLVISITOR_H
#define BMLVISITOR_H

// Predefined classes ...
class bmlnElmnt;
class hkick;
class octupole;
class thinrfcavity;
class srot;
class vkick;
class monitor;
class marker;
class drift;
class rbend;
class sbend;
class sector;
class quadrupole;
class JetQuadrupole;
class thinQuad;
class thinSextupole;
class JetthinSext;
class JetthinQuad;
class thinOctupole;
class thinDecapole;
class thin12pole;
class thin14pole;
class thin16pole;
class thin18pole;
class thinMultipole;
class sextupole;
class BBLens;
class thinSeptum;
class thinLamb;
class beamline;

// ---------------------------

class BmlVisitor
{
public:
  // No such function as: virtual void visitBmlnElmnt( bmlnElmnt* ) {}

  virtual void visitBeamline( beamline* );

  virtual void visitHkick( hkick* ) {}
  virtual void visitOctupole( octupole* ) {}
  virtual void visitThinrfcavity( thinrfcavity* ) {}
  virtual void visitSrot( srot* ) {}
  virtual void visitVkick( vkick* ) {}
  virtual void visitMonitor( monitor* ) {}
  virtual void visitMarker( marker* ) {}
  virtual void visitDrift( drift* ) {}
  virtual void visitRbend( rbend* ) {}
  virtual void visitSbend( sbend* ) {}
  virtual void visitSector( sector* ) {}
  virtual void visitQuadrupole( quadrupole* ) {}
  virtual void visitJetQuadrupole( JetQuadrupole* ) {}
  virtual void visitThinQuad( thinQuad* ) {}
  virtual void visitThinSextupole( thinSextupole* ) {}
  virtual void visitJetthinSext( JetthinSext* ) {}
  virtual void visitJetthinQuad( JetthinQuad* ) {}
  virtual void visitThinOctupole( thinOctupole* ) {}
  virtual void visitThinDecapole( thinDecapole* ) {}
  virtual void visitThin12pole( thin12pole* ) {}
  virtual void visitThin14pole( thin14pole* ) {}
  virtual void visitThin16pole( thin16pole* ) {}
  virtual void visitThin18pole( thin18pole* ) {}
  virtual void visitThinMultipole( thinMultipole* ) {}
  virtual void visitSextupole( sextupole* ) {}
  virtual void visitBBLens( BBLens* ) {}
  virtual void visitThinSeptum( thinSeptum* ) {}
  virtual void visitThinLamb( thinLamb* ) {}

protected:
  BmlVisitor();
  virtual ~BmlVisitor();

};

#endif // BMLVISITOR_H
