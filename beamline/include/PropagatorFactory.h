/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      PropagatorFactory.h
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

#ifndef PROPAGATORFACTORY_H
#define PROPAGATORFACTORY_H

#include <map>
#include <boost/function.hpp>

class Particle;
class JetParticle;

template <typename Particle_t>
class TBunch;

typedef TBunch<Particle> ParticleBunch;

class bmlnElmnt;


class  PropagatorFactory {

 class  PropagatorFactoryImpl; 

 public: 

// static PropagatorFactoryImpl& Instance()
//    {
//      //if (!pInstance_) { pInstance_ = new  PropagatorFactoryImpl(); }
//      //return *pInstance_;
//    }
 
 private:

   PropagatorFactory();
   PropagatorFactory(PropagatorFactory const&);

   static PropagatorFactory* pInstance_;
   

};


//-------------------------------------------------------------------------------

class  PropagatorFactoryImpl {

 typedef  boost::function<void(bmlnElmnt&,    Particle&)   >    particle_propagator_t;
 typedef  boost::function<void(bmlnElmnt&, JetParticle&)   > jetparticle_propagator_t;
 typedef  boost::function<void(bmlnElmnt&, ParticleBunch&) >       bunch_propagator_t;

 public:

 particle_propagator_t    CreateParticlePropagator(int id);

 jetparticle_propagator_t CreateJetParticlePropagator(int id);

 jetparticle_propagator_t const& ParticleBunchPropagator(int id);

 template <typename Propagator_t>
 void RegisterParticlePropagator( int elm_type_id, Propagator_t prop);

 template <typename Propagator_t>
 void RegisterJetParticlePropagator( int elm_type_id, Propagator_t prop);

 bool    UnRegisterParticlePropagator( int elm_type_id );  
 bool UnRegisterJetParticlePropagator( int elm_type_id );  

 private:

  std::map<int, particle_propagator_t>       particle_propagators_map_;
  std::map<int, jetparticle_propagator_t> jetparticle_propagators_map_;

 };

// ------------------------------------------------------------------------------



template <typename Propagator_t>
void
PropagatorFactoryImpl::RegisterParticlePropagator( int elm_id, Propagator_t prop) 
{  
  particle_propagators_map_.insert( std::map<int, particle_propagator_t>::value_type( elm_id, particle_propagator_t(prop() ) ) );
}


template <typename Propagator_t>
void
PropagatorFactoryImpl::RegisterJetParticlePropagator( int elm_id, Propagator_t prop ) 
{  
  particle_propagators_map_.insert( std::map<int, jetparticle_propagator_t>::value_type( elm_id, jetparticle_propagator_t(prop())) );
 
}

#endif // PROPAGATORFACTORY_H

