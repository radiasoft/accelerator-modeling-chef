/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      ParticleBunch.h
******                                                                
******  Copyright (c) 2007 Fermi Research Alliance, Inc.    
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
#ifndef PARTICLEBUNCH_H
#define PARTICLEBUNCH_H

#include <beamline/TBunch.h>
#include <beamline/Particle.h>

typedef TBunch<Particle> ParticleBunch;

#endif // PARTICLEBUNCH_H
