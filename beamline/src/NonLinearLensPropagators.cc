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

using namespace std;
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
void propagate( nonLinearLens& elm, Particle_t& p );

template<>
void propagate( nonLinearLens& elm, Particle& p )
{

  typedef PropagatorTraits<Particle>::State_t       State_t;
  typedef PropagatorTraits<Particle>::Component_t   Component_t;

  State_t& state = p.State();

  double const knll_ = elm.get_knll();
  double const cnll_ = elm.get_cnll();
  double const bcoeff_ = elm.get_bcoeff();
  double const dcoeff_ = elm.get_dcoeff();

  Component_t xbar = state[i_x] / cnll_;
  Component_t ybar = state[i_y] / cnll_;

  Component_t sq_xpy = sqrt((xbar + 1.)*(xbar + 1.) + ybar * ybar);
  Component_t sq_xmy = sqrt((xbar - 1.)*(xbar - 1.) + ybar * ybar);

  Component_t xi  = 0.5 * (sq_xpy + sq_xmy);
  Component_t eta = 0.5 * (sq_xpy - sq_xmy);

  Component_t dU_dxbar;
  Component_t dU_dybar;

  if ( ( ybar == 0 ) && ( abs(xbar) == 1. ) ) {
    (*pcerr) << "*** ERROR *** "
             << "\n*** ERROR *** " << __FILE__ << ", " << __LINE__
             << "\nNonLinearLensPropagator propagates with singular points: "
             << xbar
             << endl;
  } else if ( ( ybar == 0 ) && ( std::abs(xbar) != 1. ) ) {
    dU_dxbar = - eta;
    dU_dxbar += sqrt( 1. - eta * eta ) * ( bcoeff_ / knll_ + acos(eta) );
    dU_dxbar += - eta * eta * ( bcoeff_ / knll_ + acos(eta) ) / sqrt( 1. - eta * eta );
    dU_dxbar += 2. * eta * eta * sqrt( 1. - eta * eta ) * ( bcoeff_ / knll_ + acos(eta) ) / ( 1. - eta * eta );

    dU_dxbar /= ( 1. - eta * eta );
    dU_dxbar *=  ( knll_ / cnll_ );

    dU_dybar = 0.0;
  } else {
    Component_t dxi_dxbar  = 0.5 * ((xbar+1.) / sq_xpy + (xbar-1.) / sq_xmy);
    Component_t dxi_dybar  = 0.5 * ybar * (1. / sq_xpy + 1. / sq_xmy);
    Component_t deta_dxbar = 0.5 * ((xbar+1.) / sq_xpy - (xbar-1.) / sq_xmy);
    Component_t deta_dybar = 0.5 * ybar * (1. / sq_xpy - 1. / sq_xmy);

    Component_t dU_dxi = xi;
    dU_dxi += sqrt( xi * xi - 1. ) * ( dcoeff_ / knll_ + log( xi + sqrt(xi * xi - 1. ) ) );
    dU_dxi += xi * xi * ( dcoeff_ / knll_ + log( xi + sqrt(xi * xi - 1. ) ) ) / sqrt( xi * xi - 1. );
    dU_dxi += - 2. * xi * xi * sqrt( xi * xi - 1. ) * ( dcoeff_ / knll_ + log( xi + sqrt(xi * xi - 1. ) ) ) / ( xi * xi - eta * eta );
    dU_dxi += - 2. * xi * eta * sqrt( 1. - eta * eta ) * ( bcoeff_ / knll_ + acos(eta) ) / ( xi * xi - eta * eta );

    dU_dxi /= ( xi * xi - eta * eta );

    Component_t dU_deta = - eta;
    dU_deta += sqrt( 1. - eta * eta ) * ( bcoeff_ / knll_ + acos(eta) );
    dU_deta += - eta * eta * ( bcoeff_ / knll_ + acos(eta) ) / sqrt( 1. - eta * eta);
    dU_deta += 2. * eta * xi * sqrt( xi * xi - 1. ) * ( dcoeff_ / knll_ + log( xi + sqrt(xi * xi - 1. ) ) ) / ( xi * xi - eta * eta );
    dU_deta += 2. * eta * eta * sqrt( 1. - eta * eta ) * ( bcoeff_ / knll_ + acos(eta) ) / ( xi * xi - eta * eta );

    dU_deta /= ( xi * xi - eta * eta );

    dU_dxbar = (dU_dxi * dxi_dxbar + dU_deta * deta_dxbar) * ( knll_ / cnll_ );
    dU_dybar = (dU_dxi * dxi_dybar + dU_deta * deta_dybar) * ( knll_ / cnll_ );
  }

  state[i_npx] += dU_dxbar;
  state[i_npy] += dU_dybar;
}

