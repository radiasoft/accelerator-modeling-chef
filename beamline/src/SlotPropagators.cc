/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SlotPropagators.cc
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
**************************************************************************
**************************************************************************
*************************************************************************/

#include <basic_toolkit/GenericException.h>
#include <beamline/SlotPropagators.h>
#include <beamline/Slot.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


bool betaParallelTest( double const&  betaParallel ) {
    return (betaParallel <= 0.0); 
}

bool betaParallelTest( Jet const&   betaParallel ) {
    return (betaParallel.standardPart() )<= 0.0; 
}

template <typename Particle_t>
void propagate( Slot& elm, Particle_t& p )
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t           State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
  typedef typename PropagatorTraits<Particle_t>::Vector_t         Vector_t;
 
  State_t& state = p.State();

  Frame const& outframe = elm.getOutFrame();

 //----------------------------------------
 // Propagate as drift to the out-plane
 //-----------------------------------------

  Vector_t r(3);

  r[0] = state[i_x];
  r[1] = state[i_y];

  Vector_t beta ( p.VectorBeta() );

  Vector q    = outframe.getOrigin();
  Vector u_1  = outframe.getAxis(0);
  Vector u_2  = outframe.getAxis(1);
  Vector u_3  = outframe.getAxis(2);

  Component_t betaParallel = beta * u_3;
  
  if ( betaParallelTest  (betaParallel) ) {
   std::ostringstream uic;
      uic << elm.Type() << "  " << elm.Name()
          << ": Velocity is not forward: it may be NAN.";
      throw( GenericException( __FILE__, __LINE__,
             "void Slot::localPropagate( Particle& p )", 
             uic.str().c_str() ) );
  };

  Component_t tau = (q-r) * u_3 / betaParallel;

  r += tau*beta;
  r -= q;

  state[ i_x   ]  = r*u_1;
  state[ i_y   ]  = r*u_2;
  state[ i_cdt ] += ( tau - elm.getReferenceTime() );

  // Momentum transformation

  Vector_t momntm = ( p.NormalizedVectorMomentum() );

  state[ i_npx ] = momntm*u_1;
  state[ i_npy ] = momntm*u_2;
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(          Slot& elm,    Particle& p );
template void propagate(          Slot& elm, JetParticle& p );

#endif


} // anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::Propagator::setup(Slot& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::Propagator::operator()( Slot& elm, Particle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::Propagator::operator()( Slot& elm, JetParticle&     p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
