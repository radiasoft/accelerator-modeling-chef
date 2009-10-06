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
******  Author:    Jean-Francois Ostiguy  
******             ostiguy@fnal.gov
******                                                                
****** REVISION HISTORY:
******
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation
******
************************************************************************
************************************************************************
*************************************************************************/

#include <basic_toolkit/iosetup.h>
#include <beamline/LinacCavity.h>
#include <beamline/LinacCavityParts.h>
#include <beamline/LinacCavityPropagators.h>
#include <beamline/LinacCavityParts.h>
#include <beamline/WakeKick.h>

#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

using boost::dynamic_pointer_cast;

using FNAL::pcerr;
using FNAL::pcout;

namespace {

  typedef PhaseSpaceIndexing::index index;
 
 index const i_x   = Particle::i_x;
 index const i_y   = Particle::i_y;
 index const i_cdt = Particle::i_cdt;
 index const i_npx = Particle::i_npx;
 index const i_npy = Particle::i_npy;
 index const i_ndp = Particle::i_ndp;

template<typename Particle_t>
void propagate( LinacCavity const& elm, Particle_t&  p, BmlPtr bml )
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.state();

  for ( beamline::const_iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }

  // **** NOTE****  
  // reference time adjustment is done at the _element level_
  // in the nested beamline. 

}

} // namespace


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LinacCavity::Propagator::Propagator()
    : BasePropagator()
  {}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LinacCavity::Propagator::Propagator(LinacCavity const& elm)
    : BasePropagator(elm)
{
  ctor(elm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LinacCavity::Propagator::Propagator(Propagator const& p)
    : BasePropagator(p)
  {}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::Propagator::ctor( BmlnElmnt const& arg ) 
{
 
  LinacCavity const& elm = static_cast< LinacCavity const&>(arg);

  bml_ = BmlPtr(new beamline("LINACCAVITY_INTERNALS") );

  bml_->append( LCavityUpstreamPtr( new LCavityUpstream( "LC-upstream",   elm.Length()/2.0,   elm.frequency(), 
                                                                         elm.Strength()/2.0, elm.phi() )   )  );
  wake_ = WakeKickPtr( new WakeKick ( "WAKE" ) ); 
 
  if ( elm.wakeOn()) bml_->append(wake_); 

  bml_->append( LCavityDnstreamPtr( new LCavityDnstream( "LC-downstream", elm.Length()/2.0, elm.frequency(), 
                                                                         elm.Strength()/2.0, elm.phi() )   )  );

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool LinacCavity::Propagator::wakeIsOn( LinacCavity const& arg ) const
{

  for ( beamline::const_iterator it = bml_->begin(); it != bml_->end(); ++it ) {
    if ( boost::dynamic_pointer_cast<WakeKick>(*it) ) return true; 
  }
  
  return false;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  LinacCavity::Propagator::setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
  if ( name == "STRENGTH" ) { 

     (bml_->front())->setStrength( elm.Strength()/2.0 ); 
     (bml_->back() )->setStrength( elm.Strength()/2.0 ); 
     return;
  }
  if ( name == "LENGTH" ) { 

     (bml_->front())->setLength( elm.Length()/2.0 ); 
     (bml_->back() )->setLength( elm.Length()/2.0 ); 
     return;
  }
  if ( name == "FREQUENCY" ) { 

     dynamic_pointer_cast<LCavityUpstream>(bml_->front())->setFrequency(  static_cast<LinacCavity&>(elm).frequency() ); 
     dynamic_pointer_cast<LCavityDnstream>(bml_->back() )->setFrequency(  static_cast<LinacCavity&>(elm).frequency() ); 
     return;
  }

  if ( name == "PHASE" ) { 

     dynamic_pointer_cast<LCavityUpstream>(bml_->front())->setPhi( static_cast<LinacCavity&>(elm).phi() ); 
     dynamic_pointer_cast<LCavityDnstream>(bml_->back() )->setPhi( static_cast<LinacCavity&>(elm).phi() ); 
     return;
  }

  if ( name == "WAKEON" ) { 

  
     bool wakeon =   static_cast<LinacCavity&>(elm).wakeOn();

     bool set    =   wakeIsOn( static_cast<LinacCavity&>(elm) );  

     if (  set   &&  wakeon   ) return; // wake is already set   just return

     if ( (!set) && (!wakeon) ) return; // wake is already unset just return
 
     if ( set ) {
       beamline::iterator it = bml_->begin(); 
       bml_->putBelow( it, wake_);
     }
     else {
      for ( beamline::iterator it = bml_->begin(); it != bml_->end(); ++it ) {
         if ( boost::dynamic_pointer_cast<WakeKick>(*it) ) { it = bml_->erase(it); } 
       } 
     }
     return;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::Propagator::operator()( BmlnElmnt const& elm, Particle& p ) 
{
  ::propagate( static_cast<LinacCavity const&>(elm),p, bml_);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p ) 
{
  ::propagate( static_cast< LinacCavity const&>(elm),p, bml_);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::Propagator::operator()( BmlnElmnt const& elm, ParticleBunch& b ) 
{
  for ( beamline::const_iterator it = bml_->begin(); it != bml_->end(); ++it) {
    (*it)->localPropagate(b); 
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::Propagator::operator()( BmlnElmnt const& elm, JetParticleBunch& b ) 
{
  for ( beamline::const_iterator it = bml_->begin(); it != bml_->end(); ++it) {
    (*it)->localPropagate(b); 
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  LinacCavity::Propagator::getReferenceTime() const 
{

  ctRef_ = 0.0;

  for ( beamline::const_iterator it  = bml_->begin(); 
                                 it != bml_->end(); ++it ) {
        
    ctRef_  += (*it)->getReferenceTime();
  }

  return ctRef_;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::Propagator::propagateReference( Particle& particle, double initialBRho, bool scaling ) 
{               
  if ( bml_) {
    bml_->propagateReference(particle, initialBRho, scaling );
  }

  return;
}