template<>
void propagate( nonLinearLens& elm, JetParticle& p )
{
   
  typedef PropagatorTraits<JetParticle>::State_t       State_t;
  typedef PropagatorTraits<JetParticle>::Component_t   Component_t;

  State_t& state = p.State();

  double const knll_ = elm.get_knll();
  double const cnll_ = elm.get_cnll();
  double const bcoeff_ = elm.get_bcoeff();
  double const dcoeff_ = elm.get_dcoeff();

  Component_t xbar = state[i_x] / cnll_;
  Component_t ybar = state[i_y] / cnll_;

  Component_t sq_xpy = sqrt((xbar + 1.)*(xbar + 1.) + ybar * ybar);
  Component_t sq_xmy = sqrt((xbar - 1.)*(xbar - 1.) + ybar * ybar);

  Component_t xi  = 0.5 * (sq_xpy + sq_xmy);
  Component_t eta = 0.5 * (sq_xpy - sq_xmy);

  Component_t dU_dxbar;
  Component_t dU_dybar;

  if ( ( ybar.standardPart() == 0 ) && ( abs(xbar.standardPart()) == 1. ) ) {
    (*pcerr) << "*** ERROR *** "
             << "\n*** ERROR *** " << __FILE__ << ", " << __LINE__
             << "\nNonLinearLensPropagator propagates with singular points: "
             << xbar
             << endl;
  } else if ( ( ybar.standardPart() == 0 ) && ( std::abs(xbar.standardPart()) != 1. ) ) {
    dU_dxbar = - eta;
    dU_dxbar += sqrt( 1. - eta * eta ) * ( bcoeff_ / knll_ + acos(eta) );
    dU_dxbar += - eta * eta * ( bcoeff_ / knll_ + acos(eta) ) / sqrt( 1. - eta * eta );
    dU_dxbar += 2. * eta * eta * sqrt( 1. - eta * eta ) * ( bcoeff_ / knll_ + acos(eta) ) / ( 1. - eta * eta );

    dU_dxbar /= ( 1. - eta * eta );
    dU_dxbar *=  ( knll_ / cnll_ );

    dU_dybar = 0.0;
  } else {
    Component_t dxi_dxbar  = 0.5 * ((xbar+1.) / sq_xpy + (xbar-1.) / sq_xmy);
    Component_t dxi_dybar  = 0.5 * ybar * (1. / sq_xpy + 1. / sq_xmy);
    Component_t deta_dxbar = 0.5 * ((xbar+1.) / sq_xpy - (xbar-1.) / sq_xmy);
    Component_t deta_dybar = 0.5 * ybar * (1. / sq_xpy - 1. / sq_xmy);

    Component_t dU_dxi = xi;
    dU_dxi += sqrt( xi * xi - 1. ) * ( dcoeff_ / knll_ + log( xi + sqrt(xi * xi - 1. ) ) );
    dU_dxi += xi * xi * ( dcoeff_ / knll_ + log( xi + sqrt(xi * xi - 1. ) ) ) / sqrt( xi * xi - 1. );
    dU_dxi += - 2. * xi * xi * sqrt( xi * xi - 1. ) * ( dcoeff_ / knll_ + log( xi + sqrt(xi * xi - 1. ) ) ) / ( xi * xi - eta * eta );
    dU_dxi += - 2. * xi * eta * sqrt( 1. - eta * eta ) * ( bcoeff_ / knll_ + acos(eta) ) / ( xi * xi - eta * eta );

    dU_dxi /= ( xi * xi - eta * eta );

    Component_t dU_deta = - eta;
    dU_deta += sqrt( 1. - eta * eta ) * ( bcoeff_ / knll_ + acos(eta) );
    dU_deta += - eta * eta * ( bcoeff_ / knll_ + acos(eta) ) / sqrt( 1. - eta * eta);
    dU_deta += 2. * eta * xi * sqrt( xi * xi - 1. ) * ( dcoeff_ / knll_ + log( xi + sqrt(xi * xi - 1. ) ) ) / ( xi * xi - eta * eta );
    dU_deta += 2. * eta * eta * sqrt( 1. - eta * eta ) * ( bcoeff_ / knll_ + acos(eta) ) / ( xi * xi - eta * eta );

    dU_deta /= ( xi * xi - eta * eta );

    dU_dxbar = (dU_dxi * dxi_dxbar + dU_deta * deta_dxbar) * ( knll_ / cnll_ );
    dU_dybar = (dU_dxi * dxi_dybar + dU_deta * deta_dybar) * ( knll_ / cnll_ );
  }

  state[i_npx] += dU_dxbar;
  state[i_npy] += dU_dybar;
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
