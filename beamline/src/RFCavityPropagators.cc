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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  REVISION HISTORY
******  
******  December 2006 ostiguy@fnal.gov 
******  -Efficiency improvements. 
******  -Particle state passed by reference.
******
******  January 4, 2006   (Wednesday)   michelotti@fnal.gov
******  Stored in cvs repository
******  after major reorganization of files.
******  - lpjm
******
******  January 12, 2007  (Friday)   michelotti@fnal.gov
******  Upgrades: preparation for use in ILC studies.
******  - lpjm
******
******  February 2, 2007  (Friday)   michelotti@fnal.gov
******  Fixed edge focusing following recommendation of Paul Lebrun
******  to use actual electric field upon entry.
******  In addition, included change to "effective length" calculation.
******  !! WARNING: THESE CHANGES ARE TENTATIVE AND BEING REVIEWED !!
******  - lpjm
******
******  May 9, 2007       (Wednesday)   michelotti@fnal.gov
******  A minor bug fix; produces minimal effect.  Another major overhaul
******  will be coming soon anyway.
******  - lpjm
******
******  May 14, 2007      (Monday)   michelotti@fnal.gov
******  This version is an intermediate, combining both old and new.
******  In essence, it is a reversion to the strategy of the 
******  November, 2005 version with a few details changed.
******  The internal structure of rfcavity is (temporarily?) being ignored.
******  - lpjm
******  
**************************************************************************
*************************************************************************/

#include <cmath>
#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/RFCavityPropagators.h>
#include <beamline/rfcavity.h>

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
void driftpropagate( double const& length, bmlnElmnt& elm, Particle_t& p )
{
     typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
     typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
     State_t& state = p.State();

     Component_t npz = p.get_npz();

     Component_t xpr = state[i_npx] / npz;
     Component_t ypr = state[i_npy] / npz;

     state[i_x] += length* xpr;
     state[i_y] += length* ypr;

     state[i_cdt] += length*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 

}

template<typename T>
double norm( T const& value);

template<>
double norm(double const& value) { return std::abs(value); } 

template<>
double norm(Jet const& value) { return std::abs(value.standardPart()); } 


template<typename Particle_t>
void propagate( rfcavity& elm, Particle_t&  p)
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  if( elm.Strength() == 0.0 ) { 
   ::driftpropagate( elm.Length()/2.0, elm, p);
   state[i_cdt] -= elm.getReferenceTime();
   return; 
  }

  double const m         = p.Mass();
  double const phi_s     = elm.getPhi(); 
  double const w_rf      = elm.getRadialFrequency(); 

  Component_t const p_i       = p.get_npz() * p.ReferenceMomentum();
  Component_t const E_i       = p.Energy();

  double const         referenceEnergyGain = elm.Strength()*sin(phi_s);
  Component_t    const onaxisEnergyGain    = elm.Strength()*sin( phi_s + state[i_cdt] * w_rf / PH_MKS_c );
  Component_t    const eE_z                = onaxisEnergyGain / elm.Length();

  // Thin lens kick upon entry

  Component_t k     = - ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

  state[i_npx] += k*state[i_x];  // ??? Improve!
  state[i_npy] += k*state[i_y];


  // Free space propagation through "effective half length"
  // of the cavity.

  Component_t x_in  = state[i_x];
  Component_t y_in  = state[i_y];

  ::driftpropagate( elm.Length()/2.0, elm, p);

  Component_t w = (onaxisEnergyGain/2.0) / p.Energy();

  if( norm(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                                       { w = 1.0;           }

  state[i_x]   = ( 1.0 - w )*x_in + w*state[i_x];
  state[i_y]   = ( 1.0 - w )*y_in + w*state[i_y];

  // Cavity increases energy and momentum of the particle
 
  Component_t E  = p.Energy() + onaxisEnergyGain/2.0;

  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  double newRefP = p.ReferenceMomentum();

  state[i_npx] *= ( oldRefP / newRefP );
  state[i_npy] *= ( oldRefP / newRefP );

  state[i_ndp] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;
  

  //--------------------------------------
  // PUT TRANSVERSE WAKE FIELD KICK HERE...
  //--------------------------------------

  // Cavity increases energy and momentum of the particle

  E = p.Energy() + onaxisEnergyGain/2.0;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2.0 );
  newRefP = p.ReferenceMomentum();

  state[i_npx] *= ( oldRefP / newRefP );
  state[i_npy] *= ( oldRefP / newRefP );

  state[i_ndp] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;


  // Free space propagation through "effective half length"
  // of the cavity.

  x_in  = state[i_x];
  y_in  = state[i_y];

  ::driftpropagate(elm.Length()/2, elm, p);

  w = (onaxisEnergyGain/2.0) / p.Energy();
  if( norm(w) > 1.0e-8 )     { w = (log(1.+w)/w); }
  else                       { w = 1.0;           }

  state[i_x]   = ( 1.0 - w )*x_in + w*state[i_x];
  state[i_y]   = ( 1.0 - w )*y_in + w*state[i_y];

  // Thin lens kick upon exit

  k         = ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

  state[i_npx] += k*state[i_x];
  state[i_npy] += k*state[i_y];


 Component_t rfct =   elm.Length() * (   (p.get_npz() * p.ReferenceMomentum() - p_i)
                                      /  (p.Energy()                          - E_i) );

  state[i_cdt] += rfct - elm.getReferenceTime();
}


template<typename Particle_t>
void propagate( thinrfcavity& elm, Particle_t&  p)
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

} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::setup( rfcavity& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::operator()( rfcavity& elm, Particle& p)
{
  ::propagate(elm, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Propagator::operator()( rfcavity& elm, JetParticle& p)
{
  ::propagate(elm, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::setup( thinrfcavity& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::operator()( thinrfcavity& elm, Particle& p)
{
  ::propagate(elm, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::Propagator::operator()( thinrfcavity& elm, JetParticle& p)
{
  ::propagate(elm, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
