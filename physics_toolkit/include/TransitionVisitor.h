/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      TransitionVisitor.h
******  Version:   2.2
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


/*
 * File: TransitionVisitor.h
 * 
 * Header for class TransitionVisitor
 * 
 * Calculates transition energy or gamma-t.
 * 
 * Leo Michelotti
 * May 15, 2002
 * 
 */

#ifndef TRANSITIONVISITOR_H
#define TRANSITIONVISITOR_H


#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Particle.h>



class TransitionVisitor : public BmlVisitor
{

 public:

  TransitionVisitor();
  TransitionVisitor( const Particle& );
  TransitionVisitor( const TransitionVisitor& );
  ~TransitionVisitor();

  void visit( beamline&   );
  void visit( bmlnElmnt&  );
  void visit( rbend&      );
  void visit( sbend&      );
  void visit( CF_rbend&   );
  void visit( CF_sbend&   );

  void setParticle( Particle const& );

  double getTransitionGamma()    const;
  double getMomentumCompaction() const;

  // Error codes
  int         getErrorCode()       const;
  const char* getErrorMessage()    const;
  const char* getErrorMessage(int) const;

  static const int NUMERRS;
  static const int OKAY;
  static const int NEGLEVEL;
  static const int ZEROLENGTH;
  static const int NEGCOMP;
  static const int DOUBLED;
  static const int NOPARTICLE;
  static const int NOLATTFUNC;
  static const int VERDISP;
  static const int NOELEMENTS;
  static const int NULLBMLPTR;

 private:


  void  visit_bend( bmlnElmnt& );
  void  set_prev( bmlnElmnt const& );

  double     s_;
  double     alpha_;
  double     gamma_t_;
  Particle*  particlePtr_;

  int        level_;
  double     D_;
  double     Dprime_;

  MatrixD    coeff_;
  MatrixD    b_;
  MatrixD    f_;

  bool       calcDone_;

  int        errorCode_;

  static const char*  errorMessage_[];

};

#endif // TRANSITIONVISITOR_H
