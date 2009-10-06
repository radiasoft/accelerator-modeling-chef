/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      SBendPropagators.cc
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
******    in operator()() implementation.
******
******
**************************************************************************
*************************************************************************/

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/quadrupole.h>
#include <beamline/beamline.h>
#include <beamline/Edge.h>
#include <beamline/Bend.h>
#include <beamline/sbend.h>
#include <beamline/SBendPropagators.h>
#include <beamline/beamline.h>
#include <iostream>

namespace {

  typedef PhaseSpaceIndexing::index index;

  index const i_x   = Particle::i_x;
  index const i_y   = Particle::i_y;
  index const i_cdt = Particle::i_cdt;
  index const i_npx = Particle::i_npx;
  index const i_npy = Particle::i_npy;
  index const i_ndp = Particle::i_ndp;

template <typename T>
inline double toDouble( T const& );

template<>
inline double toDouble( Jet const& value)    { return value.standardPart(); }

template<>
inline double toDouble( double const& value) { return value; }
 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename Particle_t>
void propagate( sbend const& elm, Particle_t& p, BmlPtr bml )
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.state();

  for ( beamline:: const_iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }
  
  state[i_cdt] -= elm.getReferenceTime();   
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Element_t, typename Particle_t>
void mad_propagate( sbend const& elm, Particle_t& p, double n )
{

  typedef typename PropagatorTraits<Particle_t>::State_t     State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t Component_t;

  State_t& state = p.state();

  double const rho = toDouble(p.brho())/elm.Strength();
  
  double const h      = 1.0/rho;          // curvature
  double const length = elm.Length();         
  double const gamma  =  toDouble(p.gamma());
  double const beta   =  toDouble(p.beta() );

  double const angle   = elm.getBendAngle();

  if(angle == 0.0) {
    state[i_x  ] +=   length * state[i_npx];
    state[i_y  ] +=   length * state[i_npy];
    state[i_cdt] +=  -length /(beta*gamma*gamma) * state[i_ndp];
    return;
  }  

  static const int BMLN_dynDim = 6;

  double  matrix [ BMLN_dynDim ][BMLN_dynDim];
  
  for   ( int i=0; i < BMLN_dynDim; ++i) {
    for ( int j=0; j < BMLN_dynDim; ++j){
      matrix[i][j] = ( i==j) ? 1.0 : 0.0;
    }
  }
  
  double const kx     = h * sqrt(1.0 - n ); 
  double const ky     = h * sqrt( n ); 
  
  
  // entrance focusing 
  
  state[i_npx] +=  tan(elm.getEntryAngle())/rho *  state[i_x]; 
  state[i_npy] -=  tan(elm.getEntryAngle())/rho *  state[i_y]; 

  matrix[i_x  ][i_x  ] =           cos(kx*length);
  matrix[i_x  ][i_npx] =  (1.0/kx)*sin(kx*length);
  matrix[i_x  ][i_ndp] =  h/(kx*kx)*(1.0 - cos(kx*length));

  matrix[i_npx][i_x   ] = -kx*sin(kx*length);
  matrix[i_npx][i_npx ] =  cos(kx*length);
  matrix[i_npx][i_ndp ] =  h/kx * sin(kx*length); 

  matrix[i_y]  [i_y   ] =  cos(ky*length);
  matrix[i_y]  [i_npy ] = ( ky == 0.0 ) ? length : (1.0/ky)*sin(ky*length);

  matrix[i_npy][i_y   ] = -ky*sin(ky*length);
  matrix[i_npy][i_npy ] =  cos(ky*length);

  double const cnv =  - 1.0/beta;  

  //----------------------------------------------------------------------------------
  // Note: the std optical matrix formalism assumes the arc length s as the independent 
  //       coordinate. CHEF uses cdt with t > 0 when the particle is *late* w/r to 
  //       a reference particle. 
  //       A conversion factor cnv from ds to cdt is introduced to convert from cdt   
  //       to ds. A subtle difference is that cdt depends on dp/p not only 
  //       through the change in path length, but also through the change in velocity
  //       associated with a change in momentum. This latter effect is the origin
  //       of the extra term   - length/(beta*gamma*gamma);   
  // ----------------------------------------------------------------------------------

  matrix[i_cdt][i_x]    =  cnv * (h/kx)*sin(kx*length);
  matrix[i_cdt][i_npx]  =  cnv * (h/kx*kx)*(1.0-cos(kx*length));
  matrix[i_cdt][i_ndp]  =  cnv * (h*h)/(kx*kx*kx)*(kx*length - sin(kx*length) ) - length/(beta*gamma*gamma); 

  // Finally .. the mapping ...............................

  State_t const& inState  = p.state();
  State_t        outState( p.state().Dim() ); 

  for(  int i=0; i<BMLN_dynDim; ++i  ) {
    outState[i] = 0.0;
    for( int j=0; j<BMLN_dynDim; ++j  ) {
      outState[i] += matrix[i][j]*inState[j];
    }
  }

  std::copy( outState.begin(), outState.end(), state.begin() ); 

  // exit focusing ......................... 

  state[i_npx] +=  tan( elm.getExitAngle())/rho *  state[i_x]; 
  state[i_npy] -=  tan( elm.getExitAngle())/rho *  state[i_y]; 

}

}// namespace



