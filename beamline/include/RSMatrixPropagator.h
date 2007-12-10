/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      RSMatrixPropagator.h
******
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov      
******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/

//------------------------------------------------------------------------ 
//
//  Propagator based on the Rosenzweig - Serafini Matrix  
//  Alternate propagator for LinacCavity
//
//-------------------------------------------------------------------------

#ifndef RSMATRIXPROPAGATOR_H
#define RSMATRIXPROPAGATOR_H

#include <beamline/bmlnElmnt.h>

class Particle;
class JetParticle;

class RSMatrixPropagator { 

public:

  void operator()( bmlnElmnt& elm,  Particle&    p ) {  propagate<Particle>( elm,p); }
  void operator()( bmlnElmnt& elm,  JetParticle& p ) {  propagate<JetParticle>( elm,p); }

  template<typename Particle_t>
  void propagate( bmlnElmnt& elm_arg, Particle_t& p ); 
};

#endif


