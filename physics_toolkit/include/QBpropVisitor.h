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
******                                                                
**************************************************************************
*************************************************************************/


#ifndef QBPROPVISITOR_H
#define QBPROPVISITOR_H

#include <physics_toolkit/particleVisitor.h>
#include <beamline/Slot.h>
#include <beamline/CF_rbend.h>

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
  void visitThinQuad( thinQuad* );
  void visitSlot( Slot* );
  void visitCF_rbend( CF_rbend* );
  void visitMarker( marker* );

  Vector& getState();
  Vector  State();
};

#endif // QBPROPVISITOR_H
