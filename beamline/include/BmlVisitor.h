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
class beamline;

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

// ---------------------------

class BmlVisitor
{
public:
  virtual void visitBeamline( beamline* );
  virtual void visitBmlnElmnt( bmlnElmnt* ) {}

  virtual void visitHkick( hkick* x )
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitOctupole( octupole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThinrfcavity( thinrfcavity* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitSrot( srot* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitVkick( vkick* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitMonitor( monitor* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitMarker( marker* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitDrift( drift* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitRbend( rbend* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitSbend( sbend* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitSector( sector* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitQuadrupole( quadrupole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitJetQuadrupole( JetQuadrupole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThinQuad( thinQuad* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThinSextupole( thinSextupole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitJetthinSext( JetthinSext* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitJetthinQuad( JetthinQuad* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThinOctupole( thinOctupole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThinDecapole( thinDecapole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThin12pole( thin12pole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThin14pole( thin14pole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThin16pole( thin16pole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThin18pole( thin18pole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThinMultipole( thinMultipole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitSextupole( sextupole* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitBBLens( BBLens* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThinSeptum( thinSeptum* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitThinLamb( thinLamb* x ) 
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }
  virtual void visitCombinedFunction( combinedFunction* x )
                         { visitBmlnElmnt( (bmlnElmnt*) x ); }

protected:
  BmlVisitor() {}
  virtual ~BmlVisitor() {}

};

#endif // BMLVISITOR_H
