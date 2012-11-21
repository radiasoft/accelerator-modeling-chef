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
void propagate( septum& elm, Particle_t& p );

template <>
void propagate( septum& elm, Particle& p )
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
  Component_t delta_x = zero;

  double const length = elm.Length();
  bool is_finite = true;
  int i = 0;
  while( is_finite && (i < 6) ) {
    if( 0 == finite( state[i] ) ) {
      p.setTag( p.getTag() + std::string(" / SEPTUM: INFINITE STATE / ") );
      is_finite = false;
    }
    ++i;
  }

  if( is_finite && (0.0 >= elm.getWireX()) ) {
    if( x < elm.getWireX() ) {
      x -= elm.getWireX();
      if( x >= - elm.getWireWidth() ) {                // hitting wire
        elm.numberBadHits_++;
        p.setTag( p.getTag() + std::string(" / SEPTUM: WIRE HIT / ") );
        kick = zero;
      }
      else {
        x += elm.getWireWidth();
        if( x > - elm.getGap() ) {                     // paritlce is kicked
          // unit_conversion: [N] = [kg m/s^2] to [Gev/c]
          double unit_conversion = PH_MKS_c / (1.0e9 * PH_MKS_e);
          kick = unit_conversion / p.ReferenceMomentum();
          delta_x = kick;
          // constant electric force
          double q = p.Charge();
          double voltage = elm.getVoltage();
          double gap = elm.getGap();
          kick *= - q * voltage * 1e3 / gap;  // [CV/m] = [N]
          delta_x *= - q * voltage * 1e3 / gap;
          // \delta_{t} = \delta_{z} / (\beta * c)
          kick *= length / (p.Beta() * PH_MKS_c);             // [sec]
          delta_x *= (length * length / 2.0) / (p.Beta() * PH_MKS_c);
          double npz = p.get_npz();
          elm.numberKicked_++;
          p.setTag( p.getTag() + std::string(" / SEPTUM: KICKED / ") );
        }
        else {
          elm.numberOutGap_++;
          p.setTag( p.getTag() + std::string(" / SEPTUM: OUTSIDE GAP! / ") );
        }
      }
    }
    else {
      kick = zero;  // Almost always zero.
      delta_x = zero;
    }
  }
  Component_t const xi = state[i_x];
  Vector pinned_state(p.State());
  if (xi <= elm.getWireX()) {
      p.setTag( p.getTag() + std::string(" / SEPTUM: ENTER SEPTUM / ") );
      elm.loadPinnedCoordinates(p, pinned_state);
      std::cout << elm.Name() << ": entrance: " << setprecision(16)
              << state[i_x] << "  " << pinned_state[3] << "  "
              << state[i_y] << "  " << state[i_npy] << std::endl;
  }
  Component_t npz = p.get_npz();
  Component_t xpr = state[i_npx] / npz;
  Component_t ypr = state[i_npy] / npz;

  state[i_x] += ( length * xpr) + delta_x / npz;
  state[i_y] += ( length * ypr);

  state[i_npx] += kick;
  Component_t D = length * sqrt( 1.0 + xpr * xpr + ypr * ypr );
  state[i_cdt] += ( D / p.Beta() ) - elm.getReferenceTime();

  Component_t const xf = state[i_x];
  if ((xi - elm.getWireX()) * (xf - elm.getWireX()) < 0.0) {
      p.setTag(elm.Name() + std::string(" / SEPTUM: INTERSECT WIRE PLANE / "));
      elm.loadPinnedCoordinates(p, pinned_state);
      std::cout << elm.Name() << ": hit septum plane: " << setprecision(16)
              << pinned_state[0] << "  " << pinned_state[3] << "  "
              << pinned_state[1] << "  " << pinned_state[4] << std::endl;
  } else if (xf <= elm.getWireX()) {
      p.setTag(p.getTag() + std::string(" / SEPTUM: EXIT SEPTUM / ") );
      elm.loadPinnedCoordinates(p, pinned_state);
      std::cout << elm.Name() << ": exit    : " << setprecision(16)
              << pinned_state[0] << "  " << pinned_state[3] << "  "
              << pinned_state[1] << "  " << pinned_state[4] << std::endl;
  }
}

template <>
void propagate( septum& elm, JetParticle& p )
{
  typedef PropagatorTraits<JetParticle>::State_t       State_t;
  typedef PropagatorTraits<JetParticle>::Component_t   Component_t;

  Component_t const zero(0);

  State_t& state = p.State();

  double const length = elm.Length();

  Component_t kick;
  Component_t delta_x;
  if (state[i_x].standardPart() < elm.getWireX()) {
      double unit_conversion = PH_MKS_c / (1.0e9 * PH_MKS_e);
      kick = unit_conversion / p.ReferenceMomentum();
      delta_x = kick;
      // constant electric force
      double q = p.Charge();
      double voltage = elm.getVoltage();
      double gap = elm.getGap();
      kick *= - q * voltage * 1e3 / gap;  // [CV/m] = [N]
      delta_x *= - q * voltage * 1e3 / gap;
      // \delta_{t} = \delta_{z} / (\beta * c)
      kick *= length / (p.Beta() * PH_MKS_c);             // [sec]
      delta_x *= (length * length / 2.0) / (p.Beta() * PH_MKS_c);
  } else {
      kick = zero;
      delta_x = zero;
  }
  Component_t npz = p.get_npz();
  Component_t xpr = state[i_npx] / npz;
  Component_t ypr = state[i_npy] / npz;

  state[i_x] += ( length * xpr) + delta_x / npz;
  state[i_y] += ( length * ypr);

  state[i_npx] += kick;

  Component_t D = elm.Length() * sqrt( 1.0 + xpr * xpr + ypr * ypr );
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

  if( is_finite && (0.0 >= elm.getWireX()) ) {
    if( x < elm.getWireX() ) {
      x -= elm.getWireX();
      if( x >= - elm.getWireWidth() ) {
        elm.numberBadHits_++;
        p.setTag( p.getTag() + std::string(" / SEPTUM: WIRE HIT / ") );
      }
      else {
        x += elm.getWireWidth();
        if( x > - elm.getGap() ) {
          kick = elm.getNegStrength();   // electrostatic septum, negative KICK
          elm.numberKicked_++;
          p.setTag( p.getTag() + std::string(" / SEPTUM: KICKED / ") );
          std::cout << "kicked(before): "
                  << x + elm.getWireX() - elm.getWireWidth() << "  "
                  << npx << "  " << y << "  " << npy << std::endl;
          std::cout << "kicked(after): "
                  << x + elm.getWireX() - elm.getWireWidth() << "  "
                  << npx + kick << "  " << y << "  " << npy << std::endl;
        }
        else {
          elm.numberOutGap_++;
          p.setTag( p.getTag() + std::string(" / SEPTUM: OUTSIDE GAP! / ") );
        }
      }
    }
    //else if( x < 2.0*elm.getWireX() ) {
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

  Component_t kick = ( state[i_x].standardPart() < elm.getWireX() ) ?
                       elm.getNegStrength()/(1.0 + state[i_ndp]) :  elm.getPosStrength()/(1.0 + state[i_ndp]);

  state[3] += kick;
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

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

