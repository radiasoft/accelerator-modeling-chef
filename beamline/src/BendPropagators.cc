/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BendPropagators.cc
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
******  Authors:   Leo Michelotti         michelotti@fnal.gov
******             Jean-Francois Ostiguy  ostiguy@fnal.gov
******
******
******
****** Note: The Bend propagation algorithm is documented in  
******       L. Michelotti, "Exact Map Through Ideal Bends (Again ?)
******       Proceedings of the 1995 PAC, p 2828  
******   
******
**************************************************************************
*************************************************************************/

#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/beamline.h>
#include <beamline/Bend.h>
#include <beamline/BendPropagators.h>
#include <beamline/beamline.h>

namespace {

  std::complex<double> const complex_i(0,1.0);

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template<typename Particle_t>
void propagate( std::complex<double> const& propPhase, 
                std::complex<double> const& propTerm, double const& dphi, Bend& elm, Particle_t& p ) 
{

 typedef typename PropagatorTraits<Particle_t>::State_t              State_t;
 typedef typename PropagatorTraits<Particle_t>::Component_t          Component_t;
 typedef typename PropagatorTraits<Particle_t>::ComplexComponent_t   ComplexComponent_t;

 State_t& state = p.State();

 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 // Preliminary filter from state coordinates

 Component_t psq = 1.0 + state[i_ndp];
 psq *= psq;

 Component_t E_factor = 1.0 / sqrt( psq + p.PNI2() );

 Component_t beta_1 = E_factor * state[i_npx];
 Component_t beta_2 = E_factor * state[i_npy];
 Component_t beta_3 = E_factor * sqrt( psq - state[i_npx]*state[i_npx] 
                                           - state[i_npy]*state[i_npy] );

 ComplexComponent_t ui  =   complex_i* state[i_x];
 ComplexComponent_t vui =   PH_MKS_c*beta_3 + complex_i*PH_MKS_c*beta_1;

 // Step 1.

 Component_t omega         = csq_red * elm.Strength() /  p.Energy();
 ComplexComponent_t bi     = ( complex_i*vui / omega ) - ui;

 // Step 2.

 ComplexComponent_t bf  =  bi* propPhase + propTerm;

 // Step 3.

 Component_t rho      =   PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 Component_t dthmdphi =   asin( real(bi)/rho ) - asin( real(bf)/rho );

 // Step 4.

 ComplexComponent_t expF = exp( complex_i * dthmdphi );
 ComplexComponent_t vuf  = vui*expF;
 ComplexComponent_t uf   = ( ui + bi )*expF - bf;

 // Final filter back to state coordinates

 Component_t dtheta =   dthmdphi +  dphi;
 Component_t cdt    = - PH_MKS_c * dtheta / omega;

 state[i_x  ]   =   imag( uf );
 state[i_y  ]  +=   beta_2*cdt;

 state[i_cdt]  +=  cdt - elm.getReferenceTime();

 state[i_npx]   =   imag( vuf )/( E_factor * PH_MKS_c );
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate( std::complex<double> const& propPhase, 
                         std::complex<double> const& propTerm, double const& dphi, Bend& elm, Particle_t& p ); 
template void propagate( std::complex<double> const& propPhase, 
                         std::complex<double> const& propTerm, double const& dphi, Bend& elm, JetParticle_t& p ); 
#endif
//-----------------------------------------------------------------------------------

} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Bend::Propagator::setup(Bend& elm) 
{

  double& angle_       = Bend::bend_core_access::get_angle(elm);  
  double& usAngle_     = Bend::bend_core_access::get_usAngle(elm); 
  double& dsAngle_     = Bend::bend_core_access::get_dsAngle(elm); 
  double& usFaceAngle_ = Bend::bend_core_access::get_usFaceAngle(elm); 
  double& dsFaceAngle_ = Bend::bend_core_access::get_dsFaceAngle(elm); 

  Bend::BendType bend_type = elm.getBendType();

  double psi = 0.0;
  double rho = 0.0;

  switch( bend_type ) {

  case type_sbend:

    psi         = angle_ - ( usFaceAngle_ + dsFaceAngle_ );
    dphi_       = - psi;
    rho         = elm.Length()/angle_; //  Note: angle_ and rho have the same sign.

    propPhase_  = std::exp(std::complex<double>(0.0, psi ));

    propTerm_ =        std::complex<double> ( 0.0, rho )
                     * std::complex<double> ( 1.0 - cos(angle_), -sin(angle_) )
                     * std::complex<double> ( cos( dsFaceAngle_),-sin(dsFaceAngle_) );


    break;

  case type_rbend :

    psi         =  - ( usFaceAngle_ + dsFaceAngle_ );
    dphi_       =  - psi;
    propPhase_  = std::exp(std::complex<double>(0.0, psi ));
    propTerm_   = elm.Length() * std::complex<double> ( cos( dsFaceAngle_), -sin(dsFaceAngle_) );

    break;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Bend::Propagator::operator()( Bend& elm, Particle& p ) 
{
  ::propagate(propPhase_, propTerm_, dphi_, elm, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Bend::Propagator::operator()( Bend& elm, JetParticle& p ) 
{
  ::propagate( propPhase_, propTerm_, dphi_, elm, p);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

