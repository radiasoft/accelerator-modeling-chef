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
*****
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
****** REVISION HISTORY:
******
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation.
****** 
***************************************************************************
***************************************************************************
**************************************************************************/

#include <beamline/thinpoles.h>
#include <beamline/ThinPolesPropagators.h>
#include <mxyzptlk/Jet.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <complex>
#include <numeric>

namespace {

  template<typename T>
  inline T local_pow(T const& value, int n );

  template<>
  inline JetC local_pow( JetC const& value, int n ) 
  { return pow(value, n); }

  template<>
  inline std::complex<double> local_pow( std::complex<double> const& value, int n ) 
  { return std::pow(value, n); }
  
  std::complex<double> const complex_i(0.0,1.0);

  typedef PhaseSpaceIndexing::index index;

  index const i_x   = Particle::i_x;
  index const i_y   = Particle::i_y;
  index const i_cdt = Particle::i_cdt;
  index const i_npx = Particle::i_npx;
  index const i_npy = Particle::i_npy;
  index const i_ndp = Particle::i_ndp;


template<typename Element_t, typename Particle_t>
void propagate( Element_t const& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
  if (elm.Strength() == 0.0 ) return;

  State_t& state = p.state();

  double const angle =  elm.Strength()/p.refBrho();

  state[i_npx]  -= angle;

 //----------------------------------------
 // thin2pole rotates the reference frame 
 //----------------------------------------
 
  state[i_npx]   = cos( angle )*state[i_npx] + sin( angle ) * p.npz();

}

//-------------------------------------------------------------------------
// the code below applies to normal multipoles. It should be generalized by 
// making strength a complex quantity
//-------------------------------------------------------------------------

template<typename Element_t, typename Particle_t> 
void propagate( int n, Element_t& elm, Particle_t& p )
{
  typedef typename PropagatorTraits<Particle_t>::State_t              State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t          Component_t;
  typedef typename PropagatorTraits<Particle_t>::ComplexComponent_t   ComplexComponent_t;
 
  if(elm.Strength() == 0.0)  return;

  State_t& state = p.state();

  double const k = elm.Strength()/p.refBrho();
 
  ComplexComponent_t z( state[i_x] +  complex_i*state[i_y] );
 
  z = ::local_pow(z,n);

  state[i_npx] -= k * real(z);
  state[i_npy] += k * imag(z);
}

} // anonymous namespace


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ThinPole::Propagator::Propagator()
 : BasePropagator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ThinPole::Propagator::Propagator(ThinPole const& elm)
 : BasePropagator(elm)
{}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ThinPole::Propagator::Propagator(Propagator const& p)
 : BasePropagator(p)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ThinPole::Propagator::ctor( BmlnElmnt const& elm) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ThinPole::Propagator::operator()( BmlnElmnt const& elm, Particle& p ) 
{
   ThinPole const& tp = static_cast<ThinPole const&>(elm);
  (tp.getPole() == 2) ? ::propagate(tp,p) : ::propagate(tp.getPole()/2-1,tp,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ThinPole::Propagator::operator()( BmlnElmnt const& elm, JetParticle&     p ) 
{
  ThinPole const& tp = static_cast<ThinPole const&>(elm);
  (tp.getPole() == 2) ? ::propagate(elm,p) : ::propagate(tp.getPole()/2-1,elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
