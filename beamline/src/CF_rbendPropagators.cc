/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      CF_rbendPropagators.cc
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
******  Authors:   Leo Michelotti         michelotti@fnal.gov
******             Jean-Francois Ostiguy  ostiguy@fnal.gov
******
******  REVISION HISTORY:
******  
******  Apr 2008            michelotti@fnal.gov
******  - bug fix: changed arguments sent to rbend constructors 
******    in CF_rbend::Propagator::setup
******  - bug fix: reduced length of separator element by two
******  - nullified edge effects from internal bends.
******    : edge effects to be handled by elements usedge and dsedge only
******  May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation
******  
**************************************************************************
*************************************************************************/

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/beamline.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <beamline/rbend.h>
#include <beamline/Edge.h>
#include <beamline/Bend.h>
#include <beamline/CF_rbendPropagators.h>
#include <iostream>

namespace {

  typedef PhaseSpaceIndexing::index index;

  index const i_x   = Particle::i_x;
  index const i_y   = Particle::i_y;
  index const i_cdt = Particle::i_cdt;
  index const i_npx = Particle::i_npx;
  index const i_npy = Particle::i_npy;
  index const i_ndp = Particle::i_ndp;

template<typename Particle_t>
void propagate( CF_rbend const& elm, Particle_t&  p, BmlPtr bml)
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.state();

  Component_t cdt_in = state[i_cdt];

  for ( beamline::const_iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }
 
  state[i_cdt] -= elm.getReferenceTime(); 
}

} // namespace


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::Propagator::Propagator ( int n)
   : n_(n), BasePropagator()
 {}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::Propagator::Propagator ( CF_rbend const& elm, int n)
   : n_(n), BasePropagator(elm)
{
  ctor(elm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CF_rbend::Propagator::Propagator ( Propagator const& p)
   : n_(p.n_), BasePropagator(p)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::Propagator::ctor( BmlnElmnt const& arg ) 
{

  CF_rbend const& elm = static_cast<CF_rbend const&>(arg);

  //----------------------------------------------------------------------------
  // NOTE: the proportions below come from a quadrature rule meant to minimize 
  //       the error when a magnet is split into 4 parts. See R. Talman 
  //         
  // 2*6/15  *  (L/4) +   3*16/15 * (L/4)  = 60/15 * ( L/4) =  L
  // 2* ends +   body     =  L  
  //----------------------------------------------------------------------------       

  double field       =  elm.Strength();
  double frontLength =   (6.0/15.0)*( elm.Length()/(4.0*n_) );
  double sepLength   =  (16.0/15.0)*( elm.Length()/(4.0*n_) );
  
  Edge       usedge( "",  tan( elm.getEntryAngle()*field ) );  
  rbend      usbend( "" , frontLength, field,  0.0,    elm.getEntryFaceAngle(),  0.0                   ); 
  rbend      dsbend( "",  frontLength, field,  0.0,    0.0,                      elm.getExitFaceAngle());
  Edge       dsedge( "",  -tan( elm.getExitAngle()*field ) );  

  rbend   separator( "",  frontLength, field,  0.0,    0.0,                      0.0                   );
  rbend        body( "",  sepLength,   field,  0.0,    0.0,                      0.0                   );

     usbend.enableEdges(false, false);
  separator.enableEdges(false, false);
     dsbend.enableEdges(false, false);
       body.enableEdges(false, false);

  thinSextupole ts( "",0.0 );
  thinQuad      tq( "",0.0 );

  bml_ = BmlPtr( new beamline("CF_RBEND_INTERNALS") );

  for( int i=0; i<n_; ++i) {

    if ( i == 0 ) { 
      if( elm.hasUpstreamEdge() ) { bml_->append( usedge ); }
      bml_->append( usbend );
    } 
    else {
      bml_->append( separator );
    }

    bml_->append( ts       );
    bml_->append( tq       );
    bml_->append( body     );
    bml_->append( ts       );
    bml_->append( tq       );
    bml_->append( body     );
    bml_->append( ts       );
    bml_->append( tq       );
    bml_->append( body     );
    bml_->append( ts       );
    bml_->append( tq       );
  
    if ( i == n_-1 ) { 
      bml_->append( dsbend );
      if( elm.hasDownstreamEdge()) { bml_->append( dsedge ); }
    } 
    else {
      bml_->append( separator );
    }
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  CF_rbend::Propagator::setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value )
{ 
 ctor(elm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::Propagator::operator()( BmlnElmnt const& elm, Particle& p )
{ 
  ::propagate( static_cast<CF_rbend const&>(elm), p, bml_);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p )
{ 
  ::propagate(  static_cast<CF_rbend const&>(elm), p, bml_);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

