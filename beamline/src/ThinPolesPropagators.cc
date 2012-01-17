/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      ThinPolesPropagators.cc
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
******  Authors: Jean-Francois Ostiguy ostiguy@fnal.gov
******           Leo Michelotti        michelotti@fnal.gov
******
******  REVISION HISTORY
******
******  Jul 2011             michelotti@fnal.gov
******  - class ThinPole extended to permit arbitrary
******    collection of normal and skew multipoles
******
**************************************************************************
*************************************************************************/

#include <beamline/thinpoles.h>
#include <beamline/ThinPolesPropagators.h>
#include <mxyzptlk/Jet.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <complex>
#include <numeric>

namespace 
{
  template<typename T>
  T local_pow(T const& value, int n );

  template<>
  JetC local_pow( JetC const& value, int n )
  { return pow(value, n); }

  template<>
  std::complex<double> local_pow( std::complex<double> const& value, int n )
  { return std::pow(value, n); }

  std::complex<double> const complex_i(0.0,1.0);

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template<typename Element_t, typename Particle_t>
void simple_propagate( Element_t& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  if (elm.Strength() == 0.0 ) return;

  State_t& state = p.State();

  double const angle =  elm.Strength()/p.ReferenceBRho();

  state[i_npx]  -= angle;

 //----------------------------------------
 // thin2pole rotates the reference frame   ????????????????
 //----------------------------------------

  state[i_npx]   = cos( angle )*state[i_npx] + sin( angle ) * p.get_npz();

}


template<typename Element_t, typename Particle_t>
void simple_propagate( int n, Element_t& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t              State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t          Component_t;
  typedef typename PropagatorTraits<Particle_t>::ComplexComponent_t   ComplexComponent_t;

  if(elm.Strength() == 0.0)  return;

  State_t& state = p.State();

  double const k = elm.Strength()/p.ReferenceBRho();

  ComplexComponent_t z( state[i_x] +  complex_i*state[i_y] );

  z = ::local_pow(z,n);

  state[i_npx] -= k * real(z);
  state[i_npy] += k * imag(z);
}


template<typename Element_t, typename Particle_t>
void compound_propagate( Element_t& elm, Particle_t& p )
{
  // PRECONDITION: elm.c_.size() >= 2, guaranteed by ThinPole constructor

  typedef typename PropagatorTraits<Particle_t>::State_t              State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t          Component_t;
  typedef typename PropagatorTraits<Particle_t>::ComplexComponent_t   ComplexComponent_t;

  if(elm.Strength() == 0.0)  return;

  State_t& state = p.State();

  double const k = elm.Strength()/p.ReferenceBRho();

  ComplexComponent_t z( state[i_x] +  complex_i*state[i_y] );
  ComplexComponent_t arg( z );

  std::vector<std::complex<double> > const& c( elm.getMultipoleCoefficients() );

  // NOTE: less efficient than telescoping       ??? FIX THIS ???
  // NOTE: but automatically gets right type     ??? FIX THIS ???
  // NOTE:
  // NOTE: another efficiency issue: wasted      ??? FIX THIS ???
  // NOTE: operations; any number of the         ??? FIX THIS ???
  // NOTE: multipole coefficients could be zero. ??? FIX THIS ???
  ComplexComponent_t kick = c[1]*arg;
  int n = 1;
  while( ++n < c.size() ) {
    arg *= z;
    kick += c[n]*arg;
  }

  state[i_npx] -= k * real(kick);
  state[i_npy] += k * imag(kick);
}


//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void simple_propagate(        ThinPole& elm,      Particle& p );
template void simple_propagate(        ThinPole& elm,   JetParticle& p );
template void simple_propagate( int n, ThinPole& elm,      Particle& p );
template void simple_propagate( int n, ThinPole& elm,   JetParticle& p );
template void compound_propagate(      ThinPole& elm,      Particle& p );
template void compound_propagate(      ThinPole& elm,   JetParticle& p );

#endif


} // anonymous namespace


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ThinPole::Propagator::setup( ThinPole& elm )
{
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ThinPole::Propagator::operator()( ThinPole& elm, Particle& p )
{
  if( elm.isSimple() ) {
    (elm.getPole() == 2) ? ::simple_propagate(elm,p) : ::simple_propagate(elm.getPole()/2-1,elm,p);
  }
  else {
    ::compound_propagate(elm,p);
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ThinPole::Propagator::operator()( ThinPole& elm, JetParticle& p )
{
  if( elm.isSimple() ) {
    (elm.getPole() == 2) ? ::simple_propagate(elm,p) : ::simple_propagate(elm.getPole()/2-1,elm,p);
  }
  else {
    ::compound_propagate(elm,p);
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
