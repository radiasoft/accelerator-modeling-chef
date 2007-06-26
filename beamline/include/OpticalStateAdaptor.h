/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      OpticalStateAdaptor.h
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
#ifndef OPTICALSTATEADAPTOR_H
#define OPTICALSTATEADAPTOR_H


#include <beamline/Particle.h>
#include <beamline/JetParticle.h>


//--------------------------------------------------------------------------------

template <typename Particle_t>
struct OpticalStateAdaptorTraits {
};

template <>
struct OpticalStateAdaptorTraits<Particle> {
 
  typedef double  Component_t; 
  typedef Vector  State_t ;

};

template <>
struct OpticalStateAdaptorTraits<JetParticle> {
 
  typedef Jet    Component_t; 
  typedef Mapping    State_t; 

};

//--------------------------------------------------------------------------------

template <typename Particle_t>
class  OpticalStateAdaptor {

 public:

  typedef typename OpticalStateAdaptorTraits<Particle_t>::State_t          State_t; 
  typedef typename OpticalStateAdaptorTraits<Particle_t>::Component_t  Component_t; 
 
  OpticalStateAdaptor( Particle_t&  p);
 ~OpticalStateAdaptor();

  Component_t const&  operator[](int i)  const; 
  Component_t&        operator[](int i);

  void sync();  

 private:

  bool            state_is_valid_;
  Particle_t&     particle_;
  State_t         optical_state_;
 
};

#include  <beamline/OpticalStateAdaptor.tcc>

#endif // OPTICALSTATEADAPTOR

