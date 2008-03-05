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
******  Authors:   Leo Michelotti         michelotti@fnal.gov
******             Jean-Francois Ostiguy  ostiguy@fnal.gov
******
******
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

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template<typename Particle_t>
void propagate( rbend& elm, Particle_t& p )
{
   
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr( elm );

  for ( beamline::iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }

 state[i_cdt] -= elm.getReferenceTime();  

}


template <typename T>
double toDouble( T const& value);

template <>
double toDouble( double const& value)
{ return value;}

template <>
double toDouble( Jet const& value)
{ return value.standardPart();}

#if 0
template <typename Particle_t> 
void mad_propagate( rbend& elm, Particle_t& p ) 
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t&     state    = p.State();

  Component_t rho   = p.BRho()/ elm.Strength();

  double const length = elm.Length();
  double const angle  = 2.0 * asin( length /( 2.0* p.ReferenceBrho() )); // ????? 

  Component_t m00 =  cos(angle);
  Component_t m03 =  sin(angle)*rho;
  Component_t m05 =  rho*(1.0 - cos(angle))/p.Beta();
  Component_t m14 =  length;
  Component_t m20 = -sin(angle)/p.Beta();
  Component_t m23 = -m05;
  Component_t m25 =  length/p.Beta()/p.Gamma()/p.Gamma() - ( length - sin(angle)*rho )/p.Beta();
  Component_t m30 = -sin(angle)/rho;
  Component_t m33 =  m00;
  Component_t m35 = -m20;
  
  Component_t edge30 =  tan( angle/2.0 )/(rho);
  Component_t edge41 = -edge30;
 
 // entrance edge
   
  State_t      instate  = state;

  state[i_npx] = edge30*state[i_x];
  state[i_npy] = edge41*state[i_y];

  // bend  

  instate  = state;
   
  state[i_x  ] = m00*instate[i_x  ] + m03*instate[i_npx] +  m05*instate[i_ndp];  
  state[i_y  ] = m14*instate[i_npy];
  state[i_cdt] = m20*instate[i_x  ] + m23*instate[i_npx] +  m25*instate[i_ndp];
  state[i_npx] = m30*instate[i_x  ] + m33*instate[i_npx] +  m35*instate[i_ndp];

  instate  = state;

 // exit edge

  state[i_npx] = edge30*state[i_x];
  state[i_npy] = edge41*state[i_y];

}
#endif


//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(     rbend& elm,    Particle& p );
template void propagate(     rbend& elm, JetParticle& p );

#endif

} // namespace

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::Propagator::setup( rbend& arg) 
{
  
  BmlPtr bml = bmlnElmnt::core_access::get_BmlPtr( arg); 
  bml = BmlPtr(new beamline("RBEND_PRIVATE") );
 
  double& usFaceAngle_ = rbend::rbend_core_access::get_usFaceAngle(arg); 
  double& dsFaceAngle_ = rbend::rbend_core_access::get_dsFaceAngle(arg); 
  double& usAngle_     = rbend::rbend_core_access::get_usAngle(arg); 
  double& dsAngle_     = rbend::rbend_core_access::get_dsAngle(arg); 


  EdgePtr uedge( new Edge("",  tan(usAngle_) * arg.Strength() ) );

  BendPtr bend( new Bend( "RBEND_PRIVATE",  arg.Length(), arg.Strength() , arg.getBendAngle(),  usAngle_,  dsAngle_, 
                                                          usFaceAngle_,  dsFaceAngle_ , Bend::type_rbend)  );

  EdgePtr dedge( new Edge( "", -tan(dsAngle_) * arg.Strength() ) );


  bml->append( uedge );
  bml->append( bend  );
  bml->append( dedge );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::Propagator::operator()( rbend& elm, Particle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rbend::Propagator::operator()( rbend& elm, JetParticle& p ) 
{
  ::propagate(elm,p);
}
