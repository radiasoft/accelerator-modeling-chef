/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      particleVisitor.h
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

template<typename T1, typename T2> 
class TJet;
typedef TJet<double,FNAL::Complex> Jet;
// class Jet;

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
