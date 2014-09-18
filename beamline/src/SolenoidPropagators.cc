/*************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      SolenoidPropgators.cc
******
******  Copyright Universities Research Association, Inc./ Fermilab
******            All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under U.S. Department of Energy
******  Contract No. DE-AC02-76CH03000 and
******  Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******  Author:    Leo Michelotti
******             Email: michelotti@fnal.gov
******
******
******  REVISION HISTORY:
******  ----------------
******  April 10, 2007  michelotti@fnal.gov
******  Original version.
******
******  November 2007   ostiguy@fnal.gov
******  - adapted to new propagator architecture
******  - templated version
******  - various optimizations
******
******  April 2008      michelotti@fnal.gov
******  - bug fixes: errors had been made in transcribing
******    from original algorithm to current one
******
******  September 2014  michelotti@fnal.gov
******  - removed 0/0 singularity for (effectively) zero strength
******    solenoids with propagation through a drift.
******  - "effectively" means | B | < 10^{-13} Tesla.
******    This bound can be modified by changing the value
******    of parameter "smallest_nonzero_field" below.
******
**************************************************************************
*************************************************************************/

#include <basic_toolkit/GenericException.h>
#include <beamline/drift.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/SolenoidPropagators.h>

namespace {

 double const smallest_nonzero_field = 1.0e-13; // Tesla

 Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
 Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
 Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
 Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
 Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
 Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template <typename Particle_t>
void propagate( Solenoid& elm, Particle_t & p )
{

  // ***  NOTE: This implementation assumes the magnetic axis of
  // **** NOTE: the solenoid AND the beam current are in the +z direction.

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;


  State_t& state = p.State();

  Component_t& x   = state[i_x  ];
  Component_t& y   = state[i_y  ];
  Component_t& npx = state[i_npx];
  Component_t& npy = state[i_npy];
  Component_t& cdt = state[i_cdt];


  // If the solenoid's strength is (effectively) zero,
  // propagate through the drift that was
  // instantiated by the setup function ...

  if( std::abs( elm.Strength() ) < smallest_nonzero_field )
  {
    // This first step is added to handle the bizarre, 
    // remote possibility that the length of a Solenoid
    // may be changed after instantiation.
    // 
    // It should be removed as soon as a sensible mechanism   ??? FIXME
    // is put in place which adjusts to changes in beamline   ??? FIXME
    // element attributes.                                    ??? FIXME

    ElmPtr& elm_ptr = bmlnElmnt::core_access::get_ElmPtr(elm);
    elm_ptr->setLength( elm.Length() );

    // Traverse the drift.

    elm_ptr->propagate( p );

    // NOTE: RefRegVisitor does not touch this element,
    // so this propagation should advance cdt, which now
    // must be handled.

    cdt -= elm.getReferenceTime();
  }

  // Otherwise, ordinary solenoid propagation ...

  else
  {
    //------------------
    // Entry edge effect
    // -----------------

    double const edgeKick = elm.Strength() / ( 2.0* p.ReferenceBRho() );

    if( elm.hasInEdge() ) {
      npx += edgeKick*y;
      npy -= edgeKick*x;
    }

    //------------
    // Body field
    // -----------

    Component_t dtheta =   ( elm.Strength() * elm.Length() / p.ReferenceBRho() )
                   / p.get_npz();

    Component_t sn = sin(dtheta);
    Component_t cs = cos(dtheta);

    Component_t x_i   = x;
    Component_t y_i   = y;
    Component_t npx_i = npx;
    Component_t npy_i = npy;

    npx =     cs*npx_i + sn*npy_i;
    npy =  (-sn)*npx_i + cs*npy_i;

    cs -= 1.0;

    x += (    cs*(-npy_i) + sn*npx_i ) / (2.0*edgeKick);
    y += ( (-sn)*(-npy_i) + cs*npx_i ) / (2.0*edgeKick);


    //-----------------
    // Exit edge effect
    // ----------------

    if( elm.hasOutEdge() ) {
      npx -= edgeKick*y;
      npy += edgeKick*x;
    }

    // It's about time
    // Assumes pre-registration via RefRegVisitor
    //
    // NOTE: p.get_npz() is non-negative by definition.
    // NOTE: I may want to change this some day.
    // -----------------------------------------------

    Component_t duration =   elm.Length()
                     / ( p.get_npz() * p.ReferenceMomentum() / p.Energy() );

    cdt += ( duration - elm.getReferenceTime() );
  }
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(          Solenoid& elm,    Particle& p );
template void propagate(          Solenoid& elm, JetParticle& p );

#endif

} // anonymous namespace


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::Propagator::setup( Solenoid& arg)
{
  // Create a drift to be used when strength is zero.
  // The propagator equations have a 0/0 singularity
  // at that point.

  ElmPtr& elm_ptr = bmlnElmnt::core_access::get_ElmPtr(arg);
  elm_ptr = ElmPtr( new drift( "", arg.Length() ) );
  
  // This step should not be necessary, as the drift
  // constructor (currently) handles this. Nonetheless,
  // it is included for paranoia's sake.

  elm_ptr->setReferenceTime( 0. );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::Propagator::operator()( Solenoid& elm, Particle& p )
{
  ::propagate( elm, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::Propagator::operator()( Solenoid& elm, JetParticle& p )
{
  ::propagate( elm, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

