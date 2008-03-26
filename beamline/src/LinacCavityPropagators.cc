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
void propagate( LinacCavity& elm, Particle_t&  p)
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

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate( LinacCavity& elm,    Particle& p );
template void propagate( LinacCavity& elm, JetParticle& p );

#endif

//-----------------------------------------------------------------------------------

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
  bml->append( LCavityDnstreamPtr( new LCavityDnstream( "LC-downstream", arg.Length()/2.0, arg.getFrequency(), 
                                                                         arg.Strength()/2.0, arg.getPhi() )   )  );

  // wake

  elm = WakeKickPtr( new WakeKick ( "Wake", wake_propagator_ref ) ); 


}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool LinacCavity::Propagator::wakeOn( LinacCavity const& arg ) const
{

  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr( const_cast<LinacCavity&>(arg) );

  for ( beamline::const_iterator it = bml->begin(); it != bml->end(); ++it ) {
    if ( boost::dynamic_pointer_cast<WakeKick>(*it) ) return true; 
  }
  
  return false;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::Propagator::setWakeOn( LinacCavity& arg, bool set ) 
{


  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr(arg);
  ElmPtr& elm = bmlnElmnt::core_access::get_ElmPtr(arg);
  
  bool wakeon =  wakeOn(arg);

  if (  set   &&  wakeon   ) return; // wake is already set   just return
  if ( (!set) && (!wakeon) ) return; // wake is already unset just return
 
  if ( set ) {
    beamline::iterator it = bml->begin(); 
    bml->putBelow( it, elm);
  }
  else {
   for ( beamline::iterator it = bml->begin(); it != bml->end(); ++it ) {
     if ( boost::dynamic_pointer_cast<WakeKick>(*it) ) { it = bml->erase(it); } 
   } 
 
  }
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

void LinacCavity::Propagator::operator()( LinacCavity& elm, ParticleBunch& b ) 
{
  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr(elm);

  for ( beamline::iterator it = bml->begin(); it != bml->end(); ++it) {
    (*it)->localPropagate(b); 
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::Propagator::operator()( LinacCavity& elm, JetParticleBunch& b ) 
{
  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr(elm);

  for ( beamline::iterator it = bml->begin(); it != bml->end(); ++it) {
    (*it)->localPropagate(b); 
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

