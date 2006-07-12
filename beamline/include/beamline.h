/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      beamline.h
******  Version:   2.1
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
**************************************************************************
*************************************************************************/


#ifndef BEAMLINE_H
#define BEAMLINE_H

#include <PhysicsConstants.h>
#include <math.h>
#include <MathConstants.h>
#include <mxyzptlk.h>
#include <slist.h>
#include <Barnacle.h>
#include <VectorD.h>
#include <Particle.h>

// ****************
// BEAMLINE CLASSES
// ****************

#include <bmlnElmnt.h>
#include <kick.h>
#include <rfcavity.h>
#include <srot.h>
#include <monitor.h>
#include <marker.h>
#include <drift.h>
#include <rbend.h>
#include <sbend.h>
#include <CF_sbend.h>
#include <CF_rbend.h>
#include <sector.h>
#include <quadrupole.h>
#include <sextupole.h>
#include <octupole.h>
#include <decapole.h>
#include <thinpoles.h>
#include <BBLens.h>
#include <septum.h>
#include <lambertson.h>
#include <BeamlineIterator.h>
#include <circuit.h>
#include <ICircuit.h>
#include <FCircuit.h>
#include <mover.h>
#include <BipolarCircuit.h>
#include <Aperture.h>
#include <BmlVisitor.h>
#include <FDPolarity.h>
#include <MappPropFunc.h>
#include <Particle.h>
#include <ParticleBunch.h>
#include <Slot.h>
#include <combinedFunction.h>
#include <pinger.h>

#endif // BEAMLINE_H
