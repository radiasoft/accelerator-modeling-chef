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
******  Apr 2008            michelotti@fnal.gov
******  - bug fix: changed arguments sent to rbend constructors 
******    in CF_rbend::Propagator::setup
******  - bug fix: reduced length of separator element by two
******  - nullified edge effects from internal bends.
******    : edge effects to be handled by elements usedge and dsedge only
******
******  Feb 2014            michelotti@fnal.gov
******  - bug fix: added same "KLUDGE" that has been in place
******    in file CF_sbendPropagators.cc since (at least)
******    December, 2007.
******    : this should be "done right" in both files one day.
******    : thanks to Eric Stern for drilling down the effects
******      of this error through multiple layers of code.
******
******  Dec 2014            michelotti@fnal.gov
******  - removed the "KLUDGE" that prevented the .setup routine
******    from working more than once on the same or a cloned CF_rbend.
******    : as a reminder, the issue of multiply redundant setups
******      has never been handled satisfactorily.
******  
******  Jan 2015            michelotti@fnal.gov
******  - bug fix: added code to the .setup routine for reinitializing
******    a pre-existing CF_rbend with fewer than two edge elements.
******    : repeat "as a reminder" from Dec 2014 entry (above).
******  
******  Apr 2015            michelotti@fnal.gov
******  - bug fix: displaced parenthesis in Edge constructor argument.
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

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;

template<typename Particle_t>
void propagate( CF_rbend& elm, Particle_t&  p)
{
  
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  State_t& state = p.State();

  Component_t cdt_in = state[i_cdt];

  BmlPtr& bml = bmlnElmnt::core_access::get_BmlPtr( elm );

  for ( beamline::iterator it = bml->begin(); it != bml->end(); ++it ) { 
     (*it)->localPropagate( p );
  }
 
  state[i_cdt] -= elm.getReferenceTime(); 
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate( CF_rbend& elm,    Particle& p );
template void propagate( CF_rbend& elm, JetParticle& p );

#endif
//-----------------------------------------------------------------------------------

} // namespace


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::Propagator::setup( CF_rbend& arg ) 
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

  //----------------------------------------------------------------------------
  // NOTE: the proportions below come from a quadrature rule meant to minimize 
  //       the error when a magnet is split into 4 parts. See R. Talman 
  //         
  // 2*6/15  *  (L/4) +   3*16/15 * (L/4)  = 60/15 * ( L/4) =  L
  // 2* ends +   body     =  L  
  //----------------------------------------------------------------------------       

  double field       =  arg.Strength();
  double frontLength =   (6.0/15.0)*( arg.Length()/(4.0*n_) );
  double sepLength   =  (16.0/15.0)*( arg.Length()/(4.0*n_) );
  
  double quadStrength = arg.getQuadrupole();
  double sextStrength = arg.getSextupole();
  double octoStrength = arg.getOctupole();

  Edge       usedge( "",  tan(arg.getEntryAngle())*field );
  rbend      usbend( "" , frontLength, field,  0.0,    arg.getEntryFaceAngle(),  0.0                   ); 
  rbend      dsbend( "",  frontLength, field,  0.0,    0.0,                      arg.getExitFaceAngle());
  Edge       dsedge( "",  -tan(arg.getExitAngle())*field );

  rbend   separator( "",  frontLength, field,  0.0,    0.0,                      0.0                   );
  rbend        body( "",  sepLength,   field,  0.0,    0.0,                      0.0                   );

  usbend.nullEntryEdge();
  usbend.nullExitEdge();
  separator.nullEntryEdge();
  separator.nullExitEdge();
  dsbend.nullEntryEdge();
  dsbend.nullExitEdge();
  body.nullEntryEdge();
  body.nullExitEdge();

  thinSextupole ts( "",0.0 );
  thinQuad      tq( "",0.0 );

  bml = BmlPtr( new beamline("CF_RBEND_INTERNALS") );

  for( int i=0; i<n_; ++i) {

    if ( i == 0 ) { 
      bml->append( usedge );
      bml->append( usbend );
    } 
    else {
      bml->append( separator );
    }

    bml->append( ts       );
    bml->append( tq       );
    bml->append( body     );
    bml->append( ts       );
    bml->append( tq       );
    bml->append( body     );
    bml->append( ts       );
    bml->append( tq       );
    bml->append( body     );
    bml->append( ts       );
    bml->append( tq       );
  
    if ( i == n_-1 ) { 
      bml->append( dsbend );
      bml->append( dsedge );
    } 
    else {
      bml->append( separator );
    }
  }

  arg.setQuadrupole( quadStrength );
  arg.setSextupole ( sextStrength );

  if( hasOldBml ) 
  {
    if( !hasEntryEdge ) { arg.nullEntryEdge(); }
    if( !hasExitEdge  ) { arg.nullExitEdge();  }
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::Propagator::operator()(CF_rbend& elm, Particle& p )
{ 
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CF_rbend::Propagator::operator()(CF_rbend& elm, JetParticle& p )
{ 
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