sbend::Propagator::Propagator()
  : BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sbend::Propagator::Propagator( sbend const& elm )
  : BasePropagator(elm)
{ 
  ctor(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sbend::Propagator::Propagator( Propagator const& p )
  : BasePropagator(p)
{}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::Propagator::ctor( BmlnElmnt const& arg )
{ 
  
  sbend const& elm = static_cast<sbend const&>(arg);

  bml_ = BmlPtr( new beamline("SBEND_PRIVATE") );
 
  double angle_       = sbend::sbend_core_access::get_angle(elm);  
  double usAngle_     = sbend::sbend_core_access::get_usAngle(elm); 
  double dsAngle_     = sbend::sbend_core_access::get_dsAngle(elm); 
  double usFaceAngle_ = sbend::sbend_core_access::get_usFaceAngle(elm); 
  double dsFaceAngle_ = sbend::sbend_core_access::get_dsFaceAngle(elm); 

  double str = arg.Strength();

  if (elm.hasUpstreamEdge() ) {
    EdgePtr uedge( new Edge("",  tan(usAngle_) * str ) );
    bml_->append( uedge );
  }

  BendPtr  bend( new Bend( "", elm.Length(),  str, angle_,  usAngle_,  dsAngle_, usFaceAngle_,  dsFaceAngle_ , Bend::type_sbend ) );
  bml_->append( bend  );

  if ( elm.hasDownstreamEdge() ) {
    EdgePtr dedge( new Edge( "", -tan(dsAngle_)* str ) );
    bml_->append( dedge );
  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  sbend::Propagator::setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  ctor(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::Propagator::operator()( BmlnElmnt const& elm, Particle& p)
{
  ::propagate( static_cast<sbend const&>(elm), p, bml_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p)
{
  ::propagate( static_cast<sbend const&>(elm), p, bml_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


sbend::MADPropagator::MADPropagator()
  : BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sbend::MADPropagator::MADPropagator( sbend const& elm )
  : BasePropagator(elm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sbend::MADPropagator::MADPropagator( MADPropagator const& p )
  : BasePropagator(p)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  sbend::MADPropagator::ctor( BmlnElmnt const& elm )
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  sbend::MADPropagator::setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  ctor(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::MADPropagator::operator()( BmlnElmnt const& elm, Particle& p)
{
  ::mad_propagate<sbend,Particle>( static_cast<sbend const&>(elm), p,  0.0);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::MADPropagator::operator()( BmlnElmnt const& elm, JetParticle& p)
{
  ::mad_propagate<sbend,JetParticle>( static_cast<sbend const&>(elm), p,  0.0);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

