/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      NonLinearLensPropagators.cc
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
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
******  Author:    Chong Shik Park
******             Email: cspark@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******
******  Aug 2014            cspark@fnal.gov
******  - implement non-linear lens first time
******
**************************************************************************
*************************************************************************/

#include <beamline/NonLinearLensPropagators.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/nonLinearLens.h>

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;

template<typename Particle_t>
void propagate( nonLinearLens& elm, Particle_t& p )
{
   
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  double const knll_ = elm.get_knll();
  double const cnll_ = elm.get_cnll();
  double const bcoeff_ = elm.get_bcoeff();
  double const dcoeff_ = elm.get_dcoeff();

  Component_t xbar = state[i_x] / cnll_;
  Component_t ybar = state[i_y] / cnll_;

  Component_t sq_xpy = sqrt((xbar + 1.0)*(xbar + 1.0) + ybar * ybar);
  Component_t sq_xmy = sqrt((xbar - 1.0)*(xbar - 1.0) + ybar * ybar);

  Component_t xi  = 0.5 * (sq_xpy + sq_xmy);
  Component_t eta = 0.5 * (sq_xpy - sq_xmy);

  Component_t dxi_dxbar  = 0.5 * ((xbar+1.0) / sq_xpy + (xbar-1.0) / sq_xmy);
  Component_t dxi_dybar  = 0.5 * ybar * (1.0 / sq_xpy + 1.0 / sq_xmy);
  Component_t deta_dxbar = 0.5 * ((xbar+1.0) / sq_xpy - (xbar-1.0) / sq_xmy);
  Component_t deta_dybar = 0.5 * ybar * (1.0 / sq_xpy - 1.0 / sq_xmy);

  Component_t xixi_m_etaeta = (xi * xi - eta * eta);
  Component_t sq_xixi_m_1 = sqrt(xi * xi - 1.0);
  Component_t sq_1_m_etaeta = sqrt(1.0 - eta * eta);
  Component_t singluar_term = (xi == 1.0) ? 0.0 : xi * xi / sq_xixi_m_1;
  Component_t d_hacn_xi = dcoeff_ + knll_ * log(xi + sq_xixi_m_1);
  Component_t b_acn_eta = bcoeff_ + knll_ * acos(eta);

  Component_t f2_g2 = (xi * sq_xixi_m_1 * d_hacn_xi + eta * sq_1_m_etaeta * b_acn_eta);

  Component_t dU_dxi = ( knll_ * xi + sq_xixi_m_1 * d_hacn_xi + singluar_term * d_hacn_xi) / xixi_m_etaeta - 2.0 * xi * f2_g2 / (xixi_m_etaeta * xixi_m_etaeta);
  Component_t dU_deta = - (knll_ * eta - sq_1_m_etaeta * b_acn_eta + eta * eta / sq_1_m_etaeta * b_acn_eta) / xixi_m_etaeta + 2.0 * eta * f2_g2 / (xixi_m_etaeta * xixi_m_etaeta);

  state[i_npx] += (dU_dxi * dxi_dxbar + dU_deta * deta_dxbar) / cnll_;
  state[i_npy] += (dU_dxi * dxi_dybar + dU_deta * deta_dybar) / cnll_;

  state[i_cdt] -= elm.getReferenceTime();
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(     nonLinearLens& elm,    Particle& p );
template void propagate(     nonLinearLens& elm, JetParticle& p );

#endif

} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void nonLinearLens::Propagator::setup( nonLinearLens& elm )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void nonLinearLens::Propagator::operator()( nonLinearLens& elm, Particle& p ) 
{
  ::propagate(elm ,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void nonLinearLens::Propagator::operator()( nonLinearLens& elm, JetParticle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

namespace {
} // anonymous namespace
