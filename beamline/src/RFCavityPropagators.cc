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
#include <beamline/drift.h>

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
void propagate( thinrfcavity& elm, Particle_t&  p)
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  if( elm.Strength() == 0.0 ) return;

  double const m         = p.Mass();
  double const phi_s     = elm.getPhi(); 
  double const w_rf      = elm.getRadialFrequency(); 

  State_t& state = p.State(); 
  
  Component_t E = p.Energy() + elm.Strength()*sin( phi_s + state[i_cdt] * w_rf / PH_MKS_c );

  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + elm.Strength()*sin(phi_s) );
  double newRefP = p.ReferenceMomentum();

  state[i_npx] *= ( oldRefP / newRefP );
  state[i_npy] *= ( oldRefP / newRefP );

  state[i_ndp]  = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0 ;
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename Particle_t>
void propagate( rfcavity& elm, Particle_t&  p)
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr( elm );

  for ( beamline::iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(         rfcavity& elm,    Particle& p );
template void propagate(         rfcavity& elm, JetParticle& p );
template void propagate(     thinrfcavity& elm,    Particle& p );
template void propagate(     thinrfcavity& elm, JetParticle& p );

#endif


} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::setup( rfcavity& elm)
{
  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr( elm );

  bml = BmlPtr(new beamline(""));
  bml->append( ElmPtr(new drift( "", elm.Length()/2.0 ) ) );
  bml->append( ElmPtr(new thinrfcavity( "", elm.getRadialFrequency()/(2*M_PI), elm.Strength(), elm.getPhi(), elm.getQ(), elm.getR() )));
  bml->append( ElmPtr(new drift( "", elm.Length()/2.0 ) ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::operator()( rfcavity& elm, Particle& p)
{
  ::propagate(elm, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::operator()( rfcavity& elm, JetParticle& p)
{
  ::propagate(elm, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::setup( thinrfcavity& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::operator()( thinrfcavity& elm, Particle& p)
{
  ::propagate(elm, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::operator()( thinrfcavity& elm, JetParticle& p)
{
  ::propagate(elm, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

