/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      RbendPropagators.tcc
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
******  Authors:   Leo Michelotti           michelotti@fnal.gov
******             Jean-Francois Ostiguy    ostiguy@fnal.gov
******
****** REVISION HISTORY:
******
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation.
******
**************************************************************************
*************************************************************************/

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/beamline.h>
#include <beamline/RBendPropagators.h>
#include <beamline/Edge.h>
#include <beamline/Bend.h>
#include <beamline/rbend.h>
#include <beamline/drift.h>
#include <iostream>

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::i_x;
  Particle::PhaseSpaceIndex const& i_y   = Particle::i_y;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::i_cdt;
  Particle::PhaseSpaceIndex const& i_npx = Particle::i_npx;
  Particle::PhaseSpaceIndex const& i_npy = Particle::i_npy;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::i_ndp;


template <typename T>
inline double toDouble( T const& );

template<>
inline double toDouble( Jet const& value)    { return value.standardPart(); }

template<>
inline double toDouble( double const& value) { return value; }
 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename Particle_t>
void propagate( rbend const& elm, Particle_t& p )
{
   
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  BmlPtr const& bml = bmlnElmnt::core_access::get_BmlPtr( elm );

  for ( beamline::const_iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }

 state[i_cdt] -= elm.getReferenceTime();  

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template < typename Element_t, typename Particle_t>
void mad_propagate( Element_t const& elm, Particle_t& p, double const& rho, double const& n )
{

  int const BMLN_dynDim = 6;

  typedef typename PropagatorTraits<Particle_t>::State_t     State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t Component_t;

  State_t& state = p.State();
  
  double const h      = 1/rho;                    // curvature
  double angle        = 2.0 * asin(elm.Length()/( 2.0*toDouble(rho)));
  double const length = angle*rho;         
  double const gamma  = toDouble( p.Gamma() );
  double const beta   = toDouble( p.Beta()  );


  if(angle == 0.0) {
    state[i_x  ] +=   length * state[i_npx];
    state[i_y  ] +=   length * state[i_npy];
    state[i_cdt] +=  -length /(beta*gamma*gamma) * state[i_ndp];
    return 0;
  }  

  double  matrix [ BMLN_dynDim ][BMLN_dynDim];
  
  for   ( int i=0; i < BMLN_dynDim; ++i) {
    for ( int j=0; j < BMLN_dynDim; ++j){
      matrix[i][j] = ( i==j) ? 1.0 : 0.0;
    }
  }
  
  double const kx     = h * sqrt(1.0 - n ); 
  double const ky     = h * sqrt( n ); 
  
  
  // entrance focusing 
  
  state[i_npx] +=  tan(angle/2.0+elm.getEntryAngle())/rho *  state[i_x]; 
  state[i_npy] -=  tan(angle/2.0-elm.getEntryAngle())/rho *  state[i_y]; 

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

  const double cnv =  - 1.0/beta;  

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

  State_t const& inState  = p.State();
  State_t        outState( p.State().Dim() ); 

  for(  int i=0; i<BMLN_dynDim; ++i  ) {
    outState[i] = 0.0;
    for( int j=0; j<BMLN_dynDim; ++j  ) {
      outState[i] += matrix[i][j]*inState[j];
    }
  }

  std::copy( outState.begin(), outState.end(), state.begin() ); 

  // exit focusing ......................... 

  state[i_npx] +=  tan( angle/2.0+elm.getExitAngle())/rho *  state[i_x]; 
  state[i_npy] -=  tan(-angle/2.0+elm.getExitAngle())/rho *  state[i_y]; 

}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(     rbend& elm,    Particle& p );
template void propagate(     rbend& elm, JetParticle& p );
template void mad_propagate( rbend& elm,    Particle& p, double const& rho, double const& n );
template void mad_propagate( rbend& elm, JetParticle& p, double const& rho, double const& n );
template void mad_propagate( rbend& elm,  Particle_t& p )
template void mad_propagate( rbend& elm,  Particle_t& p )
template double toDouble( double const& );
template double toDouble( Jet    const& );

#endif

} // namespace

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::Propagator::setup( bmlnElmnt& arg                                ) 
{
  
  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr( arg); 
  bml = BmlPtr(new beamline("RBEND_PRIVATE") );
 
  rbend& elm = static_cast<rbend&>(arg); 
 
  double& usFaceAngle_ = rbend::rbend_core_access::get_usFaceAngle(elm); 
  double& dsFaceAngle_ = rbend::rbend_core_access::get_dsFaceAngle(elm); 
  double& usAngle_     = rbend::rbend_core_access::get_usAngle(elm); 
  double& dsAngle_     = rbend::rbend_core_access::get_dsAngle(elm); 

  if ( elm.hasUpstreamEdge() ) {
       EdgePtr uedge( new Edge("",  tan(usAngle_) * elm.Strength() ) );
       bml->append( uedge );
  }

  BendPtr bend( new Bend( "RBEND_PRIVATE",  elm.Length(), elm.Strength(), elm.getBendAngle(),  usAngle_,  dsAngle_, 
                                                          usFaceAngle_,  dsFaceAngle_ , Bend::type_rbend)  );
  bml->append( bend  );

  if (elm.hasDownstreamEdge() ) {
     EdgePtr dedge( new Edge( "", -tan(dsAngle_) * elm.Strength() ));
     bml->append( dedge );
  }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  rbend::Propagator::setAttribute( bmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  setup(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::Propagator::operator()( bmlnElmnt const& elm, Particle& p ) 
{
  ::propagate( static_cast<rbend const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::Propagator::operator()( bmlnElmnt const& elm, JetParticle& p ) 
{
  ::propagate(static_cast<rbend const&>(elm), p );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
void rbend::MADPropagator::operator()( bmlnElmnt const& elm, Particle& p ) 
{

  double const rho    = toDouble( p.BRho() )/elm.Strength();

  // field index = -(dB/B)/ (dx/rho) 

  double const n      = - ( 1.0/ elm.Length() ) * elm.getQuadrupole() / elm.Strength() * rho;                       
  ::propagate(static_cast<rbend const&>(elm), p, n);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::MADPropagator::operator()( bmlnElmnt const& elm, JetParticle& p ) 
{
  double const rho    = toDouble( p.BRho() )/elm.Strength();

  // field index = -(dB/B)/ (dx/rho) 

  double const n      = - ( 1.0/ elm.Length() ) * elm.getQuadrupole() / elm.Strength() * rho;                       
  ::propagate(static_cast<rbend const&>(elm), p, n);
}


#endif
