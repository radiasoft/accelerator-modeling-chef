/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.1
******                                    
******  File:      JetParticleVisitor.h
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
****** - this header split from ParticleVisitor.h
****** - use STL rather than custom list container
****** - visit function(s) use element reference as parameter
******                                                                
**************************************************************************
*************************************************************************/

#ifndef JETPARTICLEVISITOR_H
#define JETPARTICLEVISITOR_H

#include <mxyzptlk/Jet.h>
#include <mxyzptlk/Mapping.h>
#include <beamline/BmlVisitor.h>
#include <beamline/JetParticle.h>


class JetParticleVisitor : public BmlVisitor {

 public:

  JetParticleVisitor(JetParticle const&);
  JetParticleVisitor(JetParticleVisitor const&);
 ~JetParticleVisitor();

  void  setParticle(JetParticle const&);
  void  setState(Mapping const&);

  JetParticle const& getParticle();

 protected:

  JetParticle  particle_;
  Mapping      state_;
  int          dim_;
  int          x_;
  int          xp_;
  int          y_;
  int          yp_;
  int          cdt_;
  int          dpop_;
};


#endif // JETPARTICLEVISITOR_H
