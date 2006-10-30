/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      beamline_elements.h
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

#ifndef BEAMLINE_ELEMENTS_H
#define BEAMLINE_ELEMENTS_H

#include <cmath>
#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/slist.h>
#include <basic_toolkit/Barnacle.h>
#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/mxyzptlk.h>
#include <beamline/Particle.h>

// ****************
// BEAMLINE CLASSES
// ****************

#include <beamline/bmlnElmnt.h>
#include <beamline/beamline.h>
#include <beamline/kick.h>
#include <beamline/rfcavity.h>
#include <beamline/srot.h>
#include <beamline/monitor.h>
#include <beamline/marker.h>
#include <beamline/drift.h>
#include <beamline/rbend.h>
#include <beamline/sbend.h>
#include <beamline/CF_sbend.h>
#include <beamline/CF_rbend.h>
#include <beamline/sector.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <beamline/octupole.h>
#include <beamline/decapole.h>
#include <beamline/thinpoles.h>
#include <beamline/BBLens.h>
#include <beamline/septum.h>
#include <beamline/lambertson.h>
#include <beamline/BeamlineIterator.h>
#include <beamline/circuit.h>
#include <beamline/ICircuit.h>
#include <beamline/FCircuit.h>
#include <beamline/mover.h>
#include <beamline/BipolarCircuit.h>
#include <beamline/Aperture.h>
#include <beamline/BmlVisitor.h>
#include <beamline/FDPolarity.h>
#include <beamline/MappPropFunc.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/Slot.h>
#include <beamline/combinedFunction.h>
#include <beamline/pinger.h>

#endif // BEAMLINE_ELEMENTS_H
