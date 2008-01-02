/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LinacCavityPropagators.cc
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******            
******  Author:    Jean-Francois Ostiguy  ostiguy@fnal.gov                                              
******                                                                
********************************************************* **************
************************************************************************
*************************************************************************/

#include <basic_toolkit/iosetup.h>
#include <beamline/LinacCavity.h>
#include <beamline/LinacCavityPropagators.h>
#include <beamline/LinacCavityParts.h>
#include <beamline/WakeKick.h>
#include <beamline/WakeKickPropagator.h>

#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

using FNAL::pcerr;
using FNAL::pcout;

namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;

// ---------------------------------------------------------------------------------------------
//  NOTE: cloning semantics is not appropriate for this propagator; we store a reference in 
//        the boost function object. 

  WakeKickPropagator wake_propagator(256, 12 * 300.0e-6 );  
  boost::function< void( ParticleBunch& ) > wake_propagator_ref = boost::ref( wake_propagator);  
  //--------------------------------------------------------------------------------------------


template<typename Particle_t>
void propagate( bmlnElmnt& elm, Particle_t&  p)
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr( elm );

  for ( beamline::iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }

  // reference time adjustment is done at the element level in the nested beamline. 
}

} // namespace


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::Propagator::setup( LinacCavity& arg ) 
{
 
  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr(arg);
  ElmPtr& elm = bmlnElmnt::core_access::get_ElmPtr(arg);
  
  bml = BmlPtr(new beamline("LINACCAVITY_INTERNALS") );
  
  bml->append( LCavityUpstreamPtr( new LCavityUpstream( "LC-upstream",   arg.Length()/2.0,  arg.getFrequency(), 
                                                                         arg.Strength()/2.0, arg.getPhi() )   )  );

  elm= WakeKickPtr( new WakeKick ( "Wake", wake_propagator_ref )  );  
 
  if (arg.wakeOn()) bml->append(elm); 

  bml->append( LCavityDnstreamPtr( new LCavityDnstream( "LC-downstream", arg.Length()/2.0, arg.getFrequency(), 
                                                                         arg.Strength()/2.0, arg.getPhi() )   )  );

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::Propagator::operator()( LinacCavity& elm, Particle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::Propagator::operator()( LinacCavity& elm, JetParticle& p ) 
{
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
