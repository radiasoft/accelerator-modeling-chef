/**************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      PropagatorFactory.cc
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

#include <beamline/PropagatorFactory.h>

PropagatorFactoryImpl* PropagatorFactory::pInstance_ = 0;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

PropagatorFactoryImpl::particle_propagator_t const&   PropagatorFactoryImpl::CreateParticlePropagator(int id)
{

  std::map<int, particle_propagator_t>::const_iterator it =  particle_propagators_map_.find(id);

  if( it ==  particle_propagators_map_.end() ) {
    throw std::runtime_error("Unknown element id" );
  }

  return (it->second);
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


 PropagatorFactoryImpl::jetparticle_propagator_t const& PropagatorFactoryImpl::CreateJetParticlePropagator( int id)
{

  std::map<int, jetparticle_propagator_t>::const_iterator it =  jetparticle_propagators_map_.find(id);

  if( it ==  jetparticle_propagators_map_.end() ) {
    throw std::runtime_error("Unknown element id" );
  }

  return (it->second);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool PropagatorFactoryImpl::UnRegisterParticlePropagator( int id )
{

  return particle_propagators_map_.erase(id);

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool PropagatorFactoryImpl::UnRegisterJetParticlePropagator( int id )
{

  return jetparticle_propagators_map_.erase(id);
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



