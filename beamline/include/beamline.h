/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
******                                    
******  File:      beamline.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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


#ifndef BEAMLINE_H
#define BEAMLINE_H

//#include <complex.h>   // ??? Why is this here?
#ifndef __VISUAL_CPP__
#include <stdio.h>
#include <string.h>
#endif

#ifdef __SUNPRO_CC
  #ifndef __STL_NEED_EXPLICIT
    #define __STL_NEED_EXPLICIT
  #endif
#endif

// ??? Are all these really needed???
#include "PhysicsConstants.h"
#include <math.h>
// #include <string>   (More trouble than it's worth.)
#include "MathConstants.h"
#include "mxyzptlk.h"
#include "slist.h"
#include "Barnacle.h"
#include "VectorD.h"
#include "Particle.h"

#if defined (__VISUAL_CPP__) && !defined(__KCC)
#define strcasecmp stricmp
#endif

#ifdef __BORLAND_CPP__
#define strcasecmp strcmpi
#endif

// ****************
// BEAMLINE CLASSES
// ****************

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

#ifndef KICK_H
#include "kick.h"
#endif

#ifndef RFCAVITY_H
#include "rfcavity.h"
#endif

#ifndef SROT_H
#include "srot.h"
#endif

#ifndef MONITOR_H
#include "monitor.h"
#endif

#ifndef MARKER_H
#include "marker.h"
#endif

#ifndef DRIFT_H
#include "drift.h"
#endif


#ifndef RBEND_H
#include "rbend.h"
#endif

#ifndef SBEND_H
#include "sbend.h"
#endif

#ifndef CF_SBEND_H
#include "CF_sbend.h"
#endif

#ifndef CF_RBEND_H
#include "CF_rbend.h"
#endif

#ifndef SECTOR_H
#include "sector.h"
#endif

#ifndef QUADRUPOLE_H
#include "quadrupole.h"
#endif

#ifndef JETQUADRUPOLE_H
#include "JetQuadrupole.h"
#endif

#ifndef SEXTUPOLE_H
#include "sextupole.h"
#endif

#ifndef OCTUPOLE_H
#include "octupole.h"
#endif

#ifndef DECAPOLE_H
#include "decapole.h"
#endif

#ifndef THINPOLES_H
#include "thinpoles.h"
#endif


#ifndef BBLENS_H
#include "BBLens.h"
#endif

#ifndef SEPTUM_H
#include "septum.h"
#endif

#ifndef LAMBERTSON_H
#include "lambertson.h"
#endif

#ifndef BEAMLINE_ITERATOR
#include "BeamlineIterator.h"
#endif


#ifndef CIRCUIT_H
#include "circuit.h"
#endif

#ifndef ICIRCUIT_H
#include "ICircuit.h"
#endif

#ifndef FCIRCUIT_H
#include "FCircuit.h"
#endif

#ifndef MOVER_H
#include "mover.h"
#endif

#ifndef BIPOLARCIRCUIT_H
#include "BipolarCircuit.h"
#endif

#ifndef APERTURE_H
#include "Aperture.h"
#endif

#ifndef BMLVISITOR_H
#include "BmlVisitor.h"
#endif

// ??? REMOVE #ifndef BUNCHPREDICATE_H
// ??? REMOVE #include "BunchPredicate.h"
// ??? REMOVE #endif

#ifndef FDPOLARITY_H
#include "FDPolarity.h"
#endif

#ifndef MAPPPROPFUNC_H
#include "MappPropFunc.h"
#endif

#ifndef PARTICLE_H
#include "Particle.h"
#endif

#ifndef PARTICLEBUNCH_H
#include "ParticleBunch.h"
#endif

#ifndef SLOT_H
#include "Slot.h"
#endif

#ifndef COMBINEDFUNCTION_H
#include "combinedFunction.h"
#endif

#ifndef PINGER_H
#include "pinger.h"
#endif

#endif // BEAMLINE_H
