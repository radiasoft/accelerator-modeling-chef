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
******           Leo Michelotti         michelotti@fnal.gov
******
**************************************************************************
*************************************************************************/

#include <beamline/MonitorPropagators.h>
#include <beamline/Monitor.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/TBunch.h>

namespace {
  
  typedef PhaseSpaceIndexing::index index;

  index const i_x   = Particle::i_x;
  index const i_y   = Particle::i_y;
  index const i_cdt = Particle::i_cdt;
  index const i_npx = Particle::i_npx;
  index const i_npy = Particle::i_npy;
  index const i_ndp = Particle::i_ndp;


template<typename Particle_t>
void driftpropagate( double const& length, BmlnElmnt const& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.state();

     Component_t npz = p.npz();

     Component_t xpr = state[i_npx] / npz;
     Component_t ypr = state[i_npy] / npz;

     state[i_x] += length* xpr;
     state[i_y] += length* ypr;

     state[i_cdt] += length*sqrt( 1.0 + xpr*xpr + ypr*ypr )/ p.beta(); 
}


template<typename T>
double toDouble( T const& );

template<>
double toDouble(double const& value) { return value; }

template<>
double toDouble(Jet    const& value) { return value.standardPart(); }

template<typename Elm_t, typename Particle_t>
double setMonitorState( Elm_t elm,  Particle_t const& p );

//-------------------------------------------------------------------------------

template<typename Particle_t>
double setMonitorState( Monitor const& elm,  Particle_t const& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t const& state = p.state();

  elm.setHposition( toDouble(state[i_x]) );
        elm.setNpx( toDouble(state[i_npx]) );
  elm.setVposition( toDouble(state[i_y]) );
        elm.setNpy( toDouble(state[i_npy]) );
}

//-------------------------------------------------------------------------------

template<typename Particle_t>
double setMonitorState( VMonitor const& elm,  Particle_t const& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t const& state = p.state();
  elm.setVposition( toDouble(state[i_y]) );
  elm.setNpy( toDouble(state[i_npy]) );
}

//-------------------------------------------------------------------------------

template<typename Particle_t>
double setMonitorState( HMonitor const& elm,  Particle_t const& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t const& state = p.state();
  elm.setHposition( toDouble(state[i_x]) );
  elm.setNpx( toDouble(state[i_npx]) );
}

//-------------------------------------------------------------------------------

template <typename Element_t, typename Particle_t>
void propagate( Element_t const& elm, Particle_t& p )
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  State_t& state = p.state();

  if( elm.Length() > 0.0 ) {
     ::driftpropagate( elm.Length() * elm.getDriftFraction(),       elm, p ); 
     ::setMonitorState( elm, p);    
     ::driftpropagate( elm.Length() * (1.0-elm.getDriftFraction()), elm, p ); 
  } 
  else {
   ::setMonitorState(elm,p);    
  }    

  state[i_cdt] -= elm.getReferenceTime();
}

//-------------------------------------------------------------------------------

template<typename Element_t>
void propagate( Element_t const& elm, ParticleBunch& b );

