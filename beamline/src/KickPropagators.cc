/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      KickPropagators.cc
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/kick.h>
#include <beamline/KickPropagators.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template<typename Particle_t>
void driftpropagate( double const& length, bmlnElmnt& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.State();

     Component_t npz = p.get_npz();

     Component_t xpr = state[i_npx] / npz;
     Component_t ypr = state[i_npy] / npz;

     state[i_x] += length* xpr;
     state[i_y] += length* ypr;

     state[i_cdt] += length*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 
}

template<typename Particle_t>
void applyKick( kick& elm, Particle_t& p)
{
    typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
    typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

    State_t& state = p.State();

    state[i_npx] += elm.horizontalStrength();
    state[i_npy] += elm.verticalStrength();
}

template<typename Particle_t>
void applyKick( hkick& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  state[i_npx] += elm.Strength();
}

template<typename Particle_t>
void applyKick( vkick& elm, Particle_t& p)
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  state[i_npy] += elm.Strength();
}

template<typename Element_t, typename Particle_t>
void propagate( Element_t& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t& state = p.State();

  if (elm.Length() > 0.0 ) {

    Component_t const cdt_in = state[i_cdt];

    ::driftpropagate( elm.Length()/2, elm, p );   // Drift through first  half of the length
    applyKick(elm,p); 
    ::driftpropagate( elm.Length()/2, elm, p );   // Drift through second half of the length.

    state[i_cdt] -= elm.getReferenceTime();  
 }
  else {
    applyKick(elm,p); 
  }
 
 }


} // anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void kick::Propagator::setup( kick& elm ) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void kick::Propagator::operator()( kick& elm, Particle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void kick::Propagator::operator()( kick& elm, JetParticle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hkick::Propagator::setup( hkick& elm ) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hkick::Propagator::operator()( hkick& elm, Particle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hkick::Propagator::operator()( hkick& elm, JetParticle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vkick::Propagator::setup( vkick& elm ) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vkick::Propagator::operator()( vkick& elm, Particle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vkick::Propagator::operator()( vkick& elm, JetParticle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
