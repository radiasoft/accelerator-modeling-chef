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
******  Copyright (c) Fermi Research Alliance, LLC
******                Universities Research Association, Inc.
******                Fermilab
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contracts No. DE-AC02-76CH03000
******  and No. DE-AC02-07CH11359.
******
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
******  ----------------
******  REVISION HISTORY
******  ----------------
******  
******  Jan 2015            michelotti@fnal.gov
******  - bug fix: added code to the .setup routine for reinitializing
******    a pre-existing sbend with fewer than two edge elements.
******    : as a reminder, the issue of multiply redundant setups
******      has never been handled satisfactorily.
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

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;

template<typename Particle_t>
void propagate( sbend& elm, Particle_t&     p )
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


template <typename Particle_t>
void mad_propagate( sbend& elm, Particle_t& p)
{

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t&     state    = p.State();

  Component_t rho   = p.BRho()/ elm.Strength();

  double const length = elm.Length();
  double const angle  = elm.getBendAngle();
 
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

  // bend  

  State_t instate  = state;
   
  state[i_x  ] = m00*instate[i_x  ] + m03*instate[i_npx] +  m05*instate[i_ndp];  
  state[i_y  ] = m14*instate[i_npy];
  state[i_cdt] = m20*instate[i_x  ] + m23*instate[i_npx] +  m25*instate[i_ndp];
  state[i_npx] = m30*instate[i_x  ] + m33*instate[i_npx] +  m35*instate[i_ndp];
 
}


//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate(     sbend& elm,    Particle& p );
template void propagate(     sbend& elm, JetParticle& p );
template void mad_propagate( sbend& elm,    Particle& p );
template void mad_propagate( sbend& elm, JetParticle& p );

#endif

}// namespace



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::Propagator::setup( sbend& arg )
{ 
  bool hasEntryEdge = false;
  bool hasExitEdge  = false;
  bool hasOldBml    = false;

  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr(arg); 

  if( bml ) {
    hasOldBml    = true;
    hasEntryEdge = ( typeid(*(bml->firstElement())) == typeid(Edge) );
    hasExitEdge  = ( typeid(*(bml->lastElement()))  == typeid(Edge) );
  }

  double& angle_       = sbend::sbend_core_access::get_angle(arg);  
  double& usAngle_     = sbend::sbend_core_access::get_usAngle(arg); 
  double& dsAngle_     = sbend::sbend_core_access::get_dsAngle(arg); 
  double& usFaceAngle_ = sbend::sbend_core_access::get_usFaceAngle(arg); 
  double& dsFaceAngle_ = sbend::sbend_core_access::get_dsFaceAngle(arg); 

  EdgePtr uedge( new Edge( "",  tan(usAngle_) * arg.Strength() ) );
  EdgePtr dedge( new Edge( "", -tan(dsAngle_) * arg.Strength() ) );
  BendPtr bend(  new Bend( "",  arg.Length(),   arg.Strength() , angle_,  
                                usAngle_,  dsAngle_, usFaceAngle_,  dsFaceAngle_, 
                                Bend::type_sbend ) );

  bml = BmlPtr( new beamline("SBEND_PRIVATE") );
  bml->append( uedge );
  bml->append( bend  );
  bml->append( dedge );

  if( hasOldBml ) 
  {
    if( !hasEntryEdge ) { arg.nullEntryEdge(); }
    if( !hasExitEdge  ) { arg.nullExitEdge();  }
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::Propagator::operator()( sbend& elm, Particle& p)
{
  ::propagate( elm, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sbend::Propagator::operator()( sbend& elm, JetParticle& p)
{
  ::propagate( elm, p);
}

