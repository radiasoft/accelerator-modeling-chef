/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SeptumPropagators.cc
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
******
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
******  Authors:   Leo Michelotti         michelotti@fnal.gov
******             Jean-Francois Ostiguy  ostiguy@fnal.gov
******
******
**************************************************************************
*************************************************************************/

#include <beamline/SeptumPropagators.h>
#include <beamline/septum.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template <typename Particle_t>
void propagate( thinSeptum& elm, Particle_t& p );


template <>
void propagate( thinSeptum& elm, Particle& p )
{
  typedef PropagatorTraits<Particle>::State_t       State_t;
  typedef PropagatorTraits<Particle>::Component_t   Component_t;
  
  Component_t const zero(0);
 
  State_t& state = p.State();

  Component_t x = state[i_x];
  Component_t y = state[i_y];
  Component_t npx = state[i_npx];
  Component_t npy = state[i_npy];
  Component_t kick = zero;

  bool is_finite = true;
  int i = 0;
  while( is_finite && (i < 6) ) {
    if( 0 == finite( state[i] ) ) {
      p.setTag( p.getTag() + std::string(" / SEPTUM: INFINITE STATE / ") );
      is_finite = false;
    }
    ++i;
  }

  if( is_finite && (0.0 <= elm.getWireX()) ) {
    if( x < - elm.getWireX() ) {
      x += elm.getWireX();
      if( x >= - elm.wireWidth_ ) {
        elm.numberBadHits_++;
        p.setTag( p.getTag() + std::string(" / SEPTUM: WIRE HIT / ") );
      }
      else {
        x += elm.wireWidth_;
        if( x > - elm.gap_ ) {
          kick = elm.getNegStrength();   // electrostatic septum, negative KICK
          elm.numberKicked_++;
          p.setTag( p.getTag() + std::string(" / SEPTUM: KICKED / ") );
          std::cout << "kicked(before): " << x - elm.getWireX() - elm.wireWidth_
                  << "  " << npx << "  " << y << "  " << npy << std::endl;
          std::cout << "kicked(after): " << x - elm.getWireX() - elm.wireWidth_
                  << "  " << npx+kick << "  " << y << "  " << npy << std::endl;

        }
        else {
          elm.numberOutGap_++;
          p.setTag( p.getTag() + std::string(" / SEPTUM: OUTSIDE GAP! / ") );
        }
      }
    }
    //else if( x > 2.0*elm.getWireX() ) {
    //  elm.numberBackHits_++;
    //  p.setTag( p.getTag() + std::string(" / SEPTUM: PIPE HIT / ") );
    //}
    else {
      kick = elm.getPosStrength();  // Almost always zero.
    }
  }

  #if 0
  ??? FIX THIS ???
  else {
    ostringstream uic;
    uic  << " SORRY: septum not written to handle negative wire.";
    throw( GenericException( __FILE__, __LINE__, 
           "void propagate( thinSeptum& elm, Particle& p )",
           uic.str().c_str() ) );
  }
  #endif

  state[3] += kick;
}

template <>
void propagate( thinSeptum& elm, JetParticle& p )
{
  typedef PropagatorTraits<JetParticle>::State_t       State_t;
  typedef PropagatorTraits<JetParticle>::Component_t   Component_t;
 
  State_t& state = p.State();

  Component_t kick = ( state[i_x].standardPart() < - elm.getWireX() ) ?
                       elm.getNegStrength()/(1.0 + state[i_ndp]) :  elm.getPosStrength()/(1.0 + state[i_ndp]);
  
  state[3] += kick;
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(     thinSeptum& elm,    Particle& p );
template void propagate(     thinSeptum& elm, JetParticle& p );

#endif
//----------------------------------------------------------------------------------

} // anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinSeptum::Propagator::setup(thinSeptum& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinSeptum::Propagator::operator()( thinSeptum& elm, Particle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::Propagator::operator()( thinSeptum& elm, JetParticle&     p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

