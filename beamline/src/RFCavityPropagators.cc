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
#include <beamline/drift.h>

using FNAL::pcerr;
using FNAL::pcout;

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::i_x;
  Particle::PhaseSpaceIndex const& i_y   = Particle::i_y;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::i_cdt;
  Particle::PhaseSpaceIndex const& i_npx = Particle::i_npx;
  Particle::PhaseSpaceIndex const& i_npy = Particle::i_npy;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::i_ndp;



template<typename Particle_t>
void propagate( thinrfcavity const& elm, Particle_t&  p)
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
void propagate( rfcavity const& elm, Particle_t&  p)
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  BmlPtr const& bml = bmlnElmnt::core_access::get_BmlPtr( elm );

  for ( beamline::const_iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(         rfcavity const& elm,    Particle& p );
template void propagate(         rfcavity const& elm, JetParticle& p );
template void propagate(     thinrfcavity const& elm,    Particle& p );
template void propagate(     thinrfcavity const& elm, JetParticle& p );

#endif


} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::setup( bmlnElmnt& arg)
{

  rfcavity& elm = static_cast<rfcavity&>(arg);

  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr( elm );

  bml = BmlPtr(new beamline(""));
  bml->append( ElmPtr(new drift( "", elm.Length()/2.0 ) ) );
  bml->append( ElmPtr(new thinrfcavity( "", elm.getRadialFrequency()/(2*M_PI), 1.0e9*(elm.Strength()), elm.getPhi(), elm.getQ(), elm.getR() )));
  bml->append( ElmPtr(new drift( "", elm.Length()/2.0 ) ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  rfcavity::Propagator::setAttribute( bmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  setup(elm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::operator()( bmlnElmnt const& elm, Particle& p)
{
  ::propagate(static_cast<rfcavity const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::operator()( bmlnElmnt const& elm, JetParticle& p)
{
  ::propagate(static_cast<rfcavity const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::setup( bmlnElmnt& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::operator()( bmlnElmnt const& elm, Particle& p)
{
  ::propagate(static_cast<thinrfcavity const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::operator()( bmlnElmnt const& elm, JetParticle& p)
{
  ::propagate(static_cast<thinrfcavity const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
void rfcavity::setStrength( double const& strength)
{
  bmlnElmnt::setStrength(strength);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setLength( double const& )
{
  ostringstream methodIdent;
  methodIdent << "void " << Type() << "::setLength( double const& )";
  
  (*pcerr) <<   "*** ERROR ****: "
              "\n*** ERROR ****: "  << __FILE__ << "," << __LINE__
           << "\n*** ERROR ****: void " << Type() << "::setLength( double const& )" 
              "\n*** ERROR ****: Resetting the length of " 
           << Type() << " is not allowed in this version."
              "\n*** ERROR ****: " 
           << std::endl;

  ostringstream uic;
  uic << "Resetting the length of " << Type() << " is not allowed in this version.";
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           methodIdent.str().c_str(),
           uic.str().c_str() ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#endif
