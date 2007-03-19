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
****** REVISION HISTORY
******
****** Mar 2007    ostiguy@fnal.gov
****** - use STL rather than custom list container
****** - visit function(s) use element reference as parameter
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

#include <basic_toolkit/VectorD.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Particle.h>


class ParticleVisitor : public BmlVisitor {

 public:

  ParticleVisitor(Particle const&);
  ParticleVisitor(ParticleVisitor const&);
 ~ParticleVisitor();

  void   setParticle(Particle const&);
  void      setState(Vector const&);

  Particle const& getParticle();

 protected:

  Particle  particle_;
  Vector    state_;
  int       x_;
  int       xp_;
  int       y_;
  int       yp_;
  int       cdt_;
  int       dpop_;


};

#endif // PARTICLEVISITOR_H
