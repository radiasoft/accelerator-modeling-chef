#/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ******
******             CommandPropagateReferenceOrbit.h                      ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Authors:    Jean-Francois Ostiguy                            ******
******              Fermilab                                         ****** 
******              ostiguy@fnal.gov                                 ****** 
******                                                               ******  
******  Usage, modification, and redistribution are subject to terms ******
******  of the License supplied with this software.                  ****** 
******                                                               ******
******  Software and documentation created under                     ****** 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.    ****** 
******  The U.S. Government retains a world-wide non-exclusive,      ****** 
******  royalty-free license to publish or reproduce documentation   ****** 
******  and software for U.S. Government purposes. This software     ****** 
******  is protected under the U.S. and Foreign Copyright Laws.      ****** 
******  URA/FNAL reserves all rights.                                ****** 
******                                                               ******
***************************************************************************
***************************************************************************
**************************************************************************/
#ifndef COMMANDPROPAGATEREFERENCEORBIT_H
#define COMMANDPROPAGATEREFERENCEORBIT_H

#include <physics_toolkit/BeamlineContext.h>
#include <beamline/LatticeFunctions.h>

class QWidget;

class CommandPropagateReferenceOrbit {

 public:
    
  QWidget*  operator()( QWidget* parent, BmlContextPtr bml, CSLattFuncs const& initialConditions ); 

};

#endif // COMMANDPROPAGATEREFERENCEORBIT_H


