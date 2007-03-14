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
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - visitor takes advantage of dynamic typing
******                                                   
******                                                                
**************************************************************************
*************************************************************************/

#ifndef MATRIXCALCVISITOR_H
#define MATRIXCALCVISITOR_H

#include <vector>
#include <basic_toolkit/globaldefs.h>
#include <beamline/Slot.h>
#include <beamline/CF_rbend.h>

class MatrixCalcVisitor : public BmlVisitor
{
 public:

  MatrixCalcVisitor( Particle const& );
 ~MatrixCalcVisitor();

  void visit( beamline&   );
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

  static int UNSTABLE;
  static int INTEGER_TUNE;
  static int PHASE_ERROR;
  static int DONE;

  std::vector<double> const& Beta_h();
  std::vector<double> const& Beta_v();
  std::vector<double> const& Alpha_h();
  std::vector<double> const& Alpha_v();
  std::vector<double> const& Psi_h();
  std::vector<double> const& Psi_v();
  std::vector<double> const& s();

  int   NumberOfElements() const;

 private: 

  MatrixCalcVisitor( MatrixCalcVisitor const& x ); // forbidden 

  int       doCalc();

  bool            firstTime_;
  bool            calcDone_;
  Particle        particle_;
  beamline*       myBeamlinePtr_;
  int             numberOfElements_;
  int             counter_;

  std::vector<MatrixD> linearModel_h_;
  std::vector<MatrixD> linearModel_v_;

  std::vector<double>   beta_h_;
  std::vector<double>   beta_v_;
  std::vector<double>   alpha_h_;
  std::vector<double>   alpha_v_;
  std::vector<double>   psi_h_;
  std::vector<double>   psi_v_;
  std::vector<double>   arcLength_;

  MatrixD   map_h_;
  MatrixD   map_v_;


};

#endif // MATRIXCALCVISITOR_H
