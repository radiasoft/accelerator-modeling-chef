/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      RefRegVisitor.h
******  Version:   1.2
******                                                                
******  Copyright (c) 2003  Universities Research Association, Inc.   
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
 * File: RefRegVisitor.h
 * 
 * Header for class RefRegVisitor, 
 * a "reference registration visitor."
 * 
 * Registers a user-defined reference proton with a bmlnElmnt.
 * 
 * Leo Michelotti
 * April 10, 2003
 * 
 */

#ifndef REFREGVISITOR_H
#define REFREGVISITOR_H


#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Particle.h>


#include <fstream>

class DLLEXPORT RefRegVisitor : public BmlVisitor
{
 public:
  RefRegVisitor( const Particle& );
  RefRegVisitor( const RefRegVisitor& );
  ~RefRegVisitor();

  void visitBeamline( beamline* );
  void visitBmlnElmnt( bmlnElmnt* );
  void visitCF_rbend( CF_rbend* );
  void visitCF_sbend( CF_sbend* );
  void visitSbend( sbend* );
  void visitRbend( rbend* );
  void visitThinrfcavity( thinrfcavity* );
  void visitRfcavity( rfcavity* );

  int  getErrorCode() const;

  // Error codes
  static const int OKAY;

  // So far, these two functions are used exclusively
  // by class rfcavity.
  double getCdt();
  void setCdt(double);

 private:
  Particle*    _prtnPtr;
  int          _errorCode;
  double       _revolutionFrequency;
  const double _initialMomentum;
};

#endif // REFREGVISITOR_H
