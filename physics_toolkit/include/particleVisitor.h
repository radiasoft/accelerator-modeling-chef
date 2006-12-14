/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.1
******                                    
******  File:      particleVisitor.h
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

#include <beamline/BmlVisitor.h>
#include <mxyzptlk/Jet.h>
#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/Mapping.h>

class Particle;
class JetParticle;


class particleVisitor : public BmlVisitor {
 protected:
  Particle* particle;
  Vector    state;
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
  void setState(Vector const&);
  const Particle& getParticle();
};

class JetParticleVisitor : public BmlVisitor {
 protected:
  JetParticle* particle;
  Mapping      state;
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
  void setState(Mapping const&);
  const JetParticle& getParticle();
};


#endif // PARTICLEVISITOR_H
