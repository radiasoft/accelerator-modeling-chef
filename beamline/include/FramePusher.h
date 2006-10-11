/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   2.0
******                                    
******  File:      FramePusher.h
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


/*
 * File: FramePusher.h
 * 
 * Header for class FramePusher.
 * 
 * Moves a Frame around a beamline, 
 * in anticipation of drawing the footprint.
 * 
 * Leo Michelotti
 * May 11, 2001
 * 
 */

#ifndef FRAMEPUSHER_H
#define FRAMEPUSHER_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlVisitor.h>
#include <basic_toolkit/Frame.h>



class FramePusher : public ConstBmlVisitor
{
 public:
  FramePusher();
  FramePusher( const Frame& );
  FramePusher( const FramePusher& );
  ~FramePusher();

  void visitBmlnElmnt( const bmlnElmnt* );

  void visitSlot     ( const Slot*     );
  void visitSbend    ( const sbend*    );
  void visitCF_sbend ( const CF_sbend* );
  void visitRbend    ( const rbend*    );
  void visitCF_rbend ( const CF_rbend* );
  void visitSector   ( const sector*   );

  Frame getFrame();

 private: 
  Frame _frame;
  int _errorCode;

  static const int OKAY;
  static const int SECTORVISITED;
};

#include "FramePusher.icc"

#endif // FRAMEPUSHER_H
