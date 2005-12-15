/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      RefRegVisitor.h
******  Version:   1.1
******                                                                
******  Copyright (c) 2003  Universities Research Association, Inc.   
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
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

#ifndef BMLVISITOR_H
#include "BmlVisitor.h"
#endif

#ifndef PARTICLE_H
#include "Particle.h"
#endif

#include <fstream>

class RefRegVisitor : public BmlVisitor
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

  int  getErrorCode() const;

  // Error codes
  static const int OKAY;

 private:
  Particle* _prtnPtr;
  int       _errorCode;
  double    _revolutionFrequency;
};

#endif // REFREGVISITOR_H
