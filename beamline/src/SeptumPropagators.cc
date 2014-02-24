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

template<typename Particle>
double calculateKick( septum& elm, Particle& p )
{
  // unit_conversion: [N] = [kg m/s^2] to [Gev/c]
  double const unit_conversion = PH_MKS_c / (1.0e9 * PH_MKS_e);
  double pref = p.ReferenceMomentum() / unit_conversion;
  // electric force: Fx = [CV/m] = [N]
  int sgn = (elm.getWireX() > 0.0) ? 1 : -1;
  double const fx = sgn * p.Charge() * elm.getVoltage() * 1.0e3 / elm.getGap();
  // \delta_{t} = \delta_{z} / (\beta * c)
  double deltaT_ = elm.Length() / (p.Beta() * PH_MKS_c);
  // Fx * dz
  double kick_ = ( 1.0 / pref ) * fx * deltaT_;
  return kick_;
}

template <typename Particle_t>
void propagate( septum& elm, Particle_t& p );

template <>
void propagate( septum& elm, Particle& p )
{
  typedef PropagatorTraits<Particle>::State_t       State_t;
  typedef PropagatorTraits<Particle>::Component_t   Component_t;

  State_t& state = p.State();

  Component_t x = state[i_x];
  Component_t const npz = p.get_npz();
  Component_t const xpr = state[i_npx] / npz;
  Component_t const ypr = state[i_npy] / npz;

  Component_t const zero(0);
  Component_t kick_ = zero;
  Component_t deltaX_ = zero;

  double const length_ = elm.Length();
  double const xWire_ = elm.getWireX();
  double const wireWidth_ = elm.getWireWidth();
  double const gap_ = elm.getGap();

  bool is_finite = true;
  int i = 0;
  while( is_finite && (i < 6) ) {
    if( 0 == finite( state[i] ) ) {
      p.setTag( p.getTag() + std::string(" / SEPTUM: INFINITE STATE / ") );
      is_finite = false;
    }
    ++i;
  }

  if ( xWire_ > 0.0 && is_finite ) {
    if ( x > xWire_ ) {
      x -= xWire_;
      if ( x <= wireWidth_ ) {              // hitting septum head
        kick_   = zero;
        deltaX_ = zero;
        elm.numberBadHits_++;
        p.setTag( p.getTag() + std::string( " / SEPTUM: WIRE HIT! / " ) );
      } else {
        x -= wireWidth_;
        if ( x < gap_ ) {                   // particle will be kicked
          kick_   = calculateKick(elm, p);
          deltaX_ = kick_ * ( 0.5 * length_ ) / npz;
          elm.numberKicked_++;
          p.setTag( p.getTag() + std::string( " / SEPTUM: KICKED / ") );
        } else {                            // hitting septum gap
          kick_   = zero;
          deltaX_ = zero;
          elm.numberOutGap_++;
          p.setTag( p.getTag() + std::string( " / SEPTUM: OUTSIDE GAP! / " ) );
        }
      }
    } else {                                // circulating particles
      kick_ = zero;                         // almost always zero
      deltaX_ = zero;
      p.setTag( std::string( " " ) );
    }
  } else if ( xWire_ < 0.0 && is_finite ) {
    if ( x < xWire_ ) {
      x -= xWire_;
      if ( x >= - wireWidth_ ) {            // hitting septum head
        kick_   = zero;
        deltaX_ = zero;
        elm.numberBadHits_++;
        p.setTag( p.getTag() + std::string( " / SEPTUM: WIRE HIT! / " ) );
      } else {
        x += wireWidth_;
        if ( x > - gap_ ) {                 // particle will be kicked
          kick_   = calculateKick(elm, p);
          deltaX_ = kick_ * ( 0.5 * length_ ) / npz;
          elm.numberKicked_++;
          p.setTag( p.getTag() + std::string( " / SEPTUM: KICKED / ") );
        } else {                            // hitting septum gap
          kick_   = zero;
          deltaX_ = zero;
          elm.numberOutGap_++;
          p.setTag( p.getTag() + std::string( " / SEPTUM: OUTSIDE GAP! / " ) );
        }
      }
    } else {                                // circulating particles
      kick_   = zero;                       // almost always zero
      deltaX_ = zero;
      p.setTag( std::string( " " ) );
    }
  } else if ( xWire_ == 0.0 ) {
    ostringstream uic;
    uic  << " SORRY: septum plane cannot be sitting at zero xz-plane.";
    throw( GenericException( __FILE__, __LINE__,
           "void propagate( septum& elm, Particle& p )",
           uic.str().c_str() ) );
  }

  // print particle coordinates when it enters septum field region
  Vector pinned_state(p.State());
  if ( (p.getTag()).find("SEPTUM: KICKED") != std::string::npos ) {
    elm.loadPinnedCoordinates(p, pinned_state, 0.0);
    std::cout << elm.Name() << ": entrance: " << setprecision(16)
            << pinned_state[0] << "  " << pinned_state[3] << "  "
            << pinned_state[1] << "  " << pinned_state[4] << "  "
            << state[i_cdt] << "  " << state[i_ndp] << std::endl;
  }

  state[i_x] += length_ * xpr + deltaX_;
  state[i_y] += length_ * ypr;
  state[i_npx] += kick_;
  Component_t D = length_ * sqrt( 1.0 + xpr * xpr + ypr * ypr );
  state[i_cdt] += ( D / p.Beta() ) - elm.getReferenceTime();

  // print particle coordinates when it exits septum field region
  if ( (p.getTag()).find("SEPTUM: KICKED") != std::string::npos ) {
    elm.loadPinnedCoordinates(p, pinned_state);
    std::cout << elm.Name() << ": exit: " << setprecision(16)
            << pinned_state[0] << "  " << pinned_state[3] << "  "
            << pinned_state[1] << "  " << pinned_state[4] << "  "
            << state[i_cdt] << "  " << state[i_ndp] << std::endl;
  }
}

