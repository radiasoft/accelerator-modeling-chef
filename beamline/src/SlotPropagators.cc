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
****** REVISION HISTORY:
******
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation
******
******
**************************************************************************
**************************************************************************
*************************************************************************/

#include <beamline/SlotPropagators.h>
#include <beamline/Slot.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <basic_toolkit/GenericException.h>

namespace {

 typedef PhaseSpaceIndexing::index index;

 index const i_x   = Particle::i_x;
 index const i_y   = Particle::i_y;
 index const i_cdt = Particle::i_cdt;
 index const i_npx = Particle::i_npx;
 index const i_npy = Particle::i_npy;
 index const i_ndp = Particle::i_ndp;


inline bool betaParallelTest( double const&  betaParallel ) {
    return (betaParallel <= 0.0); 
}

inline bool betaParallelTest( Jet const&   betaParallel ) {
    return (betaParallel.standardPart() )<= 0.0; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename Particle_t>
void processFrame( Frame const& frm,  Particle_t & p ) 
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  static bool firstTime = true;
  static Vector u_z(3);
  static Vector u_y(3);
 

  if( firstTime ) {
    firstTime = false;
    u_y[0] = 0.0; u_y[1] = 1.0; u_y[2] = 0.0;
    u_z[0] = 0.0; u_z[1] = 0.0; u_z[2] = 1.0;
  }

   
  State_t&  state = p.state();


  // Yaw -------------------------------------

  if( ( frm.getAxis(1) == u_y ) && ( frm.getAxis(2) != u_z ) ) 
  {
    // "Old" frame represented in "new" frame coordinates.

    Vector e_1 = frm.getAxis(0);
    Vector e_2 = frm.getAxis(1);
    Vector e_3 = frm.getAxis(2);
    
    double cs = e_1[0];
    double sn = e_3[0];

    // Coordinate transformation.

    State_t r        ( state[i_x]*e_1 + state[i_y]*e_2 );
    State_t dummy    ( p.vectorBeta() );
    State_t beta     ( dummy[0]*e_1 + dummy[1]*e_2 + dummy[2]*e_3 );
  
    Component_t tau   = ( - r[2] / beta[2] );
  
    state[i_x]    =   r[0] + tau*beta[0];
    state[i_y]    =   r[1] + tau*beta[1];
    state[i_cdt] +=   tau ;
  
    // Momentum transformation

    Component_t p1( state[i_npx] );
    Component_t p2( state[i_npy] );
    Component_t p3divpbar = sqrt( ( 1.0 + state[i_ndp] ) * ( 1.0 + state[i_ndp] ) - p1*p1 - p2*p2 );
  
     state[i_npx] =  cs*state[i_npx] + sn*p3divpbar ;
  }

  // Roll ------------------------------------

  else if( ( frm.getAxis(2) == u_z ) && ( frm.getAxis(1) != u_y ) ) 
  {
    double cs = ( frm.getAxis(0) )[0];
    double sn = ( frm.getAxis(1) )[0]; // ??? right?

    // sin of angle by which magnet is rolled
  
    Component_t temp   = state[i_x] * cs + state[i_y] * sn;

    state[1]   = state[i_y] * cs - state[i_x] * sn;
    state[0]   = temp;

    temp      = state[i_npx] * cs + state[i_npy] * sn;
    state[4]  = state[i_npy] * cs - state[i_npx] * sn;
    state[3]  = temp;

  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename Particle_t>
void propagate( Slot const& elm, Particle_t& p )
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t           State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
  typedef typename PropagatorTraits<Particle_t>::Vector_t         Vector_t;
 
  State_t& state = p.state();

 //----------------------------------------
 // Transform to Input Frame 
 //-----------------------------------------

  ::processFrame( elm.getInFrame(), p ); 

 //----------------------------------------
 // Propagate as drift to the out-plane
 //-----------------------------------------

  Frame const& outframe = elm.getOutFrame();

  Vector_t r(3);

  r[0] = state[i_x];
  r[1] = state[i_y];

  Vector_t beta ( p.vectorBeta() );

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
             "void propagate( Slot const& elm, Particle_t& p )", 
             uic.str().c_str() ) );
  };

  Component_t tau = (q-r) * u_3 / betaParallel;

  r += tau*beta;
  r -= q;

  state[ i_x   ]  = r*u_1;
  state[ i_y   ]  = r*u_2;
  state[ i_cdt ] += ( tau - elm.getReferenceTime() );

  // Momentum transformation

  Vector_t momntm = ( p.normalizedVectorMomentum() );

  state[ i_npx ] = momntm*u_1;
  state[ i_npy ] = momntm*u_2;

 //----------------------------------------
 // Transform to Output Frame 
 //-----------------------------------------

  ::processFrame( elm.getOutFrame(), p ); 

}

} // anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::Propagator::Propagator()
  : BasePropagator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::Propagator::Propagator(Slot const& elm)
  : BasePropagator(elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Slot::Propagator::Propagator( Propagator const& p)
  : BasePropagator(p)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::Propagator::ctor( BmlnElmnt const& elm)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::Propagator::operator()( BmlnElmnt const& elm, Particle& p ) 
{
  ::propagate(static_cast<Slot const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Slot::Propagator::operator()( BmlnElmnt const& elm, JetParticle&     p ) 
{
  ::propagate(static_cast<Slot const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
