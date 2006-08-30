/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      MatrixCalcVisitor.h
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


#ifndef MATRIXCALCVISITOR_H
#define MATRIXCALCVISITOR_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/Slot.h>
#include <beamline/CF_rbend.h>

class MatrixCalcVisitor : public BmlVisitor
{
 public:
  MatrixCalcVisitor( const Particle& );
  MatrixCalcVisitor( const MatrixCalcVisitor& );
  ~MatrixCalcVisitor();

  void visitBeamline( beamline* );
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

  void getState( Vector& );
  void getState( Vector* );
  void getState( double* );
  Vector State();

  static int UNSTABLE;
  static int INTEGER_TUNE;
  static int PHASE_ERROR;
  static int DONE;

  const double* Beta_h();
  const double* Beta_v();
  const double* Alpha_h();
  const double* Alpha_v();
  const double* Psi_h();
  const double* Psi_v();
  const double* s();
  int           NumberOfElements();

 private: 
  char      _firstTime;
  char      _calcDone;
  int       _doCalc();
  Particle* _myParticle;
  beamline* _myBeamlinePtr;
  int       _numberOfElements;
  int       _counter;
  MatrixD** _linearModel_h;
  MatrixD** _linearModel_v;
  double*   _beta_h;
  double*   _beta_v;
  double*   _alpha_h;
  double*   _alpha_v;
  double*   _psi_h;
  double*   _psi_v;
  double*   _arcLength;
  MatrixD   _map_h;
  MatrixD   _map_v;
};

#include <physics_toolkit/MatrixCalcVisitor.icc>

#endif // MATRIXCALCVISITOR_H
