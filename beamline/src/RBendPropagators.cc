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
******    a pre-existing rbend with fewer than two edge elements.
******    : as a reminder, the issue of multiply redundant setups
******      has never been handled satisfactorily.
******  
******  Apr 2015            michelotti@fnal.gov
******  - added option of using dynamically calculated entry and exit
******    angles in the edge propagators, instead of the angles 
******    "hard-wired" by RefRegVisitor. To enable this option,
******    pass a -DNO_FIXED_ENTRY_ANGLE macro definition to the compiler.
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
  bool hasEntryEdge = false;
  bool hasExitEdge  = false;
  bool hasOldBml    = false;

  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr( arg); 

  if( bml ) {
    hasOldBml    = true;
    hasEntryEdge = ( typeid(*(bml->firstElement())) == typeid(Edge) );
    hasExitEdge  = ( typeid(*(bml->lastElement()))  == typeid(Edge) );
  }

  double& usFaceAngle_ = rbend::rbend_core_access::get_usFaceAngle(arg); 
  double& dsFaceAngle_ = rbend::rbend_core_access::get_dsFaceAngle(arg); 
  double& usAngle_     = rbend::rbend_core_access::get_usAngle(arg); 
  double& dsAngle_     = rbend::rbend_core_access::get_dsAngle(arg); 

  #ifdef NO_FIXED_ENTRY_ANGLE
  EdgePtr uedge( new Edge( "",  arg.Strength() ) );
  EdgePtr dedge( new Edge( "", -arg.Strength() ) );
  #else
  EdgePtr uedge( new Edge( "",  tan(usAngle_) * arg.Strength() ) );
  EdgePtr dedge( new Edge( "", -tan(dsAngle_) * arg.Strength() ) );
  #endif

  BendPtr bend ( new Bend( "",  arg.Length(),   arg.Strength(), arg.getBendAngle(),
                                usAngle_,  dsAngle_, usFaceAngle_,  dsFaceAngle_,  // ??? Bends should not need
                                Bend::type_rbend ) );                              // ??? usAngle_ and dsAngle_

  bml = BmlPtr(new beamline("RBEND_PRIVATE") );
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
