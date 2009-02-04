/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      ParticleBunchFwd.h
******                                                                
******  Copyright (c) 2008 Fermi Research Alliance, Inc.    
******                All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******   
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov
******
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef PARTICLEBUNCHFWD_H
#define PARTICLEBUNCHFWD_H

#include <beamline/ParticleFwd.h>

template <typename Particle_t> 
class TBunch;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

#endif // PARTICLEBUNCHFWD_H
