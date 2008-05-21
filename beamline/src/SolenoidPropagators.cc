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
******  Revision Date: 
******  Original version: April 10, 2007
****** 
******  November 2007 ostiguy@fnal.gov 
******  - adapted to new propagator architecture
******  - templated version
******  - various optimizations
******  
******  April 2008    michelotti@fnal.gov
******  - bug fixes: errors had been made in transcribing 
******    from original algorithm to current one
******  
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/GenericException.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/SolenoidPropagators.h>

namespace {

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

  #if 0
  //----------------------------------------------------------------------------
  // ... In what follows, I assume that dtheta is smaller than pi/4.
  const double threshold = 1.0e-14;
  // ??? const double threshold = 4.0*numeric_limits<double>::epsilon(); ???
  int counter = 0;
  double eps = ( 1 - sn*sn - cs*cs ) / (2.0*cs);
  while( threshold < fabs(eps) ) {
    if( counter < 8 ) {
      cs += eps;
      eps = ( 1 - sn*sn - cs*cs ) / (2.0*cs);
      ++counter;
    }
    else {
      throw GenericException( __FILE__, __LINE__, 
            "void Solenoid::localPropagate( Particle& p )",
            "Too many iterations to normalize cosine." );
    }
  }
  //----------------------------------------------------------------------------
  #endif
  
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
  // empty for the moment ...
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

