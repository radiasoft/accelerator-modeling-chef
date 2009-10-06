/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      RFCavityPropagators.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
******
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
******  Authors:   Jean-Francois Ostiguy  ostiguy@fnal.gov
******             Leo Michelotti         michelotti@fnal.gov
******             
****** REVISION HISTORY:
******
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation.
******                                                                
******   
**************************************************************************
*************************************************************************/

#include <cmath>
#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/RFCavityPropagators.h>
#include <beamline/beamline.h>
#include <beamline/rfcavity.h>
#include <beamline/Drift.h>

using namespace PhysicsConstants;
using FNAL::pcerr;
using FNAL::pcout;

namespace {

  typedef PhaseSpaceIndexing::index index;

  index const i_x   = Particle::i_x;
  index const i_y   = Particle::i_y;
  index const i_cdt = Particle::i_cdt;
  index const i_npx = Particle::i_npx;
  index const i_npy = Particle::i_npy;
  index const i_ndp = Particle::i_ndp;



template<typename Particle_t>
void propagate( thinrfcavity const& elm, Particle_t&  p)
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  if( elm.Strength() == 0.0 ) return;

  double const m       = p.mass();
  double const phi_s   = elm.phi(); 
  double const wrf     = elm.frequency()*2.0*M_PI; 

  State_t& state = p.state(); 
  
  Component_t E = p.energy() + elm.Strength()*sin( phi_s + state[i_cdt] * wrf / PH_MKS_c );

  double oldRefP = p.refMomentum();
  p.setRefEnergy( p.refEnergy() + elm.Strength()*sin(phi_s) );
  double newRefP = p.refMomentum();

  state[i_npx] *= ( oldRefP / newRefP );
  state[i_npy] *= ( oldRefP / newRefP );

  state[i_ndp]  = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0 ;
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename Particle_t>
void propagate( rfcavity const& elm, Particle_t&  p, BmlPtr bml)
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.state();

  for ( beamline::const_iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }
}

} // namespace

rfcavity::Propagator::Propagator()
  : BasePropagator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity::Propagator::Propagator(rfcavity const& elm)
  : BasePropagator(elm)
{
  ctor(elm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity::Propagator::Propagator(Propagator const& p)
  : BasePropagator(p)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::ctor( BmlnElmnt const& elm)
{

  rfcavity const& cav = static_cast<rfcavity const&>(elm);


  bml_ = BmlPtr(new beamline(""));
  bml_->append( ElmPtr(new Drift( "", elm.Length()/2.0 ) ) );
  bml_->append( ElmPtr(new thinrfcavity( "", cav.frequency(), cav.Strength()*1.0e-9, cav.phi(), cav.Q(), cav.R() )));
  bml_->append( ElmPtr(new Drift( "", elm.Length()/2.0 ) ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rfcavity::Propagator::getReferenceTime() const 
{
  double value = 0;

  for( beamline::iterator it = bml_->begin(); it != bml_->end();  ++it ) {
    value += (*it)->getReferenceTime();
  }

  return value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  rfcavity::Propagator::setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  ctor(elm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::operator()( BmlnElmnt const& elm, Particle& p)
{
  ::propagate(static_cast<rfcavity const&>(elm), p, bml_);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p)
{
  ::propagate(static_cast<rfcavity const&>(elm), p, bml_);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinrfcavity::Propagator::Propagator()
  : BasePropagator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinrfcavity::Propagator::Propagator(thinrfcavity const& elm)
  : BasePropagator(elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinrfcavity::Propagator::Propagator(Propagator const& p)
  : BasePropagator(p)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::ctor( BmlnElmnt const& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::operator()( BmlnElmnt const& elm, Particle& p)
{
  ::propagate(static_cast<thinrfcavity const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p)
{
  ::propagate(static_cast<thinrfcavity const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