template<>
void propagate( Monitor const& elm, ParticleBunch& b )
{ 

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

//-------------------------------------------------------------------------------

template<>
void propagate( HMonitor const& elm, ParticleBunch& b )
{ 

  double xpos = 0.0;

  for( ParticleBunch::iterator it = b.begin(); it != b.end(); ++it ) 
  {
    ::propagate(elm, *it); 
    xpos +=  elm.hposition();
  }
  
  // record the bunch centroid position (average position)
 
  elm.setHposition( xpos/b.size());
}

//-------------------------------------------------------------------------------

template<>
void propagate( VMonitor const& elm, ParticleBunch& b )
{ 

  double ypos = 0.0;

  for( ParticleBunch::iterator it = b.begin(); it != b.end(); ++it ) 
  {
    ::propagate(elm, *it); 
    ypos +=  elm.vposition();
  }
  
  // record the bunch centroid position (average position)
 
  elm.setVposition( ypos/b.size());
}


//-------------------------------------------------------------------------------

template<typename Element_t>
void propagate( Element_t const& elm, JetParticleBunch& b ); 

template<>
void propagate( Monitor const& elm, JetParticleBunch& b ) 
{ 
  double xpos = 0.0;
  double ypos = 0.0;

  for( JetParticleBunch::iterator it = b.begin(); it != b.end(); ++it ) 
  {
    ::propagate(elm, *it); 
    xpos +=  elm.hposition();  
    ypos +=  elm.vposition();  
  }
  
  // record the bunch centroid position (average position)
 
  elm.setHposition( xpos/b.size());
  elm.setVposition( ypos/b.size());

}

//-------------------------------------------------------------------------------

template<>
void propagate( HMonitor const& elm, JetParticleBunch& b ) 
{ 
  double xpos = 0.0;

  for( JetParticleBunch::iterator it = b.begin(); it != b.end(); ++it ) 
  {
    ::propagate(elm, *it); 
    xpos +=  elm.hposition();  
  }
  
  // record the bunch centroid position (average position)
 
  elm.setHposition( xpos/b.size());
}


//-------------------------------------------------------------------------------

template<>
void propagate( VMonitor const& elm, JetParticleBunch& b ) 
{ 
  double ypos = 0.0;

  for( JetParticleBunch::iterator it = b.begin(); it != b.end(); ++it ) 
  {
    ::propagate(elm, *it); 
    ypos +=  elm.vposition();  
  }
  
  // record the bunch centroid position (average position)
 
  elm.setVposition( ypos/b.size());

}


} // anonymous namespace


Monitor::Propagator::Propagator()
  : BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Monitor::Propagator::Propagator(Monitor const& elm)
  : BasePropagator(elm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Monitor::Propagator::Propagator(Propagator const& p)
  : BasePropagator(p)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Monitor::Propagator::ctor( BmlnElmnt const& elm) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void Monitor::Propagator::operator()( BmlnElmnt const& elm, Particle& p) 
{
  ::propagate( static_cast<Monitor const&>(elm),p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Monitor::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p) 
{
  ::propagate( static_cast<Monitor const&>(elm),p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Monitor::Propagator::operator()( BmlnElmnt const& elm, ParticleBunch& p) 
{
  ::propagate( static_cast<Monitor const&>(elm),p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Monitor::Propagator::operator()( BmlnElmnt const& elm, JetParticleBunch& p) 
{
  ::propagate( static_cast<Monitor const&>(elm),p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VMonitor::Propagator::Propagator()
  : BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VMonitor::Propagator::Propagator(VMonitor const& elm)
  : BasePropagator(elm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VMonitor::Propagator::Propagator(Propagator const& p)
  : BasePropagator(p)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void VMonitor::Propagator::ctor( BmlnElmnt const& elm) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void VMonitor::Propagator::operator()( BmlnElmnt const& elm, Particle& p) 
{
  ::propagate( static_cast<VMonitor const&>(elm),p);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void VMonitor::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p) 
{
  ::propagate( static_cast<VMonitor const&>(elm),p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void VMonitor::Propagator::operator()( BmlnElmnt const& elm, ParticleBunch& p) 
{
  ::propagate( static_cast<VMonitor const&>(elm),p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void VMonitor::Propagator::operator()( BmlnElmnt const& elm, JetParticleBunch& p) 
{
  ::propagate( static_cast<VMonitor const&>(elm),p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HMonitor::Propagator::Propagator()
  : BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HMonitor::Propagator::Propagator(HMonitor const& elm)
  : BasePropagator(elm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HMonitor::Propagator::Propagator(Propagator const& p)
  : BasePropagator(p)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void HMonitor::Propagator::ctor( BmlnElmnt const& elm) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void HMonitor::Propagator::operator()( BmlnElmnt const& elm, Particle& p) 
{
  ::propagate( static_cast<HMonitor const&>(elm),p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void HMonitor::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p) 
{
  ::propagate( static_cast<HMonitor const&>(elm),p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void HMonitor::Propagator::operator()( BmlnElmnt const& elm, ParticleBunch& p) 
{
  ::propagate( static_cast<HMonitor const&>(elm),p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void HMonitor::Propagator::operator()( BmlnElmnt const& elm, JetParticleBunch& p) 
{
  ::propagate( static_cast<HMonitor const&>(elm),p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

