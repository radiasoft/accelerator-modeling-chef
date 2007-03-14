/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      QBpropVisitor.h
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
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
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - visitor takes advantage of dynamic typing
******                                                   
******                                                                
**************************************************************************
*************************************************************************/


#ifndef QBPROPVISITOR_H
#define QBPROPVISITOR_H

#include <physics_toolkit/ParticleVisitor.h>
#include <beamline/Slot.h>
#include <beamline/CF_rbend.h>

struct QBpropVisitor : public ParticleVisitor
{
  QBpropVisitor( Particle      const& );
  QBpropVisitor( QBpropVisitor const& );
 ~QBpropVisitor();

  void visit( bmlnElmnt&  );
  void visit( drift&      );
  void visit( rbend&      );
  void visit( sbend&      );
  void visit( sector&     );
  void visit( quadrupole& );
  void visit( thinQuad&   );
  void visit( Slot&       );
  void visit( CF_rbend&   );
  void visit( marker&     );

  Vector& getState();
  Vector  State();
};

#endif // QBPROPVISITOR_H