template <>
void propagate( septum& elm, JetParticle& p )
{
  typedef PropagatorTraits<JetParticle>::State_t       State_t;
  typedef PropagatorTraits<JetParticle>::Component_t   Component_t;

  Component_t const zero(0);

  State_t& state = p.State();

  Component_t npz = p.get_npz(); 
  Component_t xpr = state[i_npx] / npz;
  Component_t ypr = state[i_npy] / npz;

  double const length_ = elm.Length();
  double const xWire_  = elm.getWireX();

  Component_t kick_ = zero;
  Component_t deltaX_ = zero;

  if ( xWire_ > 0.0 ) {
    if ( state[i_x].standardPart() > xWire_ ) {
      //kick = calculateKick(elm. p);
      double const unit_conversion = PH_MKS_c / (1.0e9 * PH_MKS_e);
      Component_t pref = p.ReferenceMomentum() / unit_conversion;
      double const fx = p.Charge() * elm.getVoltage() * 1.0e3 / elm.getGap();
      Component_t deltaT_ = length_ / (p.Beta() * PH_MKS_c);
      kick_ = ( 1.0 / pref ) * fx * deltaT_;
      deltaX_ = kick_ * ( 0.5 * length_ ) / npz;
    } else {
      kick_ = zero;
      deltaX_ = zero;
    }
  } else if ( xWire_ < 0.0 ) {
    if ( state[i_x].standardPart() < xWire_ ) {
      //kick_ = calculateKick(elm, p);
      double const unit_conversion = PH_MKS_c / (1.0e9 * PH_MKS_e);
      Component_t pref = p.ReferenceMomentum() / unit_conversion;
      double const fx = p.Charge() * elm.getVoltage() * 1.0e3 / elm.getGap();
      Component_t deltaT_ = length_ / (p.Beta() * PH_MKS_c);
      kick_ = ( 1.0 / pref ) * fx * deltaT_;
      deltaX_ = length_ * xpr + kick_ * ( 0.5 * length_ ) / npz;
    } else {
      kick_ = zero;
      deltaX_ = zero;
    }
  }

  state[i_x] += length_ * xpr + deltaX_;
  state[i_y] += length_ * ypr;
  state[i_npx] += kick_;

  Component_t D = length_ * sqrt( 1.0 + xpr * xpr + ypr * ypr );
  state[i_cdt] += ( D / p.Beta() ) - elm.getReferenceTime();
}

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
  Component_t kick_ = zero;

  double const xWire_ = elm.getWireX();

  bool is_finite = true;
  int i = 0;
  while( is_finite && (i < 6) ) {
    if( 0 == finite( state[i] ) ) {
      p.setTag( p.getTag() + std::string(" / SEPTUM: INFINITE STATE / ") );
      is_finite = false;
    }
    ++i;
  }

  if( is_finite && (0.0 <= xWire_) ) {
    if ( x > xWire_ ) {
      x -= xWire_;
      if( x <= elm.getWireWidth() ) {
        elm.numberBadHits_++;
        p.setTag( p.getTag() + std::string(" / SEPTUM: WIRE HIT / ") );
      }
      else {
        x -= elm.getWireWidth();
        if( x < elm.getGap() ) {
          kick_ = elm.getPosStrength();
          elm.numberKicked_++;
          p.setTag( p.getTag() + std::string(" / SEPTUM: KICKED / ") );
          std::cout << "kicked(before): "
                  << x + elm.getWireX() + elm.getWireWidth() << "  "
                  << npx << "  " << y << "  " << npy << std::endl;
          std::cout << "kicked(after): "
                  << x + elm.getWireX() + elm.getWireWidth() << "  "
                  << npx + kick_ << "  " << y << "  " << npy << std::endl;
        }
        else {
          elm.numberOutGap_++;
          p.setTag( p.getTag() + std::string(" / SEPTUM: OUTSIDE GAP! / ") );
        }
      }
    }
  }
  else if( is_finite && (0.0 >= xWire_) ) {
    if( x < xWire_ ) {
      x -= xWire_;
      if( x >= - elm.getWireWidth() ) {
        elm.numberBadHits_++;
        p.setTag( p.getTag() + std::string(" / SEPTUM: WIRE HIT / ") );
      }
      else {
        x += elm.getWireWidth();
        if( x > - elm.getGap() ) {
          kick_ = elm.getNegStrength();
          elm.numberKicked_++;
          p.setTag( p.getTag() + std::string(" / SEPTUM: KICKED / ") );
          std::cout << "kicked(before): "
                  << x + elm.getWireX() - elm.getWireWidth() << "  "
                  << npx << "  " << y << "  " << npy << std::endl;
          std::cout << "kicked(after): "
                  << x + elm.getWireX() - elm.getWireWidth() << "  "
                  << npx + kick_ << "  " << y << "  " << npy << std::endl;
        }
        else {
          elm.numberOutGap_++;
          p.setTag( p.getTag() + std::string(" / SEPTUM: OUTSIDE GAP! / ") );
        }
      }
    }
  }
  else if ( xWire_ == 0.0 ) {
    ostringstream uic;
    uic  << " SORRY: septum plane cannot be sitting at zero xz-plane.";
    throw( GenericException( __FILE__, __LINE__,
           "void propagate( septum& elm, Particle& p )",
           uic.str().c_str() ) );
  }

  state[3] += kick_;
}

template <>
void propagate( thinSeptum& elm, JetParticle& p )
{
  typedef PropagatorTraits<JetParticle>::State_t       State_t;
  typedef PropagatorTraits<JetParticle>::Component_t   Component_t;
 
  State_t& state = p.State();

  Component_t kick_ = ( state[i_x].standardPart() < elm.getWireX() ) ?
                       elm.getNegStrength()/(1.0 + state[i_ndp]) :  elm.getPosStrength()/(1.0 + state[i_ndp]);

  state[3] += kick_;
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template double calculateKick(     septum& elm,    Particle& p );

template void propagate(         septum& elm,    Particle& p );
template void propagate(         septum& elm, JetParticle& p );
template void propagate(     thinSeptum& elm,    Particle& p );
template void propagate(     thinSeptum& elm, JetParticle& p );

#endif
//----------------------------------------------------------------------------------

} // anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void septum::Propagator::setup(septum& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void septum::Propagator::operator()( septum& elm, Particle& p )
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::Propagator::operator()( septum& elm, JetParticle&     p )
{
  ::propagate(elm,p);
}

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

