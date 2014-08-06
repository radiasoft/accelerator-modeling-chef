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
******  ----------------
******  REVISION HISTORY
******  ----------------
******  Feb 2009    michelotti@fnal.gov
******  - fixed scale error in rfcavity::Propagator::setup
******  
******  Dec 2012    michelotti@fnal.gov
******  - added functionality for multiple harmonics
******    : simple-minded implementation; should be improved some day.
******   
******  Jul 2014    michelotti@fnal.gov
******  - added functionality for cavity with frequency not
******    multiple of the principal harmonic
******    : to simulate slip stacking.
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
  double const anh_phase = thinrfcavity::thinrfcavity_core_access::get_cumulative_displaced_phase_slip( elm );

  State_t& state = p.State(); 

  Component_t phase_slip_argument = ( state[i_cdt] * w_rf / PH_MKS_c ) + anh_phase;

  std::vector<rfcavity::multiple_harmonic_parameters> const & mhp = thinrfcavity::thinrfcavity_core_access::get_harmonics_data( elm );

  Component_t strength_factor = Component_t(0.0);
  for( int i = 0; i < mhp.size(); ++i ) {
    strength_factor += mhp[i].relative_strength * sin( mhp[i].harmonic_multiple*(phi_s + phase_slip_argument) + mhp[i].phase_shift );
  }

  Component_t E = p.Energy() + elm.Strength()*strength_factor;

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

void rfcavity::Propagator::setup( rfcavity& rfc )
{
  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr( rfc );
  bml = BmlPtr(new beamline(""));
  // 
  // -- First drift
  // 
  bml->append( ElmPtr(new drift( "", rfc.Length()/2.0 ) ) );

  // 
  // -- Thin cavity
  // 
  thinrfcavity* ptr_thin_rfc = new thinrfcavity( "", 
                                                 rfc.getRadialFrequency()/(2*M_PI), 
                                                 1.0e9*(rfc.Strength()), 
                                                 rfc.getPhi(), 
                                                 rfc.getQ(), 
                                                 rfc.getR() );

  thinrfcavity::thinrfcavity_core_access::get_harmonics_data( *ptr_thin_rfc ) 
    = rfcavity    ::rfcavity_core_access::get_harmonics_data(           rfc );

  bml->append( ElmPtr(ptr_thin_rfc) );

  // 
  // -- Second drift
  // 
  bml->append( ElmPtr(new drift( "", rfc.Length()/2.0 ) ) );
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

