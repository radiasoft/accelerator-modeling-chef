/*
**
** File: particleVisitor.h
** 
** Header file for the visitor class particleVisitor.
** 
** --- James Holt
** --- August 25, 1997
**
*/

#ifndef PARTICLEVISITOR_H
#define PARTICLEVISITOR_H

#include "BmlVisitor.h"

class Particle;
class JetParticle;
class Jet;

// ---------------------------
class particleVisitor : public BmlVisitor {
 protected:
  Particle* particle;
  double* state;
  int _x;
  int _xp;
  int _y;
  int _yp;
  int _cdt;
  int _dpop;

 public:

  particleVisitor();
  particleVisitor(const Particle&);
  particleVisitor(const particleVisitor&);
  virtual ~particleVisitor();
  void setParticle(const Particle&);
  void setState(double*);
  const Particle& getParticle();
};

class JetParticleVisitor : public BmlVisitor {
 protected:
  JetParticle* particle;
  Jet* state;
  int dim;
  int _x;
  int _xp;
  int _y;
  int _yp;
  int _cdt;
  int _dpop;

 public:

  JetParticleVisitor();
  JetParticleVisitor(const JetParticle&);
  JetParticleVisitor(const JetParticleVisitor&);
  virtual ~JetParticleVisitor();
  void setParticle(const JetParticle&);
  void setState(Jet*);
  const JetParticle& getParticle();
};


#endif // PARTICLEVISITOR_H
