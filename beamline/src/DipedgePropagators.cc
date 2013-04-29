/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      DipedgePropagators.cc
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
******
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
******  Author:    Leo Michelotti
******             Phone: (630) 840 4956
******             Email: michelotti@fnal.gov
******
******
****** REVISION HISTORY
****** ----------------
******
****** Apr 2013           Leo Michelotti
******                    michelotti@fnal.gov
****** - initial implementation, complete with errors.
******   THIS IS ONLY A PROTOTYPE VERSION, little
******   more than a placeholder file. 
****** - class Dipedge is meant to mimic the DIPEDGE element
******   used by MAD-X.
****** - Associated files:
******     beamline/include/Dipedge.h
******     beamline/include/DipedgePropagators.h
******     beamline/src/Dipedge.cc
******
**************************************************************************
*************************************************************************/

#include <iostream>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/DipedgePropagators.h>

namespace 
{
  // Frequently included
  // -------------------
  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template<typename Particle_t>
void propagate( Dipedge& elm, Particle_t& p )
{
  // Typical preliminaries
  // ---------------------
  typedef typename PropagatorTraits<Particle_t>::State_t     State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t Component_t;


  // Typically include line ...
  // -------------------------
  // if( elm.Strength() == 0.0 ) return;

  // These lines of code correspond to MAD's SUBROUTINE tmrfng,
  // as provided by Frank Schmidt in March, 2013.
  //
  // It assumes MAD ordering:
  // 1->x
  // 2->npx = p_x/p_ref
  // 3->y
  // 4->npy = p_y/p_ref
  // ----------------------------------------
  

  State_t& state = p.State();

  Component_t const x   = state[i_x];
  Component_t const y   = state[i_y];
  Component_t const npx = state[i_npx];
  Component_t const npy = state[i_npy];


  // -----------
  // Linear part

  double tanedg =   tan( elm.edge() );
  double secedg =   1.0 / cos( elm.edge() );
  double sn     =   sin( elm.edge() );
  double psip   =   elm.edge() - elm.corr()*secedg*( 1.0 + sn*sn );
  double re_2_1 =   elm.h()*tanedg;
  double re_4_3 = - elm.h()*tan( psip );


  // ----------------------
  // Second order terms

  double te_1_1_1 = 0.0;
  double te_1_3_3 = 0.0;
  double te_2_1_1 = 0.0;
  double te_2_1_2 = 0.0;
  double te_2_3_3 = 0.0;
  double te_2_3_4 = 0.0;
  double te_3_1_3 = 0.0;
  double te_4_1_3 = 0.0;
  double te_4_1_4 = 0.0;
  double te_4_2_3 = 0.0;

  if( elm.fsec() ) {
    double hh = elm.sig()*( elm.h()/2.0 );

    te_1_1_1 = -hh*tanedg*tanedg;
    te_1_3_3 =  hh*secedg*secedg;

    te_2_1_1 =   (elm.h()/2.0) * elm.he() * (secedg*secedg*secedg)
               + elm.sk1() * tanedg;
    te_2_1_2 = - te_1_1_1;
    te_2_3_3 =   hh * elm.h() * (tanedg*tanedg*tanedg)  -  te_2_1_1;
    te_2_3_4 =   te_1_1_1;

    te_3_1_3 = - te_1_1_1;

    te_4_1_3 = - te_2_1_1;
    te_4_1_4 =   te_1_1_1;
    te_4_2_3 = - te_1_3_3;

    if( elm.sig() > 0 ) {
      te_2_3_3 += (elm.h()*secedg)*(elm.h()*secedg)*(tanedg/2.0);
    }
    else {
      te_2_1_1 -= (elm.h()*secedg)*(elm.h()*secedg)*(tanedg/2.0);
      te_4_1_3 += (elm.h()*secedg)*(elm.h()*secedg)*(tanedg/2.0);
    }
  }


  // -----------------------------
  // The transformation itself ...

  // ... linear terms

  state[i_npx] +=   re_2_1*x;
  state[i_npy] +=   re_4_3*y;

  // ... quadratic terms

  state[i_x]   +=   te_1_1_1*x*x   + te_1_3_3*y*y;
  state[i_npx] +=   te_2_1_1*x*x   + te_2_3_3*y*y
                  + te_2_1_2*x*npx + te_2_3_4*y*npy;
  state[i_y]   +=   te_3_1_3*x*y;
  state[i_npy] +=   te_4_1_3*x*y;
                  + te_4_1_4*x*npy + te_4_2_3*npx*y;
  

  // ??? what does call tmsymm do ???

  // ------------------
  // Typical final step ??? ??? ???

  // state[i_cdt] -= elm.getReferenceTime(); ??? ??? ???
}


//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate( Dipedge& elm,    Particle& p );
template void propagate( Dipedge& elm, JetParticle& p );

#endif

} // namespace



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Dipedge::Propagator::setup( Dipedge& arg)
{ 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Dipedge::Propagator::operator()( Dipedge& elm, Particle& p)
{
  ::propagate( elm, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Dipedge::Propagator::operator()( Dipedge& elm, JetParticle& p)
{
  ::propagate( elm, p);
}
