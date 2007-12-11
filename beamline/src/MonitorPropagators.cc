/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      MonitorPropagators.cc
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
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
******                                                                
******  Authors: Jean-Francois Ostiguy  ostiguy@fnal.gov
******
******
**************************************************************************
*************************************************************************/

#include <beamline/MonitorPropagators.h>
#include <beamline/monitor.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>

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


template<typename T>
double toDouble( T const& );

template<>
double toDouble(double const& value) { return value; }

template<>
double toDouble(Jet    const& value) { return value.standardPart(); }

template<typename Elm_t, typename Particle_t>
double setMonitorState( Elm_t elm,  Particle_t& p );

template<typename Particle_t>
double setMonitorState( monitor& elm,  Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t& state = p.State();

  elm.setHposition( toDouble(state[i_x]) );
  elm.setNpx( toDouble(state[i_npx]) );
  elm.setVposition( toDouble(state[i_y]) );
  elm.setNpy( toDouble(state[i_npy]) );
}

template<typename Particle_t>
double setMonitorState( vmonitor & elm,  Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t& state = p.State();
  elm.setVposition( toDouble(state[i_y]) );
  elm.setNpy( toDouble(state[i_npy]) );
}

template<typename Particle_t>
double setMonitorState( hmonitor & elm,  Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t& state = p.State();
  elm.setHposition( toDouble(state[i_x]) );
  elm.setNpx( toDouble(state[i_npx]) );
}

template <typename Element_t, typename Particle_t>
void propagate( Element_t& elm, Particle_t& p )
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t& state = p.State();

  if( elm.Length() > 0.0 ) {
     ::driftpropagate( elm. Length()*elm.getDriftFraction(), elm, p ); 
    ::setMonitorState( elm, p);    
     ::driftpropagate( elm. Length()*(1.0-elm.getDriftFraction()), elm, p ); 
  } 
  else {
   ::setMonitorState(elm,p);    
  }    

  state[i_cdt] -= elm.getReferenceTime();
}


template<typename Element_t>
void propagate( Element_t& elm, ParticleBunch& b ) 
{ 
  // note: specialized versions for H and V monitors would be a tad more efficient

  double xpos = 0.0;
  double ypos = 0.0;

  for( ParticleBunch::iterator it = b.begin(); it != b.end(); ++it ) 
  {
    ::propagate(elm, *it); 
    xpos +=  elm.hposition();
    ypos +=  elm.vposition();  
  }
  
  // record the bunch centroid position (average position)
 
  elm.setHposition( xpos/b.size());
  elm.setVposition( ypos/b.size());

}

template<typename Element_t>
void propagate( Element_t& elm, JetParticleBunch& b ) 
{ 
  // note: specialized versions for H and V monitors would be a tad more efficient

  double xpos = 0.0;
  double ypos = 0.0;

  for( ParticleBunch::iterator it = b.begin(); it != b.end(); ++it ) 
  {
    ::propagate(elm, *it); 
    xpos +=  elm.hposition();
    ypos +=  elm.vposition();  
  }
  
  // record the bunch centroid position (average position)
 
  elm.setHposition( xpos/b.size());
  elm.setVposition( ypos/b.size());

}

} // anonymous namespace


void monitor::Propagator::setup( monitor& elm) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void monitor::Propagator::operator()( monitor& elm, Particle& p) 
{
  ::propagate(elm,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void monitor::Propagator::operator()( monitor& elm, JetParticle& p) 
{
  ::propagate(elm,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void monitor::Propagator::operator()( monitor& elm, ParticleBunch& p) 
{
  ::propagate(elm,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void monitor::Propagator::operator()( monitor& elm, JetParticleBunch& p) 
{
  ::propagate(elm,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vmonitor::Propagator::setup( vmonitor& elm) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vmonitor::Propagator::operator()( vmonitor& elm, Particle& p) 
{
  ::propagate(elm,p);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vmonitor::Propagator::operator()( vmonitor& elm, JetParticle& p) 
{
  ::propagate(elm,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vmonitor::Propagator::operator()( vmonitor& elm, ParticleBunch& p) 
{
  ::propagate(elm,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vmonitor::Propagator::operator()( vmonitor& elm, JetParticleBunch& p) 
{
  ::propagate(elm,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hmonitor::Propagator::setup( hmonitor& elm) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hmonitor::Propagator::operator()( hmonitor& elm, Particle& p) 
{
  ::propagate(elm,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hmonitor::Propagator::operator()( hmonitor& elm, JetParticle& p) 
{
  ::propagate(elm,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hmonitor::Propagator::operator()( hmonitor& elm, ParticleBunch& p) 
{
  ::propagate(elm,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hmonitor::Propagator::operator()( hmonitor& elm, JetParticleBunch& p) 
{
  ::propagate(elm,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

