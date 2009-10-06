/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SectorPropagators.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
****** REVISION HISTORY
******
****** July 2007  ostiguy@fnal.gov
******                            
****** - refactored and streamlined code: take advantage of 
******   Vector and Mapping operators.  
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation.
******                                    
******
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/SectorPropagators.h>
#include <beamline/sector.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

namespace {

  typedef PhaseSpaceIndexing::index index;

  index const i_x   = Particle::i_x;
  index const i_y   = Particle::i_y;
  index const i_cdt = Particle::i_cdt;
  index const i_npx = Particle::i_npx;
  index const i_npy = Particle::i_npy;
  index const i_ndp = Particle::i_ndp;


template<typename Particle_t>
void propagate( sector const& elm, Particle_t& p )
{

 typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
 typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
 State_t& state = p.state();

 if ( elm.isMatrix() ) { 
  state =  elm.getMatrix()   * state; 
 } 
 else {
  state =  elm.getMap()      * state; // composition
 }
}

} // anonymous namespace 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void sector::Propagator::setup( BmlnElmnt& elm )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void sector::Propagator::operator()( BmlnElmnt const& elm, Particle& p ) 
{
  ::propagate(static_cast<sector const&>(elm),p);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sector::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p ) 
{
  ::propagate(static_cast<sector const&>(elm),p);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

